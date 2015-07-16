#pragma once
#include "ISession.h"

class SesFactory
{
public:
	SesFactory(void);
	~SesFactory(void);
public:
	static ISession* Create(_LINK_TYPE T, boost::asio::io_service& ios);
};
