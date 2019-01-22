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
	string m_strName;//���ƣ���MATCH���ָ��
    int m_iLevel;//��Σ�������״ͼ����ȣ���MATCH���ָ��
    bool m_bIsFinalResult;//��RETURN���ָ�������Ϊtrue����strProperty����ʼ��
    bool m_bIsAll;
    std::vector<string> m_strProperty;//��RETURN���ָ��������������ݱ�ǩ
};









}
}

