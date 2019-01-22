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


//创建数据库，输入文件保存路径
void GraphDataBase::createDataBase(const string& sPath, ErrorCode err)
{	
	m_IOhandler = std::make_shared<XMLStrategy>(sPath);
    dataExport(err);
}

//打开数据库，输入当前文件路径
void GraphDataBase::openDataBase(const string& sPath, ErrorCode err)
{
	m_IOhandler = std::make_shared<XMLStrategy>(sPath);
    dataImport(err);
}

//XML批量数据导入
void GraphDataBase::dataImport(ErrorCode err)
{
	m_graph = m_IOhandler->readFile(err);
    if (err != NoError)
    {
        m_graph = NULL;
    }
}

//XML批量数据导出
void GraphDataBase::dataExport(ErrorCode err)
{
    auto doc = m_graph->toXML(err);
    if (err != NoError)
    {
        return;
    }
    m_IOhandler->saveFile(doc, err);
}

//数据库另存为
void GraphDataBase::dataBaseSaveAs(const string& newPath, ErrorCode err)
{
	m_IOhandler->setPath(newPath);
    dataExport(err);
}

//插入数据节点
void GraphDataBase::addNodeData(const string& sName, string sType, ErrorCode err, const string& sProperities)
{
	m_graph->addNode(sName, sType, err, sProperities);
}

//插入数据节点2
void GraphDataBase::addNodeData(std::shared_ptr<Node> oriNode, ErrorCode err)
{
	m_graph->addNode(oriNode, err);
}

//插入关系连边
void GraphDataBase::addEdgeData(const string& sStartName, const string& sStartType,
                        const string& sEndName, const string& sEndType,
                        const string& sType, ErrorCode err, const string& sValue)
{
	m_graph->addEdge(sStartName, sStartType, sEndName, sEndType, sType, err, sValue);
}

//修改节点内部数据
void GraphDataBase::modifyNodeData(const string& sName, const string& sType,
                            ModifyOperation opt, const string& sProperities, ErrorCode err)
{
	m_graph->modifyNode(sName, sType, opt, sProperities, err);
}

//修改连边取值
void GraphDataBase::modifyEdgeData(const string& sStartName, const string& sStartType,
                        const string& sEndName, const string& sEndType,
                        const string& sType, const string& sValue, ErrorCode err)
{
	m_graph->modifyEdge(sStartName, sStartType, sEndName, sEndType, sType, sValue, err);
}

//删除数据节点
void GraphDataBase::removeNodeData(const string& sName, const string& sType, ErrorCode err)
{
	m_graph->removeNode(sName, sType, err);
}

//删除关系连边
void GraphDataBase::removeEdgeData(const string& sStartName, const string& sStartType,
                        const string& sEndName, const string& sEndType,
                        const string& sType, ErrorCode err)
{
	m_graph->removeEdge(sStartName, sStartType, sEndName, sEndType, sType, err);
}

//执行查询语句
string GraphDataBase::dataQueryExecute(const string& strCypher, ErrorCode err)
{
	return m_parser->queryExecute(m_graph, strCypher, err);
}

//查询函数，返回指定索引处的节点
std::shared_ptr<Node> GraphDataBase::getNodeByIndex(int index)
{
	return m_graph->getNodeByIndex(index);
}

//查询函数，返回节点列表中指定名称和类型的节点
std::shared_ptr<Node> GraphDataBase::getNodeByName(const string& sName, const string& sType)
{
	return m_graph->getNodesByNameAndType(sName, sType);
}

//查询函数，返回指定名称和类型的节点的索引
int GraphDataBase::getIndexByNameAndType(const string& sName, const string& sType)
{
	return m_graph->getIndexByNameAndType(sName, sType);
}

//查询函数，返回指定名称和类型的节点间的连边
std::shared_ptr<Edge> GraphDataBase::getEdgeByNameAndType(const string& sName, const string& sType, const string& eName, const string& eType)
{
	return m_graph->getEdgeByNameAndType(sName, sType, eName, eType);
}



