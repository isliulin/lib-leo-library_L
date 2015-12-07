#include "stdafx.h"
#include "server_run.h"
#include "..\database.h"
#include "ExServerSession.h"
#include "ExServerProxy.h"
#include <boost/thread.hpp>


namespace CAppExt
{
	CString GetModuleDirectory(void)
	{
		TCHAR szFull[_MAX_PATH];
		TCHAR szDrive[_MAX_DRIVE];
		TCHAR szDir[_MAX_DIR];

		::GetModuleFileName(NULL, szFull, sizeof(szFull)/sizeof(TCHAR));
		_tsplitpath_s(szFull, szDrive, _MAX_DRIVE, szDir, _MAX_DIR, NULL, 0, NULL, 0);
		_tcscpy_s(szFull, MAX_PATH, szDrive);
		_tcscat_s(szFull, MAX_PATH, szDir);

		return CString(szFull);
	}
}

boost::asio::io_service io_service;

void io_server_run() 
{
	try
	{
		// 初始化数据库
		CString szModuleDir = CAppExt::GetModuleDirectory();
		CString str = szModuleDir + _T("config.xml");
		std::string szFile = CT2A(str, CP_ACP);
		theDataBase.SetFilePath(szFile);
		// 
		str = szModuleDir + _T("task.xml");
		szFile = CT2A(str, CP_ACP);
		theTaskDB.SetFilePath(szFile);

		// 初始化主机连接
		ULONG net_addr;
		USHORT net_port;
		if (!theDataBase.GetNetParameter(net_addr, net_port))
		{
			theDataBase.SetNetParameter(net_addr, net_port);
		}
		in_addr mid_addr;
		mid_addr.s_addr = net_addr;
		boost::asio::ip::udp::endpoint t6800_endpoint(boost::asio::ip::address::from_string(inet_ntoa(mid_addr)), ntohs(net_port));
		t6800_session t6800_session_(io_service, t6800_endpoint);
		t6800_session_.SetTargetSockAddr(net_addr, net_port); // 兼容处理
		// 数据库
		theDataBase.SetParamter(&t6800_session_.m_sys);
		theDataBase.Load();
		theTaskDB.Load();          
		theTaskDB.SortTask();
		// 初始化监听
		boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(), (unsigned short)theDataBase.GetServerPort());
		proxy_server proxy_server_(io_service, endpoint, &t6800_session_);
		//
		t6800_session_.Proxy_server(&proxy_server_);
		t6800_session_.timer_get_host_state_start();
		//
		io_service.run();
	}
	catch (const std::exception& e)
	{
		std::cerr << "Unexpected error: " << e.what() << std::endl;
	}
	catch (...)
	{
		std::cerr << "Unknown error" << std::endl;
	}
}

void io_server_stop()
{
	io_service.stop();
	io_service.reset();
	theDataBase.m_pDoc->Clear();
	theTaskDB.m_pDoc->Clear();
	boost::this_thread::sleep(boost::posix_time::milliseconds(10));
}