  > 自己项目需要，封装C语言读写文件。 为了兼容低版本的编译器，因为低版本的编译器（比如，Vs2010,Vs2008）他们可能不支持 modern c++。 

## 项目
- 使用 **cmake**管理的项目。
- 可以在 [gitee](https://gitee.com/mohistH/file_assistant/tree/master) 下载源码


  ## 接口说明
  |函数名|功能|
  |:---|:---|
  |init_|初始化， 打开文件|
  |file_|获取初始化的文件|
  |is_opened_|检查文件是否打开|
  |length_|返回文件长度，单位：字节|
  |write_|写文件，函数有重载|
  |read_|读文件，函数有重载|
  |is_existed_|检查参数中的文件是否存在|
  |uninit_|释放资源,销毁文件读写对象前， 请先调用该函数，避免析构出现异常|

  ## 一个使用范例
```
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
		cout << "文件读取失败, ret=" << ret;
	}
	else
	{
		cout << "文件内容是" << str_content.c_str() << "\n";
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
		cout << "文件内容是" << arr << "\n";
	}


	/// 下面是写文件
	cout << "\n下面是std::string写文件";
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


int main(int argc, char* argv[], char* env[])
{
	call_file();

	//system("pause");
	return 0;
}
```