#include "ilog/log.h"
#include "universe_def.h"
#include <algorithm>


#include <iostream>

#if defined (os_is_win)
#include <windows.h>
#include <wtypes.h>

#include <time.h>
#include <sys/timeb.h>
#include <io.h>
#include <direct.h>


#elif defined (os_is_linux)
#include <sys/types.h> 
#include <sys/stat.h> 
#include <sys/statfs.h>
#include <string>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/io.h>
#include <sys/stat.h>



#endif /// 

namespace oct_log
{


	/// --------------------------------------------------------------------------------------------------------
	///	@brief: 
	/// --------------------------------------------------------------------------------------------------------
	log_imp::log_imp()
		: log_info_()
		, pfile_(NULL)
		, cur_log_file_()
		, has_written_bytes_(0)
		, kits_()
	{

	}

	/// --------------------------------------------------------------------------------------------------------
	///	@brief: 
	/// --------------------------------------------------------------------------------------------------------
	log_imp::~log_imp()
	{
		uninit_();
	}

	/// --------------------------------------------------------------------------------------------------------
	///	@brief: 
	/// --------------------------------------------------------------------------------------------------------
	int log_imp::init_(const oct_toolkits::st_log_info& info)
	{
		/// 1.参数check
		/// 日志文件路不能为空
		if (3 >= info.path_.length())
			return 1;

		/// 防止多次初始化
		uninit_();

		/// 保存参数
		log_info_			= info;

		/// 将参数中路径转为适合对应平台下的格式
		get_real_path_();

		/// 创建日志文件所在目录
		bool ret_create_result =  create_log_path_();
		/// 创建失败， 则返回false 
		if (false == ret_create_result)
			return 3;

		/// 如果磁盘空间剩余不够，则禁止创建日志文件对象
		 bool is_ok			= check_disk_free_space_();	
		if (false			== is_ok)
		{
			return 2;
		}
		else
		{
			int ret			= create_log_file_();
			if (0			!= ret)
				return 5;

			/// 日志文件创建成功，可以写日志了

		}


		return 0;
	}

	/// --------------------------------------------------------------------------------------------------------
	///	@brief: 写日志
	/// --------------------------------------------------------------------------------------------------------
	int log_imp::log_text_(const std::string& str_log)
	{
		bool check_status_is_ok					= before_logging_(str_log.length());
		/// 无法写入日志，可能是日志文件创建失败，或者磁盘剩余空间不足5G
		if (false								== check_status_is_ok)
			return 1;

		/// 日志文件前面加上日期字符串： [yyyy-mm-dd hh:mm:ss:sss] 
		oct_toolkits::st_now_date date			= kits_.get_date_now_();
		std::string str_write					= std::string("[") + kits_.str_format_("%04d-%02d-%02d %02d:%02d:%02d:%3d", 
													date.year_, date.month_, date.day_, date.hour_, date.minute_, date.second_, date.mill_sec_) 
													+ std::string("] ") + str_log + std::string("\n");
		/// 可以写入， 统计本次写入的长度
		has_written_bytes_						+= str_write.length();

		/// 写入文件
		if (pfile_)
		{
			pfile_->write_(str_write);
		}
		else
			return 2;

		return 0;
	}

	/// --------------------------------------------------------------------------------------------------------
	///	@brief: 
	/// --------------------------------------------------------------------------------------------------------
	int log_imp::log_text_(const char* pdata, unsigned int pdata_len)
	{
		if ((NULL == pdata) || (nullptr == pdata) || (0 == pdata_len))
			return 1;

		bool check_status_is_ok					= before_logging_(pdata_len);
		/// 无法写入日志，可能是日志文件创建失败，或者磁盘剩余空间不足5G
		if (false								== check_status_is_ok)
			return 2;

		std::string str_tmp(pdata, pdata_len);

		///// 日志文件前面加上日期字符串： [yyyy-mm-dd hh:mm:ss:sss] 
		//oct_toolkits::st_now_date date			= kits_.get_date_now_();
		//std::string str_write					= kits_.str_format_("[%04d-%02d-%02d %02d:%02d:%02d:%03d] ",
		//											date.year_, date.month_, date.day_, date.hour_, date.minute_, date.second_, date.mill_sec_) 
		//											+ str_tmp + std::string("\n");
		//
		///// 可以写入， 统计本次写入的长度
		//has_written_bytes_						+= str_write.length();

		///// 写入文件
		//if (pfile_)
		//{
		//	pfile_->write_(str_write);
		//}
		//else
		//	return 3;

		int ret = log_text_(str_tmp);
		if (0 != ret)
			return 3;

		return 0;
	}

