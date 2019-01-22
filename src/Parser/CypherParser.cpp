#include "CypherParser.h"
#include "MidResult.h"
#include "MatchRule.h"
#include "FilterRule.h"
#include "../Common/PublicFun.h"

#include <Poco/RegularExpression.h>







using namespace GraphDB::Parser;
using namespace GraphDB::Core;


string CypherParser::queryExecute(std::shared_ptr<Graph> graph, const string& sCypher, ErrorCode err)
{
	string strResult;
    cypherInit(sCypher, err);
    if (err != NoError)
    {
        return "";
    }
    //��ͼ��ѡȡ�ڵ㣬startRule
    selectStarter(graph, err);
    if (err != NoError)
    {
        return "";
    }
    //����matchRule���ң����������ݴ����¼��
    query();
    //���˽����
    filtrateResult(graph);
    //���ؽ��������midResult�еı��
    strResult = resultOutput(graph);

    return strResult;
}


void CypherParser::cypherInit(const string& sCypher, ErrorCode err)
{
	m_strCypher = sCypher;
    m_op = operationJudge(m_strCypher);
    switch (m_op)
    {
        case CypherOperation_Query:
            queryInit(err);
            break;
        case CypherOperation_Create:
            createInit(err);
            break;
        default:
            err = CypherInvalid;
            return;
    }
    if (err != NoError)
    {
        return;
    }
    err = NoError;
    return;
}

//START node(*) MATCH a-[:Rule]->b<-[:Connect 5..5]-c WHERE * RETURN b.Name
//�ж�Cypher���Ĳ�����ֻ���ؼ���˳�򣬲������Ӿ�����﷨
CypherOperation CypherParser::operationJudge(const string& sCypher)
{
	const string strQueryPattern = "START[\\s\\S]*MATCH[\\s\\S]*WHERE[\\s\\S]*RETURN[\\s\\S]*";  //ƥ��Ŀ��"����+ȡֵ"���
    const string strCreatePattern = "CREATE[\\s\\S]*";  //ƥ��Ŀ��"����+ȡֵ"���
    Poco::RegularExpression::MatchVec matches;

	Poco::RegularExpression regQueryObj(strQueryPattern);//������ʽ��ʼ��������ƥ��ģʽ
	int matchCount = regQueryObj.match(sCypher, 0, matches);//������ʽ�Էִʽ������ƥ��
    if (matchCount > 0)
    {
        return CypherOperation_Query;
    }

	Poco::RegularExpression regCreateObj(strCreatePattern);//������ʽ��ʼ��������ƥ��ģʽ
	matchCount = regCreateObj.match(sCypher, 0, matches);//������ʽ�Էִʽ������ƥ��
    if (matchCount > 0)
    {
        return CypherOperation_Create;
    }
    return CypherOperation_Invalid;
}

