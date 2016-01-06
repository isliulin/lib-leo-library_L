#pragma  once
#include <string>
#include <windows.h>
#include <AtlBase.h>
#include <atlconv.h>

namespace Win32API
{

#pragma region _DECLARE
	std::string getCurrentFileName();
	std::wstring getCurrentFileNameW();
	std::string getCurrentDir();
	std::wstring getCurrentDirW();
#pragma region _ENCODE
	std::wstring convertToWstring(const std::string & t_str);
	std::string  convertToString(const std::wstring & t_str);
	std::string  convertToUTF8(const std::wstring & t_str);
#pragma endregion _ENCODE

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

	inline std::wstring getCurrentFileNameW()
	{
		wchar_t buffer[MAX_PATH];
		GetModuleFileNameW(NULL, buffer, MAX_PATH);  // base on code W/A
		return std::wstring(buffer);
	}



	inline std::string getCurrentDir()
	{
		char buffer[MAX_PATH];
		GetModuleFileNameA(NULL, buffer, MAX_PATH);
		std::string i_path(buffer);
		return i_path.substr(0, i_path.rfind("\\") + 1);
		 
	}

	inline std::wstring getCurrentDirW()
	{
		wchar_t buffer[MAX_PATH];
		GetModuleFileNameW(NULL, buffer, MAX_PATH);
		std::wstring i_path(buffer);
		return i_path.substr(0, i_path.rfind(L"\\") + 1);
	}

#pragma region _ENCODE
	inline std::string convertToString(const std::wstring & t_str)
	{
		CW2A cw2a(t_str.c_str());
		return std::string(cw2a);
	}

	inline std::wstring convertToWstring(const std::string & t_str)
	{
		CA2W ca2w(t_str.c_str());
		return std::wstring(ca2w);
	}


	inline std::string convertToUTF8(const std::wstring & t_str)
	{
		CW2A cw2a(t_str.c_str(), CP_UTF8);
		return std::string(cw2a);

	}


#pragma endregion _ENCODE

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

