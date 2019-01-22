#include "Graph.h"




using namespace GraphDB::Core;
using namespace Poco;





Graph::Graph()
{
	m_iMaxNodeNum = 0;
    m_iMaxEdgeNum = 0;
}

Graph::Graph(Poco::XML::Document& doc, ErrorCode err)
{
	m_iMaxNodeNum = 0;
    m_iMaxEdgeNum = 0;

	//取出根节点
	Poco::XML::Node* xmlroot = NULL;
	{
		XML::NodeList* vector = doc.getElementsByTagName("Graph");
		if(NULL == vector)
		{
			err = NoXmlRoot;
			return;
		}
		
		xmlroot = vector->item(0);
		if(NULL == xmlroot)
		{
			err = NoXmlRoot;
			return;
		}
	}


	Poco::XML::Node *xmlNodes = NULL, *xmlEdges = NULL;
	{
		Poco::XML::NodeList* childNodes = xmlroot->childNodes();
		if(NULL != childNodes)
		{
			const int iLen = childNodes->length();
			for(int iPos = 0; iLen > iPos; ++iPos)
			{
				Poco::XML::Node* xNode = childNodes->item(iPos);
				if (xNode->nodeName() == "Nodes")
				{
					xmlNodes = xNode;
				}
				if (xNode->nodeName() == "Edges")
				{
					xmlEdges = xNode;
				}
			}
		}		
	}	

	//解析node
	{
		if (NULL == xmlNodes)
		{
			err = NoError;
			return;
		}

		Poco::XML::NodeList* childNodes = xmlNodes->childNodes();
		if(NULL != childNodes)
		{
			const int iLen = childNodes->length();
			for(int iPos = 0; iLen > iPos; ++iPos)
			{
				Poco::XML::Element* xNode = (Poco::XML::Element*)childNodes->item(iPos);
				//生成新节点
				std::shared_ptr<Node> newNode = std::make_shared<Node>(m_iMaxNodeNum, *xNode);
				m_iMaxNodeNum++;
				//加入图
				m_nodeList.push_back(newNode);
			}
		}
	}	

	//处理Edge
	{
		if (NULL == xmlEdges)
		{
			err = NoError;
			return;
		}

		std::shared_ptr<Edge> newEdge;
		string strStart, strEnd;
		std::shared_ptr<Node> nodeStart, nodeEnd;

		Poco::XML::NodeList* childNodes = xmlEdges->childNodes();
		if(NULL != childNodes)
		{
			const int iLen = childNodes->length();
			for(int iPos = 0; iLen > iPos; ++iPos)
			{
				Poco::XML::Element* xNode = (Poco::XML::Element*)childNodes->item(iPos);

				//生成新连边
				newEdge = std::make_shared<Edge>(m_iMaxEdgeNum, *xNode);
				//获取连边的起始和终止节点编号
				strStart = getText(xNode, "Start");
				strEnd = getText(xNode, "End");
				
				nodeStart = getNodeByIndex(atoi(strStart.c_str()));
				nodeEnd = getNodeByIndex(atoi(strEnd.c_str()));
				if (NULL == nodeStart.get() || NULL == nodeEnd.get())
				{
					err = InvaildIndex;
					continue;
				}
				m_iMaxEdgeNum++;
				//加入图
				if (addEdge(nodeStart, nodeEnd, newEdge) == false)
				{
					err = AddEdgeFailed;
					continue;
				}
			}
		}
	}	

    return;
}


Poco::XML::Document* Graph::toXML(ErrorCode err)
{
	char buf[128];

	Poco::XML::Document* doc = new Poco::XML::Document();
    //所有网络数据都保存为xml格式
	Poco::XML::Element* root = doc->createElement("Graph");
    Poco::XML::Element *Nodes = NULL, *Edges = NULL;

    adjustNodeIndex();
    Nodes = doc->createElement("Nodes");
	snprintf(buf, sizeof(buf), "%d", m_iMaxNodeNum);
    Nodes->setAttribute("NodeNumber", buf);
    for (auto curNode : m_nodeList)
    {
        Nodes->appendChild(curNode->toXML(*doc));     //循环调用底层节点的输出函数
    }
    root->appendChild(Nodes);
    Edges = doc->createElement("Edges");
	snprintf(buf, sizeof(buf), "%d", m_iMaxEdgeNum);
    Edges->setAttribute("EdgeNumber", buf);
    for (auto curEdge : m_edgeList)
    {
        Edges->appendChild(curEdge->toXML(*doc)); //循环调用底层节点的输出函数
    }
    root->appendChild(Edges);
    doc->appendChild(root);

    return doc;
}

