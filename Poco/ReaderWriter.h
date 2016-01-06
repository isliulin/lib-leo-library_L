#pragma once

#include <iostream>
#include <string>
using namespace std;

#include "Poco/FileStream.h"
using namespace Poco;

class STLReader : public FileInputStream
{
	// ios::trunc , ios::binary , ios::in 

public:
	string readAll()
	{
		this->seekg(0, this->end);
		long length = this->tellg();
		this->seekg(0, this->beg);

		char * buffer = new char[length];
		// read data as a block:
		this->read(buffer, length);
		delete[] buffer;
		return string(buffer, length);
	}

};
