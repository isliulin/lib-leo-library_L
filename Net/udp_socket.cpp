#include "StdAfx.h"
#include "udp_socket.h"
#include <boost/bind.hpp>
#include <iostream>

bool CUDPSocket::is_connect()
{
	return is_connect_;
}

bool CUDPSocket::open( const S_CONNECT_INFO& info )
{
	try {
		if (stopped_)
			close();
		sender_endpoint_ = udp::endpoint(boost::asio::ip::address::from_string(info.ip_), info.ip_port_);
		Ip(info.ip_);
		Port(info.ip_port_);
		socket_.open(udp::v4());
		socket_.async_receive_from(
			boost::asio::buffer(read_buf_), sender_endpoint_,
			boost::bind(&CUDPSocket::handle_read, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
		is_connect_ = true;
	}
	catch(std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return false;
	}
	return true;
}

void CUDPSocket::close()
{
	is_connect_ = false;
	stopped_ = true;
	boost::system::error_code ignored_ec;
	socket_.cancel(ignored_ec);
	socket_.close(ignored_ec);
}

void CUDPSocket::write( char* buff, int len )
{
	if (stopped_)
		return;
	if ( len >= 1024 )
		return;
	memcpy(write_buf_, buff, len);

	socket_.async_send_to(boost::asio::buffer(write_buf_, len), sender_endpoint_,
		boost::bind(&CUDPSocket::handle_write,
		this, 
		boost::asio::placeholders::error,
		boost::asio::placeholders::bytes_transferred));
}

void CUDPSocket::handle_write( const boost::system::error_code& error, std::size_t bytes )
{
}

void CUDPSocket::handle_read( const boost::system::error_code& error, std::size_t bytes )
{
	if (stopped_)
		return;

	if (!error && bytes > 0)
	{
		handle_receive(read_buf_, bytes);
	}

	try
	{
		socket_.async_receive_from(
			boost::asio::buffer(read_buf_), sender_endpoint_,
			boost::bind(&CUDPSocket::handle_read, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred) );
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
}