//��ѯ�����ṹ���ʼ��
void CypherParser::queryInit(ErrorCode err)
{
	const string strStartPattern = "START[\\s\\S]*MATCH";
    const string strMatchPattern = "MATCH[\\s\\S]*WHERE";
    const string strWherePattern = "WHERE[\\s\\S]*RETURN";
    const string strReturnPattern = "RETURN[\\s\\S]*";  
    Poco::RegularExpression::MatchVec matches;
    string subString;

    //START�Ӿ�
	Poco::RegularExpression regStartObj(strStartPattern);//������ʽ��ʼ��������ƥ��ģʽ
	int matchCount = regStartObj.match(m_strCypher, 0, matches);//������ʽ�Էִʽ������ƥ��
    if (matchCount == 0)
    {
        err = StartSegInvalid;
        return;
    }
    subString = matches[0].name;
    subString = Poco::replaceInPlace(subString, "START", "");
    subString = Poco::replaceInPlace(subString, "MATCH", "");

	m_sRule = std::make_shared<StartRule>(Poco::trim(subString), err);
    if (err != NoError)
    {
        return;
    }
    //MATCH�Ӿ�
	Poco::RegularExpression regMatchObj(strMatchPattern);//������ʽ��ʼ��������ƥ��ģʽ
	matchCount = regMatchObj.match(m_strCypher, 0, matches);//������ʽ�Էִʽ������ƥ��
    if (matchCount == 0)
    {
        err = MatchSegInvalid;
        return;
    }
    subString = matches[0].name;
    subString = Poco::replaceInPlace(subString, "MATCH", "");
    subString = Poco::replaceInPlace(subString, "WHERE", "");
    matchInit(Poco::trim(subString), err);
    if (err != NoError)
    {
        return;
    }
    //WHERE�Ӿ�
	Poco::RegularExpression regWhereObj(strWherePattern);//������ʽ��ʼ��������ƥ��ģʽ
	matchCount = regWhereObj.match(m_strCypher, 0, matches);//������ʽ�Էִʽ������ƥ��
    if (matchCount == 0)
    {
        err = WhereSegInvalid;
        return;
    }
    subString = matches[0].name;
    subString = Poco::replaceInPlace(subString, "WHERE", "");
    subString = Poco::replaceInPlace(subString, "RETURN", "");
    filterInit(Poco::trim(subString));
            
    //RETURN�Ӿ�
	Poco::RegularExpression regReturnObj(strReturnPattern);//������ʽ��ʼ��������ƥ��ģʽ
	matchCount = regReturnObj.match(m_strCypher, 0, matches);//������ʽ�Էִʽ������ƥ��
    if (matchCount == 0)
    {
        err = ReturnSegInvalid;
        return;
    }
    subString = matches[0].name;
    subString = Poco::replaceInPlace(subString, "RETURN", "");
    returnInit(Poco::trim(subString), err);
    if (err != NoError)
    {
        return;
    }
    err = NoError;
    return;
}

//��������������м�����б��ʼ��
void CypherParser::matchInit(const string& strSub, ErrorCode err)
{
	//(Kingdom)-[:Rule]->(District)<-[:Connect 5..5]-(Neibhour)
    const string strNodePattern = "\\([\\w]+\\)";
    std::vector<string> strNode;
    std::vector<string> strEdge;
    std::shared_ptr<MatchRule> curRule;
    Poco::RegularExpression::MatchVec matches;
    string subString;
    int intLevel = 0;

	Poco::RegularExpression regNodeObj(strNodePattern);//������ʽ��ʼ��������ƥ��ģʽ
	int matchCount = regNodeObj.match(strSub, 0, matches);//������ʽ�Էִʽ������ƥ��
    if (matchCount < 2)
    {
        err = MatchSegInvalid;
        return;
    }
    for (auto match : matches)//����ƥ���б�
    {
        subString = match.name;
        m_midRes.push_back(std::make_shared<MidResult>(Poco::replaceInPlace(Poco::replaceInPlace(subString, "(",""), ")", ""), intLevel));
        strNode.push_back(subString);
        intLevel++;
    }

    strEdge = splitSub(strSub, strNode);
    for (string sSub : strEdge)//����ƥ���б�
    {
		curRule = std::make_shared<MatchRule>(Poco::trim(sSub), err);
        if (err != NoError)
        {
            err = NoError;
            continue;
        }
        m_mRule.push_back(curRule);
    }
    if (m_mRule.size() != strNode.size() - 1)
    {//�������������ܼ������-1
        err = MatchSegInvalid;
        return;
    }
    err = NoError;
    return;
}

std::vector<string> CypherParser::splitSub(const string& sMatch, std::vector<string>& sNode)
{
	std::vector<string> strEdge;
    string strStart, strEnd;
    int iStart, iEnd;

    for (int i = 0; i < sNode.size() - 1; i++)
    {
        strStart = sNode[i];
        strEnd = sNode[i+1];
		size_t pos = sMatch.find(strStart);
		if(string::npos == pos)
		{
			continue;
		}

        iStart = pos+strStart.size();
        iEnd = sMatch.find(strEnd) - iStart;
        strEdge.push_back(Poco::trim(sMatch.substr(iStart, iEnd - iStart)));
    }
    return strEdge;
}

