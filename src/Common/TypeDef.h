#pragma once 


#include <stdlib.h>
#include <vector>
#include <string>


#include "Poco/DOM/Document.h"
#include "Poco/DOM/Element.h"
#include "Poco/DOM/Text.h"
#include "Poco/DOM/AutoPtr.h"
#include "Poco/DOM/NodeList.h"
#include "Poco/DOM/DOMWriter.h"
#include "Poco/XML/XMLWriter.h"





class TreeNode
{
public:
	int getLevel(){}
	std::string getText(){}
	std::vector<std::shared_ptr<TreeNode>> getNodes(){}
	std::shared_ptr<TreeNode> getParent(){}
};


namespace GraphDB
{






//系统错误码
enum ErrorCode
{
    NoError = 0,
    OpenFileFailed = 1,
    SaveFileFailed = 2,
    NoXmlRoot = 3,
    InvaildIndex = 10,
    NodeExists = 11,
    CreateNodeFailed = 12,
    NodeNotExists = 13,
    EdgeExists = 15,
    CreateEdgeFailed = 16,
    EdgeNotExists = 17,
    AddEdgeFailed = 18,
    CypherInvalid = 40,
    StartSegInvalid = 41,
    MatchSegInvalid = 42,
    WhereSegInvalid = 43,
    ReturnSegInvalid = 44,
    NoStartNode = 60,
};



//修改操作选项
enum ModifyOperation
{
    Append = 0,
    Replace = 1,
    ReplaceAll = 2,
    Delete = 3,
};



namespace Core{
	class Edge;
	class NodeProperty;
	class Node;

	typedef std::vector<std::shared_ptr<Edge>> EdgeList;
	typedef std::vector<std::shared_ptr<NodeProperty>> AttributeList;
	typedef std::vector<std::shared_ptr<Node>> NodeList;
}



namespace Parser{
	class MidResult;
	class MatchRule;
	class FilterRule;

	typedef std::vector<std::shared_ptr<MidResult>> MidResultList;
	typedef std::vector<std::shared_ptr<MatchRule>> MatchRuleList;
	typedef std::vector<std::shared_ptr<FilterRule>> FilterRuleList;
}







typedef std::vector<std::shared_ptr<TreeNode>> TreeNodeList;











}









