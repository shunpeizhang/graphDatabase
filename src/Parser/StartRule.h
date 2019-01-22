#pragma once 

#include <string>
#include <vector>


#include "../Common/TypeDef.h"


using std::string;




namespace GraphDB{
namespace Parser{



class StartRule
{
	typedef std::vector<string> SubRule;
public:
	StartRule(const string& strSub, ErrorCode& err);

	bool match(std::shared_ptr<Core::Node> tarNode);


protected:
	void fillListIndex(const string& sSub);
	void fillListName(const string& sSub);
	bool matchNumber(const string& strRule, int index);
	bool matchNameType(const string& strRule, const string& sName, const string& sType);


private:
	bool m_isName;//"Index", "NameType"
    bool m_isAll;
    SubRule m_subRule;//×Ó¹æÔò
};














}
}


