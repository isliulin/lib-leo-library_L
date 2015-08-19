#pragma once
#include <boost/asio.hpp>

enum _LINK_TYPE{TYPE_SERIAL_PORT, TYPE_TCP, TYPE_UDP};


struct S_CONNECT_INFO 
{
	S_CONNECT_INFO() {}
	S_CONNECT_INFO(_LINK_TYPE linkType, const std::string& ip, unsigned short ip_port, unsigned short serial_port);
	bool equal(const S_CONNECT_INFO& sec);

	_LINK_TYPE		type_;
	std::string		ip_;
	unsigned short	ip_port_;
	unsigned int	serial_port_;
};

class IIOObserver {
public:
	virtual void handle_receive(char* buff, int len) {}
	virtual void handle_connect(int error_code) {}
};

class ISession
{
public:
	ISession(boost::asio::io_service& ios)
		: ios_(ios), ip_(""), port_(0), p_observer_(NULL) {}
	virtual ~ISession(){ delete_observer(); }
public:
	virtual bool is_connect() = 0;
	virtual bool open(const S_CONNECT_INFO& info) = 0;
	virtual void close() = 0;
	virtual void write(char* buff, int len) = 0;

	void register_observer(IIOObserver* val) { p_observer_ = val; }
	void delete_observer() { p_observer_ = NULL; }
protected:
	void handle_receive(char* buff, int len); // handle receive chars
	void handle_connect(int error_code);

	std::string Ip() const { return ip_; }
	void Ip(std::string val) { ip_ = val; }
	short Port() const { return port_; }
	void Port(short val) { port_ = val; }
private:
	IIOObserver* p_observer_;
	std::string ip_;
	short port_;
protected:
	boost::asio::io_service& ios_;
};
