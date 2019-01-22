#include "GraphDataBase.h"
#include "IO/XMLStrategy.h"

using namespace GraphDB;
using namespace GraphDB::Core;
using namespace GraphDB::Parser;
using namespace GraphDB::IO;


GraphDataBase::GraphDataBase()
{	
	m_graph = std::make_shared<Graph>();	
	m_parser = std::make_shared<CypherParser>();
}

GraphDataBase::GraphDataBase(int iNum)
{
	m_graph = std::make_shared<Graph>();
}


//�������ݿ⣬�����ļ�����·��
void GraphDataBase::createDataBase(const string& sPath, ErrorCode err)
{	
	m_IOhandler = std::make_shared<XMLStrategy>(sPath);
    dataExport(err);
}

//�����ݿ⣬���뵱ǰ�ļ�·��
void GraphDataBase::openDataBase(const string& sPath, ErrorCode err)
{
	m_IOhandler = std::make_shared<XMLStrategy>(sPath);
    dataImport(err);
}

//XML�������ݵ���
void GraphDataBase::dataImport(ErrorCode err)
{
	m_graph = m_IOhandler->readFile(err);
    if (err != NoError)
    {
        m_graph = NULL;
    }
}

//XML�������ݵ���
void GraphDataBase::dataExport(ErrorCode err)
{
    auto doc = m_graph->toXML(err);
    if (err != NoError)
    {
        return;
    }
    m_IOhandler->saveFile(doc, err);
}

//���ݿ����Ϊ
void GraphDataBase::dataBaseSaveAs(const string& newPath, ErrorCode err)
{
	m_IOhandler->setPath(newPath);
    dataExport(err);
}

//�������ݽڵ�
void GraphDataBase::addNodeData(const string& sName, string sType, ErrorCode err, const string& sProperities)
{
	m_graph->addNode(sName, sType, err, sProperities);
}

//�������ݽڵ�2
void GraphDataBase::addNodeData(std::shared_ptr<Node> oriNode, ErrorCode err)
{
	m_graph->addNode(oriNode, err);
}

//�����ϵ����
void GraphDataBase::addEdgeData(const string& sStartName, const string& sStartType,
                        const string& sEndName, const string& sEndType,
                        const string& sType, ErrorCode err, const string& sValue)
{
	m_graph->addEdge(sStartName, sStartType, sEndName, sEndType, sType, err, sValue);
}

//�޸Ľڵ��ڲ�����
void GraphDataBase::modifyNodeData(const string& sName, const string& sType,
                            ModifyOperation opt, const string& sProperities, ErrorCode err)
{
	m_graph->modifyNode(sName, sType, opt, sProperities, err);
}

//�޸�����ȡֵ
void GraphDataBase::modifyEdgeData(const string& sStartName, const string& sStartType,
                        const string& sEndName, const string& sEndType,
                        const string& sType, const string& sValue, ErrorCode err)
{
	m_graph->modifyEdge(sStartName, sStartType, sEndName, sEndType, sType, sValue, err);
}

//ɾ�����ݽڵ�
void GraphDataBase::removeNodeData(const string& sName, const string& sType, ErrorCode err)
{
	m_graph->removeNode(sName, sType, err);
}

//ɾ����ϵ����
void GraphDataBase::removeEdgeData(const string& sStartName, const string& sStartType,
                        const string& sEndName, const string& sEndType,
                        const string& sType, ErrorCode err)
{
	m_graph->removeEdge(sStartName, sStartType, sEndName, sEndType, sType, err);
}

//ִ�в�ѯ���
string GraphDataBase::dataQueryExecute(const string& strCypher, ErrorCode err)
{
	return m_parser->queryExecute(m_graph, strCypher, err);
}

//��ѯ����������ָ���������Ľڵ�
std::shared_ptr<Node> GraphDataBase::getNodeByIndex(int index)
{
	return m_graph->getNodeByIndex(index);
}

//��ѯ���������ؽڵ��б���ָ�����ƺ����͵Ľڵ�
std::shared_ptr<Node> GraphDataBase::getNodeByName(const string& sName, const string& sType)
{
	return m_graph->getNodesByNameAndType(sName, sType);
}

//��ѯ����������ָ�����ƺ����͵Ľڵ������
int GraphDataBase::getIndexByNameAndType(const string& sName, const string& sType)
{
	return m_graph->getIndexByNameAndType(sName, sType);
}

//��ѯ����������ָ�����ƺ����͵Ľڵ�������
std::shared_ptr<Edge> GraphDataBase::getEdgeByNameAndType(const string& sName, const string& sType, const string& eName, const string& eType)
{
	return m_graph->getEdgeByNameAndType(sName, sType, eName, eType);
}



