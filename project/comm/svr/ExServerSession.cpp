#include "stdafx.h"
#include "ExServerSession.h"
#include "ExServerProxy.h"
#include <boost/foreach.hpp>

void t6800_session::handle_timer_sending( const boost::system::error_code& error )
{
	if (error == boost::asio::error::operation_aborted)
	{
		// Timer was cancelled, take necessary action.
		return;
	}
	if (!m_cmdList.size())
	{
		// 命令队列为空
		return;
	}

	TSessionCmdList::iterator iter = m_cmdList.begin();
	if (iter->nUseableSendTimes > 0)
	{
		iter->nUseableSendTimes--;
		send_cmd(iter);
		timer_set((long)__GetElapseTimer(iter->buf[0]));
	} 
	else
	{
		m_bDevOnline = false;
		m_cmdFlag = _CmdFlag_Idle;
		// 通知发送未成功
		TSessionCmdList::iterator iterCmd = m_cmdList.begin();
		BYTE buf[] = {CF_ERR_CONNECT, iterCmd->buf[0], iterCmd->buf[1]};
		SendToClient(buf, 3);
		// 清空命令堆
		m_cmdList.clear();
	}
}

void t6800_session::handle_timer_get_host( const boost::system::error_code& error )
{
	TRACE("handle_timer_get_host.start- ");
	if (error == boost::asio::error::operation_aborted)
	{
		TRACE("error\n");
		// Timer was cancelled, take necessary action.
		return;
	}
	static CTime store_time = CTime::GetCurrentTime();
	CTimeSpan time_span = CTime::GetCurrentTime() - store_time;
	LONGLONG expect_second = time_span.GetTotalSeconds() - 1;
	if ( expect_second > 1 || expect_second < -1 )
	{
		TRACE("systime changed");
		m_bNeedSetTime = true;
	}
	store_time = CTime::GetCurrentTime();

	if (!m_cmdList.size())
	{
		m_countdownToSetHostTime--;
		if (m_countdownToSetHostTime <= 0)
		{
			m_countdownToSetHostTime = 10000;
			m_bNeedSetTime = true;
		}
		SimulateSession ss(this);
		ss.SetParam(NULL);

		if (m_bDevOnline)
		{
			ss._GetHostState();
		} 
		else
		{
			ss._GetSerialNumAndMac();
		}
	}
	else
	{
		StartExecuteCmd();
	}
	timer_get_host_state_start();
	TRACE(" -end\n");
}

BOOL t6800_session::AddCmd(BYTE* pBuf, int nLen, LPARAM lParam/* =NULL */, BYTE nUseableSendTimes/* =3 */)
{
	if (nLen>=1024)
	{
		return FALSE;
	}

	if (pBuf[0] == CF_GET_HOST_STATE) // 未处理独占模式时的问题
	{
		if (m_cmdList.size())
			return FALSE;
	}

	S_SESSION_CMD cmd;
	cmd.lParam = lParam;
	memcpy_s(cmd.buf, sizeof(cmd.buf), pBuf, nLen);
	cmd.nLen = nLen;
	cmd.nUseableSendTimes = nUseableSendTimes;

	switch (cmd.buf[0])
	{
	case CF_GET_IP_ADDR:
	case CF_SET_IP_ADDR:
	case CF_GET_SERIAL_NUM:
	case CF_SET_SERIAL_NUM:
	case CF_SET_REGISTER_NUM:
		m_cmdList.push_front(cmd);
		break;
	default:
		m_cmdList.push_back(cmd);
		break;
	}

	// 立即发送命令
	StartExecuteCmd();
	return TRUE;
}

void t6800_session::StartExecuteCmd()
{
	if (_CmdFlag_Idle == m_cmdFlag && m_cmdList.size())
	{
		TSessionCmdList::iterator iter = m_cmdList.begin();
		send_cmd(iter);
		if (__IsRetCmd(iter->buf[0]))
		{
			timer_set((long)__GetElapseTimer(iter->buf[0]));
		}
		else
		{
			// 不需要返回的指令(控制命令)，直接完成该命令
			CompleteOneCmd();
		}
	}
}

void t6800_session::CompleteOneCmd()
{
	if (m_cmdList.size())
	{
		timer_.cancel();
		m_cmdFlag = _CmdFlag_Idle;
		// 发送到客户端确认完成命令
		TSessionCmdList::iterator iterCmd = m_cmdList.begin();
		boost::asio::deadline_timer t(io_service_, boost::posix_time::milliseconds(5));
		switch (iterCmd->buf[0])
		{
		case CF_PROGRAM_DOWN_ENTRY:
		case CF_PROGRAM_DOWN:
		case CF_PROGRAM_END:
			t.wait();
			break;
		}
		SendToClient(iterCmd->buf, 1, FALSE);
		m_cmdList.pop_front();
		if (m_cmdList.size())
		{
			StartExecuteCmd();
		}
	}
}

int t6800_session::IReceive(BYTE* pBuf, int nLen)
{
	ISession::IReceive(pBuf, nLen);

	// 发送到客户端
	SendToClient(pBuf, nLen);
	
	// 2.判断命令是否完成
	TSessionCmdList::iterator iter = m_cmdList.begin();
	//
	if (iter != m_cmdList.end())
	{
		if (iter->nUseableSendTimes != 0)
		{
			if ('B' == pBuf[3] && CF_GET_HOST_STATE == iter->buf[0])
			{
				CompleteOneCmd();
			}
			else if (pBuf[8] == __GetCmdRetByte(iter->buf[0]))
			{
				CompleteOneCmd();
			}
		}
	}
	return 0;
}

