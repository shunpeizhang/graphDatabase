#include "Node.h"
#include "../Parser/MatchRule.h"


#include <Poco/RegularExpression.h>



using namespace Poco;

using Poco::XML::Document;
using Poco::XML::Element;
using Poco::XML::Text;
using Poco::XML::AutoPtr;
using Poco::XML::DOMWriter;
using Poco::XML::XMLWriter;

GraphDB::Core::Node::Node(int intMaxNodeNum, const string& newName, const string& newType, const string& sProperities)
{
	m_iNodeNum = intMaxNodeNum;
    m_strNodeName = newName;
    m_strNodeType = newType;
    m_iSaveIndex = m_iNodeNum;
    if(sProperities != "")
    {
        addProperty(sProperities);
    }
}

GraphDB::Core::Node::Node(int intMaxNodeNum, Node& oriNode)
{
	m_iNodeNum = intMaxNodeNum;
	m_strNodeName = oriNode.getNodeName();
	m_strNodeType = oriNode.getNodeType();
    m_iSaveIndex = m_iNodeNum;
    for(auto np : oriNode.getAttributeList())
    {
		m_attributeList.push_back(std::shared_ptr<NodeProperty>(new NodeProperty(np->getKey(), np->getValue())));
    }
}

GraphDB::Core::Node::Node(int intMaxNodeNum, Poco::XML::Element& xNode)
{
	string newType, newName;

	m_iNodeNum = intMaxNodeNum;
	//取出制定标签的Inner Text
	newType = getText(&xNode, "Type");
	newName = getText(&xNode, "Name");
	
	//赋值与初始化
	m_strNodeName = newType;
	m_strNodeType = newName;
	m_iSaveIndex = m_iNodeNum;

	//加入用户自定义属性
	{
		Poco::XML::NodeList* nodes = xNode.getElementsByTagName("Properties");
		if(NULL != nodes)
		{
			Poco::XML::Node* xmlProperties = nodes->item(0);
			Poco::XML::NodeList* childNodes = xmlProperties->childNodes();
			if(NULL != childNodes)
			{
				const int iLen = childNodes->length();
				for(int iPos = 0; iLen > iPos; ++iPos)
				{
					Poco::XML::Element* pNode = (Poco::XML::Element*)childNodes->item(iPos);
					m_attributeList.push_back(std::make_shared<GraphDB::Core::NodeProperty>(*pNode));
				}
			}
		}
	}
}



Element* GraphDB::Core::Node::toXML(Document& doc)
{
	Element* curNode = doc.createElement("Node");
    Element* curProperties = doc.createElement("Properties");
	char buf[128];

	snprintf(buf, sizeof(buf), "%d", m_iSaveIndex);
    curNode->setAttribute("num", buf);//创建各属性的Tag元素
    //节点类型
    Element* name_xml = doc.createElement("Name");
    Element* type_xml = doc.createElement("Type");
    //创建各属性的文本元素
    Text* name_txt = doc.createTextNode(m_strNodeName);
    Text* type_txt = doc.createTextNode(m_strNodeType);
    //将标题元素赋予文本内容
    name_xml->appendChild(name_txt);
    type_xml->appendChild(type_txt);

    for (auto np : m_attributeList)
    {
        curProperties->appendChild(np->toXML(doc));
    }

    //向当前节点中加入各属性节点
    curNode->appendChild(name_xml);
    curNode->appendChild(type_xml);
    curNode->appendChild(curProperties);

    return curNode;
}


void GraphDB::Core::Node::addProperty(const string& sProperities, ModifyOperation opt)
{
	if (opt == ReplaceAll)
    {
        m_attributeList.clear();
    }

	const string strKeyPairPattern = "[\\w]+:[\\w]+";  //匹配目标"名称+取值"组合
	RegularExpression::MatchVec matches;
	RegularExpression regexKeyPair(strKeyPairPattern);
	int matchCount = regexKeyPair.match(sProperities, 0, matches);//正则表达式对分词结果进行匹配
	if (0 == matchCount)
    {
        return;
    }

	std::shared_ptr<NodeProperty> newProperty;
	for(RegularExpression::Match match : matches)
	{
        newProperty = buildProperty(match.name);
		if (NULL != newProperty.get())
		{
			if (NULL == getProperty(newProperty->getKey()).get())
			{
				if (Replace != opt)
                {
                    m_attributeList.push_back(newProperty);
                }
			}
			else
            {
                if (Replace == opt)
                {
                    modifyProperty(*newProperty);
                }
            }
		}
	}
}


std::shared_ptr<GraphDB::Core::NodeProperty> GraphDB::Core::Node::buildProperty(const string& sProperty)
{
	string key, value;
	Poco::XML::Name::split(sProperty, key, value);

	std::shared_ptr<NodeProperty> nodeProperty = std::make_shared<GraphDB::Core::NodeProperty>(key, value);

	return nodeProperty;
}

std::shared_ptr<GraphDB::Core::NodeProperty> GraphDB::Core::Node::getProperty(const string& sKey)
{
	for(std::shared_ptr<NodeProperty> item : m_attributeList)
	{
		if(sKey == item->getKey())
		{			
			return item;
		}
	}

	return std::shared_ptr<NodeProperty>();
}

