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


	//创建数据库，输入文件保存路径
    void createDataBase(const string& sPath, ErrorCode err);
	//打开数据库，输入当前文件路径
    void openDataBase(const string& sPath, ErrorCode err);
	//XML批量数据导入
    void dataImport(ErrorCode err);
	//XML批量数据导出
    void dataExport(ErrorCode err);
	//数据库另存为
    void dataBaseSaveAs(const string& newPath, ErrorCode err);
	//插入数据节点
    void addNodeData(const string& sName, string sType, ErrorCode err, const string& sProperities = "1");
	//插入数据节点2
    void addNodeData(std::shared_ptr<Core::Node> oriNode, ErrorCode err);
	//插入关系连边
    void addEdgeData(const string& sStartName, const string& sStartType,
                            const string& sEndName, const string& sEndType,
                            const string& sType, ErrorCode err, const string& sValue = "");
	//修改节点内部数据
    void modifyNodeData(const string& sName, const string& sType,
                             ModifyOperation opt, const string& sProperities, ErrorCode err);
	//修改连边取值
    void modifyEdgeData(const string& sStartName, const string& sStartType,
                            const string& sEndName, const string& sEndType,
                            const string& sType, const string& sValue, ErrorCode err);
	//删除数据节点
    void removeNodeData(const string& sName, const string& sType, ErrorCode err);
	//删除关系连边
    void removeEdgeData(const string& sStartName, const string& sStartType,
                            const string& sEndName, const string& sEndType,
                            const string& sType, ErrorCode err);
	//执行查询语句
    string dataQueryExecute(const string& strCypher, ErrorCode err);
	//查询函数，返回指定索引处的节点
    std::shared_ptr<Core::Node> getNodeByIndex(int index);
	//查询函数，返回节点列表中指定名称和类型的节点
    std::shared_ptr<Core::Node> getNodeByName(const string& sName, const string& sType);
	//查询函数，返回指定名称和类型的节点的索引
    int getIndexByNameAndType(const string& sName, const string& sType);
	//查询函数，返回指定名称和类型的节点间的连边
    std::shared_ptr<Core::Edge> getEdgeByNameAndType(const string& sName, const string& sType, const string& eName, const string& eType);
	

	//获取文件存放路径
	string getPath(){ m_IOhandler->getPath(); }
	//获取数据库节点总数
	int getNodeNum(){ m_graph->getNodeNum(); }
	//获取数据库连边总数
	int getEdgeNum(){ m_graph->getEdgeNum(); }
	//获取节点列表
	const Core::NodeList& getNodeList(){ return m_graph->getNodeList(); }


private:
	std::shared_ptr<Core::Graph> m_graph;
    std::shared_ptr<IO::IfIOStrategy> m_IOhandler;
    std::shared_ptr<Parser::CypherParser> m_parser;
};












}





