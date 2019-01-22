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
    //判断Cypher语句的操作，只检查关键字顺序，不检查各子句具体语法
    CypherOperation operationJudge(const string& sCypher);
	//查询操作结构体初始化
    void queryInit(ErrorCode err);
	//查找条件规则和中间变量列表初始化
    void matchInit(const string& strSub, ErrorCode err);
	std::vector<string> splitSub(const string& sMatch, std::vector<string>& sNode);
	//结果过滤条件初始化
    void filterInit(const string& strSub);
	//返回值初始化
    void returnInit(const string& strSub, ErrorCode err);
	//创建操作结构体初始化
    void createInit(ErrorCode err);
	//初始节点挑选函数
    void selectStarter(std::shared_ptr<Core::Graph> graph, ErrorCode err);
	//查询函数
    void query();
	//过滤函数
    void filtrateResult(std::shared_ptr<Core::Graph> graph);
	//结果输出函数
    string resultOutput(std::shared_ptr<Core::Graph> graph);
	//组织输出结果
    string dataFormat(Core::NodeList& resList, std::shared_ptr<MidResult> mr);
	//从图中获取结果树中指定的节点
    std::shared_ptr<Core::NodeList> getTreeNode(std::shared_ptr<Core::Graph> graph, int iLevel, bool bDistinct = false);
	std::vector<int> clearRepeat(std::vector<int>& IndexList);
	//获取结果树中某一层的所有节点
    void findNodeByLevel(TreeNode& ctn, int iLevel, std::vector<int>& IndexList);
	//移除树节点
    void removeTreeNode(const string& strText, int iLevel);
	//根据编号查找树中节点
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

