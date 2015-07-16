#include "StdAfx.h"
#include "ISession.h"

bool S_CONNECT_INFO::equal(const S_CONNECT_INFO& sec )
{
	if (this->type_ != sec.type_)
		return false;

	switch (type_)
	{
	case TYPE_SERIAL_PORT:
		if (serial_port_ == sec.serial_port_)
			return true;
		break;
	case TYPE_TCP:
	case TYPE_UDP:
		if (ip_ == sec.ip_ && ip_port_ == sec.ip_port_)
			return true;
		break;
	}
	return false;
}

S_CONNECT_INFO::S_CONNECT_INFO( _LINK_TYPE linkType, const std::string& ip, unsigned short ip_port, unsigned short serial_port )
{ 
	this->type_ = linkType;
	this->ip_ = ip;
	this->ip_port_ = ip_port;
	this->serial_port_ = serial_port;
}

void ISession::handle_receive( char* buff, int len )
{
	this->p_observer_->handle_receive(buff, len);
}

void ISession::handle_connect(int error_code)
{
	this->p_observer_->handle_connect(error_code);
}