	/// --------------------------------------------------------------------------------------------------------
	///	@brief: 将参数【str_log】每个字节的16进制写入文件，全部大写
	/// --------------------------------------------------------------------------------------------------------
	int log_imp::log_hex_(const std::string& str_log)
	{
		bool check_status_is_ok					= before_logging_(str_log.length());
		/// 无法写入日志，可能是日志文件创建失败，或者磁盘剩余空间不足5G
		if (false								== check_status_is_ok)
			return 1;

		/// ----------------------------------------------------------------------------------------
		/// 构造十六进制字符串
		std::string str_write;
		for (unsigned int index					= 0; index < (unsigned int)str_log.length(); ++index)
		{
			///std::cout << "index=" << instr_tmpdex << ", ch=" << str_log[index] << "\n\n";

			std::string str_tmp					= kits_.str_format_("%.2X", str_log[index]);
			if (2 < str_tmp.length())
				str_tmp							= str_tmp.substr(str_tmp.length() - 2, str_tmp.length());

			str_write							+= (str_tmp + std::string(" "));
		}

		/// 日志文件前面加上日期字符串： [yyyy-mm-dd hh:mm:ss:sss] 
		oct_toolkits::st_now_date date			= kits_.get_date_now_();
		str_write								= std::string("[") + kits_.str_format_("%04d-%02d-%02d %02d:%02d:%02d:%3d",
													date.year_, date.month_, date.day_, date.hour_, date.minute_, date.second_, date.mill_sec_) 
													+ std::string("] ") + str_write + std::string("\n");


		/// 统计本次写入的长度
		has_written_bytes_						+= str_write.length();
		
		/// 全部转为大写
		std::transform(str_write.begin(), str_write.end(), str_write.begin(), toupper);

		if (pfile_)
		{
			pfile_->write_(str_write);
		}
		else
			return 2;

		return 0;
	}

	/// --------------------------------------------------------------------------------------------------------
	///	@brief: 将参数【pdata】每个字节的16进制写入文件，全部大写
	/// --------------------------------------------------------------------------------------------------------
	int log_imp::log_hex_(const char* pdata, unsigned int pdata_len)
	{
		if ((NULL == pdata)						|| (nullptr == pdata) || (0 == pdata_len))
			return 1;

		bool check_status_is_ok					= before_logging_(pdata_len);
		/// 无法写入日志，可能是日志文件创建失败，或者磁盘剩余空间不足5G
		if (false								== check_status_is_ok)
			return 2;


		std::string str_tmp(pdata, pdata_len);


		int ret = log_hex_(str_tmp);
		if (0 != ret)
			return 3;


		//		/// 构造十六进制字符串
		//std::string str_write;
		//for (unsigned int index					= 0; index < pdata_len; ++index)
		//{
		//	std::string str_tmp					= kits_.str_format_("%.2X", pdata[index]);
		//	if (2 < str_tmp.length())
		//		str_tmp							= str_tmp.substr(str_tmp.length() - 2, str_tmp.length());

		//	str_write							+= str_tmp + std::string(" ");
		//}


		///// 日志文件前面加上日期字符串： [yyyy-mm-dd hh:mm:ss:sss] 
		//oct_toolkits::st_now_date date			= kits_.get_date_now_();
		//str_write								= kits_.str_format_("[%04d-%02d-%02d %02d:%02d:%02d:%03d] ",
		//											date.year_, date.month_, date.day_, date.hour_, date.minute_, date.second_, date.mill_sec_) 
		//											+ str_write + std::string("\n");

		///// 统计本次写入的长度
		//has_written_bytes_						+= str_write.length();
		//
		///// 全部转为大写
		//std::transform(str_write.begin(), str_write.end(), str_write.begin(), toupper);


		//if (pfile_)
		//	pfile_->write_(str_write);
		//else
		//	return 3;

		return 0;
	}

