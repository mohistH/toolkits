#include "ifile/file_assistant.h"
#include "universe_def.h"
#include <algorithm>

#if defined(os_is_win)

#elif defined(os_is_linux)
	#include <unistd.h>
	#include <fcntl.h>
#endif /// 


namespace oct_file
{



	/// --------------------------------------------------------------------------------------------------------
	///	@brief: constructor
	/// --------------------------------------------------------------------------------------------------------
	file_assistant::file_assistant()
		: str_file_()
		, pfile_(NULL)
	{

	}

	/// --------------------------------------------------------------------------------------------------------
	///	@brief: 
	/// --------------------------------------------------------------------------------------------------------
	file_assistant::~file_assistant()
	{
		uninit_();
	}

	/// --------------------------------------------------------------------------------------------------------
	///	@brief: 初始化
	/// --------------------------------------------------------------------------------------------------------
	int file_assistant::init_(const std::string& str_file, const oct_toolkits::en_file_open_mode open_mode)
	{

		/// ----------------------------------------------------------------------------------------
		/// 防止多次调用init_函数
		uninit_();

		/// ----------------------------------------------------------------------------------------
		/// 1.参数check
		
		/// 1.1 文件字符串为空
		if (0									== str_file.length())
			return 1;
		
		/// 1.2 打开的模式不是枚举的之一
		std::string str_mode;
		bool is_ok								= get_mode_(open_mode, str_mode);
		/// 文件打开方式不正确
		if (false								== is_ok)
			return 2;
		
		/// ----------------------------------------------------------------------------------------
		/// 打开文件
		get_real_file_(str_file);

		pfile_									= fopen(str_file_.c_str(), str_mode.c_str());
		
		/// 文件打开失败
		if (NULL								== pfile_)
			return 3;
		
		return 0;
	}

	/// --------------------------------------------------------------------------------------------------------
	///	@brief: 返回初始化的文件
	/// --------------------------------------------------------------------------------------------------------
	std::string file_assistant::file_()
	{
		return str_file_;
	}

	/// --------------------------------------------------------------------------------------------------------
	///	@brief: 文件是否打开
	/// --------------------------------------------------------------------------------------------------------
	bool file_assistant::is_opened_()
	{
		return  ((NULL == pfile_) ? false : true);
	}

	/// --------------------------------------------------------------------------------------------------------
	///	@brief: 返回文件长度
	/// --------------------------------------------------------------------------------------------------------
	int file_assistant::length_()
	{
		if (NULL						== pfile_)
			return -2;
		
		/// 将文件指针移动到文件尾
		fseek(pfile_, 0, SEEK_END);

		int file_length_bytes			= ftell(pfile_);

		//fseek(pfile_, 0, SEEK_SET);

		return file_length_bytes;
	}

	/// --------------------------------------------------------------------------------------------------------
	///	@brief: 写文件
	/// --------------------------------------------------------------------------------------------------------
	int file_assistant::write_(const char* pbuf, unsigned int pbuf_len)
	{
		if (NULL == pfile_)
			return 1;

		if ((NULL == pbuf)	|| (nullptr == pbuf))
			return 2;

		fseek(pfile_, 0, SEEK_END);
		int ret				= fwrite(pbuf, pbuf_len, 1, pfile_);
		fflush(pfile_);
		if ( 1				!= ret)
			return 3;

		return 0;
	}

	/// --------------------------------------------------------------------------------------------------------
	///	@brief: 
	/// --------------------------------------------------------------------------------------------------------
	int file_assistant::write_(const std::string& str_write)
	{
		if (NULL == pfile_)
			return 1;

		if (0 == str_write.length())
			return 2;

		fseek(pfile_, 0, SEEK_END);
		int ret				= fwrite(str_write.c_str(), str_write.length(), 1, pfile_);
		fflush(pfile_);
		if ( 1				!= ret)
			return 3;

		return 0;
	}

	/// --------------------------------------------------------------------------------------------------------
	///	@brief: 
	/// --------------------------------------------------------------------------------------------------------
	int file_assistant::read_(std::string& str_read)
	{
		if (NULL				== pfile_)
			return 1;
	

		/// 得到文件长度
		unsigned int len		= length_();
		/// 申请读取文件的缓存
		char *pread				= (char*)malloc(len + 1);
		if (NULL				== pread)
			return 2;
		
		/// 先把文件指针移动到文件头
		fseek(pfile_, 0, SEEK_SET);
		int ret					= fread(pread, len, 1, pfile_);

		/// ----------------------------------------------------------------------------------------
		/// 读取失败
		if (1					!= ret)
		{
			free(pread);
			pread				= NULL;
			return 3;
		}
		

		/// ----------------------------------------------------------------------------------------
		/// 读取成功
		pread[len]				= '\0';

		/// 读取成功
		str_read				= std::string(pread, len);

		free(pread);
		pread					= NULL;

		return 0;
	}

