#ifndef file_assistant_h
#define file_assistant_h
#include "oct_toolkits.h"
#include <stdio.h>

namespace oct_file
{

	class file_assistant : public oct_toolkits::ifile
	{
	public:
		explicit file_assistant();
		virtual ~file_assistant();



		///  --------------------------------------------------------------------------------
		///  @brief:	初始化
		///  @str_file - 文件所在绝对路径， 例如： C:/demo/logs/1.txt
		///  @open_mode - 以哪种方式打开文件
		///  @return - int	
		/// 			0 - 成功
		///				1 - 失败, 参数【str_file】字符串为空
		///				2 - 失败，参数【open_mode】不是给定的枚举范围
		///				3 - 失败，文件打开失败。
		///  --------------------------------------------------------------------------------
		virtual int init_(const std::string& str_file, const oct_toolkits::en_file_open_mode open_mode) ;


		///  --------------------------------------------------------------------------------
		///  	返回当前初始化的文件
		///  @return - std::string	
		/// 			
		///  --------------------------------------------------------------------------------
		virtual std::string file_();


		///  --------------------------------------------------------------------------------
		///  	文件是否打开
		///  @return - bool	
		/// 			true - 打开
		///				false - 没有打开
		///  --------------------------------------------------------------------------------
		virtual bool is_opened_();


		///  --------------------------------------------------------------------------------
		///  	返回文件长度,单位： 字节
		///  @return - int	
		/// 			返回值X， 
		///				-2 - 失败，请先执行初始化
		///				X >=0 , 文件长度，单位：字节
		///  --------------------------------------------------------------------------------
		virtual int length_() ;


		///  --------------------------------------------------------------------------------
		///  	写文件, 将数据写入
		///  @pbuf - 待写入内容
		///  @pbuf_len - 待写入数据长度
		///  @return - int	
		/// 			0 - 成功
		///				1 - 失败，请先初始化
		///				2 - 失败，参数【pbuf】为nullptr或者NULL
		///				3 - 失败，写文件失败
		///  --------------------------------------------------------------------------------
		virtual int write_(const char* pbuf, unsigned int pbuf_len) ;
		///  --------------------------------------------------------------------------------
		///  	写文件
		///  @str_write - 待文件内容
		///  @return - int	
		/// 			0 - 成功
		///				1 - 失败，请先初始化
		///				2 - 失败，参数【str_write】的长度为0
		///				3 - 失败，写文件失败
		///  --------------------------------------------------------------------------------
		virtual int write_(const std::string& str_write) ;

		///  --------------------------------------------------------------------------------
		///  	将文件内容儒道参数【str_read】中
		///  @str_read - 返回文件内容
		///  @return - int	
		/// 			0 - 成功
		///				1 - 失败，请先初始化
		///				2 - 失败，读取文件缓冲创建失败
		///				3 - 失败，读取文件失败
		///  --------------------------------------------------------------------------------
		virtual int read_(std::string& str_read) ;

		///  --------------------------------------------------------------------------------
		///   读文件内容， 参数【pout_buf】需要外部申请，函数内不会维护，读取成功后，文件内容存放在pout_buf中。
		///  @pout_buf - 返回读取文件内容
		///  @pout_buf_len - 【pout_buf】缓冲区的长度
		///  @return - int	
		/// 			0 - 成功
		///				1 - 失败，请先初始化
		///				2 - 失败，参数【pout_buf】为空 或者 参数【pout_buf_len】的值是0
		///  --------------------------------------------------------------------------------
		virtual int read_(char* pout_buf, unsigned int pout_buf_len) ;

		///  --------------------------------------------------------------------------------
		///  	判断参数【str_check_file】的文件是否存在
		///  @str_check_file - 
		///  @return - bool	
		/// 			
		///  --------------------------------------------------------------------------------
		virtual bool is_existed_(const std::string& str_check_file) ;

		///  --------------------------------------------------------------------------------
		///  	避免析构时出现异常，请销毁前先调用该函数完成内部资源释放
		///  @return - int	
		/// 			0 - 成功
		///  --------------------------------------------------------------------------------
		virtual int uninit_() ;


	private:
		///  --------------------------------------------------------------------------------
		///  	将参数file_mode中的对应到C语言文件操作方式
		///  @file_mode - 参数中文件打开的方式
		///	 @ out_str_mode - 对应C文件读写方式
		///  @return - std::string	
		/// 			false - 传入的 file_mode错误
		///				true - 传入的file_mode正确
		///  --------------------------------------------------------------------------------
		bool get_mode_(const oct_toolkits::en_file_open_mode file_mode, std::string& out_str_mode);

		///  --------------------------------------------------------------------------------
		///  将参数中的文件转为对应平台能适应的文件路径
		///  @str_src - 原始文件路径
		///  --------------------------------------------------------------------------------
		void get_real_file_(std::string str_src);

	private:
		/// ----------------------------------------------------------------------------------------
		/// 当前打开操作的时哪个文件
		std::string			str_file_;

		/// 文件操作指针
		FILE*				pfile_;

	};

}

#endif /// file_assistant_h