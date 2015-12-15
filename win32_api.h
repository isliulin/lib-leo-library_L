#pragma  once
#include <string>
#include <windows.h>
#include "Poco/Path.h"
#include "Poco/Process.h"

namespace Win32API
{

#pragma region _DECLARE
	std::string getCurrentFileName();
	std::string getCurrentDir();
	DWORD       getProcessIDByTitle(LPCTSTR t_title);
	HWND        getWndByProcessID(unsigned long process_id);
#pragma endregion _DECLARE




#pragma region _BODY

	inline std::string getCurrentFileName()
	{
		char buffer[MAX_PATH];
		GetModuleFileNameA(NULL, buffer, MAX_PATH);  // base on code W/A
		return std::string(buffer);
	}

	std::string getCurrentDir()
	{
		char buffer[MAX_PATH];
		GetModuleFileNameA(NULL, buffer, MAX_PATH);
		Poco::Path i_path(buffer);
		return std::string(i_path.parent().toString());
	}



	inline	DWORD getProcessIDByTitle(LPCTSTR t_title)
	{
		DWORD process_ID = 0;

		if (t_title == NULL)
		{
			return process_ID;
		}
		HWND i_handle = ::FindWindow(nullptr, t_title);
		if (i_handle == NULL)
		{
			return process_ID;
		}
		GetWindowThreadProcessId(i_handle, &process_ID);
		return process_ID;
	}


#pragma region _WND_HANDLE
	struct handle_data {
		unsigned long process_id;
		HWND best_handle;
	};

	BOOL is_main_window(HWND handle)
	{
		return GetWindow(handle, GW_OWNER) == (HWND)0 && IsWindowVisible(handle);
	}


	BOOL CALLBACK enum_windows_callback(HWND handle, LPARAM lParam)
	{
		handle_data& data = *(handle_data*)lParam;
		unsigned long process_id = 0;
		GetWindowThreadProcessId(handle, &process_id);
		if (data.process_id != process_id || !is_main_window(handle)) {
			return TRUE;
		}
		data.best_handle = handle;
		return FALSE;
	}
#pragma endregion _WND_HANDLE

	inline HWND getWndByProcessID(unsigned long process_id)
	{
		handle_data data;
		data.process_id = process_id;
		data.best_handle = 0;
		EnumWindows(enum_windows_callback, (LPARAM)&data);
		return data.best_handle;

	}


#pragma endregion _BODY



}