//�������������ʼ��
void CypherParser::filterInit(const string& strSub)
{
	const string strConditionPattern = "[\\w]+\\.[\\w]+[\\s]*(==|!=|>=|>|<=|<)[\\s]*[\\w]+";
    Poco::RegularExpression::MatchVec matches;
    ErrorCode err = NoError;

    if (strSub == "*")
    {
        return;
    }

	Poco::RegularExpression regConditionObj(strConditionPattern);//������ʽ��ʼ��������ƥ��ģʽ
	int matchCount = regConditionObj.match(strSub, 0, matches);//������ʽ�Էִʽ������ƥ��
    if (matchCount < 1)
    {
        return;
    }
    for (auto match : matches)//����ƥ���б�
    {
		std::shared_ptr<FilterRule> curRule = std::make_shared<FilterRule>(Poco::trim(match.name), err);
        if (err != NoError)
        {
            continue;
        }
        m_fRule.push_back(curRule);
    }
    return;
}

//����ֵ��ʼ��
void CypherParser::returnInit(const string& strSub, ErrorCode err)
{
	const string strConditionPattern = "[\\w]+\\.[*\\w]+";
    Poco::RegularExpression::MatchVec matches;
    std::vector<string> strSeg;

    if (strSub == "*")
    {
        for (auto mr : m_midRes)
        {
            mr->setResult();
            mr->setAll();
        }
        err = NoError;
        return;
    }
	
	Poco::RegularExpression regConditionObj(strConditionPattern);//������ʽ��ʼ��������ƥ��ģʽ
	int matchCount = regConditionObj.match(strSub, 0, matches);//������ʽ�Էִʽ������ƥ��
    if (matchCount < 1)
    {
        err = ReturnSegInvalid;
        return;
    }
    for (auto match : matches)//����ƥ���б�
    {
		strSeg.clear();
		split(match.name, strSeg, ".");
        if (strSeg.size() != 2)
        {
            continue;
        }
        for (auto mr : m_midRes)
        {
            if (mr->getName() == Poco::trim(strSeg[0]))
            {
                mr->setResult();
                if (Poco::trim(strSeg[1]) == "*")
                {
                    mr->setAll();
                    continue;
                }
                mr->addProperty(Poco::trim(strSeg[1]));
            }
        }
    }
    err = NoError;
    return;
}

//���������ṹ���ʼ��
void CypherParser::createInit(ErrorCode err)
{

}

//��ʼ�ڵ���ѡ����
void CypherParser::selectStarter(std::shared_ptr<Graph> graph, ErrorCode err)
{
    for (auto curNode : graph->getNodeList())
    {
        if (m_sRule->match(curNode) == true)
        {
            m_Starter.push_back(curNode);
        }
    }
    if (m_Starter.size() == 0)
    {
        err = NoStartNode;
        return;
    }
    err = NoError;
    return; 
}

//��ѯ����
void CypherParser::query()
{
	std::shared_ptr<TreeNode> newTN;

    for (auto curNode : m_Starter)
    {
        newTN = curNode->search(m_mRule, 0);
        if (NULL != newTN.get())
        {
            m_ResultTree.push_back(newTN);
        }
    }
}

//���˺���
void CypherParser::filtrateResult(std::shared_ptr<Graph> graph)
{
	char buf[128];
	std::shared_ptr<Core::NodeList> resList;
    for (auto fr : m_fRule)
    {
        for (auto mr : m_midRes)
        {
            if (mr->getName() != fr->getName())
            {
                continue;
            }
            resList = getTreeNode(graph, mr->getLevel());
			if(NULL == resList.get())
			{
				continue;
			}

            for (auto curNode : *resList)
            {
                if (fr->filtrate(curNode) == true)
                {
                    continue;
                }
				snprintf(buf, sizeof(buf), "%d", curNode->getNodeNum());
                removeTreeNode(buf, mr->getLevel());
            }
        }
    }
}

