#pragma once 
#include <vector>
#include <string>
#include <map>
#include <windows.h>

namespace Clay
{

using namespace std;

	
bool Is_valid_ip(const char * tptr_ip);

vector<int> Get_IP_interger_from_string(const char * tptr_ip);

bool Is_valid_mark_address(const char * tptr_data);

vector<int> Get_mark_interger_from_string(const char * tptr_data);


string getCurrentFileName();

HWND find_main_window(unsigned long process_id)



#pragma region body


inline	string getCurrentFileName()
{
	char buffer[MAX_PATH];
	GetModuleFileNameA(NULL, buffer, MAX_PATH);  // base on code W/A
	return std::string(buffer);
}




inline bool Is_valid_ip(const char * tptr_ip)
{
	if (tptr_ip == NULL)
		return false;
	char temp[4];
	int count = 0;
	while (true){
		int index = 0;
		while (*tptr_ip != '\0' && *tptr_ip != '.' && count < 4){
			temp[index++] = *tptr_ip;
			tptr_ip++;
		}
		if (index == 4)
			return false;
		temp[index] = '\0';
		int num = atoi(temp);
		if (!(num >= 0 && num <= 255))
			return false;
		count++;
		if (*tptr_ip == '\0'){
			if (count == 4)
				return true;
			else
				return false;
		}
		else
			tptr_ip++;
	}
}

inline vector<int> Get_IP_interger_from_string(const char * tptr_ip)
{
	vector<int> i_IP_array;

	if (tptr_ip == NULL)
		return i_IP_array;

	char temp[4];
	int count = 0;
	while (true){
		int index = 0;
		while (*tptr_ip != '\0' && *tptr_ip != '.' && count < 4){
			temp[index++] = *tptr_ip;
			tptr_ip++;
		}
		if (index == 4)
			return i_IP_array;
		temp[index] = '\0';
		int num = atoi(temp);
		if (!(num >= 0 && num <= 255))
			return i_IP_array;
		else
			i_IP_array.push_back(num);
		count++;
		if (*tptr_ip == '\0'){
			if (count == 4)
				return i_IP_array;
			else
				return i_IP_array;
		}
		else
			tptr_ip++;
	}

}

inline bool Is_valid_mark_address(const char * tptr_data)
{
	if (tptr_data == NULL)
		return false;
	char temp[6];
	int count = 0;
	while (true){
		int index = 0;
		while (*tptr_data != '\0' && *tptr_data != ':' && count < 6){
			temp[index++] = *tptr_data;
			tptr_data++;
		}
		if (index == 6)
			return false;
		temp[index] = '\0';
		int num = atoi(temp);
		if (!(num >= 0 && num <= 255))
			return false;
		count++;
		if (*tptr_data == '\0'){
			if (count == 6)
				return true;
			else
				return false;
		}
		else
			tptr_data++;
	}
}


inline vector<int> Get_mark_interger_from_string(const char * tptr_data)
{
	vector<int> i_mark_array;

	if (tptr_data == NULL)
		return i_mark_array;

	char temp[6];
	int count = 0;
	while (true){
		int index = 0;
		while (*tptr_data != '\0' && *tptr_data != ':' && count < 6){
			temp[index++] = *tptr_data;
			tptr_data++;
		}
		if (index == 6)
			return i_mark_array;
		temp[index] = '\0';
		int num = strtol(temp,NULL, 0);

		if (!(num >= 0 && num <= 255))
			return i_mark_array;
		else
			i_mark_array.push_back(num);
		count++;
		if (*tptr_data == '\0'){
			if (count == 6)
				return i_mark_array;
			else
				return i_mark_array;
		}
		else
			tptr_data++;
	}

}

	bool Filter_time( const string & t_str_time, vector<int> & t_list_time )
	{
		if (t_str_time.empty())
		{
			return false;
		}
		
		string temp_test = t_str_time;
		vector<int>  temp_list;
		
		while (true)
		{
			if (temp_test.empty())
			{
				break;
			}
			size_t i_index = temp_test.find(":");
			
			if (i_index == string::npos)
			{
				temp_list.push_back(std::atoi(temp_test.c_str()));
				break;
			}
			temp_list.push_back(std::atoi(temp_test.substr(0, i_index).c_str()));
			temp_test = temp_test.substr(i_index + 1, temp_test.size() - i_index - 1);
		}
		if (temp_list.size() != 3)
		{
			return false;
		}
		
		t_list_time = temp_list;
		
		return true;
					
	}
	
#pragma endregion body

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

inline	HWND find_main_window(unsigned long process_id)
	{
		handle_data data;
		data.process_id = process_id;
		data.best_handle = 0;
		EnumWindows(enum_windows_callback, (LPARAM)&data);
		return data.best_handle;
	}



	
}

