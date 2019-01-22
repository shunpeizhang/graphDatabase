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
	//������Edge���캯��
	Edge(int intMaxEdgeNum, const string& newType, const string& value = "1");
	//������Edge���캯��
	Edge(int intMaxEdgeNum, Poco::XML::Element& xNode);
	virtual ~Edge();
			
	//���������ݱ���Ϊxml��ʽ
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
	//���ߺ�������xml�ڵ��ж�ȡĳ����ǩ��InnerText
	string getText(Poco::XML::Element& curNode, const string& sLabel);
	
	
			
private:
	int m_iEdgeNum;
	std::shared_ptr<Node> m_nodeStart;//�������
	std::shared_ptr<Node> m_nodeEnd;//�����յ�
	string m_strEdgeType;//��������
	string m_strEdgeValue;//����ȡֵ
};
		
		
		
		
		
}
}












