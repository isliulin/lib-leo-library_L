#pragma once
#include <boost/asio.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/thread.hpp>
#include "jc_event.h"
class io_help {
public:
	io_help() : ios_(), idle_work_(), is_run_(false), hThread(NULL) {
		start();
	}
	~io_help() {
		stop();
	}
public:
	void stop() {
		if (!is_run_) return;

		idle_work_.reset();
		ios_.stop();
		::WaitForSingleObject(hThread, INFINITE);
		ios_.reset();
		is_run_ = false;
	}
	void start() {
		if (is_run_)
			return;
		idle_work_.reset(new boost::asio::io_service::work(ios_));
		unsigned threadID;
		eve_.ResetEvent();
		hThread = (HANDLE)_beginthreadex( NULL, 0, &io_help::run_service, (void*)this, 0, &threadID );
		if (!threadID)
		{
			return;
		}
		is_run_ = true;
		::WaitForSingleObject(eve_, INFINITE);
	}

	static unsigned __stdcall run_service(void* pArguments)
	{
		io_help* this_ = (io_help*)pArguments;
		this_->eve_.SetEvent();
		this_->ios_.run();
		return 0;
	}

public:
	MyEvent eve_;
	boost::asio::io_service ios_;
	HANDLE hThread;
private:
	bool is_run_;
	boost::scoped_ptr<boost::asio::io_service::work> idle_work_;
};
//#include <boost/shared_ptr.hpp>
// __declspec(selectany) boost::shared_ptr<io_help> g_sp_io;
