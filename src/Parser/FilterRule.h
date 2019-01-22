#pragma once




#include <string>
#include <vector>


#include "../Common/TypeDef.h"
#include "../Core/Node.h"


using std::string;




namespace GraphDB{
namespace Parser{



class FilterRule
{
public:
	FilterRule(const string& strSub, ErrorCode& err);

	bool filtrate(std::shared_ptr<Core::Node> curNode);


	string getName(){ return m_strName; }
	string getProperty(){ return m_strProperty; }
	string getOp(){ return m_strOp; }
	string getValue(){ return m_strValue; }


private:
	string m_strName;
	string m_strProperty;
	string m_strOp;
	string m_strValue;
};









}
}