int t6800_session::ISendTo(BYTE* pBuf, int nLen)
{
	int i = 0;
	send_buffer_.at(i++) = 'i';
	send_buffer_.at(i++) = 't';
	send_buffer_.at(i++) = 'c';
	for (int nPos=0; nPos<nLen; nPos++)
	{
		send_buffer_.at(i++) = pBuf[nPos];
	}

	// 判断是否广播
	if (pBuf[0] == CF_GET_IP_ADDR ||
		pBuf[0] == CF_SET_IP_ADDR ||
		pBuf[0] == CF_GET_SERIAL_NUM ||
		pBuf[0] == CF_SET_SERIAL_NUM ||
		pBuf[0] == CF_SET_REGISTER_NUM )
	{
		if (!m_bBoradcast)
		{
			m_bBoradcast = TRUE;
			boost::asio::socket_base::broadcast option(true);
			socket_.set_option(option);
		}
		boost::asio::ip::udp::endpoint broad_endpoint(boost::asio::ip::address::from_string("255.255.255.255"), remote_endpoint_.port());
		socket_.async_send_to(boost::asio::buffer(send_buffer_, i), broad_endpoint,
			boost::bind(&t6800_session::handle_send, this, boost::asio::placeholders::error,	boost::asio::placeholders::bytes_transferred)
			);
		recv_endpoint_ = broad_endpoint;
	}
	else
	{
		if (m_bBoradcast)
		{
			m_bBoradcast = FALSE;
			boost::asio::socket_base::broadcast option(false);
			socket_.set_option(option);
		}
		socket_.async_send_to(boost::asio::buffer(send_buffer_, i), remote_endpoint_,
			boost::bind(&t6800_session::handle_send, this, boost::asio::placeholders::error,	boost::asio::placeholders::bytes_transferred)
			);
		recv_endpoint_ = remote_endpoint_;
	}

	m_cmdFlag = _CmdFlag_Busy;
	start_receive();
	return 1;
}

#define PR_HEAD_LEN 3
void t6800_session::handle_receive(const boost::system::error_code& error,
					std::size_t bytes_transferred)
{
	if (error || 0==bytes_transferred)
	{
		//错误处理
		return;
	}
	// 处理
	const BYTE szHead[] = {'i','t','c'/*,'b','r','o','a','d'*/};
	if (bytes_transferred>PR_HEAD_LEN)
	{
		if (szHead[0] == recv_buffer_.at(0) && 
			szHead[1] == recv_buffer_.at(1) &&
			szHead[2] == recv_buffer_.at(2) )
		{
			if (remote_endpoint_ != recv_endpoint_)
			{
				TRACE("ServerSession::OnReceive DEV IP CHANGE \n");
				remote_endpoint_ = recv_endpoint_;
				ULONG net_addr = htonl(remote_endpoint_.address().to_v4().to_ulong());
				USHORT net_port = htons(remote_endpoint_.port());
				if (net_addr != INADDR_NONE)
				{
					SetTargetSockAddr(net_addr, net_port);
					theDataBase.SetNetParameter(net_addr, net_port);
					m_bNeedSetTime = true;
					int nPos = 0;
					BYTE buf[50];
					buf[nPos++] = CF_LINK_IP;
					buf[nPos++] = CF_SUB_LINK_IP_GET;
					IN_ADDR sin_addr;
					sin_addr.S_un.S_addr = net_addr;
					buf[nPos++] = sin_addr.S_un.S_un_b.s_b1;
					buf[nPos++] = sin_addr.S_un.S_un_b.s_b2;
					buf[nPos++] = sin_addr.S_un.S_un_b.s_b3;
					buf[nPos++] = sin_addr.S_un.S_un_b.s_b4;
					buf[nPos++] = (char)net_port;
					buf[nPos++] = (char)(net_port>>8);
					SendToClient(buf, nPos);
				}
			}

			m_bDevOnline = true;
			for (int nLen=0; nLen<(int)bytes_transferred; nLen++)
			{
				m_recvBuf[nLen] = recv_buffer_.at(nLen);
			}
			IReceive(m_recvBuf, (int)bytes_transferred);
		}
	}

	// 继续接收
	start_receive();
}

void t6800_session::SendToClient(BYTE* pBuf, int nLen, BOOL bAll/*=TRUE*/)
{
	if (bAll)
	{
		if (p_proxy_server_ != NULL)
		{
			BOOST_FOREACH( proxy_session_ptr p_session, p_proxy_server_->session_list_)
			{
				p_session->write(pBuf, nLen);
			}
		}
		else
		{
			//ASSERT(FALSE);
		}
	}
	else
	{
		TSessionCmdList::iterator iterCmd = m_cmdList.begin();
		if (m_cmdList.end() == iterCmd)
		{
			return;
		}
		proxy_session* p_ses = (proxy_session*)(iterCmd->lParam);
		if (p_ses != NULL)
		{
			p_ses->write(pBuf, nLen);
		}
	}
}

