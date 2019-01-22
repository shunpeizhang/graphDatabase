#include "MidResult.h"


#include <Poco/RegularExpression.h>


using namespace GraphDB::Parser;


MidResult::MidResult(const string& sName, int iLevel)
{
	m_strName = sName;
    m_iLevel = iLevel;
    m_bIsFinalResult = false;
}

void MidResult::setResult()
{
	m_bIsFinalResult = true;
}

void MidResult::setAll()
{
	m_bIsAll = true;
}

void MidResult::addProperty(const string& sLabel)
{
	m_strProperty.push_back(sLabel);
}






