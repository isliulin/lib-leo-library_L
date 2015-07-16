#include "StdAfx.h"
#include "SesFactory.h"
#include "tcp_socket.h"
#include "udp_socket.h"
#ifndef BOOST_ASIO_DISABLE_IOCP 
#include "SerialPortEx.h"
#endif


SesFactory::SesFactory(void)
{
}

SesFactory::~SesFactory(void)
{
}

ISession* SesFactory::Create( _LINK_TYPE T, boost::asio::io_service& ios )
{
	switch (T)
	{
	case TYPE_SERIAL_PORT:
#ifndef BOOST_ASIO_DISABLE_IOCP 
		return new CSerialPortEx(ios);
#endif
		break;
	case TYPE_TCP:
		return new CTCPSocket(ios); 
		break;
	case TYPE_UDP:
		return new CUDPSocket(ios);
		break;
	default:
		break;
	}
	ASSERT(0);
	return NULL;
}
