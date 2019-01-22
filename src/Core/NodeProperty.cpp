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

//将连边数据保存为xml格式
Element* NodeProperty::toXML(Document& doc)
{
	Element* curProperty = doc.createElement(m_strKey);         //创建连边元素
    Text* value_txt = NULL;

    //创建各属性的文本元素
    value_txt = doc.createTextNode(m_strValue);
    //向当前节点中加入各属性节点
    curProperty->appendChild(value_txt);

    return curProperty;
}







