#include "oct_toolkits.h"
#include "ifile/file_assistant.h"
#include "ilog/log.h"
#include "universe_def.h"

#include <stdarg.h>
#include <stdio.h>


/// ----------------------------------------------------------------------------------------
#if defined(os_is_win)
	#include <time.h>
	#include <sys/timeb.h>
	#include <Windows.h>

	#include<vector>


#elif defined (os_is_linux)
	#include <limits.h>
	#include <stdio.h>
	#include <string.h>
	#include <unistd.h>
	#include <sys/timeb.h> 


#endif /// 



namespace oct_toolkits
{





#if defined(os_is_win)


	/// ---------------------------------------------------------------------------
	/// @brief: UTF8转 unicode
	/// ---------------------------------------------------------------------------
	std::wstring Utf82Unicode(const std::string& utf8string)
	{
		int widesize = ::MultiByteToWideChar(CP_UTF8, 0, utf8string.c_str(), -1, NULL, 0);
		if ( widesize == ERROR_NO_UNICODE_TRANSLATION )
		{
			return std::wstring(L"");

			///throw std::exception("Invalid UTF-8 sequence.");
		}
		if ( widesize == 0 )
		{
			return std::wstring(L"");
			/// throw std::exception("Error in conversion.");
		}

		std::vector<wchar_t> resultstring(widesize);
		int convresult = ::MultiByteToWideChar(CP_UTF8, 0, utf8string.c_str(), -1, &resultstring[0], widesize);

		if ( convresult != widesize )
		{
			return std::wstring(L"");
			;///throw std::exception("La falla!");
		}

		return std::wstring(&resultstring[0]);
	}



	/// ---------------------------------------------------------------------------
	/// @brief: unicode 转为 ascii
	/// ---------------------------------------------------------------------------
	std::string WideByte2Acsi(std::wstring& wstrcode)
	{
		int asciisize = ::WideCharToMultiByte(CP_OEMCP, 0, wstrcode.c_str(), -1, NULL, 0, NULL, NULL);
		if ( asciisize == ERROR_NO_UNICODE_TRANSLATION )
		{
			return std::string("");
			;/// throw std::exception("Invalid UTF-8 sequence.");
		}
		if ( asciisize == 0 )
		{
			return std::string("");
			;///throw std::exception("Error in conversion.");
		}
		std::vector<char> resultstring(asciisize);
		int convresult =::WideCharToMultiByte(CP_OEMCP, 0, wstrcode.c_str(), -1, &resultstring[0], asciisize, NULL, NULL);

		if ( convresult != asciisize )
		{
			return std::string("");
			///throw std::exception("La falla!");
		}

		return std::string(&resultstring[0]);
	}




	/// ---------------------------------------------------------------------------
	/// @brief: ascii 转 Unicode
	/// ---------------------------------------------------------------------------
	std::wstring Acsi2WideByte(const std::string& strascii)
	{
		int widesize = MultiByteToWideChar(CP_ACP, 0, (char*)strascii.c_str(), -1, NULL, 0);
		if ( widesize == ERROR_NO_UNICODE_TRANSLATION )
		{
			return std::wstring(L"");

			/// throw std::exception("Invalid UTF-8 sequence.");
		}
		if ( widesize == 0 )
		{
			return std::wstring(L"");
			/// throw std::exception("Error in conversion.");
		}
		std::vector<wchar_t> resultstring(widesize);
		int convresult = MultiByteToWideChar(CP_ACP, 0, (char*)strascii.c_str(), -1, &resultstring[0], widesize);


		if ( convresult != widesize )
		{
			return std::wstring(L"");
			/// throw std::exception("La falla!");
		}

		return std::wstring(&resultstring[0]);
	}


	/// ---------------------------------------------------------------------------
	/// @brief: unicode 转uft8
	/// ---------------------------------------------------------------------------
	std::string Unicode2Utf8(const std::wstring& widestring)
	{
		int utf8size = ::WideCharToMultiByte(CP_UTF8, 0, widestring.c_str(), -1, NULL, 0, NULL, NULL);
		if ( utf8size == 0 )
		{
			return std::string("");
			/// throw std::exception("Error in conversion.");
		}

		std::vector<char> resultstring(utf8size);

		int convresult = ::WideCharToMultiByte(CP_UTF8, 0, widestring.c_str(), -1, &resultstring[0], utf8size, NULL, NULL);

		if ( convresult != utf8size )
		{
			return std::string("");
			/// throw std::exception("La falla!");
		}

		return std::string(&resultstring[0]);
	}


#endif /// 



	/// --------------------------------------------------------------------------------------------------------
	///	@brief: 
	/// --------------------------------------------------------------------------------------------------------
	ifile* create_ifile_()
	{
		using namespace oct_file;
		file_assistant *pobj	= new(std::nothrow) file_assistant;

		return pobj;
	}

	/// --------------------------------------------------------------------------------------------------------
	///	@brief: 
	/// --------------------------------------------------------------------------------------------------------
	ifile* release_ifile_(ifile* pobj)
	{
		if (pobj)
		{
			pobj->uninit_();
			delete pobj;
		}

		pobj = NULL;

		return pobj;
	}

