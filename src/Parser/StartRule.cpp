#include "StartRule.h"
#include "../Common/PublicFun.h"

#include <Poco/RegularExpression.h>
#include <Poco/String.h>


using namespace GraphDB::Parser;


StartRule::StartRule(const string& strSub, ErrorCode& err)
{
	//1.正则表达式提取“”,没有则为Index，有则为NameType
    const string strIndexPattern = "node\\(([^\\']*)\\)";
    const string strNamePattern = "node\\(\\'([^\\']*)\\'\\)";
    Poco::RegularExpression::MatchVec matches;
    string subString;

	matches.clear();
    Poco::RegularExpression regNameObj(strNamePattern);//正则表达式初始化，载入匹配模式	
    int matchCount = regNameObj.match(strSub, 0, matches);//正则表达式对分词结果进行匹配
    if (1 == matchCount)
    {
        m_isName = true;
    }
    else
    {
		matches.clear();
        Poco::RegularExpression regIndexObj(strIndexPattern);//正则表达式初始化，载入匹配模式
        matchCount = regIndexObj.match(strSub, 0, matches);//正则表达式对分词结果进行匹配
        if (1 == matchCount)
        {
            m_isName = false;
        }
        else
        {
            err = StartSegInvalid;
            return;
        }
    }
    subString = matches[0].name;
    if (m_isName)
    {
		Poco::replaceInPlace(subString, "node('", "");
		Poco::replaceInPlace(subString, "')", "");
    }
    else
    {
		Poco::replaceInPlace(subString, "node(", "");
		Poco::replaceInPlace(subString, ")", "");
    }
    subString = Poco::trim(subString);
    //2.对于Index场景，只有一个*就是全部节点 m_isAll = true;
    if (subString == "*")
    {
        m_isAll = true;
        err = NoError;
        return;
    }
    //3在子句中匹配非负整数“\d+”和非负整数段“\d+\.\.\d+”
    if (!m_isName)
    {
        fillListIndex(subString);
        err = NoError;
        return;
    }
    //4.对于NameType场景，通配符格式为“[\w]+-[\w]+”，“*-[\w]+”，“[\w]+-*”
    fillListName(subString);
    if (m_subRule.size() == 0)
    {
        m_isAll = true;
    }

    err = NoError;
    return;
}

bool StartRule::match(std::shared_ptr<Core::Node> tarNode)
{


}



void StartRule::fillListIndex(const string& sSub)
{
	std::vector<string> strSeg;
	split(sSub, strSeg, ",");
    const string strSinglePattern = "[\\d]+";
    const string strGroupPattern = "[\\d]+\\.\\.[\\d]+";
    Poco::RegularExpression::MatchVec matches;
    
    for (string sSeg : strSeg)
    {
        Poco::RegularExpression regGroupObj(strGroupPattern);//正则表达式初始化，载入匹配模式
		int matchCount = regGroupObj.match(sSeg, 0, matches);//正则表达式对分词结果进行匹配
        if (1 == matchCount)
        {
            m_subRule.push_back(Poco::trim(matches[0].name));
            continue;
        }

		matches.clear();
        Poco::RegularExpression regSingleObj(strSinglePattern);//正则表达式初始化，载入匹配模式
        matchCount = regSingleObj.match(sSeg, 0, matches);//正则表达式对分词结果进行匹配
        if (1 == matchCount)
        {
            m_subRule.push_back(Poco::trim(matches[0].name));
        }
    }

}

void StartRule::fillListName(const string& sSub)
{
	std::vector<string> strSeg;
	split(sSub, strSeg, ",");
    const string strNamePattern = "[\\w\\*]+-[\\w\\*]+";
    Poco::RegularExpression::MatchVec matches;

    if(0 == strSeg.size())
	{
		return;
	}

    for (string sSeg : strSeg)
    {
		Poco::RegularExpression regNameObj(strNamePattern);
        int matchCount = regNameObj.match(sSeg, 0, matches);;//正则表达式对分词结果进行匹配
        if (1 == matchCount)
        {
            m_subRule.push_back(Poco::trim(matches[0].name));
        }
    }
}

bool StartRule::matchNumber(const string& strRule, int index)
{
	int intUp = 0, intDown = 0, intTemp = 0;
    string strUp, strDown;

    size_t intDot = strRule.find("..");
    if (string::npos == intDot)
    {
        intUp = atoi(strRule.c_str());
        if (intUp == index)
        {
            return true;
        }
        return false;
    }
    strDown = strRule.substr(0, intDot + 1) + "..";
	//strUp = Poco::replaceInPlace(strRule, strDown, string(""), 0);

    intDown = atoi(strDown.c_str());
    intUp = atoi(strUp.c_str());
    if (intUp < intDown)
    {
        intTemp = intUp;
        intUp = intDown;
        intDown = intTemp;
    }
    if (intDown <= index && index <= intUp)
    {
        return true;
    }
    return false;
}

bool StartRule::matchNameType(const string& strRule, const string& sName, const string& sType)
{
	std::vector<string> strSeg;
	split(strRule, strSeg, "-");
    bool bolName = false, bolType = false;

    if (0 == strSeg.size())
    {
        return false;
    }

    bolName = bolType = false;
    if (Poco::trim(strSeg[0]) == "*")
    {
        bolName = true;
    }
    else
    {
        if (strSeg[0] == sName)
        {
            bolName = true;
        }
    }
    if (Poco::trim(strSeg[1]) == "*")
    {
        bolType = true;
    }
    else
    {
        if (strSeg[1] == sType)
        {
            bolType = true;
        }
    }
    if (bolName == true && bolType == true)
    {
        return true;
    }

    return false;
}






