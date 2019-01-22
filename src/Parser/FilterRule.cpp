#include "FilterRule.h"

#include <Poco/RegularExpression.h>


using namespace GraphDB::Parser;





FilterRule::FilterRule(const string& strSub, ErrorCode& err)
{
	const string strWordPattern = "[\\w]+";
    const string strOpPattern = "(==|!=|>=|>|<=|<)";
    Poco::RegularExpression::MatchVec matches;

	Poco::RegularExpression regWordObj(strWordPattern);//正则表达式初始化，载入匹配模式
	int matchCount = regWordObj.match(strSub, 0, matches);//正则表达式对分词结果进行匹配
    if (3 != matchCount)
    {
        err = WhereSegInvalid;
        return;
    }
    m_strName = Poco::trim(matches[0].name);
    m_strProperty = Poco::trim(matches[1].name);
    m_strValue = Poco::trim(matches[2].name);

	Poco::RegularExpression regOpObj(strOpPattern);//正则表达式初始化，载入匹配模式
	matchCount = regOpObj.match(strSub, 0, matches);//正则表达式对分词结果进行匹配
    if (1 != matchCount)
    {
        err = WhereSegInvalid;
        return;
    }
    m_strOp = Poco::trim(matches[0].name);
    err = NoError;

    return;
}

bool FilterRule::filtrate(std::shared_ptr<Core::Node> curNode)
{
	double dubRuleValue, dubNpValue;

	if ("Name" == m_strProperty)
	{
		if(m_strOp == "==")
		{
			if (curNode->getNodeName() == m_strValue)
			{
				return true;
			}
		}
		else if(m_strOp == "!=")
		{
			if (m_strValue != curNode->getNodeName())
			{
				return true;
			}
		}
		
		return false;
	}
	if ("Type" == m_strProperty)
	{
		if(m_strOp == "==")
		{
			if (m_strValue == curNode->getNodeType())
			{
				return true;
			}
		}
		else if(m_strOp == "!=")
		{
			if (m_strValue != curNode->getNodeType())
			{
				return true;
			}
		}

		return false;
	}
	for (auto np : curNode->getAttributeList())
	{
		if (m_strProperty != np->getKey())
		{
			continue;
		}
		if (m_strOp == "==" || m_strOp == "!=")
		{
			if(m_strOp == "==")
			{
				if (m_strValue == np->getValue())
				{
					return true;
				}
			}
			else if(m_strOp == "!=")
			{
				if (m_strValue != np->getValue())
				{
					return true;
				}
			}

			continue;
		}

		double dubRuleValue = atof(m_strValue.c_str());
		double dubNpValue = atof(np->getValue().c_str());
		if (0.0000001 > dubRuleValue || 0.0000001 > dubNpValue)
		{
			return false;
		}		
		
		if(m_strOp == ">=")
		{
			if (dubNpValue >= dubRuleValue)
			{
				return true;
			}
		}
		else if(m_strOp == ">")
		{
			if (dubNpValue > dubRuleValue)
			{
				return true;
			}
		}
		else if(m_strOp == "<=")
		{
			if (dubNpValue <= dubRuleValue)
			{
				return true;
			}
		}
		else if(m_strOp == "<")
		{
			if (dubNpValue < dubRuleValue)
			{
				return true;
			}
		}		
	}

	return false;
}