std::shared_ptr<Node> Graph::getNodeByIndex(int index)
{
	if (index >= m_iMaxNodeNum || index < 0)
    {
        return std::shared_ptr<Node>();
    }
    return m_nodeList[index];
}

std::shared_ptr<Node> Graph::getNodesByNameAndType(const string& sName, const string& type)
{
	//遍历节点列表
    for (auto curNode : m_nodeList)
    {
        if (curNode->getNodeName() == sName && curNode->getNodeType() == type)
        {
            return curNode;
        }
    }

    return std::shared_ptr<Node>();
}

int Graph::getIndexByNameAndType(const string& sName, const string& sType)
{
	int index = 0;
    //遍历节点列表
    for (auto curNode : m_nodeList)
    {
        if (curNode->getNodeName() == sName && curNode->getNodeType() == sType)
        {
            return index;
        }
        index++;
    }

    return -1;
}


std::shared_ptr<NodeList> Graph::getNodesOfType(const string& type)
{
	std::shared_ptr<NodeList> vectorPtr = std::make_shared<NodeList>();
    //遍历节点列表
    for (auto curNode : m_nodeList)
    {
        if (curNode->getNodeType() == type)
        {//将符合type要求的节点加入返回结果列表
            vectorPtr->push_back(curNode);
        }
    }
    return vectorPtr;
}

std::shared_ptr<Edge> Graph::getEdgeByNameAndType(const string& sName, const string& sType, const string& eName, const string& eType)
{
	std::shared_ptr<Node> startNode, endNode;
    startNode = getNodesByNameAndType(sName, sType);
    if (NULL == startNode.get())
    {
        return std::shared_ptr<Edge>();
    }
    endNode = getNodesByNameAndType(eName, eType);
    if (NULL == endNode.get())
    {
        return std::shared_ptr<Edge>();
    }

    return getEdgeByType(startNode, endNode);
}

void Graph::addNode(const string& sName, const string& sType, ErrorCode err, const string& sProperities)
{
	std::shared_ptr<Node> newNode;

    //检查节点是否已经存在“名称+类型一致”
    if (NULL != getNodesByNameAndType(sName, sType).get())
    {
        err = NodeExists;
        return;
    }
    //构造新的节点
	newNode = std::make_shared<Node>(m_iMaxNodeNum, sType, sProperities);
    if (NULL == newNode.get())
    {
        err = CreateNodeFailed;
        return;
    }

    m_iMaxNodeNum++;
    addNode(newNode);
    err = NoError;

    return;
}

void Graph::addNode(std::shared_ptr<Node> oriNode, ErrorCode err)
{
	std::shared_ptr<Node> newNode;

    //检查节点是否已经存在“名称+类型一致”
    if (NULL != getNodesByNameAndType(oriNode->getNodeName(), oriNode->getNodeType()).get())
    {
        err = NodeExists;
        return;
    }
    //构造新的节点
	newNode = std::make_shared<Node>(m_iMaxNodeNum, *oriNode);
    if (NULL == newNode.get())
    {
        err = CreateNodeFailed;
        return;
    }
    m_iMaxNodeNum++;
    addNode(newNode);
    err = NoError;

    return;
}

