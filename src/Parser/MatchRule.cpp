#include "MatchRule.h"
#include <Poco/RegularExpression.h>
#include <Poco/String.h>


using namespace GraphDB::Parser;


MatchRule::MatchRule(const string& sSub, ErrorCode& err)
{
	const string strConditionPattern = "\\[[\\s\\S][^\\[\\]]+\\]";
    const string strLabelPattern = ":[\\w]+";
    const string strLimitPattern = "[\\d]+\\.\\.[\\d]+";
    Poco::RegularExpression::MatchVec matches;
    string strCondition;

	//确定方向<在头为in，>在尾为out
	if(string::npos != sSub.find("<"))
    {
        m_strDirection = "IN";
    }
	else if(string::npos != sSub.find(">"))
    {
        m_strDirection = "OUT";
    }
    else
    {
        err = MatchSegInvalid;
        return;
    }

	Poco::RegularExpression regConditionObj(strConditionPattern);//正则表达式初始化，载入匹配模式
	int matchCount = regConditionObj.match(sSub, 0, matches);//正则表达式对分词结果进行匹配
    if (1 != matchCount)
    {
        err = MatchSegInvalid;
        return;
    }
    strCondition = matches[0].name;
    //提取"\d+\.\.\d+"分别设置上下限
	matches.clear();
	Poco::RegularExpression regLimitObj(strLimitPattern);//正则表达式初始化，载入匹配模式
	matchCount = regLimitObj.match(sSub, 0, matches);//正则表达式对分词结果进行匹配
    if (matchCount > 0)
    {
        m_bNumLimit = true;
        buildMatchLimit(matches[0].name);
                
    }
    //查找*，找到则label
    m_isAll = false;
    if (string::npos != strCondition.find(":*"))
    {
        m_isAll = true;
    }
	//提取":[\w]+",去除：就是规则标签，加入List
	matches.clear();
	Poco::RegularExpression regLabelObj(strLabelPattern);//正则表达式初始化，载入匹配模式
	matchCount = regLimitObj.match(sSub, 0, matches);//正则表达式对分词结果进行匹配
    if ( 0 == matchCount)
    {
        m_isAll = true;
    }
    else
    {
        for (auto match : matches)//遍历匹配列表
        {
            m_labels.push_back(Poco::replaceInPlace(match.name, ":",""));
        }
    }

    err = NoError;

    return;
}


bool MatchRule::matchType(const string& sType)
{
	if (m_isAll)
	{//没有设置匹配类型，则直接返回true
		return true;
	}
	for(auto strLabel : m_labels)
	{//只要有一个相同为True
		if(strLabel == sType)
		{
			return true;
		}
	}
	//完全没有匹配上的返回false
	return false;
}

bool MatchRule::matchCount(int iNodes)
{
	//默认>0为true
    if (iNodes <= 0)
    {
        return false;
    }
    if (!m_bNumLimit)
    {
        return true;
    }
    if (m_downlimit <= iNodes && iNodes <= m_uplimit)
    {
        return true;
    }
	return false;
}



void MatchRule::buildMatchLimit(const string& strLimit)
{
	const string strSinglePattern = "[\\d]+";
    Poco::RegularExpression::MatchVec matches;
    int tmp;

	Poco::RegularExpression regSingleObj(strSinglePattern);//正则表达式初始化，载入匹配模式
	int matchCount = regSingleObj.match(strLimit, 0, matches);//正则表达式对分词结果进行匹配
    if (2 != matchCount)
    {
        m_bNumLimit = false;
    }
    m_uplimit = atoi(matches[0].name.c_str());
    m_downlimit = atoi(matches[1].name.c_str());
    if (m_uplimit < m_downlimit)
    {
        tmp = m_uplimit;
        m_uplimit = m_downlimit;
        m_downlimit = tmp;
    }
}