	/// --------------------------------------------------------------------------------------------------------
	///	@brief: 读文件内容， 参数【pout_buf】需要外部申请，函数内不会维护
	/// --------------------------------------------------------------------------------------------------------
	int file_assistant::read_(char* pout_buf, unsigned int pout_buf_len)
	{
		if (NULL				== pfile_)
			return 1;
	
		if ((NULL == pout_buf) || (nullptr == pout_buf) || (0 == pout_buf_len))
			return 2;

		fseek(pfile_, 0, SEEK_SET);
		//long pos = ftell(pfile_);
		int ret					= fread(pout_buf, pout_buf_len, 1, pfile_);

		return 0;
	}

	/// --------------------------------------------------------------------------------------------------------
	///	@brief: 检查文件是否存在
	/// --------------------------------------------------------------------------------------------------------
	bool file_assistant::is_existed_(const std::string& str_check_file)
	{

//		std::string str_file;
//#ifdef UNICODE
//		strPortName = wstringToString(portName);
//		strFriendlyName = wstringToString(fname);
//#else
//		str_file		= str_check_file;
//#endif

#if defined(os_is_win)
		struct stat st;

		return (0 == stat(str_check_file.c_str(), &st));
#elif defined(os_is_linux)
		return (-1 == access(str_check_file.c_str(), F_OK)? false : true);

#endif /// 
	}

	/// --------------------------------------------------------------------------------------------------------
	///	@brief: 释放资源，
	/// --------------------------------------------------------------------------------------------------------
	int file_assistant::uninit_()
	{
		try
		{
			if (pfile_)
			{
				int ret = fclose(pfile_);
				if (0 != ret)
				{
					;/// 关闭文件异常
				}
				pfile_ = NULL;
			}

			str_file_.clear();
		}
		catch (...)
		{
			;/// take them and do not expand
		}

		return 0;
	}

	/// --------------------------------------------------------------------------------------------------------
	///	@brief: 将参数file_mode中的对应到C语言文件操作方式
	/// --------------------------------------------------------------------------------------------------------
	bool file_assistant::get_mode_(const oct_toolkits::en_file_open_mode file_mode, std::string& out_str_mode)
	{
		/// 返回值
		bool mode_is_correct						= true;
	
		switch (file_mode)
		{

		case oct_toolkits::kmode_r:
			out_str_mode							= std::string("r");
			break;

		case oct_toolkits::kmode_rp:
			out_str_mode							= std::string("r+");
			break;

		case oct_toolkits::kmode_rbp:
			out_str_mode							= std::string("rb+");
			break;

		case oct_toolkits::kmode_rtp:
			out_str_mode							= std::string("rt+");
			break;

		case oct_toolkits::kmode_w:
			out_str_mode							= std::string("w");
			break;

		case oct_toolkits::kmode_wp:
			out_str_mode							= std::string("w+");
			break;


		case oct_toolkits::kmode_append:
			out_str_mode							= std::string("a");
			break;

		case oct_toolkits::kmode_append_p:
			out_str_mode							= std::string("a+");
			break;

		case oct_toolkits::kmode_wb:
			out_str_mode							= std::string("wb");
			break;

		case oct_toolkits::kmode_wbp:
			out_str_mode							= std::string("wb+");
			break;

		case oct_toolkits::kmode_wtp:
			out_str_mode							= std::string("wt+");
			break;

		case oct_toolkits::kmode_atp:
			out_str_mode							= std::string("at+");
			break;

		case oct_toolkits::kmode_abp:
			out_str_mode							= std::string("ab+");
			break;

		default:
			mode_is_correct							= false;
			break;
		}
		
		return mode_is_correct;
	}

	/// --------------------------------------------------------------------------------------------------------
	///	@brief: 将参数中的文件转为对应平台能适应的文件路径
	/// --------------------------------------------------------------------------------------------------------
	void file_assistant::get_real_file_(std::string str_src)
	{
#if defined(os_is_win)

		int replace_pos				= str_src.find('/');
		if (0						<= replace_pos)
			std::replace(str_src.begin(), str_src.end(), '/', '\\');

		str_file_					= str_src;

#elif defined(os_is_linux)
		int replace_pos				= str_src.find('\\');
		if (0						<= replace_pos)
			std::replace(str_src.begin(), str_src.end(), '\\', '/');
		
		str_file_					= str_src;
#endif /// 

	}

}