void GraphDB::Core::Node::modifyProperty(const NodeProperty& sProperty)
{
	for(std::shared_ptr<NodeProperty> item : m_attributeList)
	{
		if(sProperty.getKey() == item->getKey())
		{
			item->setValue(sProperty.getValue());
		}
	}
}

void GraphDB::Core::Node::removeProperty(const string& sProperities)
{
	const string strKeyPairPattern = "[\\w]+";  //匹配目标"名称"组合
    RegularExpression::MatchVec matches;
    RegularExpression regexKeyPair(strKeyPairPattern);
	int matchCount = regexKeyPair.match(sProperities, 0, matches);//正则表达式对分词结果进行匹配
	if (0 == matchCount)
    {
        return;
    }

	std::shared_ptr<NodeProperty> tp;
	for(RegularExpression::Match match : matches)
	{
        tp = getProperty(match.name);
		if (NULL != tp.get())
		{
			AttributeList::iterator iter = m_attributeList.begin();
			for(; m_attributeList.end() != iter; ++iter)
			{
				if((*iter)->getKey() == tp->getKey() && (*iter)->getValue() == tp->getValue())
				{
					m_attributeList.erase(iter);
					break;
				}
			}
		}
	}
    
    return;
}

bool GraphDB::Core::Node::addEdge(std::shared_ptr<Edge> newEdge)
{
	if (NULL == newEdge.get())
    {
        return false;
    }

    //检测条件：当前边的起始节点是本节点，且终止节点不是本节点
    if (newEdge->getStartNode()->getNodeNum() != m_iNodeNum || newEdge->getEndNode()->getNodeNum() == m_iNodeNum)
    {
        return false;
    }
    //如果OutbOund已经包含该边
    if (outBoundContainsEdge(*newEdge) == true)
    {
        return false;
    }
    //向Links中加入新项目
	m_outLinkList.push_back(newEdge);

    return true;
}

bool GraphDB::Core::Node::registerInbound(std::shared_ptr<Edge> newEdge)
{
	if (NULL == newEdge.get())
    {
        return false;
    }

    //检测条件：当前边的起始节点不是本节点，且终止节点是本节点
    if (newEdge->getEndNode()->getNodeNum() != m_iNodeNum || newEdge->getEndNode()->getNodeNum() == m_iNodeNum)
    {
        return false;
    }
    //如果Inbound包含该边则不注册
    if (inBoundContainsEdge(*newEdge) == true)
    {
        return false;
    }
    //加入新边
    m_inLinkList.push_back(newEdge);

    return true;
}

bool GraphDB::Core::Node::removeEdge(std::shared_ptr<Edge> curEdge)
{
	if (NULL == curEdge.get())
    {
        return false;
    }

    //检测条件：当前边的起始节点是本节点，且终止节点不是本节点
    if (curEdge->getStartNode()->getNodeNum() != m_iNodeNum || curEdge->getEndNode()->getNodeNum() == m_iNodeNum)
    {
        return false;
    }
    //如果OutbOund不包含该边则退出
    if (outBoundContainsEdge(*curEdge) == false)
    {
        return false;
    }

	EdgeList::iterator iter = m_outLinkList.begin();
	for(; m_outLinkList.end() != iter; ++iter)
	{
		if((*iter)->getEdgeNum() == curEdge->getEdgeNum())
		{
			m_outLinkList.erase(iter);
			break;
		}
	}

    return true;
}

std::shared_ptr<GraphDB::Core::EdgeList> GraphDB::Core::Node::clearEdge()
{
	std::shared_ptr<GraphDB::Core::EdgeList> edgeList = std::make_shared<GraphDB::Core::EdgeList>();
    //首先将OutBound中所有连边的终止节点中注销该边
    for (auto edge : m_outLinkList)
    {
        edge->getEndNode()->unRegisterInbound(edge);
        //当前边加入返回结果列表
		edgeList->push_back(edge);
    }
    //从OutBound中清除所有边
    m_outLinkList.clear();
    //首先将InBound中所有连边的起始节点中去除该边
    for (auto edge : m_inLinkList)
    {
        edge->getStartNode()->removeEdge(edge);
        //当前边加入返回结果列表
        edgeList->push_back(edge);
    }
    //从InBound中清除所有边
    m_inLinkList.clear();
    //返回本节点涉及的连边列表
    return edgeList;
}

bool GraphDB::Core::Node::unRegisterInbound(std::shared_ptr<Edge> curEdge)
{
	if (NULL == curEdge.get())
    {
        return false;
    }
    //检测条件：当前边的起始节点不是本节点，且终止节点是本节点
    if (curEdge->getEndNode()->getNodeNum() != m_iNodeNum || curEdge->getStartNode()->getNodeNum() == m_iNodeNum)//检测条件：当前节点与目标节点不相连，且目标节点不是当前节点
    {
        return false;
    }
    //如果Inbound不包含当前边则不注销
    if (inBoundContainsEdge(*curEdge) == false)
    {
        return false;
    }

	EdgeList::iterator iter = m_inLinkList.begin();
	for(; m_inLinkList.end() != iter; ++iter)
	{
		if((*iter)->getEdgeNum() == curEdge->getEdgeNum())
		{
			m_outLinkList.erase(iter);
			break;
		}
	}

    return true;
}

