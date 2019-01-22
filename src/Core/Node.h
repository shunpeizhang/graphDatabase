#pragma once



#include <string>
#include <vector>



#include "Edge.h"
#include "NodeProperty.h"
#include "../Common/TypeDef.h"




using std::string;


namespace GraphDB {
namespace Core
{


class Node
{
public:
	Node(int intMaxNodeNum, const string& newName, const string& newType, const string& sProperities = "");
	Node(int intMaxNodeNum, Node& oriNode);
	Node(int intMaxNodeNum, Poco::XML::Element& xNode);

	bool equal(std::shared_ptr<Node> other);
	//将节点数据保存为xml格式
	virtual Poco::XML::Element* toXML(Poco::XML::Document& doc);
	//增加自定义属性对
	void addProperty(const string& sProperities, ModifyOperation opt = Append);
	//构造属性对
	std::shared_ptr<NodeProperty> buildProperty(const string& sProperty);
	//检查属性对的key是否已经存在
    std::shared_ptr<NodeProperty> getProperty(const string& sKey);
	//修改指定key的属性
    void modifyProperty(const NodeProperty& sProperty);
	//删除属性对
    void removeProperty(const string& sProperities);
	//增加连边
    bool addEdge(std::shared_ptr<Edge> newEdge);
	//Inbound边注册
    bool registerInbound(std::shared_ptr<Edge> newEdge);
	//去除连边
    bool removeEdge(std::shared_ptr<Edge> curEdge);
	//清除所有连边,返回被清除的边列表
    std::shared_ptr<EdgeList> clearEdge();
	//Inbound注销
    bool unRegisterInbound(std::shared_ptr<Edge> curEdge);
	//返回OutBound是否包含和目标节点间的连边
    bool outBoundContainsEdge(Edge& newEdge);
	//返回InBound是否包含和目标节点间的连边
    bool inBoundContainsEdge(Edge& newEdge);
	string fieldOutputAll();
	string fieldOutput(const std::vector<string>& labels);
	string dataOutputAll();
	string dataOutput(const std::vector<string>& labels);
	string dataOutput();
	//查找连边
    std::shared_ptr<Edge> getEdge(const string& sName, const string& sType, const string& opt);
	std::shared_ptr<TreeNode> search(Parser::MatchRuleList& mRule, int level);


	int getNodeNum(){ return m_iNodeNum; }
	string getNodeName(){ return m_strNodeName; }
	string getNodeType(){ return m_strNodeType; }
	const AttributeList& getAttributeList(){ return m_attributeList; }
	int getInDegree(){ return m_inLinkList.size(); }
	int getOutDegree(){ return m_outLinkList.size(); }
	const EdgeList& getOutBound(){ return m_outLinkList; }
	const EdgeList& getInBound(){ return m_inLinkList; }
	void setSaveIndex(int index){ m_iSaveIndex = index; }
	int getSaveIndex(){ return m_iSaveIndex; }

protected:
	//工具函数，从xml节点中读取某个标签的InnerText
	string getText(Poco::XML::Element* curNode, const string& sLabel);
	bool isExistEdgeInList(EdgeList& edgeList, Edge& newEdge);

private:
	int m_iNodeNum;//节点编号
    string m_strNodeName;
    string m_strNodeType;
	
    AttributeList m_attributeList;	
    EdgeList m_outLinkList;//连边 使用字典结构存放（目标节点号，连边对象）
    EdgeList m_inLinkList;
    int m_iSaveIndex;
};



















}
}



