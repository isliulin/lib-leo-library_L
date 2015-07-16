///////////////////////////////////////////////////////////////
/*
File name 	:   
Author 		: 	LEO
Date 		:	2015-3-27
Email  		:	lion_117@126.com
Description:

All Rights Reserved 
*/
///////////////////////////////////////////////////////////////


#include "StdAfx.h"
#include "logger_recorder.h"
#include "Poco/PatternFormatter.h"
#include "Poco/FormattingChannel.h"
#include "Poco/FileChannel.h"
#include "Poco/AutoPtr.h"
#include "Poco/Format.h"

using namespace Poco;


	
void Logger_initiation( string t_file , unsigned int t_max_size )
{
	string file_size = format("%uM", t_max_size);

	AutoPtr<FileChannel> file_channel(new FileChannel());
	file_channel->setProperty("rotation", file_size);
	file_channel->setProperty("archive", "timestamp");
	file_channel->setProperty("path", t_file);
	AutoPtr<PatternFormatter> pattern_formatter(new PatternFormatter("Date :%y-%n-%d Time: %L%H:%M:%S \r\nPosition: %U\r\nCode line : %u\r\n%p: %t"));
	AutoPtr<FormattingChannel> formatter_channle(new FormattingChannel(pattern_formatter, file_channel));
	Logger::root().setChannel(formatter_channle);
	///- finish logger initiation
}

void Setup_logger( string t_file /*= "info.log" */, unsigned int t_max_size /*= 10 */ )
{
	static bool b_setup = false;		// only allow run once time 
	if (!b_setup)
	{
		b_setup = true;
		if (t_max_size ==0 )
		{
			t_max_size = 10;
		}
		Logger_initiation(t_file, t_max_size);
	}
}
