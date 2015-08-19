#pragma once
#include "ISession.h"

using namespace boost::asio;
using boost::asio::ip::tcp;

class CTCPSocket : public ISession
{
public:
	CTCPSocket(boost::asio::io_service& ios)	
		: ISession(ios), socket_(ios)
		, is_connect_(false), stopped_(false) {
	}

	~CTCPSocket(){
// 		if (!stopped_)
// 			close();
	}
public:
	virtual bool is_connect() ;
	virtual bool open( const S_CONNECT_INFO& info ) ;
	virtual void close() ;
	virtual void write(char* buff, int len) ;
private:
	void connect_timeout(const boost::system::error_code& error);
	bool start_connect();
	void start_read();
	void handle_connect(const boost::system::error_code & ec);
	void handle_read(const boost::system::error_code& ec, std::size_t bytes);
	void handle_write(const boost::system::error_code& error);
	void Is_connect(bool val);
private:
	bool stopped_;
	bool is_connect_;
	tcp::socket socket_;
	char read_buf_[1024];
	char write_buf_[1024];
};

