#include <iostream>
#include <oct_toolkits.h>


void call_file()
{

	using namespace oct_toolkits;
	using namespace std;
	/// 创建文件对象
	ifile* pfile = create_ifile_();
	if (NULL == pfile)
	{
		cout << "call_file, pfile = null";
		return;
	}

	/// 1. 
	std::string str_file("C:/game/file_demo/ssr.txt");
	
	/// 下面这样的方式也可以
	//std::string str_file("C:\\game\\file_demo\\ssr.txt");
	/// ----------------------------------------------------------------------------------------
	/// 下面是检查文件是否存在
	if (true == pfile->is_existed_(str_file))
	{
		cout << "文件存在\n\n";
	}
	else
		cout << "文件不存在\n\n";
	
	
	int ret = 0;
	ret = pfile->init_(str_file, kmode_rtp);

	if (0 != ret)
	{
		cout << "call file, init != 0, ret=" << ret;

		pfile = release_ifile_(pfile);
		return;
	}


	/// 读取文件长度
	cout << "文件长度=" << pfile->length_() << "\n";
	cout << "文件是否打开=" << pfile->is_opened_() << "\n";
	cout << "文件是=" << pfile->file_();
	
	/// 下面读取文件内容
	cout << "\nstring 读取文件------";
	std::string str_content;
	ret = pfile->read_(str_content);
	if (0 != ret)
	{
		//cout << "文件读取失败, ret=" << ret;
	}
	else
	{
		//cout << "文件内容是" << str_content.c_str() << "\n";
	}


	/// ----------------------------------------------------------------------------------------
	/// 
	char arr[20] = { 0 };
	cout << "\nchar 读取文件------";
	ret = pfile->read_(arr, 20);
	if (0 != ret)
	{
		cout << "文件读取失败, ret=" << ret;
	}
	else
	{
		//cout << "文件内容是" << arr << "\n";
	}


	/// 下面是写文件
	//cout << "\n下面是std::string写文件";
	std::string str_write("ABCDEFG");
	ret = pfile->write_(str_write);
	if (0 != ret)
	{
		cout << "写文件发失败，ret=" << ret << "\n";
	}
	else
	{
		cout << "写文加成功, ret=" << ret << "\n";
	}

	cout << "\n\n下面是char 写文件-------";
	char arr_write[] = { "0987654321" };
	ret = pfile->write_(arr_write, 10);
	if (0 != ret)
	{
		cout << "写文件发失败，ret=" << ret << "\n";
	}
	else
	{
		cout << "写文加成功, ret=" << ret << "\n";
	}

	pfile = release_ifile_(pfile);
}



void call_log_()
{

	using namespace oct_toolkits;
	using namespace std;
	/// 创建文件对象
	ilog* plog = create_ilog_();
	if (NULL == plog)
	{
		cout << "call_log, plog = null";
		return;
	}


	st_log_info info;
	info.path_					= std::string("C:\\game\\file_demo\\log\\send");
	info.prefix_				= std::string("C");
	info.postfix_				= std::string("fc");
	info.type_					= std::string(".log");

	int ret						= plog->init_(info);
	if (0						!= ret)
	{
		cout << "call log, init, ret =" << ret;
	}

	else
	{
		cout << "call log, init success";
	}
	/// ----------------------------------------------------------------------------------------

	/// 写文本
	std::string str_text("1234567890");
	plog->log_text_(str_text);
	char arr2[] = {"9876543210"};
	plog->log_text_(arr2, 10);

	/// 写十六进制
	plog->log_hex_(str_text);
	plog->log_hex_(arr2, 10);

	plog = release_ilog_(plog);

}



void check_file()
{

	using namespace oct_toolkits;
	oct_toolkits::ifile* my_file	= oct_toolkits::create_ifile_();
	if (NULL == my_file )
		return;

	toolkits tk;

	std::string str= "C:\\game\\中文测试\\1.txt";
	/// str  = tk.utf8_to_ascii_(str);
	bool ret = my_file->is_existed_(str);

	int ret2 = my_file->init_(str, oct_toolkits::kmode_r);

	std::cout << "ret=" << ret2 << "\n\n";
  

	my_file	= oct_toolkits::release_ifile_(my_file);
}


int main(int argc, char* argv[], char* env[])
{

	check_file();

	//call_file();
	//call_log_();
	//using namespace oct_toolkits;
	//using namespace std;
	//toolkits kkkkk;
	//std::string str = kkkkk.get_executable_dir_();
	//cout << "str=" << str.c_str() << "\n\n";


	system("pause");
	return 0;
}