	/// --------------------------------------------------------------------------------------------------------
	///	@brief: 
	/// --------------------------------------------------------------------------------------------------------
	ilog* create_ilog_()
	{
		using namespace oct_log;
		log_imp* plog	= new(std::nothrow) log_imp;

		return plog;
	}

	/// --------------------------------------------------------------------------------------------------------
	///	@brief: 
	/// --------------------------------------------------------------------------------------------------------
	ilog* release_ilog_(ilog* pobj)
	{
		if (pobj)
		{
			pobj->uninit_();
			delete pobj;
		}

		pobj = NULL;

		return pobj;
	}


	/// --------------------------------------------------------------------------------------------------------
	///	@brief: 格式化字符串
	/// --------------------------------------------------------------------------------------------------------
	std::string toolkits::str_format_(const char*pformat, ...)
	{
		va_list argptr;
		va_start(argptr, pformat);

		/// 计算格式化字符串的长度
		int size			= vsnprintf(NULL, NULL, pformat, argptr);
		if (0				== size)
		{
			va_end(argptr);
			return std::string("");
		}

		/// 申请缓冲区 ， +1是为了存放结束符
		char* pbuf			= (char*)malloc(size + 1);
		/// 申请失败，记得释放VAlist
		if (NULL			== pbuf)
		{
			va_end(argptr);
			return std::string("");
		}

		memset(pbuf, 0, size + 1);

		/// 将数据写入申请的缓冲区
		int write_len		= vsnprintf(pbuf, size + 1, pformat, argptr);
		/// 释放VA
		va_end(argptr);
		pbuf[size]		= '\0';

		/// 构造函数返回值
		std::string ret(pbuf);

		/// 释放申请的缓冲区
		free(pbuf);
		pbuf				= NULL;

		return ret;
	}

	/// --------------------------------------------------------------------------------------------------------
	///	@brief: 返回年月日时分秒毫秒
	/// --------------------------------------------------------------------------------------------------------
	oct_toolkits::st_now_date toolkits::get_date_now_()
	{
		st_now_date date;

		time_t tt				= time(NULL);
		struct tm *stm			= localtime(&tt);

		date.year_				= stm->tm_year + 1900;
		date.month_				= stm->tm_mon + 1;
		date.day_				= stm->tm_mday;

		date.hour_				= stm->tm_hour;
		date.minute_			= stm->tm_min;
		date.second_			= stm->tm_sec;

		/// 得到毫秒
		struct timeb tb;
		ftime(&tb);
		date.mill_sec_			= tb.millitm;


		return date;
	}

	/// --------------------------------------------------------------------------------------------------------
	///	@brief: 
	/// --------------------------------------------------------------------------------------------------------
	toolkits::toolkits()
	{
		;
	}

	/// --------------------------------------------------------------------------------------------------------
	///	@brief: 
	/// --------------------------------------------------------------------------------------------------------
	toolkits::~toolkits()
	{
		;
	}

	/// --------------------------------------------------------------------------------------------------------
	///	@brief: 获取可执行程序所在绝对路径
	/// --------------------------------------------------------------------------------------------------------
	std::string toolkits::get_executable_dir_()
	{
#if defined (os_is_win)

		char path[255]					= { 0 };
		GetModuleFileName(NULL, path, 255);
		(strrchr(path, '\\'))[1]		= 0;

		return std::string(path);
#elif defined (os_is_linux)

		char *p                 = NULL;

		const int len           = 256;
		/// to keep the absolute path of executable's path
		char arr_tmp[len]       = {0};
    
		int n                   = readlink("/proc/self/exe", arr_tmp, len);
		if (NULL                != (p = strrchr(arr_tmp,'/')))
			*p = '\0';
		else
		{
			return std::string("");
		}

		return std::string(arr_tmp);
#endif /// 
	}

	/// --------------------------------------------------------------------------------------------------------
	///	@brief: utf8字符串转ascii
	/// --------------------------------------------------------------------------------------------------------
	std::string toolkits::utf8_to_ascii_(const std::string& str_utf8)
	{
		std::string ret_val;

#if defined(os_is_win)
		/// 先把 utf8 转为 unicode
		std::wstring wstr = Utf82Unicode(str_utf8);
		/// 最后把 unicode 转为 ascii
		ret_val = WideByte2Acsi(wstr);
#endif /// 

		return ret_val;
	}

	/// --------------------------------------------------------------------------------------------------------
	///	@brief: ascii字符编码转utf8字符串
	/// --------------------------------------------------------------------------------------------------------
	std::string toolkits::acsii_to_utf8_(const std::string& str_acsii)
	{
		std::string ret_val;

#if defined(os_is_win)
		/// 先把 ascii 转为 unicode
		std::wstring wstr	= Acsi2WideByte(str_acsii);
		/// 最后把 unicode 转为 utf8
		ret_val				= Unicode2Utf8(wstr);
#endif /// 

		return ret_val;
	}

}