void Graph::addEdge(const string& sStartName, const string& sStartType,
                const string& sEndName, const string& sEndType,
                const string& sType, ErrorCode err, const string& sValue)
{
	std::shared_ptr<Node> startNode, endNode;
    std::shared_ptr<Edge> newEdge;
    //获取起始节点，不存在报错
    startNode = getNodesByNameAndType(sStartName, sStartType);
    if (NULL == startNode.get())
    {
        err = NodeNotExists;
        return;
    }
    //获取终止节点，不存在报错
    endNode = getNodesByNameAndType(sEndName, sEndType);
    if (NULL == endNode.get())
    {
        err = NodeNotExists;
        return;
    }
    //查找两点间是否存在相同类型关系，存在报错
    if (NULL != getEdgeByType(startNode, endNode, sType).get())
    {
        err = EdgeExists;
        return;
    }
    //创建新连边
	newEdge = std::make_shared<Edge>(m_iMaxEdgeNum, sType, sValue);
    if (newEdge == NULL)
    {
        err = CreateEdgeFailed;
        return;
    }
    m_iMaxEdgeNum++;
    //在两点间加入新边
    addEdge(startNode, endNode, newEdge);
    err = NoError;

    return;
}

void Graph::modifyNode(const string& sName, const string& sType,
                    ModifyOperation opt, const string& sProperities, ErrorCode err)
{
	std::shared_ptr<Node> tarNode;

    tarNode = getNodesByNameAndType(sName, sType);
    //检查节点是否已经存在“名称+类型一致”
    if (NULL == tarNode.get())
    {
        err = NodeNotExists;
        return;
    }
    if (opt == Delete)
    {
        tarNode->removeProperty(sProperities);
    }
    else
    {
        tarNode->addProperty(sProperities, opt);
    }
    err = NoError;

    return;
}

void Graph::modifyEdge(const string& sStartName, const string& sStartType,
                    const string& sEndName, const string& sEndType,
                    const string& sType, const string& sValue, ErrorCode err)
{
	std::shared_ptr<Node> startNode, endNode;
    std::shared_ptr<Edge> tarEdge;
    //获取起始节点，不存在报错
    startNode = getNodesByNameAndType(sStartName, sStartType);
    if (NULL == startNode.get())
    {
        err = NodeNotExists;
        return;
    }
    //获取终止节点，不存在报错
    endNode = getNodesByNameAndType(sEndName, sEndType);
    if (NULL == endNode.get())
    {
        err = NodeNotExists;
        return;
    }
    //查找两点间是否存在相同类型关系，不存在报错
    tarEdge = getEdgeByType(startNode, endNode, sType);
    if (tarEdge == NULL)
    {
        err = EdgeNotExists;
        return;
    }
    tarEdge->setEdgeValue(sValue);
    err = NoError;

    return;
}

void Graph::removeNode(const string& sName, const string& sType, ErrorCode err)
{
	std::shared_ptr<Node> tarNode;

    tarNode = getNodesByNameAndType(sName, sType);
    //检查节点是否已经存在“名称+类型一致”
    if (NULL == tarNode.get())
    {
        err = NodeNotExists;
        return;
    }

    removeNode(tarNode);
    err = NoError;

    return;
}

void Graph::removeEdge(const string& sStartName, const string& sStartType,
                    const string& sEndName, const string& sEndType,
                    const string& sType, ErrorCode err)
{
	std::shared_ptr<Node> startNode, endNode;
    std::shared_ptr<Edge> tarEdge;
    //获取起始节点，不存在报错
    startNode = getNodesByNameAndType(sStartName, sStartType);
    if (NULL == startNode.get())
    {
        err = NodeNotExists;
        return;
    }
    //获取终止节点，不存在报错
    endNode = getNodesByNameAndType(sEndName, sEndType);
    if (NULL == endNode.get())
    {
        err = NodeNotExists;
        return;
    }
    //查找两点间是否存在相同类型关系，存在报错
    tarEdge = getEdgeByType(startNode, endNode, sType);
    if (NULL == tarEdge.get())
    {
        err = EdgeNotExists;
        return;
    }
    //起始节点Outbound中移除连边
    startNode->removeEdge(tarEdge);
    //从终止节点InBound中注销连边
    endNode->unRegisterInbound(tarEdge);
    //全部完成后，从总连边列表中移除该边
	{
		EdgeList::iterator iter = m_edgeList.begin();
		for(; m_edgeList.end() != iter; ++iter)
		{
			if((*iter)->equal(*(*iter)))
			{
				m_edgeList.erase(iter);
				break;
			}
		}
	}
    
    err = NoError;

    return;
}


