#pragma once



#include <string>
#include <vector>


#include "../Common/TypeDef.h"
#include "../Core/Graph.h"
#include "StartRule.h"

using std::string;




namespace GraphDB{
namespace Parser{


enum CypherOperation
{
    CypherOperation_Invalid = 0,
    CypherOperation_Query = 1,
    CypherOperation_Create = 2,
    CypherOperation_Delete = 3,
    CypherOperation_Modify = 4,
};


class CypherParser
{
public:
	string queryExecute(std::shared_ptr<Core::Graph> graph, const string& sCypher, ErrorCode err);

protected:
	void cypherInit(const string& sCypher, ErrorCode err);
	//START node(*) MATCH a-[:Rule]->b<-[:Connect 5..5]-c WHERE * RETURN b.Name
    //�ж�Cypher���Ĳ�����ֻ���ؼ���˳�򣬲������Ӿ�����﷨
    CypherOperation operationJudge(const string& sCypher);
	//��ѯ�����ṹ���ʼ��
    void queryInit(ErrorCode err);
	//��������������м�����б��ʼ��
    void matchInit(const string& strSub, ErrorCode err);
	std::vector<string> splitSub(const string& sMatch, std::vector<string>& sNode);
	//�������������ʼ��
    void filterInit(const string& strSub);
	//����ֵ��ʼ��
    void returnInit(const string& strSub, ErrorCode err);
	//���������ṹ���ʼ��
    void createInit(ErrorCode err);
	//��ʼ�ڵ���ѡ����
    void selectStarter(std::shared_ptr<Core::Graph> graph, ErrorCode err);
	//��ѯ����
    void query();
	//���˺���
    void filtrateResult(std::shared_ptr<Core::Graph> graph);
	//����������
    string resultOutput(std::shared_ptr<Core::Graph> graph);
	//��֯������
    string dataFormat(Core::NodeList& resList, std::shared_ptr<MidResult> mr);
	//��ͼ�л�ȡ�������ָ���Ľڵ�
    std::shared_ptr<Core::NodeList> getTreeNode(std::shared_ptr<Core::Graph> graph, int iLevel, bool bDistinct = false);
	std::vector<int> clearRepeat(std::vector<int>& IndexList);
	//��ȡ�������ĳһ������нڵ�
    void findNodeByLevel(TreeNode& ctn, int iLevel, std::vector<int>& IndexList);
	//�Ƴ����ڵ�
    void removeTreeNode(const string& strText, int iLevel);
	//���ݱ�Ų������нڵ�
    std::shared_ptr<TreeNode> findNodeByText(std::shared_ptr<TreeNode> ctn, const string& strText, int curLevel, int iLevel);


private:
	const bool m_bDistinct = true;
    string m_strCypher;
    CypherOperation m_op;
    MidResultList m_midRes;
    std::shared_ptr<StartRule> m_sRule;
    MatchRuleList m_mRule;
    FilterRuleList m_fRule;
    Core::NodeList m_Starter;
    TreeNodeList m_ResultTree;
};









}
}

