#include "NodeProperty.h"



using namespace GraphDB::Core;

using Poco::XML::Document;
using Poco::XML::Element;
using Poco::XML::Text;
using Poco::XML::AutoPtr;
using Poco::XML::DOMWriter;
using Poco::XML::XMLWriter;


NodeProperty::NodeProperty(const string& sKey, const string& sValue)
{
	m_strKey = sKey;
    m_strValue = sValue;
}


NodeProperty::NodeProperty(Poco::XML::Element& xNode)
{
	m_strKey = xNode.nodeName();
    m_strValue = xNode.innerText();
}

NodeProperty::~NodeProperty()
{

}

//���������ݱ���Ϊxml��ʽ
Element* NodeProperty::toXML(Document& doc)
{
	Element* curProperty = doc.createElement(m_strKey);         //��������Ԫ��
    Text* value_txt = NULL;

    //���������Ե��ı�Ԫ��
    value_txt = doc.createTextNode(m_strValue);
    //��ǰ�ڵ��м�������Խڵ�
    curProperty->appendChild(value_txt);

    return curProperty;
}







