#pragma once


#include <string>


#include "../Core/Graph.h"




using std::string;


namespace GraphDB{
namespace IO{


class IfIOStrategy
{
public:
	virtual ~IfIOStrategy(){}

	virtual std::shared_ptr<Core::Graph> readFile(ErrorCode err) = 0;
	virtual void saveFile(Poco::XML::Document* doc, ErrorCode err) = 0;

	virtual string getPath() = 0;
	virtual void setPath(const string& path) = 0;
};





}
}

