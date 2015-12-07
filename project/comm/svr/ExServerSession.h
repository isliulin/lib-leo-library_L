#include "..\ISession.h"
#include <string>
#include <list>
#include <map>
#include <boost/array.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#pragma once

#if defined(WIN32)
# include <windows.h>
#else
# error This example is for Windows only!
#endif

struct tick_count_traits
{
	// The time type. This type has no constructor that takes a DWORD to ensure
	// that the timer can only be used with relative times.
	class time_type
	{
	public:
		time_type() : ticks_(0) {}
	private:
		friend struct tick_count_traits;
		DWORD ticks_;
	};

	// The duration type. According to the TimeTraits requirements, the duration
	// must be a signed type. This means we can't handle durations larger than
	// 2^31.
	class duration_type
	{
	public:
		duration_type() : ticks_(0) {}
		duration_type(LONG ticks) : ticks_(ticks) {}
	private:
		friend struct tick_count_traits;
		LONG ticks_;
	};

	// Get the current time.
	static time_type now()
	{
		time_type result;
		result.ticks_ = ::GetTickCount();
		return result;
	}

	// Add a duration to a time.
	static time_type add(const time_type& t, const duration_type& d)
	{
		time_type result;
		result.ticks_ = t.ticks_ + d.ticks_;
		return result;
	}

	// Subtract one time from another.
	static duration_type subtract(const time_type& t1, const time_type& t2)
	{
		// DWORD tick count values can wrap (see less_than() below). We'll convert
		// to a duration by taking the absolute difference and adding the sign
		// based on which is the "lesser" absolute time.
		return duration_type(less_than(t1, t2)
			? -static_cast<LONG>(t2.ticks_ - t1.ticks_)
			: static_cast<LONG>(t1.ticks_ - t2.ticks_));
	}

	// Test whether one time is less than another.
	static bool less_than(const time_type& t1, const time_type& t2)
	{
		// DWORD tick count values wrap periodically, so we'll use a heuristic that
		// says that if subtracting t1 from t2 yields a value smaller than 2^31,
		// then t1 is probably less than t2. This means that we can't handle
		// durations larger than 2^31, which shouldn't be a problem in practice.
		return (t2.ticks_ - t1.ticks_) < static_cast<DWORD>(1 << 31);
	}

	// Convert to POSIX duration type.
	static boost::posix_time::time_duration to_posix_duration(
		const duration_type& d)
	{
		return boost::posix_time::milliseconds(d.ticks_);
	}
};

typedef boost::asio::basic_deadline_timer<
DWORD, tick_count_traits> tick_count_timer;


typedef LONG_PTR LPARAM;
struct S_SESSION_CMD
{
	enum {max_length = 1024};
	LPARAM lParam; // 存放相关信息，这里存放ServerProxy的实例的指针
	BYTE buf[max_length];
	BYTE nLen;
	BYTE nUseableSendTimes;
	S_SESSION_CMD & operator = (S_SESSION_CMD& sec){
		memcpy_s(this, sizeof(S_SESSION_CMD), &sec, sizeof(S_SESSION_CMD));
		return *this;
	}
	void LoadDefault(){
		memset(buf, NULL, max_length);
		nLen = 0;
		nUseableSendTimes = 3;
	}
};
typedef std::list<S_SESSION_CMD> TSessionCmdList;

enum E_CMD_FLAG{
	_CmdFlag_Idle=0, // 空闲
	_CmdFlag_Busy, // 忙（正在通信中）
	_CmdFlag_Wait, // 等待
};

class proxy_server;
// ServerSession command target
class t6800_session : public ISession

{
public:
	t6800_session(boost::asio::io_service& io_service, boost::asio::ip::udp::endpoint& remote_endpoint) 
		: io_service_(io_service), 
		socket_(io_service), 
		timer_(io_service), timer_get_host_state_(io_service),
		remote_endpoint_(remote_endpoint)
	{
		m_cmdFlag = _CmdFlag_Idle;
		m_bNeedSetTime = true;
		m_countdownToSetHostTime = 10000;
		m_bBoradcast=FALSE;
		m_bDevOnline = true;
		p_proxy_server_ = NULL;

		socket_.open(boost::asio::ip::udp::v4());
	}
	~t6800_session() {}