	/// --------------------------------------------------------------------------------------------------------
	///	@brief: 释放内部资源
	/// --------------------------------------------------------------------------------------------------------
	void log_imp::uninit_()
	{
		try
		{
			if (NULL != pfile_)
			{
				pfile_->uninit_();
				pfile_ = oct_toolkits::release_ifile_(pfile_);
			}

			
			log_info_.zero_();
			cur_log_file_.clear();
			has_written_bytes_	= 0;
		}
		catch (...)
		{
			;/// take them and do not expand them
		}
	}

	/// --------------------------------------------------------------------------------------------------------
	///	@brief: 将日志文件中的路径转为对应平台的方式
	/// --------------------------------------------------------------------------------------------------------
	void log_imp::get_real_path_()
	{
#if defined(os_is_win)

		int replace_pos = log_info_.path_.find('/');
		if (0 <= replace_pos)
			std::replace(log_info_.path_.begin(), log_info_.path_.end(), '/', '\\');


#elif defined(os_is_linux)
		int replace_pos = log_info_.path_.find('\\');
		if (0 <= replace_pos)
			std::replace(log_info_.path_.begin(), log_info_.path_.end(), '\\', '/');
#endif /// 
	}

	/// --------------------------------------------------------------------------------------------------------
	///	@brief: 创建日志存放的文件夹
	/// --------------------------------------------------------------------------------------------------------
	bool log_imp::create_log_path_()
	{
#if defined (os_is_win)
		std::string str_sub_dir		= log_info_.path_;
		std::string str_create_dir;
		bool is_end					= true;

		using namespace std;
		while (true == is_end)
		{
			int sub_path_pos		= str_sub_dir.find('\\');
			if (0 <= sub_path_pos)
			{
				/// 得到要检查的目录
				str_create_dir		+= str_sub_dir.substr(0, sub_path_pos + 1);/// +std::string("\\");
				//cout << "\n str_create = " << str_create_dir.c_str();
				/// 创建目录, 如果创建失败，则直接返回false
				bool ret = create_dir_(str_create_dir);
				if (false == ret)
					return false;

				/// 继续找下一级目录
				str_sub_dir			= str_sub_dir.substr(sub_path_pos + 1, str_sub_dir.length());
			}
			/// 说明已经找到了最后一级目录，
			else
			{
				/// 创建最后一级目录
				str_create_dir		+= str_sub_dir;
				bool ret = create_dir_(str_create_dir);
				if (false == ret)
					return false;

				is_end				= false;
			}
		}
#elif defined (os_is_linux)

#endif /// 

		return true;
	}

	/// --------------------------------------------------------------------------------------------------------
	///	@brief: 创建目录
	/// --------------------------------------------------------------------------------------------------------
	bool log_imp::create_dir_(const std::string str_create)
	{

		/// 文件夹不存在则创建文件夹
#if defined(os_is_win)
		if (-1				==	_access(str_create.c_str(), 0) )
#elif defined(os_is_linux)
		if (-1				==	access(str_create.c_str(), 0) )
#endif ///
		{
#if defined(os_is_win)
			
			int ret			= _mkdir(str_create.c_str());
#elif defined(os_is_linux)
			int ret			= mkdir(str_create.c_str(), S_IRWXU);
#endif /// 

			/// 创建失败，则返回 false;
			if (0			!= ret)
				return false;
		}
		else
			;	///文件夹已经存在

		return true;
	}

