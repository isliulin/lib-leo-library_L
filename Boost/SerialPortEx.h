#pragma once
#include "ISession.h"
#include "boost\asio\serial_port.hpp"

class CManager;

class CSerialPortEx : public ISession
{
public:
	CSerialPortEx(boost::asio::io_service& ios) : ISession(ios), serial_port_(ios), is_open_(false){}
	~CSerialPortEx(void) { }

	virtual bool is_connect();
	virtual bool open( const S_CONNECT_INFO& info );
	virtual void close();
	virtual void write( char* buff, int len );
protected:
	void handle_read(const boost::system::error_code& error, std::size_t bytes);
private:
	boost::asio::serial_port serial_port_;
	char write_buf_[1024];
	char read_buf_[1024];
	bool is_open_;
public:
	static bool is_online;
};