	void start_receive() 
	{
		socket_.async_receive_from(
			boost::asio::buffer(recv_buffer_, max_length), recv_endpoint_,
			boost::bind(&t6800_session::handle_receive, this,
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred));
	}

	void handle_receive(const boost::system::error_code& error,
		std::size_t bytes_transferred);

	void handle_send(const boost::system::error_code& error, std::size_t bytes_transferred)
	{
	}

	enum {max_length=1024};
	boost::asio::io_service& io_service_;
	boost::asio::ip::udp::socket socket_;
	boost::asio::ip::udp::endpoint remote_endpoint_;
	boost::asio::ip::udp::endpoint recv_endpoint_;
// 	boost::asio::deadline_timer timer_;
// 	boost::asio::deadline_timer timer_get_host_state_;
	tick_count_timer timer_;
	tick_count_timer timer_get_host_state_;

	void handle_timer_sending(const boost::system::error_code& error);
	void timer_set( long expires_time ) 
	{
//		timer_.expires_from_now(boost::posix_time::milliseconds(expires_time));
		timer_.expires_from_now(expires_time);
		timer_.async_wait(boost::bind(&t6800_session::handle_timer_sending, this, boost::asio::placeholders::error));
	}
	void handle_timer_get_host(const boost::system::error_code& error);

	void timer_get_host_state_start() 
	{
// 		timer_get_host_state_.expires_from_now(boost::posix_time::seconds(1));
		timer_get_host_state_.expires_from_now(1000);
		timer_get_host_state_.async_wait(boost::bind(&t6800_session::handle_timer_get_host, this, boost::asio::placeholders::error));
	}
	boost::array<char, max_length> send_buffer_;
	boost::array<char, max_length> recv_buffer_;

	proxy_server* p_proxy_server_;
	proxy_server* Proxy_server() const { return p_proxy_server_; }
	void Proxy_server(proxy_server* val) { p_proxy_server_ = val; }
public:
	int m_countdownToSetHostTime;

 	TSessionCmdList m_cmdList;
 	E_CMD_FLAG m_cmdFlag;
	BOOL AddCmd(BYTE* pBuf, int nLen, LPARAM lParam=NULL, BYTE nUseableSendTimes=3);
	void StartExecuteCmd();

	void send_cmd( TSessionCmdList::iterator iter ) 
	{
		if (m_bNeedSetTime && iter->buf[0] == CF_GET_HOST_STATE) 
		{
			CTime cur_time = CTime::GetCurrentTime();
			_GetHostState(TRUE, &cur_time, m_sys.alarmMode, m_sys.broadVolumeAdd);
			m_bNeedSetTime = false;
		}
		else
			ISendTo(iter->buf, iter->nLen);
	}
	void CompleteOneCmd();

	void SendToClient(BYTE* pBuf, int nLen, BOOL bAll=TRUE);
public:
	BOOL m_bBoradcast; //是否广播
	virtual int ISendTo(BYTE* pBuf, int nLen);
	virtual int IReceive(BYTE* pBuf, int nLen);

	bool m_bNeedSetTime;
	bool m_bDevOnline;
};

class SimulateSession : public ISession
{
public:
	SimulateSession(t6800_session* p_client)
		: p_client_(p_client)
	{
		m_pServerProxy = NULL;
		m_bAddCmdList = FALSE;
	}
	SimulateSession(LPARAM pServerProxy){SetParam(pServerProxy);}
	LPARAM m_pServerProxy;
	BOOL m_bAddCmdList;
	void SetParam(LPARAM pServerProxy, BOOL bAddCmdList = TRUE){
		m_pServerProxy = pServerProxy;
		m_bAddCmdList = bAddCmdList;
	}
	virtual int ISendTo(BYTE* pBuf, int nLen)
	{
		if (m_bAddCmdList)
		{
			p_client_->AddCmd(pBuf, nLen, m_pServerProxy);
		}
		return nLen;
	}
private:
	t6800_session* p_client_;
};
