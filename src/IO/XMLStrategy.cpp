#include "XMLStrategy.h"
#include "Poco/DOM/DOMParser.h"
#include "Poco/DOM/Document.h"
#include "Poco/DOM/NodeIterator.h"
#include "Poco/DOM/NodeFilter.h"
#include "Poco/DOM/AutoPtr.h"
#include "Poco/SAX/InputSource.h"
#include "Poco/Exception.h"


#include <iostream>
#include <fstream>





using namespace GraphDB::IO;


std::shared_ptr<GraphDB::Core::Graph> XMLStrategy::readFile(ErrorCode err)
{
	std::ifstream ifs;
	Poco::XML::AutoPtr<Poco::XML::Document> doc;
	std::shared_ptr<Core::Graph> newGraph;

	try
	{
		Poco::XML::InputSource src(ifs);
		ifs.open (m_strPath, std::ifstream::in);
		if(!ifs.good())
		{
			err = OpenFileFailed;
			return newGraph;
		}

		Poco::XML::DOMParser parser;
		Poco::XML::AutoPtr<Poco::XML::Document> pDoc = parser.parse(&src);
		if(NULL == pDoc.get())
		{
			err = OpenFileFailed;
			return newGraph;
		}

		ifs.close();
	}
	catch (Poco::Exception ex)
	{
		err = OpenFileFailed;
		return newGraph;
	}

	newGraph = std::make_shared<Core::Graph>(*doc.get(), err);
	
	return newGraph;
}

void XMLStrategy::saveFile(Poco::XML::Document* doc, ErrorCode err)
{
    try
    {
		std::ofstream ofs;
		ofs.open (m_strPath, std::ofstream::out | std::ofstream::app);
		if(!ofs.good())
		{
			err = SaveFileFailed;
			return;
		}

		Poco::XML::AutoPtr<Poco::XML::Document> pDoc = doc;
		Poco::XML::DOMWriter writer;
		writer.writeNode(ofs, pDoc);
        ofs.close();
    }
    catch (Poco::Exception ex)
    {        
        err = SaveFileFailed;
    }
}





