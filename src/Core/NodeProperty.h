#pragma once


#include <string>


#include "../Common/TypeDef.h"


using std::string;



namespace GraphDB {
namespace Core
{


class NodeProperty
{
public:
	NodeProperty(const string& sKey, const string& sValue);
	//�ڵ�������NodeProperty���캯��
	NodeProperty(Poco::XML::Element& xNode);
	virtual ~NodeProperty();

	//���������ݱ���Ϊxml��ʽ
	Poco::XML::Element* toXML(Poco::XML::Document& doc);

	string getKey() const { return m_strKey; }
	void setKey(const string& key) { m_strKey = key; }

	string getValue() const { return m_strValue; }
	void setValue(const string& value) { m_strValue = value; }

private:
	string m_strKey;
    string m_strValue;
};















}
}

