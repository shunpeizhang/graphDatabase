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
    //ȡ���ƶ���ǩ��Inner Text
    newType = getText(xNode, "Type");
    if (newType == "")
    {
        newType = "����";
    }
    newValue = getText(xNode, "Value");
    if (newValue == "")
    {
        newValue = "1";
    }

    //��ֵ���ʼ��
    m_iEdgeNum = intMaxEdgeNum;
    m_strEdgeType = newType;
    m_strEdgeValue= newValue;
}

Edge::~Edge()
{

}

Element* Edge::toXML(Document& doc)
{
	Element* curEdge = doc.createElement("Edge");//��������Ԫ��
	
	//�ڵ�����
	Element *type_xml = NULL, *value_xml = NULL, *Start_xml = NULL, *End_xml = NULL;
	{		
		type_xml = doc.createElement("Type");
		value_xml = doc.createElement("Value");
		//�ڵ�λ��
		Start_xml = doc.createElement("Start");
		End_xml = doc.createElement("End");	
	}

	//���������Ե��ı�Ԫ��
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

	 //������Ԫ�ظ����ı�����
    type_xml->appendChild(type_txt);                                    
    value_xml->appendChild(value_txt);
    Start_xml->appendChild(Start_txt);
    End_xml->appendChild(End_txt);   
    
   
    //��ǰ�ڵ��м�������Խڵ�
    curEdge->appendChild(type_xml);                                   
    curEdge->appendChild(value_xml);
    curEdge->appendChild(Start_xml);
    curEdge->appendChild(End_xml);

    return curEdge;
}

string Edge::getText(Poco::XML::Element& curNode, const string& sLabel)
{
    //������ǰXML�������ӱ�ǩ
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






