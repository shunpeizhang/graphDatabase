#pragma once



#include <string>
#include <vector>



#include "../Common/TypeDef.h"


using std::string;




namespace GraphDB{
namespace Parser{


class MatchRule
{
public:
	MatchRule(const string& sSub, ErrorCode& err);


	bool matchType(const string& sType);
	bool matchCount(int iNodes);

	string getDirection(){ return m_strDirection; }

protected:
	void buildMatchLimit(const string& strLimit);

private:
	string m_strDirection; //"IN","OUT"
	bool m_isAll;
	std::vector<string> m_labels;//:label1|:label2
	bool m_bNumLimit;
	int m_uplimit;
	int m_downlimit;
};









}
}

