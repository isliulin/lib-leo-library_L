#include <vector>
#include "..\database.h"
#include "..\..\publish\session\CommandPool.h"

#include <string>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>
#pragma once

 class t6800_session;
 class proxy_server;

// ServerProxy command target
class proxy_session 
	: public CommandPool
	, public boost::enable_shared_from_this<proxy_session>
{
public:
	enum { max_length = 1024};
	proxy_session(boost::asio::io_service& io_service, t6800_session * p_t6800_session, proxy_server* p_owner)
		: socket_(io_service), p_t6800_session_(p_t6800_session), p_owner_(p_owner)
	{
	}
	~proxy_session() {}

	boost::asio::ip::tcp::socket& socket()
	{
		return socket_;
	}

	void start()
	{
		socket_.async_read_some(boost::asio::buffer(read_data_,max_length),
			boost::bind(&proxy_session::handle_read, shared_from_this(),
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred));
	}

	void handle_read(const boost::system::error_code& error, std::size_t bytes_transferred)
	{
		if (error || 0==bytes_transferred)
		{
			//leave;
			disconnect();
		}
		else
		{
			//
			CommandPool::ReceiveChar((BYTE*)read_data_, bytes_transferred);
			//
			socket_.async_read_some(boost::asio::buffer(read_data_,max_length),
				boost::bind(&proxy_session::handle_read, shared_from_this(),
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));
		}
	}

	void disconnect() 
	{
		boost::system::error_code ignored_ec;
		socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);
		socket_.close();
	}
	int write(BYTE* pBuf, int nLen)
	{
		if (!socket_.is_open())
			return 0;
		int nCmdLen;
		MakeCmd(pBuf, nLen, (BYTE*)write_data_, nCmdLen);

		boost::asio::async_write(socket_,
			boost::asio::buffer(write_data_, nCmdLen),
			boost::bind(&proxy_session::handle_write, shared_from_this(),
				boost::asio::placeholders::error));

		return 1;
	}

	void handle_write(const boost::system::error_code& error)
	{
		if (!error)
		{
			// 
		}
		else
		{
			//leave
			disconnect();
		}
	}

public:
	boost::asio::ip::tcp::socket socket_;
	t6800_session * p_t6800_session_;
	proxy_server* p_owner_;
	char read_data_[max_length];
	char write_data_[max_length];

	BYTE	m_sendBuf[1024]; // the send buffer
	S_DB_USER m_user;
public:
	virtual void PostCmd(BYTE* pBuf, int nLen);
	//
	int SendCmdParseErrMsg();
	//
	BOOL ParseSessionCmd(BYTE* pBuf, int nLen);
	// 用户命令解析
	int ParseUserLogin(BYTE* pBuf, int nLen);
	int ParseUserLogout(BYTE* pBuf, int nLen);
	int ParseUserAdd(BYTE* pBuf, int nLen);
	int ParseUserDel(BYTE* pBuf, int nLen);
	int ParseUserEdit(BYTE* pBuf, int nLen);
	int ParseUserGetUser(BYTE* pBuf, int nLen);
	int ParseUserGetUserCount(BYTE* pBuf, int nLen);
	int ParseUserGetUsers(BYTE* pBuf, int nLen);
	// 连接IP（设备）
	int ParseLinkIPGet(BYTE* pBuf, int nLen);
	int ParseLinkIPSet(BYTE* pBuf, int nLen);
	// 
	int ParseGetAlarmMode(BYTE* pBuf, int nLen);
	int ParseSetAlarmMode(BYTE* pBuf, int nLen);
	int ParseGetLineInfo(BYTE* pBuf, int nLen);
	int ParseSetLineInfo(BYTE* pBuf, int nLen);
	int ParseGetZoneName(BYTE* pBuf, int nLen);
	int ParseSetZoneName(BYTE* pBuf, int nLen);
	// 定时节目
	int ParseGetGroup(BYTE*pBuf, int nLen); // 获取项目(不包含详细任务)
	int ParseGroupAdd(BYTE*pBuf, int nLen); // 添加项目
	int ParseGroupDel(BYTE*pBuf, int nLen); // 删除某项目
	int ParseGroupEditName(BYTE*pBuf, int nLen); // 删除某项目
	int ParseGetGroupTask(BYTE*pBuf, int nLen); // 获取某项目下任务
	int RetGroupTask(_DbTaskGroupVec::iterator& iter);
	int ParseSetGroupTask(BYTE*pBuf, int nLen); // 修改某项目任务
	int ParseGetProgramConfig(BYTE*pBuf, int nLen); // 获取运行程序配置信息
	int ParseSetProgramConfig(BYTE*pBuf, int nLen); // 设置运行程序配置信息
	int ParseProgramDown(BYTE*pBuf, int nLen); // 程序下载
	// 遥控器
	int ParseGetRemoteTask(BYTE*pBuf, int nLen);
	int ParseSetRemoteTask(BYTE*pBuf, int nLen);
};

typedef boost::shared_ptr<proxy_session> proxy_session_ptr;
typedef std::list<proxy_session_ptr> proxy_session_list;
//////////////////////////////////////////////////////////////////////////

class proxy_server
{
public:
	proxy_server(boost::asio::io_service& io_service, 
		const boost::asio::ip::tcp::endpoint& endpoint,
		t6800_session * p_t6800_session)
		: io_service_(io_service),
		  acceptor_(io_service, endpoint),
		  p_t6800_session_(p_t6800_session)
	{
		start_accept();
	}

	void start_accept()
	{
		proxy_session_ptr new_session(new proxy_session(io_service_, p_t6800_session_, this));
		acceptor_.async_accept(new_session->socket(),
			boost::bind(&proxy_server::handle_accept, this, 
			new_session, boost::asio::placeholders::error));
	}

	void handle_accept(proxy_session_ptr p_session,
		const boost::system::error_code& error)
	{
		if (!error)
		{
			session_list_.push_back(p_session);
			p_session->start();
		}

		start_accept();
	}

	proxy_session_list session_list_;
private:
	t6800_session * p_t6800_session_;
	boost::asio::io_service& io_service_;
	boost::asio::ip::tcp::acceptor acceptor_;
};