//����������
string CypherParser::resultOutput(std::shared_ptr<Graph> graph)
{
	string strResult = "";
    for(auto mr : m_midRes)
    {
        if (mr->getIsFinalResult() == false)
        {
            continue;
        }
        strResult += dataFormat(*getTreeNode(graph, mr->getLevel(), m_bDistinct), mr);
    }

    return strResult;
}

//��֯������
string CypherParser::dataFormat(NodeList& resList, std::shared_ptr<MidResult> mr)
{
	string strResult = "";

    strResult += mr->getName()+":\n";
    if(resList.size() < 1)
    {
        strResult += "No Result\n\n";
        return strResult;
    }
    if (mr->getIsAll() == true)
    {
        strResult += resList[0]->fieldOutputAll();
    }
    else
    {
        strResult += resList[0]->fieldOutput(mr->getProperty());
    }
    for (auto curNode : resList)
    {
        if (mr->getIsAll() == true)
        {
            strResult += curNode->dataOutputAll();
        }
        else
        {
            strResult += curNode->dataOutput(mr->getProperty());
        }
    }
    strResult += "\n";

    return strResult;
}

//��ͼ�л�ȡ�������ָ���Ľڵ�
std::shared_ptr<NodeList> CypherParser::getTreeNode(std::shared_ptr<Graph> graph, int iLevel, bool bDistinct)
{
	std::shared_ptr<NodeList> resList = std::make_shared<NodeList>();
    std::vector<int> indexList;

    for(auto ctn : m_ResultTree)
    {
        findNodeByLevel(*ctn, iLevel, indexList);
    }
    if (m_bDistinct == true)
    {
        indexList = clearRepeat(indexList);
    }
    for (int iNum : indexList)
    {
        resList->push_back(graph->getNodeByIndex(iNum));
    }

    return resList;
}

std::vector<int> CypherParser::clearRepeat(std::vector<int>& IndexList)
{
	std::vector<int> newList;

    for (int iNum : IndexList)
    {
        if (newList[iNum] < 0)
        {
            newList.push_back(iNum);
        }
    }
    return newList;
}

//��ȡ�������ĳһ������нڵ�
void CypherParser::findNodeByLevel(TreeNode& ctn, int iLevel, std::vector<int>& IndexList)
{
	if (ctn.getLevel() == iLevel)
    {
        IndexList.push_back(atoi(ctn.getText().c_str()));
        return;
    }
    for(auto chtn : ctn.getNodes())
    {
        findNodeByLevel(*chtn, iLevel, IndexList);
    }
    return;
}

//�Ƴ����ڵ�
void CypherParser::removeTreeNode(const string& strText, int iLevel)
{
	std::shared_ptr<TreeNode> tarNode, parNode;
    for (auto ctn : m_ResultTree)
    {
        tarNode = findNodeByText(ctn, strText, 0, iLevel);
        if (tarNode.get() != NULL)
        {
            break;
        }
    }
    if (tarNode.get() == NULL)
    {
        return;
    }
    do
    {
        if (tarNode->getParent().get() == NULL)
        {
            //m_ResultTree.Remove(tarNode);
            return;
        }
        parNode = tarNode->getParent();
        //parNode.Nodes.Remove(tarNode);
        tarNode = parNode;
    } while (parNode->getNodes().size() == 0);
    return;
}

//���ݱ�Ų������нڵ�
std::shared_ptr<TreeNode> CypherParser::findNodeByText(std::shared_ptr<TreeNode> ctn, const string& strText, int curLevel, int iLevel)
{
	std::shared_ptr<TreeNode> tarNode;
    if (curLevel == iLevel)
    {
        if (ctn->getText() == strText)
        {
            return ctn;
        }
    }
    for (auto chtn : ctn->getNodes())
    {
        tarNode = findNodeByText(chtn, strText, curLevel+1, iLevel);
        if (tarNode.get() != NULL)
        {
            return tarNode;
        }
    }
    return NULL;
}




