#include "StdAfx.h"
#include "tcp_socket.h"
#include <boost/bind.hpp>
#include <iostream>

bool CTCPSocket::open(const S_CONNECT_INFO& info) 
{
	try
	{
		if (is_connect_)
			return false;
		Ip(info.ip_);
		Port(info.ip_port_);
		return start_connect();
	}
	catch(std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return false;
	}
	return true;
}

void CTCPSocket::connect_timeout(const boost::system::error_code& error)
{
	if (stopped_)
		return;
	if (is_connect_)
		return;
	if (error && !is_connect())
	{
		close();
		start_connect();
	}
}

bool CTCPSocket::start_connect()
{
	if (stopped_)
		return false;
	try
	{
		//		stopped_ = false;
		// 		try
		// 		{
		//			socket_.open(boost::asio::ip::tcp::v4());
		// 			tcp::resolver resolver(ios_);
		// 			tcp::resolver::query query(boost::asio::ip::host_name(), "");
		// 			tcp::resolver::iterator iter = resolver.resolve(query);
		// 			tcp::resolver::iterator end;
		// 			while (iter != end)
		// 			{
		// 				tcp::endpoint ep = *iter++;
		// 				if (ep.address().is_v4()) {
		// 					socket_.bind(ep);
		// 					break;
		// 				}
		// 			}
		// 		}
		// 		catch(std::exception& e)
		// 		{
		// 			std::cerr << e.what() << std::endl;
		// 		}

		tcp::endpoint send_endpoint_ (boost::asio::ip::address_v4::from_string(Ip()), Port());
		socket_.async_connect(send_endpoint_,
			boost::bind(&CTCPSocket::handle_connect, this,
			boost::asio::placeholders::error));
	}
	catch(std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return false;
	}
	return true;
}

void CTCPSocket::close()
{
	stopped_ = true;
	Is_connect(false);
	boost::system::error_code ignored_ec;
	// 	try
	// 	{
	socket_.cancel(ignored_ec);
	if (socket_.is_open())
	{
		socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);
	}
	socket_.close(ignored_ec);
	// 	}
	// 	catch (std::exception& ex)
	// 	{
	// 		std::cerr << ex.what() << std::endl;
	// 		return ;
	// 	}
}

bool CTCPSocket::is_connect()
{
	return is_connect_;
}

void CTCPSocket::handle_connect( const boost::system::error_code & ec )
{
	if (stopped_ || this == NULL)
		return;

	try
	{
		if (!socket_.is_open()) 
		{
			start_connect();
		}
		else if (ec) 
		{
			boost::system::error_code ignored_ec;
			socket_.close(ignored_ec);
			if (ec.value() != boost::system::errc::operation_canceled)
			{
#ifdef AUTO_RECONNECT 
				start_connect();
#else 
				stopped_ = true;
				Is_connect(false);
				ISession::handle_connect(ec.value());
#endif
			}
		}
		else 
		{
			std::cout << "Connected to " << Ip() << ":" << (unsigned short)Port() << "\n";
			Is_connect(true);
			socket_.set_option(boost::asio::ip::tcp::no_delay(true));
			socket_.set_option(boost::asio::ip::tcp::acceptor::linger(true, 0));
			//socket_.set_option(boost::asio::socket_base::keep_alive(true));
			start_read();
			ISession::handle_connect(ec.value());
		}
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
}

void CTCPSocket::start_read()
{
	try
	{
		socket_.async_read_some(boost::asio::buffer(read_buf_, 1024), 
			boost::bind(&CTCPSocket::handle_read, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
	}
	catch(std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return ;
	}
}

void CTCPSocket::handle_read( const boost::system::error_code& ec, std::size_t bytes )
{
	if (stopped_)
		return;

	try
	{
		if (!ec)
		{
			if (bytes > 0)
			{
				ISession::handle_receive(read_buf_, bytes);
			}

			start_read();
		}
		else 
		{
			close();
#ifdef AUTO_RECONNECT 
			start_connect();
#else 
			ISession::handle_connect(ec.value());
#endif
		}
	}
	catch(std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return ;
	}
}

void CTCPSocket::write( char* buff, int len )
{
	if (stopped_)
		return;

	if (!is_connect() || !socket_.is_open())
		return;
	if ( len >= 1024 )
		return;
	memcpy(write_buf_, buff, len);
	try
	{
		socket_.async_send(boost::asio::buffer(write_buf_, len),
			boost::bind(&CTCPSocket::handle_write, this, boost::asio::placeholders::error));
	}
	catch(std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
}

void CTCPSocket::handle_write( const boost::system::error_code& error )
{

}

void CTCPSocket::Is_connect( bool val )
{
	is_connect_ = val;
}

