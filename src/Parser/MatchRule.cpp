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

	//ȷ������<��ͷΪin��>��βΪout
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

	Poco::RegularExpression regConditionObj(strConditionPattern);//������ʽ��ʼ��������ƥ��ģʽ
	int matchCount = regConditionObj.match(sSub, 0, matches);//������ʽ�Էִʽ������ƥ��
    if (1 != matchCount)
    {
        err = MatchSegInvalid;
        return;
    }
    strCondition = matches[0].name;
    //��ȡ"\d+\.\.\d+"�ֱ�����������
	matches.clear();
	Poco::RegularExpression regLimitObj(strLimitPattern);//������ʽ��ʼ��������ƥ��ģʽ
	matchCount = regLimitObj.match(sSub, 0, matches);//������ʽ�Էִʽ������ƥ��
    if (matchCount > 0)
    {
        m_bNumLimit = true;
        buildMatchLimit(matches[0].name);
                
    }
    //����*���ҵ���label
    m_isAll = false;
    if (string::npos != strCondition.find(":*"))
    {
        m_isAll = true;
    }
	//��ȡ":[\w]+",ȥ�������ǹ����ǩ������List
	matches.clear();
	Poco::RegularExpression regLabelObj(strLabelPattern);//������ʽ��ʼ��������ƥ��ģʽ
	matchCount = regLimitObj.match(sSub, 0, matches);//������ʽ�Էִʽ������ƥ��
    if ( 0 == matchCount)
    {
        m_isAll = true;
    }
    else
    {
        for (auto match : matches)//����ƥ���б�
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
	{//û������ƥ�����ͣ���ֱ�ӷ���true
		return true;
	}
	for(auto strLabel : m_labels)
	{//ֻҪ��һ����ͬΪTrue
		if(strLabel == sType)
		{
			return true;
		}
	}
	//��ȫû��ƥ���ϵķ���false
	return false;
}

bool MatchRule::matchCount(int iNodes)
{
	//Ĭ��>0Ϊtrue
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

	Poco::RegularExpression regSingleObj(strSinglePattern);//������ʽ��ʼ��������ƥ��ģʽ
	int matchCount = regSingleObj.match(strLimit, 0, matches);//������ʽ�Էִʽ������ƥ��
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





