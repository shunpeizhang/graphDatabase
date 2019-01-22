#pragma once



#include "IfIOStrategy.h"


namespace GraphDB{
namespace IO{


class XMLStrategy: public IfIOStrategy
{
public:
	XMLStrategy(const string& path){ m_strPath = path; }

	std::shared_ptr<Core::Graph> readFile(ErrorCode err);

	void saveFile(Poco::XML::Document* doc, ErrorCode err);

	string getPath(){ return m_strPath; }
	void setPath(const string& path){ m_strPath = path; }

private:
	string m_strPath;
};







}
}
