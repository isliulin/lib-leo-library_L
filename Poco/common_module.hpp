#include "stdafx.h"
#include "Poco/NumberParser.h"
using namespace Poco;




string Split_string_to_hex(const string & t_data);

	
	
	
	

	
	
	

inline string Split_string_to_hex(const string & t_text)
{
	string i_text = Poco::toUpper<string>(t_text);
	i_text.append("  ");
	vector<string>  i_list;
	int size_count = 0;

	for (int i = 0; i < i_text.size(); i++)
	{
		if (i_text.at(i) != ' ')
		{
			size_count++;

			if (size_count == 2)
				/// condition 1: find two 
			{
				string i_sub = i_text.substr(i - 1, 2);
				i_list.push_back(i_sub);
				size_count = 0;		// reset count 
			}
		}
		else if (i_text.at(i) == ' ')
		{
			if (size_count != 0)
			{
				string i_sub = i_text.substr(i - 1, 1);
				i_list.push_back(i_sub);
				size_count = 0;		// reset count 
			}
		}
	}

	typedef unsigned char u_char;
	int nsize = i_list.size();
	int valid_count = 0;
	char * parray = new char[nsize];

	for (int i = 0; i < nsize; i++)
	{
		unsigned int item;
		if (NumberParser::tryParseHex(i_list.at(i), item))
		{
			unsigned char i_byte = (unsigned char)item;
			parray[i] = (char)i_byte;
			valid_count++;
		}
	}
	string i_result(parray, valid_count);
	delete parray;

	return i_result;
}