bool GraphDB::Core::Node::outBoundContainsEdge(Edge& newEdge)
{
	if(isExistEdgeInList(m_outLinkList, newEdge))
    {
        return true;
    }

    for(auto edge : m_outLinkList)
    {
        if (edge->getEndNode()->getNodeNum() == newEdge.getEndNode()->getNodeNum())
        {
            if (edge->getEdgeType() == newEdge.getEdgeType())
            {
                return true;
            }
        }
    }

    return false;
}

bool GraphDB::Core::Node::isExistEdgeInList(EdgeList& edgeList, Edge& newEdge)
{
	for(auto item : edgeList)
	{
		if(item->equal(newEdge))
		{
			return true;
		}
	}

	return false;
}

bool GraphDB::Core::Node::inBoundContainsEdge(Edge& newEdge)
{
	if (isExistEdgeInList(m_inLinkList, newEdge))
    {
        return true;
    }
    for(auto edge : m_inLinkList)
    {
        if (edge->getStartNode()->getNodeNum() == newEdge.getStartNode()->getNodeNum())
        {
            if (edge->getEdgeType() == newEdge.getEdgeType())
            {
                return true;
            }
        }
    }
    return false;
}

string GraphDB::Core::Node::fieldOutputAll()
{
	string strResult = "";

    strResult += "Name\t";
    strResult += "Type\t";
    for (auto sProperty : m_attributeList)
    {
        strResult += sProperty->getKey() + "\t";
    }
    return strResult + "\n";
}

string GraphDB::Core::Node::fieldOutput(const std::vector<string>& labels)
{
	string strResult = "";

    for (string label : labels)
    {
        if (label == "Name")
        {
            strResult += "Name\t";
        }
        else if (label == "Type")
        {
            strResult += "Type\t";
        }
        for (auto sProperty : m_attributeList)
        {
            if (sProperty->getKey() != label)
            {
                continue;
            }
            strResult += sProperty->getKey() + "\t";
        }
    }
    return strResult + "\n";
}

string GraphDB::Core::Node::dataOutputAll()
{
	string strResult = "";

    strResult += m_strNodeName+"\t";
    strResult += m_strNodeType + "\t";
    for (auto sProperty : m_attributeList)
    {
        strResult += sProperty->getValue() + "\t";
    }
    return strResult + "\n";
}

string GraphDB::Core::Node::dataOutput(const std::vector<string>& labels)
{
	string strResult = "";

    for (string label : labels)
    {
        if (label == "Name")
        {
            strResult += m_strNodeName + "\t";
        }
        else if (label == "Type")
        {
            strResult += m_strNodeType + "\t";
        }
        for (auto sProperty : m_attributeList)
        {
            if (sProperty->getKey() != label)
            {
                continue;
            }
            strResult += sProperty->getValue() + "\t";
        }
    }
    return strResult + "\n";
}

string GraphDB::Core::Node::dataOutput()
{
	string strResult = "";

    strResult +="Name:" + m_strNodeName + "\n";
    strResult +="Type:" + m_strNodeType ;
    for (auto sProperty : m_attributeList)
    {
        strResult +="\n" + sProperty->getKey() + ":" + sProperty->getValue();
    }
            
    return strResult;
}


std::shared_ptr<GraphDB::Core::Edge> GraphDB::Core::Node::getEdge(const string& sName, const string& sType, const string& opt)
{
	if (opt == "In")
    {
        for (auto edge : m_inLinkList)
        {
            if (edge->getStartNode()->getNodeName() == sName && edge->getStartNode()->getNodeType() == sType)
            {
                return edge;
            }
        }
        return std::shared_ptr<GraphDB::Core::Edge>();
    }
    else if (opt == "Out")
    {
        for (auto edge : m_outLinkList)
        {
            if (edge->getEndNode()->getNodeName() == sName && edge->getEndNode()->getNodeType() == sType)
            {
                return edge;
            }
        }
        return std::shared_ptr<GraphDB::Core::Edge>();
    }
    
    return std::shared_ptr<GraphDB::Core::Edge>();    
}

 std::shared_ptr<TreeNode> GraphDB::Core::Node::search(GraphDB::Parser::MatchRuleList& mRule, int level)
{

}


string GraphDB::Core::Node::getText(Poco::XML::Element* curNode, const string& sLabel)
{
	Poco::XML::NodeList* nodes = curNode->childNodes();
	if(NULL == nodes)
	{
		return "";
	}

	const int iLen = nodes->length();
	for(int iPos = 0; iLen > iPos; ++iPos)
	{
		Poco::XML::Node* pNode = nodes->item(iPos);
		if (pNode->nodeName() == sLabel)
        {
            return pNode->innerText();
        }
	}

    return "";
}



