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

#pragma once
#include "Poco/Logger.h"

using  namespace Poco;					// for global decorator  


#define logger_handle   (Logger::get("logger"))
#define log_info_bound_check ("bound check error!")


void Logger_initiation(string t_file  , unsigned int t_max_size);

void Setup_logger(string t_file = "info.log" , unsigned int t_max_size = 10 );

