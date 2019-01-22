#pragma once



#include <string>
#include <vector>


#include "Core/Graph.h"
#include "IO/IfIOStrategy.h"
#include "Parser/CypherParser.h"
#include "Core/Node.h"


using std::string;




namespace GraphDB{



class GraphDataBase
{
public:
	GraphDataBase();
	GraphDataBase(int iNum);


	//�������ݿ⣬�����ļ�����·��
    void createDataBase(const string& sPath, ErrorCode err);
	//�����ݿ⣬���뵱ǰ�ļ�·��
    void openDataBase(const string& sPath, ErrorCode err);
	//XML�������ݵ���
    void dataImport(ErrorCode err);
	//XML�������ݵ���
    void dataExport(ErrorCode err);
	//���ݿ����Ϊ
    void dataBaseSaveAs(const string& newPath, ErrorCode err);
	//�������ݽڵ�
    void addNodeData(const string& sName, string sType, ErrorCode err, const string& sProperities = "1");
	//�������ݽڵ�2
    void addNodeData(std::shared_ptr<Core::Node> oriNode, ErrorCode err);
	//�����ϵ����
    void addEdgeData(const string& sStartName, const string& sStartType,
                            const string& sEndName, const string& sEndType,
                            const string& sType, ErrorCode err, const string& sValue = "");
	//�޸Ľڵ��ڲ�����
    void modifyNodeData(const string& sName, const string& sType,
                             ModifyOperation opt, const string& sProperities, ErrorCode err);
	//�޸�����ȡֵ
    void modifyEdgeData(const string& sStartName, const string& sStartType,
                            const string& sEndName, const string& sEndType,
                            const string& sType, const string& sValue, ErrorCode err);
	//ɾ�����ݽڵ�
    void removeNodeData(const string& sName, const string& sType, ErrorCode err);
	//ɾ����ϵ����
    void removeEdgeData(const string& sStartName, const string& sStartType,
                            const string& sEndName, const string& sEndType,
                            const string& sType, ErrorCode err);
	//ִ�в�ѯ���
    string dataQueryExecute(const string& strCypher, ErrorCode err);
	//��ѯ����������ָ���������Ľڵ�
    std::shared_ptr<Core::Node> getNodeByIndex(int index);
	//��ѯ���������ؽڵ��б���ָ�����ƺ����͵Ľڵ�
    std::shared_ptr<Core::Node> getNodeByName(const string& sName, const string& sType);
	//��ѯ����������ָ�����ƺ����͵Ľڵ������
    int getIndexByNameAndType(const string& sName, const string& sType);
	//��ѯ����������ָ�����ƺ����͵Ľڵ�������
    std::shared_ptr<Core::Edge> getEdgeByNameAndType(const string& sName, const string& sType, const string& eName, const string& eType);
	

	//��ȡ�ļ����·��
	string getPath(){ m_IOhandler->getPath(); }
	//��ȡ���ݿ�ڵ�����
	int getNodeNum(){ m_graph->getNodeNum(); }
	//��ȡ���ݿ���������
	int getEdgeNum(){ m_graph->getEdgeNum(); }
	//��ȡ�ڵ��б�
	const Core::NodeList& getNodeList(){ return m_graph->getNodeList(); }


private:
	std::shared_ptr<Core::Graph> m_graph;
    std::shared_ptr<IO::IfIOStrategy> m_IOhandler;
    std::shared_ptr<Parser::CypherParser> m_parser;
};












}





