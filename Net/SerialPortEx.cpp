#include "StdAfx.h"
#include "SerialPortEx.h"
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <iostream>
#include "boost/asio/basic_deadline_timer.hpp"

bool CSerialPortEx::is_online = true;
bool CSerialPortEx::is_connect()
{
	return is_open_;
}

bool CSerialPortEx::open( const S_CONNECT_INFO& info )
{
	try
	{
		if (is_open_)
			close();
		char szPort[50];
		memset(szPort, 0, 45);
		sprintf_s(szPort, "COM%d", info.serial_port_);
		serial_port_.open(szPort);
		serial_port_.set_option(boost::asio::serial_port::baud_rate(38400));
		serial_port_.set_option(boost::asio::serial_port::parity(boost::asio::serial_port::parity::none));
		serial_port_.set_option(boost::asio::serial_port::stop_bits(boost::asio::serial_port::stop_bits::one));
		serial_port_.set_option(boost::asio::serial_port::character_size(8));
		Sleep(100);
		try
		{
			serial_port_.async_read_some( boost::asio::buffer(read_buf_), 
				boost::bind(&CSerialPortEx::handle_read, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred) );
		}
		catch (std::exception& e)
		{
			std::cerr << e.what() << std::endl;
		}
// 		serial_port_.async_read_some( boost::asio::buffer(read_buf_), 
// 			boost::bind(&CSerialPortEx::handle_read, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred) );
		is_open_ =  true;
		return true;
	}
	catch(std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
	return false;
}

void CSerialPortEx::close()
{
	if (is_open_)
	{
		is_open_ = false;
		//到期，应终止在串口上的读操作。
		boost::system::error_code ignored_err;
		serial_port_.cancel(ignored_err);  
		serial_port_.close();
	}
//	CSerialPort::ClosePort();
}

void CSerialPortEx::write( char* buff, int len )
{	
	if ( len >= 1024 )
		return;
	if (len == 5)
	{
		byte writebyte[5] = {0};
		memcpy(writebyte,buff,5);
		if (writebyte[0] == 0xfe&&writebyte[4] == 0xfc)
		{
			memcpy(write_buf_, buff, len);
			if (serial_port_.is_open())
			{
				try
				{
					serial_port_.write_some(boost::asio::buffer(write_buf_, len));
				}
				catch (std::exception& e)
				{
					std::cerr << e.what() << std::endl;
				}
			}
			else
			{
				return;
			}
		}	
	}
	
	return;
}

void CSerialPortEx::handle_read( const boost::system::error_code& error, std::size_t bytes )
{
	if (!error && bytes > 0 && is_open_)
	{
		handle_receive(read_buf_, bytes);
		try
		{
			serial_port_.async_read_some( boost::asio::buffer(read_buf_), 
				boost::bind(&CSerialPortEx::handle_read, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred) );
		}
		catch (std::exception& e)
		{
			std::cerr << e.what() << std::endl;
		}
	}
// 	if (!error && bytes > 0)
// 	{
// 		if (!is_open_)
// 		{
// 			return;
// 		}
// 		handle_receive(read_buf_, bytes);
// 		try
// 		{
// 			if (!is_open_)
// 			{
// 				return;
// 			}
// 			serial_port_.async_read_some( boost::asio::buffer(read_buf_,bytes), 
// 				boost::bind(&CSerialPortEx::handle_read, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred) );
// 		}
// 		catch (std::exception& e)
// 		{
// 			std::cerr << e.what() << std::endl;
// 		}
// 	}

}
