#include "Edge.h"



using namespace GraphDB::Core;

using Poco::XML::Document;
using Poco::XML::Element;
using Poco::XML::Text;
using Poco::XML::AutoPtr;
using Poco::XML::DOMWriter;
using Poco::XML::XMLWriter;


Edge::Edge(int intMaxEdgeNum, const string& newType, const string& value)
{
	m_iEdgeNum = intMaxEdgeNum;
    m_strEdgeType = newType;
    m_strEdgeValue= value;
}

Edge::Edge(int intMaxEdgeNum, Poco::XML::Element& xNode)
{
	string newType, newValue;

    newType = newValue = "";
    //取出制定标签的Inner Text
    newType = getText(xNode, "Type");
    if (newType == "")
    {
        newType = "关联";
    }
    newValue = getText(xNode, "Value");
    if (newValue == "")
    {
        newValue = "1";
    }

    //赋值与初始化
    m_iEdgeNum = intMaxEdgeNum;
    m_strEdgeType = newType;
    m_strEdgeValue= newValue;
}

Edge::~Edge()
{

}

Element* Edge::toXML(Document& doc)
{
	Element* curEdge = doc.createElement("Edge");//创建连边元素
	
	//节点类型
	Element *type_xml = NULL, *value_xml = NULL, *Start_xml = NULL, *End_xml = NULL;
	{		
		type_xml = doc.createElement("Type");
		value_xml = doc.createElement("Value");
		//节点位置
		Start_xml = doc.createElement("Start");
		End_xml = doc.createElement("End");	
	}

	//创建各属性的文本元素
	Text *type_txt = NULL, *value_txt = NULL, *Start_txt = NULL, *End_txt = NULL;
	{
		type_txt = doc.createTextNode(m_strEdgeType);
		value_txt = doc.createTextNode(m_strEdgeValue);

		char buf[128];
		snprintf(buf, sizeof(buf), "%d", m_nodeStart->getSaveIndex());
		Start_txt = doc.createTextNode(buf);

		snprintf(buf, sizeof(buf), "%d", m_nodeEnd->getSaveIndex());
		End_txt = doc.createTextNode(buf);
	}

	 //将标题元素赋予文本内容
    type_xml->appendChild(type_txt);                                    
    value_xml->appendChild(value_txt);
    Start_xml->appendChild(Start_txt);
    End_xml->appendChild(End_txt);   
    
   
    //向当前节点中加入各属性节点
    curEdge->appendChild(type_xml);                                   
    curEdge->appendChild(value_xml);
    curEdge->appendChild(Start_xml);
    curEdge->appendChild(End_xml);

    return curEdge;
}

string Edge::getText(Poco::XML::Element& curNode, const string& sLabel)
{
    //遍历当前XML的所有子标签
	Poco::XML::NodeList* nodes = curNode.childNodes();
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

bool Edge::equal(Edge& other)
{
	if(m_iEdgeNum == other.getEdgeNum())
	{
		return true;
	}

	return false;
}






