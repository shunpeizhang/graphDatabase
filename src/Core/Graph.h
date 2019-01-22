#pragma once


#include <string>
#include <vector>
#include <vector>


#include "Edge.h"
#include "Node.h"
#include "NodeProperty.h"
#include "../Common/TypeDef.h"





namespace GraphDB {
namespace Core
{
	


class Graph
{	
public:
	Graph();
	Graph(Poco::XML::Document& doc, ErrorCode err);


	//将数据保存为XML文件（接口）
	Poco::XML::Document* toXML(ErrorCode err);
	//查询函数，返回指定索引处的节点
    std::shared_ptr<Node> getNodeByIndex(int index);
	//查询函数，返回节点列表中指定名称和类型的节点
    std::shared_ptr<Node> getNodesByNameAndType(const string& sName, const string& type);
	//查询函数，返回指定名称和类型的节点的索引
    int getIndexByNameAndType(const string& sName, const string& sType);
	//查询函数，返回节点列表中指定类型的所有节点
    std::shared_ptr<NodeList> getNodesOfType(const string& type);
	//查询函数，返回指定名称和类型的节点间的连边
    std::shared_ptr<Edge> getEdgeByNameAndType(const string& sName, const string& sType, const string& eName, const string& eType);
	//加入节点（接口）
    void addNode(const string& sName, const string& sType, ErrorCode err, const string& sProperities = "1");
	//加入节点（接口）
    void addNode(std::shared_ptr<Node> oriNode, ErrorCode err);
	//加入连边（接口）
    void addEdge(const string& sStartName, const string& sStartType,
                    const string& sEndName, const string& sEndType,
                    const string& sType, ErrorCode err, const string& sValue = "1");
	//修改节点内部数据（接口）
    void modifyNode(const string& sName, const string& sType,
                        ModifyOperation opt, const string& sProperities, ErrorCode err);
	//修改连边取值（接口）
    void modifyEdge(const string& sStartName, const string& sStartType,
                        const string& sEndName, const string& sEndType,
                        const string& sType, const string& sValue, ErrorCode err);
	//删除节点（接口）
    void removeNode(const string& sName, const string& sType, ErrorCode err);
	//删除连边（接口）
    void removeEdge(const string& sStartName, const string& sStartType,
                        const string& sEndName, const string& sEndType,
                        const string& sType, ErrorCode err);


	int getNodeNum(){ return m_nodeList.size(); }
	int getEdgeNum(){ return m_edgeList.size(); }
	const NodeList& getNodeList(){ return m_nodeList; }
	const EdgeList& getEdgeList(){ return m_edgeList; }


protected:
	//工具函数，从xml节点中读取某个标签的InnerText
	string getText(Poco::XML::Element* curNode, const string& sLabel);
	//调整节点实际索引(用于保存，和编号不完全相同)
	void adjustNodeIndex();
	//加入节点
    void addNode(std::shared_ptr<Node> newNode);
	//删除节点
    void removeNode(std::shared_ptr<Node> curNode);
	//加入连边
    bool addEdge(std::shared_ptr<Node> curNode, std::shared_ptr<Node> tarNode, std::shared_ptr<Edge> newEdge);
	//移除连边
    bool removeEdge(std::shared_ptr<Node> curNode, std::shared_ptr<Node> tarNode);
	//删除所有被解除绑定的连边
    bool clearUnusedEdge(EdgeList& UnusedList);
	//查找两点之间指定Type的连边
    std::shared_ptr<Edge> getEdgeByType(std::shared_ptr<Node> start, std::shared_ptr<Node> end, const string& sType = "");

private:
	NodeList m_nodeList;
    EdgeList m_edgeList;
    int m_iMaxNodeNum;
    int m_iMaxEdgeNum;
};











}
}