	/// --------------------------------------------------------------------------------------------------------
	///	@brief: 检查磁盘剩余空间
	/// --------------------------------------------------------------------------------------------------------
	bool log_imp::check_disk_free_space_()
	{
#if defined (os_is_win)

		/// 得到盘符， 例如： "C:\\"
		std::string str_disk_name	= log_info_.path_.substr(0, 3);
		DWORD64 qwFreeBytesToCaller = 0;
		DWORD64 qwTotalBytes		= 0;
		DWORD64 qwFreeBytes			= 0;

		///使用GetDiskFreeSpaceEx获取磁盘信息并打印结果
		 BOOL bResult				= GetDiskFreeSpaceExA(	str_disk_name.c_str(),
															(PULARGE_INTEGER)&qwFreeBytesToCaller,
															(PULARGE_INTEGER)&qwTotalBytes,
															(PULARGE_INTEGER)&qwFreeBytes) ;

		 /// 读取成功
		 if (bResult)
		 {
			 /// 如果还剩下5G空闲空间，则禁止写入日志
			 qwFreeBytesToCaller	/= (1024 * 1024 * 1024);

			 if (5 >= qwFreeBytesToCaller)
				 return false;
			
			 return true;
		 }
		 /// 读取失败
		 else
			 return false;



#elif defined (os_is_linux)
		oct_toolkits::toolkits kits;

		std::string str_executbale_dir				= kits.get_executable_dir_();

		/// 用于获取磁盘剩余空间
		struct statfs diskInfo;
		statfs(str_executbale_dir.c_str(), &diskInfo);

		unsigned long long blocksize                = diskInfo.f_bsize;	//每个block里包含的字节数
		unsigned long long totalsize                = blocksize * diskInfo.f_blocks; 	//总的字节数，f_blocks为block的数目

		unsigned long long freeDisk					= diskInfo.f_bfree * blocksize;		///剩余空间的大小
		unsigned long long availableDisk			= diskInfo.f_bavail * blocksize; 	///可用空间大小

		/// 如果剩余可用空间不足5G， 则禁止创建日志文件
		if (5										> (freeDisk >> 30))
			return false;
		else
			return true;
#endif /// 
	}

	/// --------------------------------------------------------------------------------------------------------
	///	@brief: 返回日志文件名
	/// --------------------------------------------------------------------------------------------------------
	std::string log_imp::get_log_file_name_()
	{
		/// 日志文件前面加上日期字符串： [yyyy-mm-dd hh:mm:ss:sss] 
		oct_toolkits::st_now_date date			= kits_.get_date_now_();
		std::string str_time					= kits_.str_format_("%04d_%02d_%02d_%02d_%02d_%02d_%04d", 
													date.year_, date.month_, date.day_, date.hour_, date.minute_, date.second_, date.mill_sec_);
										
#if defined(os_is_win)
		/// 构造字符串
		std::string str_log_name				= log_info_.path_ + std::string("\\") + log_info_.prefix_ + str_time  + std::string("_") + log_info_.postfix_ + log_info_.type_;
		return str_log_name;
#elif defined (os_is_linux)
		std::string str_log_name				= log_info_.path_ + std::string("/") + log_info_.prefix_ + str_time + std::string("_") + log_info_.postfix_ + log_info_.type_;
		return str_log_name;
#endif /// 


	}


	/// --------------------------------------------------------------------------------------------------------
	///	@brief: 
	/// --------------------------------------------------------------------------------------------------------
	bool log_imp::before_logging_(const unsigned long long cur_write)
	{
		/// 1. 实时检查当前磁盘空剩余空间， 如果不足写入日志，则返回false
		bool free_space_is_ok				= check_disk_free_space_();
		if (false							== free_space_is_ok)
			return false;

		/// 2.剩余空间足够写入日志， 则统计上一次 + 这一次写入的总字节数
		unsigned long long total_write		= has_written_bytes_	 + cur_write;
		
		/// 还没到5M，则继续写入
		if (oct_toolkits::klog_file_size	>= total_write)
			return true;

		/// 超过了5M, 则重新创建日志文件, 已经写入字节数归零
		has_written_bytes_					= 0;


		/// 关闭当前文件
		if (pfile_)
		{
			pfile_->uninit_();
			pfile_							= oct_toolkits::release_ifile_(pfile_);
		}
		else
			;
		
		/// 创建新的日志文件
		int ret								= create_log_file_();

		if (0								!= ret)
			return false;

		return true;
	}

	/// --------------------------------------------------------------------------------------------------------
	///	@brief: 
	/// --------------------------------------------------------------------------------------------------------
	int log_imp::create_log_file_()
	{
		if ((NULL == pfile_)	|| (nullptr == pfile_))
			pfile_				= oct_toolkits::create_ifile_();

		if ((NULL == pfile_)	|| (nullptr == pfile_))
			return 1;


		/// 获取日志文件名
		cur_log_file_			= get_log_file_name_();

		/// 创建成功
		int ret					= pfile_->init_(cur_log_file_, oct_toolkits::kmode_w);
		if (0 != ret)
			return 2;

		return 0;
	}

}