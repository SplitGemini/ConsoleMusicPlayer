//************************************************************
//		本文件为全局变量、宏、枚举类型和全局函数的定义
//************************************************************
#pragma once
#include <ncursesw/curses.h>
#include <vector>
#include <dirent.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <string>
#include <utility>
#include <locale>
#include <codecvt>
#include <string.h>

#include "ini.h"

using std::string;
using std::wstring;
using std::vector;

#include "bass.h"

#define NEXT (-999)		//定义“下一曲”常量
#define PREVIOUS (-998)		//定义“上一曲”常量

#define LYRIC_REFRESH 100		//定义歌词刷新的间隔时间（毫秒）

#define MIN_WIDTH 40		//定义控制台窗口的最小宽度
#define MIN_HIGHT 10		//定义控制台窗口的最小高度

#define WIDTH_THRESHOLD 80		//定义歌词从单行显示切换到多行显示时的界面宽度阈值

#define MAX_NUM_LENGTH 80		//定义获取音频文件长度数量的最大值

const char* VERSION = "1.36";	//程序版本

enum class Command
{
	OPEN,
	PLAY,
	CLOSE,
	PAUSE,
	STOP,
	PLAY_PAUSE,
	FF,	//快进
	REW,		//快倒
	VOLUME_UP,
	VOLUME_DOWN,
	SEEK
};

enum Color
{
	BLACK = 0,
	DARK_BLUE,
	DARK_GREEN,
	DARK_CYAN,
	DARK_RED,
	DARK_PURPLE,
	DARK_YELLOW,
	DARK_WHITE,
	GRAY,
	BLUE,
	GREEN,
	CYAN,
	RED,
	PURPLE,
	YELLOW,
	WHITE
};

struct Time
{
	int min;
	int sec;
	int msec;
};

//将int类型的时间（毫秒数）转换成Time结构
Time int2time(int time)
{
	Time result;
	result.msec = time % 1000;
	result.sec = time / 1000 % 60;
	result.min = time / 1000 / 60;
	return result;
}

//将Time结构转换成int类型（毫秒数）
inline int time2int(Time time)
{
	return time.msec + time.sec * 1000 + time.min * 60000;
}

bool operator>(Time time1, Time time2)
{
	if (time1.min != time2.min)
		return (time1.min > time2.min);
	else if (time1.sec != time2.sec)
		return(time1.sec > time2.sec);
	else if (time1.msec != time2.msec)
		return(time1.msec > time2.msec);
	else return false;
}

bool operator==(Time time1, Time time2)
{
	return (time1.min == time2.min && time1.sec == time2.sec && time1.msec == time2.msec);
}

bool operator>=(Time time1, Time time2)
{
	if (time1.min != time2.min)
		return (time1.min > time2.min);
	else if (time1.sec != time2.sec)
		return(time1.sec > time2.sec);
	else if (time1.msec != time2.msec)
		return(time1.msec > time2.msec);
	else return true;
}

int operator-(Time time1, Time time2)
{
	return (time1.min - time2.min) * 60000 + (time1.sec - time2.sec) * 1000 + (time1.msec - time2.msec);
}

void InitColors(){
	// 	BLACK = 0,
	// DARK_BLUE  = 1,
	// DARK_GREEN  = 2,
	// DARK_CYAN  = 3,
	// DARK_RED  = 4,
	// DARK_PURPLE  = 5,
	// DARK_YELLOW  = 6,
	// DARK_WHITE  = 7,
	// GRAY  = 8,
	// BLUE  = 9,
	// GREEN  = 10,
	// CYAN  = 11,
	// RED  = 12,
	// PURPLE  = 13,
	// YELLOW  = 14,
	// WHITE  = 15
	start_color();
	use_default_colors();
	init_pair(0, COLOR_BLACK, -1);
	init_pair(1, COLOR_BLUE, -1);
	init_pair(2, COLOR_GREEN, -1);
	init_pair(3, COLOR_CYAN, -1);
	init_pair(4, COLOR_RED, -1);
	init_pair(5, COLOR_BLUE, -1);
	init_pair(6, COLOR_YELLOW, -1);
	init_pair(7, COLOR_WHITE, -1);
}

int kbhit(void)
{
    int ch = getch();

    if (ch != ERR) {
        ungetch(ch);
        return 1;
    } else {
        return 0;
    }
}

//获取一个键盘输入按键并将其返回
int GetKey()
{
	int key{ getch() };
	if (key >= 'a' && key <= 'z')		//如果按的是小写字母，则自动转换成大写字母
		key -= 32;
	return key;
}

static std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;

// convert string to wstring
inline std::wstring to_wide_string(const std::string& input)
{
	return converter.from_bytes(input);
}
// convert wstring to string 
inline std::string to_byte_string(const std::wstring& input)
{
	return converter.to_bytes(input);
}

int GetRealPrintLength(const string &str){
	auto wstr = to_wide_string(str);
	int full_count = 0;
	for(auto &wc : wstr){
		if(wc < 0 || wc >= 128)
			full_count += 1;
	}
	return wstr.length() + full_count;
}

int GetRealPrintLength(const wstring &str){
	int full_count = 0;
	for(auto &wc : str){
		if(wc < 0 || wc >= 128)
			full_count += 1;
	}
	return str.length() + full_count;
}


bool EndWith(const std::string &full, const std::string &suffix) {
    return !suffix.empty() && full.length() > suffix.length()
           && 0 == full.compare(full.length() - suffix.length(), suffix.length(), suffix);
}

//获取path路径下的指定格式的文件的文件名，并保存在files容器中。format容器中储存着要获取的文件格式。最多只获取max_files个文件。
void GetAllFormatFiles(wstring path, vector<wstring>& files, const vector<string>& format, size_t max_file = 99999)
{

	DIR                     *dir;
    struct dirent           *diread;

    if ((dir = opendir(to_byte_string(path).c_str())) != nullptr) {
        while ((diread = readdir(dir)) != nullptr) {
			if (files.size() >= max_file) break;

			auto file_path = string{diread->d_name};
            if (diread->d_type == DT_REG && 
				std::any_of(format.begin(), format.end(), [&file_path](const string &fmt){
					return EndWith(file_path, fmt);
				})
			) {
                files.push_back(to_wide_string(string{diread->d_name}));
            }
        }
        closedir(dir);
    }
}

bool FileExist(const wstring& file)
{
	return access(to_byte_string(file).c_str(), F_OK) == 0;
}

//判断文件类型是否为midi音乐
bool FileIsMidi(const wstring& file_name)
{
	size_t length{ file_name.size() };
	return (file_name.substr(length - 4, 4) == L".mid" || file_name.substr(length - 5, 5) == L".midi");
}

wstring GetExePath()
{
	char result[ PATH_MAX ];
    ssize_t count = readlink( "/proc/self/exe", result, PATH_MAX );
    std::string appPath = std::string( result, (count > 0) ? count : 0 );

    std::size_t found = appPath.find_last_of("/");
    return to_wide_string(appPath.substr(0,found));
}
