#pragma once
#include <string>
#include <sstream>
#include <iostream>
using namespace std;


class PrintTableAlignment
{
public:
	static std::string centerAlignment(const string t_context, const unsigned int t_width);

	static std::string rightAlignment(const string t_context, const unsigned int t_width);

	static std::string leftAlignment(const string t_context, const unsigned int t_width);

};



inline std::string PrintTableAlignment::centerAlignment(const string t_context, const unsigned int t_width)
{

	stringstream i_result_context, i_spaces;

	if (t_width < (int)(t_context.size() - 2))
		/// context size is out range 
	{
		i_spaces << " ";
		string i_sub = t_context.substr(0, t_context.size() - 2);
		i_result_context << i_spaces.str() << i_sub << i_spaces.str();
	}

	else  // context if fitful 
	{
		int padding = t_width - t_context.size();
		for (int i = 0; i<padding / 2; ++i)
			i_spaces << " ";

		i_result_context << i_spaces.str() << t_context << i_spaces.str();

		// format with padding
		if (padding>0 && padding % 2 != 0)
			// if odd #, add 1 space
			i_result_context << " ";
	}
	return i_result_context.str();
}

inline std::string PrintTableAlignment::rightAlignment(const string t_context, const unsigned int t_width)
{
	stringstream i_result_context, i_spaces;
	if (t_width < (int)(t_context.size() - 2))
		/// context size is out range 
	{
		i_spaces << " ";
		string i_sub = t_context.substr(0, t_context.size() - 2);
		i_result_context << i_spaces.str() << i_sub << i_spaces.str();
	}
	else  // context if fitful 
	{
		int padding = t_width - t_context.size() - 1;
		for (int i = 0; i < padding; i++)
		{
			i_spaces << " ";
		}
		i_result_context << i_spaces.str() << t_context << " ";
	}
	return i_result_context.str();
}

inline std::string PrintTableAlignment::leftAlignment(const string t_context, const unsigned int t_width)
{

	stringstream i_result_context, i_spaces;
	if (t_width < (int)(t_context.size() - 2))
		/// context size is out range 
	{
		i_spaces << " ";
		string i_sub = t_context.substr(0, t_context.size() - 2);
		i_result_context << i_spaces.str() << i_sub << i_spaces.str();
	}

	else  // context if fitful 
	{
		int padding = t_width - t_context.size() - 1;
		i_result_context << " " << t_context;
		for (int i = 0; i < padding; i++)
		{
			i_spaces << " ";
		}
		i_result_context << i_spaces.str();
	}
	return i_result_context.str();
}

