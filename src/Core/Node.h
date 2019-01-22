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
	//���ڵ����ݱ���Ϊxml��ʽ
	virtual Poco::XML::Element* toXML(Poco::XML::Document& doc);
	//�����Զ������Զ�
	void addProperty(const string& sProperities, ModifyOperation opt = Append);
	//�������Զ�
	std::shared_ptr<NodeProperty> buildProperty(const string& sProperty);
	//������ԶԵ�key�Ƿ��Ѿ�����
    std::shared_ptr<NodeProperty> getProperty(const string& sKey);
	//�޸�ָ��key������
    void modifyProperty(const NodeProperty& sProperty);
	//ɾ�����Զ�
    void removeProperty(const string& sProperities);
	//��������
    bool addEdge(std::shared_ptr<Edge> newEdge);
	//Inbound��ע��
    bool registerInbound(std::shared_ptr<Edge> newEdge);
	//ȥ������
    bool removeEdge(std::shared_ptr<Edge> curEdge);
	//�����������,���ر�����ı��б�
    std::shared_ptr<EdgeList> clearEdge();
	//Inboundע��
    bool unRegisterInbound(std::shared_ptr<Edge> curEdge);
	//����OutBound�Ƿ������Ŀ��ڵ�������
    bool outBoundContainsEdge(Edge& newEdge);
	//����InBound�Ƿ������Ŀ��ڵ�������
    bool inBoundContainsEdge(Edge& newEdge);
	string fieldOutputAll();
	string fieldOutput(const std::vector<string>& labels);
	string dataOutputAll();
	string dataOutput(const std::vector<string>& labels);
	string dataOutput();
	//��������
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
	//���ߺ�������xml�ڵ��ж�ȡĳ����ǩ��InnerText
	string getText(Poco::XML::Element* curNode, const string& sLabel);
	bool isExistEdgeInList(EdgeList& edgeList, Edge& newEdge);

private:
	int m_iNodeNum;//�ڵ���
    string m_strNodeName;
    string m_strNodeType;
	
    AttributeList m_attributeList;	
    EdgeList m_outLinkList;//���� ʹ���ֵ�ṹ��ţ�Ŀ��ڵ�ţ����߶���
    EdgeList m_inLinkList;
    int m_iSaveIndex;
};



















}
}



