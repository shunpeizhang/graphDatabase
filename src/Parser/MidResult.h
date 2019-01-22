#pragma once


#include <string>
#include <vector>


#include "../Common/TypeDef.h"


using std::string;



namespace GraphDB{
namespace Parser{


class MidResult
{
public:
	MidResult(const string& sName, int iLevel);

	void setResult();
	void setAll();
	void addProperty(const string& sLabel);


	string getName(){ return m_strName; }
	int getLevel(){ return m_iLevel; }
	bool getIsFinalResult(){ return m_bIsFinalResult; }
	bool getIsAll(){ return m_bIsAll; }
	const std::vector<string>& getProperty(){ return m_strProperty; }

private:
	string m_strName;//名称，由MATCH语句指定
    int m_iLevel;//层次，处于树状图的深度，由MATCH语句指定
    bool m_bIsFinalResult;//由RETURN语句指定，如果为true，则strProperty被初始化
    bool m_bIsAll;
    std::vector<string> m_strProperty;//由RETURN语句指定，待输出的内容标签
};









}
}