string Graph::getText(Poco::XML::Element* curNode, const string& sLabel)
{
	if (NULL == curNode)
    {
        return "";
    }

	//遍历当前XML的所有子标签
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

void Graph::adjustNodeIndex()
{
	int index = 0;
    for (auto curNode : m_nodeList)
    {
        curNode->setSaveIndex(index);
        index++;
    }
}

void Graph::addNode(std::shared_ptr<Node> newNode)
{
	m_nodeList.push_back(newNode);
}

void Graph::removeNode(std::shared_ptr<Node> curNode)
{
	//清除节点所有连边
    clearUnusedEdge(*curNode->clearEdge());

    //从节点列表中移除节点
	NodeList::iterator iter = m_nodeList.begin();
	for(; m_nodeList.end() != iter; ++iter)
	{
		if((*iter)->equal(*iter))
		{
			m_nodeList.erase(iter);
			break;
		}
	}
}

bool Graph::addEdge(std::shared_ptr<Node> curNode, std::shared_ptr<Node> tarNode, std::shared_ptr<Edge> newEdge)
{
	//连边的头指针指向起节点
    newEdge->setStartNode(curNode);
    //连边的尾指针指向目标节点
    newEdge->setEndNode(tarNode);
    //将新连边加入起始节点的outbound
    if (curNode->addEdge(newEdge) == false)
    {
        return false;
    }
    //将新连边加入目标节点的Inbound
    if (tarNode->registerInbound(newEdge) == false)
    {
        return false;
    }
    //全部完成后将连边加入网络连边列表
    m_edgeList.push_back(newEdge);

    return true;
}

bool Graph::removeEdge(std::shared_ptr<Node> curNode, std::shared_ptr<Node> tarNode)
{
	std::shared_ptr<Edge> curEdge;
    //从起始节点的出边中遍历
    for (auto edge : curNode->getOutBound())
    {//查找终止节点编号和目标节点编号一致的连边
        if (edge->getEndNode()->getNodeNum() == tarNode->getNodeNum())
        {//找到则返回，本图数据库不支持两点间多连边
            curEdge = edge;
            break;
        }
    }
    if (NULL == curEdge.get())
    {//没找到直接返回
        return false;
    }
    //起始节点Outbound中移除连边
    curNode->removeEdge(curEdge);
    //从终止节点InBound中注销连边
    tarNode->unRegisterInbound(curEdge);
    //全部完成后，从总连边列表中移除该边
	{
		EdgeList::iterator iter = m_edgeList.begin();
		for(; m_edgeList.end() != iter; ++iter)
		{
			if((*iter)->getEdgeNum() == curEdge->getEdgeNum())
			{
				m_edgeList.erase(iter);
				break;
			}
		}
	}

    return true;
}

bool Graph::clearUnusedEdge(EdgeList& unusedList)
{
	//将入参列表中所有连边从总连边列表中删除
    for (auto edge : unusedList)
    {
        EdgeList::iterator iter = m_edgeList.begin();
		for(; m_edgeList.end() != iter; ++iter)
		{
			if((*iter)->equal(*(*iter)))
			{
				m_edgeList.erase(iter);
				break;
			}
		}
    }
    //清空入参列表本身内容
    unusedList.clear();

    return true;
}

std::shared_ptr<Edge> Graph::getEdgeByType(std::shared_ptr<Node> start, std::shared_ptr<Node> end, const string& sType)
{
	std::shared_ptr<Edge> res;

    res = start->getEdge(end->getNodeName(), end->getNodeType(), "Out");
    if (NULL == res.get())
    {
        return std::shared_ptr<Edge>();
    }
    if (sType == "")
    {
        return res;
    }
    if (res->getEdgeType() != sType)
    {
        return std::shared_ptr<Edge>();
    }

    return res;
}

