#pragma once


#include <string>



#include "../Common/TypeDef.h"
#include "Node.h"



using std::string;



namespace GraphDB {
namespace Core
{
		
	
	
	
class Edge
{
public:
	//连边类Edge构造函数
	Edge(int intMaxEdgeNum, const string& newType, const string& value = "1");
	//连边类Edge构造函数
	Edge(int intMaxEdgeNum, Poco::XML::Element& xNode);
	virtual ~Edge();
			
	//将连边数据保存为xml格式
	virtual Poco::XML::Element* toXML(Poco::XML::Document& doc);
	bool equal(Edge& other);



	int getEdgeNum(){ return m_iEdgeNum; }

	std::shared_ptr<Node> getStartNode(){ return m_nodeStart; }
	void setStartNode(std::shared_ptr<Node> node){ m_nodeStart = node; }

	std::shared_ptr<Node> getEndNode(){ return m_nodeEnd; }
	void setEndNode(std::shared_ptr<Node> node){ m_nodeEnd = node; }

	string getEdgeType(){ return m_strEdgeType; }
	void setEdgeType(const string& edgeType){ m_strEdgeType = edgeType; }

	string getEdgeValue(){ return m_strEdgeValue; }
	void setEdgeValue(const string& edgeValue){ m_strEdgeValue = edgeValue; }

protected:
	//工具函数，从xml节点中读取某个标签的InnerText
	string getText(Poco::XML::Element& curNode, const string& sLabel);
	
	
			
private:
	int m_iEdgeNum;
	std::shared_ptr<Node> m_nodeStart;//连边起点
	std::shared_ptr<Node> m_nodeEnd;//连边终点
	string m_strEdgeType;//连边类型
	string m_strEdgeValue;//连边取值
};
		
		
		
		
		
}
}












