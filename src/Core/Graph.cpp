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

	//ȡ�����ڵ�
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

	//����node
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
				//�����½ڵ�
				std::shared_ptr<Node> newNode = std::make_shared<Node>(m_iMaxNodeNum, *xNode);
				m_iMaxNodeNum++;
				//����ͼ
				m_nodeList.push_back(newNode);
			}
		}
	}	

	//����Edge
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

				//����������
				newEdge = std::make_shared<Edge>(m_iMaxEdgeNum, *xNode);
				//��ȡ���ߵ���ʼ����ֹ�ڵ���
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
				//����ͼ
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
    //�����������ݶ�����Ϊxml��ʽ
	Poco::XML::Element* root = doc->createElement("Graph");
    Poco::XML::Element *Nodes = NULL, *Edges = NULL;

    adjustNodeIndex();
    Nodes = doc->createElement("Nodes");
	snprintf(buf, sizeof(buf), "%d", m_iMaxNodeNum);
    Nodes->setAttribute("NodeNumber", buf);
    for (auto curNode : m_nodeList)
    {
        Nodes->appendChild(curNode->toXML(*doc));     //ѭ�����õײ�ڵ���������
    }
    root->appendChild(Nodes);
    Edges = doc->createElement("Edges");
	snprintf(buf, sizeof(buf), "%d", m_iMaxEdgeNum);
    Edges->setAttribute("EdgeNumber", buf);
    for (auto curEdge : m_edgeList)
    {
        Edges->appendChild(curEdge->toXML(*doc)); //ѭ�����õײ�ڵ���������
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
	//�����ڵ��б�
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
    //�����ڵ��б�
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
    //�����ڵ��б�
    for (auto curNode : m_nodeList)
    {
        if (curNode->getNodeType() == type)
        {//������typeҪ��Ľڵ���뷵�ؽ���б�
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

    //���ڵ��Ƿ��Ѿ����ڡ�����+����һ�¡�
    if (NULL != getNodesByNameAndType(sName, sType).get())
    {
        err = NodeExists;
        return;
    }
    //�����µĽڵ�
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

    //���ڵ��Ƿ��Ѿ����ڡ�����+����һ�¡�
    if (NULL != getNodesByNameAndType(oriNode->getNodeName(), oriNode->getNodeType()).get())
    {
        err = NodeExists;
        return;
    }
    //�����µĽڵ�
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
    //��ȡ��ʼ�ڵ㣬�����ڱ���
    startNode = getNodesByNameAndType(sStartName, sStartType);
    if (NULL == startNode.get())
    {
        err = NodeNotExists;
        return;
    }
    //��ȡ��ֹ�ڵ㣬�����ڱ���
    endNode = getNodesByNameAndType(sEndName, sEndType);
    if (NULL == endNode.get())
    {
        err = NodeNotExists;
        return;
    }
    //����������Ƿ������ͬ���͹�ϵ�����ڱ���
    if (NULL != getEdgeByType(startNode, endNode, sType).get())
    {
        err = EdgeExists;
        return;
    }
    //����������
	newEdge = std::make_shared<Edge>(m_iMaxEdgeNum, sType, sValue);
    if (newEdge == NULL)
    {
        err = CreateEdgeFailed;
        return;
    }
    m_iMaxEdgeNum++;
    //�����������±�
    addEdge(startNode, endNode, newEdge);
    err = NoError;

    return;
}

void Graph::modifyNode(const string& sName, const string& sType,
                    ModifyOperation opt, const string& sProperities, ErrorCode err)
{
	std::shared_ptr<Node> tarNode;

    tarNode = getNodesByNameAndType(sName, sType);
    //���ڵ��Ƿ��Ѿ����ڡ�����+����һ�¡�
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
    //��ȡ��ʼ�ڵ㣬�����ڱ���
    startNode = getNodesByNameAndType(sStartName, sStartType);
    if (NULL == startNode.get())
    {
        err = NodeNotExists;
        return;
    }
    //��ȡ��ֹ�ڵ㣬�����ڱ���
    endNode = getNodesByNameAndType(sEndName, sEndType);
    if (NULL == endNode.get())
    {
        err = NodeNotExists;
        return;
    }
    //����������Ƿ������ͬ���͹�ϵ�������ڱ���
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
    //���ڵ��Ƿ��Ѿ����ڡ�����+����һ�¡�
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
    //��ȡ��ʼ�ڵ㣬�����ڱ���
    startNode = getNodesByNameAndType(sStartName, sStartType);
    if (NULL == startNode.get())
    {
        err = NodeNotExists;
        return;
    }
    //��ȡ��ֹ�ڵ㣬�����ڱ���
    endNode = getNodesByNameAndType(sEndName, sEndType);
    if (NULL == endNode.get())
    {
        err = NodeNotExists;
        return;
    }
    //����������Ƿ������ͬ���͹�ϵ�����ڱ���
    tarEdge = getEdgeByType(startNode, endNode, sType);
    if (NULL == tarEdge.get())
    {
        err = EdgeNotExists;
        return;
    }
    //��ʼ�ڵ�Outbound���Ƴ�����
    startNode->removeEdge(tarEdge);
    //����ֹ�ڵ�InBound��ע������
    endNode->unRegisterInbound(tarEdge);
    //ȫ����ɺ󣬴��������б����Ƴ��ñ�
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

	//������ǰXML�������ӱ�ǩ
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
	//����ڵ���������
    clearUnusedEdge(*curNode->clearEdge());

    //�ӽڵ��б����Ƴ��ڵ�
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
	//���ߵ�ͷָ��ָ����ڵ�
    newEdge->setStartNode(curNode);
    //���ߵ�βָ��ָ��Ŀ��ڵ�
    newEdge->setEndNode(tarNode);
    //�������߼�����ʼ�ڵ��outbound
    if (curNode->addEdge(newEdge) == false)
    {
        return false;
    }
    //�������߼���Ŀ��ڵ��Inbound
    if (tarNode->registerInbound(newEdge) == false)
    {
        return false;
    }
    //ȫ����ɺ����߼������������б�
    m_edgeList.push_back(newEdge);

    return true;
}

bool Graph::removeEdge(std::shared_ptr<Node> curNode, std::shared_ptr<Node> tarNode)
{
	std::shared_ptr<Edge> curEdge;
    //����ʼ�ڵ�ĳ����б���
    for (auto edge : curNode->getOutBound())
    {//������ֹ�ڵ��ź�Ŀ��ڵ���һ�µ�����
        if (edge->getEndNode()->getNodeNum() == tarNode->getNodeNum())
        {//�ҵ��򷵻أ���ͼ���ݿⲻ֧������������
            curEdge = edge;
            break;
        }
    }
    if (NULL == curEdge.get())
    {//û�ҵ�ֱ�ӷ���
        return false;
    }
    //��ʼ�ڵ�Outbound���Ƴ�����
    curNode->removeEdge(curEdge);
    //����ֹ�ڵ�InBound��ע������
    tarNode->unRegisterInbound(curEdge);
    //ȫ����ɺ󣬴��������б����Ƴ��ñ�
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
	//������б����������ߴ��������б���ɾ��
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
    //�������б�������
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

