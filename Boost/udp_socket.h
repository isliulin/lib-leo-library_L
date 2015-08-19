#pragma once
#include "ISession.h"
#include <boost/asio.hpp>

using namespace boost::asio;
using boost::asio::ip::udp;

class CUDPSocket : public ISession
{
public:
	CUDPSocket(boost::asio::io_service& ios) 
		: ISession(ios), socket_(ios), is_connect_(false), stopped_(false) {}
	~CUDPSocket() {}
public:
	virtual bool is_connect();
	virtual bool open( const S_CONNECT_INFO& info );
	virtual void close();
	virtual void write( char* buff, int len );
private:
	void handle_write(const boost::system::error_code& error, std::size_t bytes);
	void handle_read(const boost::system::error_code& error, std::size_t bytes);
private:
	bool stopped_;
	bool is_connect_;
	udp::socket socket_;
	char read_buf_[1024];
	char write_buf_[1024];
	udp::endpoint sender_endpoint_;
};
