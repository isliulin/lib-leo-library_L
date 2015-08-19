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
#include "Poco/ConsoleChannel.h"
#include "Poco/SplitterChannel.h"
#include "Poco/Message.h"
#include "Poco/Path.h"
#include <windows.h>
#include <iostream>
using namespace Poco;


string getCurrentFileName()
{
	char buffer[MAX_PATH];
	GetModuleFileNameA(NULL, buffer, MAX_PATH);
	return std::string(buffer);

}
	
void Logger_initiation( string t_file , unsigned int t_max_size )
{
	string file_size = format("%uM", t_max_size);

	AutoPtr<FileChannel> file_channel(new FileChannel());
	file_channel->setProperty("rotation", file_size);
	file_channel->setProperty("archive", "timestamp");
	file_channel->setProperty("path", t_file);
	AutoPtr<PatternFormatter> pattern_formatter(new PatternFormatter("Datetime: %Y/%n/%d-%L%H:%M:%S  %T%I Code line : %u \r\nPosition: %U\r\n%p: %t "));
	AutoPtr<FormattingChannel> formatter_channle(new FormattingChannel(pattern_formatter, file_channel));
	Logger::root().setChannel(formatter_channle);
	///- finish logger initiation
}




void Logger_init_v2(unsigned int t_max_size)
{
	string	i_filename = getCurrentFileName();
	i_filename += ".log";
	string file_size = format("%uM", t_max_size);

	AutoPtr<SplitterChannel> splitterChannel(new SplitterChannel());
	AutoPtr<Channel> consoleChannel(new ConsoleChannel());
	AutoPtr<FileChannel> rotatedFileChannel(new FileChannel("rotated.log"));

	rotatedFileChannel->setProperty("rotation", file_size);
	rotatedFileChannel->setProperty("path", i_filename);
	rotatedFileChannel->setProperty("archive", "timestamp");

	splitterChannel->addChannel(consoleChannel);
	splitterChannel->addChannel(rotatedFileChannel);

	AutoPtr<Formatter> formatter(new PatternFormatter("Datetime: %Y/%n/%d-%L%H:%M:%S  thread: %T%I Code line : %u \r\nPosition: %U\r\n%p: %t "));
	AutoPtr<Channel> formattingChannel(new FormattingChannel(formatter, splitterChannel));
	Logger::root().setChannel(formattingChannel);

}

void Setup_logger( unsigned int t_max_size /*= 10 */ )
{
	static bool b_setup = false;		// only allow run once time 
	if (!b_setup)
	{
		b_setup = true;
		if (t_max_size ==0 )
		{
			t_max_size = 10;
		}
		Logger_init_v2(t_max_size);
	}
}
