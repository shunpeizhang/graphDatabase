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


	//�����ݱ���ΪXML�ļ����ӿڣ�
	Poco::XML::Document* toXML(ErrorCode err);
	//��ѯ����������ָ���������Ľڵ�
    std::shared_ptr<Node> getNodeByIndex(int index);
	//��ѯ���������ؽڵ��б���ָ�����ƺ����͵Ľڵ�
    std::shared_ptr<Node> getNodesByNameAndType(const string& sName, const string& type);
	//��ѯ����������ָ�����ƺ����͵Ľڵ������
    int getIndexByNameAndType(const string& sName, const string& sType);
	//��ѯ���������ؽڵ��б���ָ�����͵����нڵ�
    std::shared_ptr<NodeList> getNodesOfType(const string& type);
	//��ѯ����������ָ�����ƺ����͵Ľڵ�������
    std::shared_ptr<Edge> getEdgeByNameAndType(const string& sName, const string& sType, const string& eName, const string& eType);
	//����ڵ㣨�ӿڣ�
    void addNode(const string& sName, const string& sType, ErrorCode err, const string& sProperities = "1");
	//����ڵ㣨�ӿڣ�
    void addNode(std::shared_ptr<Node> oriNode, ErrorCode err);
	//�������ߣ��ӿڣ�
    void addEdge(const string& sStartName, const string& sStartType,
                    const string& sEndName, const string& sEndType,
                    const string& sType, ErrorCode err, const string& sValue = "1");
	//�޸Ľڵ��ڲ����ݣ��ӿڣ�
    void modifyNode(const string& sName, const string& sType,
                        ModifyOperation opt, const string& sProperities, ErrorCode err);
	//�޸�����ȡֵ���ӿڣ�
    void modifyEdge(const string& sStartName, const string& sStartType,
                        const string& sEndName, const string& sEndType,
                        const string& sType, const string& sValue, ErrorCode err);
	//ɾ���ڵ㣨�ӿڣ�
    void removeNode(const string& sName, const string& sType, ErrorCode err);
	//ɾ�����ߣ��ӿڣ�
    void removeEdge(const string& sStartName, const string& sStartType,
                        const string& sEndName, const string& sEndType,
                        const string& sType, ErrorCode err);


	int getNodeNum(){ return m_nodeList.size(); }
	int getEdgeNum(){ return m_edgeList.size(); }
	const NodeList& getNodeList(){ return m_nodeList; }
	const EdgeList& getEdgeList(){ return m_edgeList; }


protected:
	//���ߺ�������xml�ڵ��ж�ȡĳ����ǩ��InnerText
	string getText(Poco::XML::Element* curNode, const string& sLabel);
	//�����ڵ�ʵ������(���ڱ��棬�ͱ�Ų���ȫ��ͬ)
	void adjustNodeIndex();
	//����ڵ�
    void addNode(std::shared_ptr<Node> newNode);
	//ɾ���ڵ�
    void removeNode(std::shared_ptr<Node> curNode);
	//��������
    bool addEdge(std::shared_ptr<Node> curNode, std::shared_ptr<Node> tarNode, std::shared_ptr<Edge> newEdge);
	//�Ƴ�����
    bool removeEdge(std::shared_ptr<Node> curNode, std::shared_ptr<Node> tarNode);
	//ɾ�����б�����󶨵�����
    bool clearUnusedEdge(EdgeList& UnusedList);
	//��������֮��ָ��Type������
    std::shared_ptr<Edge> getEdgeByType(std::shared_ptr<Node> start, std::shared_ptr<Node> end, const string& sType = "");

private:
	NodeList m_nodeList;
    EdgeList m_edgeList;
    int m_iMaxNodeNum;
    int m_iMaxEdgeNum;
};











}
}


