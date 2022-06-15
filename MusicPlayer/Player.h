//************************************************************
//		本文件为播放器类的定义，为实现播放器的主要代码
//************************************************************
#pragma once

#include <string>
#include <vector>
#include <tuple>
#include <deque>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <iomanip>

#include "Common.h"
#include "Console.h"
#include "Lyrics.h"
#include "ini.h"

using std::ofstream;
using std::ifstream;
using std::wstring;
using std::string;
using std::vector;
using std::deque;

using PathInfo = std::tuple<wstring, int, int>;		//储存路径信息
const size_t PATH{ 0 }, TRACK{ 1 }, POSITION{ 2 };		//定义用于表示tuple<>对象中的3个字段的常量

class CPlayer
{
private:
	HSTREAM m_musicStream;
	vector<wstring> m_playlist;		//播放列表，储存音乐文件的文件名
	vector<Time> m_all_song_length;		//储存每个音乐文件的长度
	wstring m_path;		//当前播放文件的路径
	wstring m_lyric_path;	//歌词文件夹的路径
	wstring m_current_file_name;		//正在播放的文件名
	deque<PathInfo> m_recent_path;		//最近打开过的路径

	Time m_song_length;		//正在播放的文件的长度
	Time m_current_position;		//当前播放到的位置
	int m_song_length_int;		//正在播放的文件的长度（int类型）
	int m_current_position_int;		//当前播放到的位置（int类型）

	int m_index{ 0 };	//当前播放音乐的序号
	int m_song_num{ 0 };	//播放列表中的歌曲总数
	int m_error_code{ 0 };
	int m_playing{ 0 };		//正在播放标志（0：已停止，1：已暂停，2：正在播放）
	int m_repeat_mode{ 0 };		//循环模式（0：顺序播放，1：随机播放，2：列表循环，3：单曲循环）
	int m_volume{ 100 };		//音量（百分比）

	int m_display_page{ 1 };		//当前显示的播放列表的页
	int m_total_page;		//播放列表的总页数
	int m_song_per_page;		//播放列表每页显示几首歌曲
	
	CLyrics m_Lyrics;		//歌词

	int m_width;		//窗口宽度
	int m_hight;		//窗口高度
	
	char m_font[32];		//控制台字体（字体名称必须以宽字符表示）
	int m_font_size;	//字体大小

	wstring m_config_path;	//配置文件的路径
	wstring m_recent_path_dat_path;	//"recent_path.dat"文件的路径

	vector<int> m_find_result;		//储存查找结果的歌曲序号

	void IniPlayList(bool cmd_para = false);	//初始化播放列表(如果参数为true，表示从命令行直接获取歌曲文件，而不是从指定路径下搜索)
	void IniConsole();		//初始化控制台
	void IniBASS();

	void ChangePath(const wstring& path, int track = 0);		//改变当前路径

	void LoadRecentPath();		//从文件载入最近路径列表
	void EmplaceCurrentPathToRecent();		//将当前路径插入到最近路径中

	void FindFile(const wstring& key_word);		//根据参数中的关键字查找文件，将结果保存在m_find_result中

public:
	void SaveRecentPath() const;		//将最近路径列表保存到文件

public:
	CPlayer();
	void Create();		//构造CPlayer类
	void Create(const vector<wstring>& files);	//构造CPlayer类
	void MusicControl(Command command);		//控制音乐播放
	bool SongIsOver() const;		//判断当前音乐是否播放完毕
	void GetCurrentPosition();		//获取当前播放到的位置
	void GetSongLength();		//获取正在播放文件的长度

	void ShowInfo() const;		//显示歌曲信息
	void ShowPlaylist() const;		//显示播放列表
	void ShowProgressBar() const;		//显示进度条
	void ShowLyrics(bool force_refresh = false) const;		//显示歌词
	void ShowLyricsSingleLine() const;		//显示歌词（单行模式）
	void ShowLyricsMultiLine(bool force_refresh = false) const;		//显示歌词（多行模式）
	void SwitchPlaylist(int iperation);		//播放列表翻页
	int GetCurrentSecond();		//获取当前播放到的位置的秒数（用于使界面每1秒刷新一次）
	bool IsPlaying() const;			//判断当前是否正在播放

	bool PlayTrack(int song_track);		//播放指定序号的歌曲
	
	void SetVolume();		//用m_volume的值设置音量
	void SetPath();		//设置路径
	void SetTrack();		//更改正在播放的歌曲
	void SetRepeatMode();		//更改循环模式
	void SeekTo(int position);

	bool ErrorDispose();		//错误处理（已消除错误返回true，否则返回false）

	void SetTitle() const;		//设置窗口标题
	
	void SaveConfig() const;		//保存配置到ini文件
	void LoadConfig();		//从ini文件读取配置

	void IniLyrics();		//初始化歌词

	void GetWindowsSize(int width, int hight);		//获取窗口宽度和高度
	//void GetConsoleSetting();		//获取控制台中的字体和字体大小的设置

	void Find();			//查找文件
};


inline CPlayer::CPlayer()
{
	m_config_path = GetExePath() + L"/config.ini";
	m_recent_path_dat_path = GetExePath() + L"/recent_path.dat";
}

inline void CPlayer::Create()
{
	IniBASS();
	LoadConfig();
	IniConsole();
	LoadRecentPath();
	EmplaceCurrentPathToRecent();
	IniPlayList();	//初始化播放列表
	SetTitle();		//用当前正在播放的歌曲名作为窗口标题
}

inline void CPlayer::Create(const vector<wstring>& files)
{
	IniBASS();
	LoadConfig();
	IniConsole();
	LoadRecentPath();
	size_t index;
	index = files[0].find_last_of('/');
	m_path = files[0].substr(0, index + 1);
	EmplaceCurrentPathToRecent();
	for (const auto& file : files)
	{
		index = file.find_last_of('/');
		m_playlist.push_back(file.substr(index + 1));
	}
	IniPlayList(true);
	m_current_position_int = 0;
	m_current_position = { 0,0,0 };
	m_index = 0;
	SetTitle();		//用当前正在播放的歌曲名作为窗口标题
}

void CPlayer::IniConsole()
{
	CursorVisible(false);		//隐藏光标
}

inline void CPlayer::IniBASS()
{
	//初始化BASE音频库
	BASS_Init(
		-1,//默认设备
		44100,//输出采样率44100（常用值）
		BASS_DEVICE_CPSPEAKERS,//信号，BASS_DEVICE_CPSPEAKERS 注释原文如下：
							   /* Use the Windows control panel setting to detect the number of speakers.*/
							   /* Soundcards generally have their own control panel to set the speaker config,*/
							   /* so the Windows control panel setting may not be accurate unless it matches that.*/
							   /* This flag has no effect on Vista, as the speakers are already accurately detected.*/
		NULL,//程序窗口,0用于控制台程序
		NULL//类标识符,0使用默认值
	);
}

void CPlayer::IniPlayList(bool cmd_para)
{
	if (!cmd_para)
	{
		vector<string> file_fromat{ "mp3", "wma", "wav", "ogg" };
		GetAllFormatFiles(m_path, m_playlist, file_fromat, 2997);
	}
	std::sort(m_playlist.begin(), m_playlist.end());		//对播放列表按名称排序
	//m_index = 0;
	m_song_num = m_playlist.size();
	m_song_per_page = m_hight - 7;
	//计算播放列表要显示的总页数
	if (m_song_num == 0)
		m_total_page = 1;
	else
		m_total_page = (m_song_num - 1) / m_song_per_page + 1;
	m_display_page = m_index / m_song_per_page + 1;		//设定播放列表中当前显示的页
	if (m_index >= m_song_num) m_index = 0;		//确保当前歌曲序号不会超过歌曲总数

	clear();
	//获取播放列表中每一首歌曲的长度
	wchar_t buff[128] = {0};
	swprintf(buff, sizeof(buff), L"找到%d首歌曲，正在生成播放列表，请稍候……", m_song_num);
	Printstring(buff, 0, 0, DARK_WHITE);
	refresh();
	m_all_song_length.clear();
	m_all_song_length.resize(m_song_num);
	//最多只获取MAX_NUM_LENGTH首歌的长度，超过MAX_NUM_LENGTH数量的歌曲的长度在打开时获得。防止文件夹中音频文件过多导致等待时间过长
	int start;			//初始化时从第几首歌曲开始获取歌曲长度
	int count{ 0 };		//获取到歌曲长度的文件的数量
	if (m_song_num <= MAX_NUM_LENGTH)
		start = 0;
	else
		start = (m_display_page - 1)*m_song_per_page;	//如果播放列表时可歌曲数量超过了MAX_NUM_LENGTH，就从播放列表显示的当前页的第一首歌曲开始获取歌曲长度
	for (int i{ start }; i < m_song_num && count < MAX_NUM_LENGTH; i++, count++)
	{
		m_current_file_name = m_playlist[i];
		auto file_name = to_byte_string(m_path + m_current_file_name);
		m_musicStream = BASS_StreamCreateFile(FALSE, file_name.c_str(), 0, 0, 0);
		GetSongLength();
		m_all_song_length[i] = m_song_length;
		MusicControl(Command::CLOSE);
	}

	clear();

	m_song_length = {0,0,0};
	//m_current_position = {0,0,0};
	if (m_song_num==0)
		m_current_file_name = L"没有找到文件";
	else
		m_current_file_name = m_playlist[m_index];
}

void CPlayer::IniLyrics()
{
	wstring lyric_path{ m_path + m_current_file_name };		//得到路径+文件名的字符串
	lyric_path.replace(lyric_path.size() - 3, 3, L"lrc");		//将最后3个字符的扩展名替换成lrc
	if (FileExist(lyric_path))
	{
		m_Lyrics = CLyrics{ lyric_path };
	}
	else		//当前目录下没有对应的歌词文件时，就在m_lyric_path目录下寻找歌词文件
	{
		lyric_path = m_lyric_path + m_current_file_name;
		lyric_path.replace(lyric_path.size() - 3, 3, L"lrc");
		if(FileExist(lyric_path))
			m_Lyrics = CLyrics{ lyric_path };
		else
			m_Lyrics = CLyrics{};
	}
}

void CPlayer::MusicControl(Command command)
{
	string file_path;
	switch (command)
	{
	case Command::OPEN: 
		m_error_code = 0;
		file_path = to_byte_string(m_path + m_current_file_name);
		m_musicStream = BASS_StreamCreateFile(FALSE, file_path.c_str(), 0, 0, 0);
		GetSongLength();
		if (m_song_num > 0) m_all_song_length[m_index] = m_song_length;		//打开文件后再次将获取的文件长度保存到m_all_song_length容器中
		break;
	case Command::PLAY: BASS_ChannelPlay(m_musicStream, FALSE); m_playing = 2; break;
	case Command::CLOSE: BASS_StreamFree(m_musicStream); m_playing = 0; break;
	case Command::PAUSE: BASS_ChannelPause(m_musicStream); m_playing = 1; break;
	case Command::STOP:
		BASS_ChannelStop(m_musicStream);
		m_playing = 0;
		SeekTo(0);		//停止后定位到0位置
		//GetCurrentPosition();
		break;

	case Command::FF:		//快进
		GetCurrentPosition();		//获取当前位置（毫秒）
		m_current_position_int += 5000;		//每次快进5000毫秒
		if (m_current_position_int > m_song_length_int)
			m_current_position_int -= 5000;
		SeekTo(m_current_position_int);
		break;

	case Command::REW:		//快退
		GetCurrentPosition();		//获取当前位置（毫秒）
		m_current_position_int -= 5000;		//每次快退5000毫秒
		if (m_current_position_int < 0)
			m_current_position_int = 0;		//防止快退到负的位置
		SeekTo(m_current_position_int);
		break;

	case Command::PLAY_PAUSE:
		if (m_playing == 2)
		{
			BASS_ChannelPause(m_musicStream);
			m_playing = 1;
		}
		else
		{
			BASS_ChannelPlay(m_musicStream, FALSE);
			m_playing = 2;
		}
		break;
	case Command::VOLUME_UP:
		if (m_volume < 100)
		{
			m_volume+=2;
			SetVolume();
			SaveConfig();
		}
		break;
	case Command::VOLUME_DOWN:
		if (m_volume > 0)
		{
			m_volume-=2;
			SetVolume();
			SaveConfig();
		}
		break;
	case Command::SEEK:		//定位到m_current_position的位置
		if (m_current_position_int > m_song_length_int)
		{
			m_current_position_int = 0;
			m_current_position = Time{ 0, 0, 0 };
		}
		SeekTo(m_current_position_int);
		break;
	default: break;
	}
}

bool CPlayer::SongIsOver() const
{
	bool song_is_over;
	static int last_pos;
	if (m_playing == 2 && m_current_position_int == last_pos && m_current_position_int != 0
		&& m_current_position_int > m_song_length_int - 1000)	//如果正在播放且当前播放的位置没有发生变化，则判断当前歌曲播放完了
		song_is_over = true;
	else
		song_is_over = false;
	last_pos = m_current_position_int;
	return song_is_over;
}

void CPlayer::GetSongLength()
{
	QWORD lenght_bytes;
	lenght_bytes = BASS_ChannelGetLength(m_musicStream, BASS_POS_BYTE);
	double length_sec;
	length_sec = BASS_ChannelBytes2Seconds(m_musicStream, lenght_bytes);
	m_song_length_int = static_cast<int>(length_sec * 1000);
	if (m_song_length_int == -1000) m_song_length_int = 0;
	m_song_length = int2time(m_song_length_int);		//将长度转换成Time结构
}

void CPlayer::GetCurrentPosition()
{
	QWORD pos_bytes;
	pos_bytes = BASS_ChannelGetPosition(m_musicStream, BASS_POS_BYTE);
	double pos_sec;
	pos_sec = BASS_ChannelBytes2Seconds(m_musicStream, pos_bytes);
	m_current_position_int = static_cast<int>(pos_sec * 1000);
	if (m_current_position_int == -1000) m_current_position_int = 0;
	m_current_position = int2time(m_current_position_int);
}

/* int CPlayer::GetVolume()
{
	char buff[16];
	int volume;
	mciSendStringW(("status \"" + m_path + m_current_file_name + "\" volume").c_str(), buff, 15, 0);
	volume = atoi(buff);
	m_volume = volume;
	return m_volume;
}*/
 
void CPlayer::SetVolume()
{
	float volume = static_cast<float>(m_volume) / 100.0f;
	BASS_ChannelSetAttribute(m_musicStream, BASS_ATTRIB_VOL, volume);
}
 
void CPlayer::ShowInfo() const
{
	//显示播放状态
	int song_name_length{ m_width - 25 };		//歌曲标题显示的半角字符数
	if (m_error_code || m_musicStream == 0)
	{
		Printstring(L"播放出错", 0, 0, WHITE);
	}
	else
	{
		switch (m_playing)
		{
		case 1: Printstring(L"已暂停  ", 0, 0, WHITE); break;
		case 2: Printstring(L"正在播放", 0, 0, WHITE); break;
		default: Printstring(L"已停止  ", 0, 0, WHITE); break;
		}
	}
	ClearString(14, 0, song_name_length);		//清除标题处的字符
	//显示正在播放的歌曲序号
	wchar_t buff[128];
	swprintf(buff, sizeof(buff), L"%.3d", m_index + 1);
	Printstring(buff, 10, 0, DARK_CYAN);
	//显示正在播放的歌曲名称
	wstring temp;
	static int name_start{ 0 };
	static wstring last_file_name;
	if (m_current_file_name != last_file_name)		//如果正在播放的歌曲发生了切换，就要把name_start清零
	{
		name_start = 0;
		last_file_name = m_current_file_name;
	}
	if (m_current_file_name.length() <= song_name_length)		//当文件名的长度不超过song_name_length个字符时，直接显示出来
	{
		Printstring(m_current_file_name.c_str(), 14, 0, song_name_length, CYAN);		//输出正在播放的文件名（最多只输出song_name_length个字符）
	}
	else		//文件名长度超过song_name_length个字符时滚动显示，此函数每调用一次滚动一次
	{
		temp = m_current_file_name.substr(name_start, song_name_length);		//取得第i个开始的song_name_length个字符
		name_start++;
		if (name_start > m_current_file_name.length() - song_name_length)
			name_start = 0;
		Printstring(temp.c_str(), 14, 0, song_name_length, CYAN);
	}
	swprintf(buff, sizeof(buff), L"音量：%d", m_volume);
	ClearString(m_width - 3, 0, 3);
	Printstring(buff, m_width - GetRealPrintLength(buff), 0, RED);

	// Printstring(L"音量：",m_width - 9, 0, RED);
	// ClearString(m_width - 3, 0, 3);
	// PrintInt(m_volume, m_width - 3, 0, RED);
	
	Printstring(L"播放/暂停(空格) 停止(S) 上一曲(V) 下一曲(N) 快退/快进(<-->) 设置路径(T) 跳转(K) 音量(上下) 浏览文件(E) 查找(F) 退出(Q)", 0, 1, m_width, YELLOW);
	
	Printstring(L"循环模式(M):", m_width - 20, 4, GREEN);
	switch (m_repeat_mode)
	{
	case 0: Printstring(L"顺序播放", m_width - 8, 4, GREEN); break;
	case 1: Printstring(L"随机播放", m_width - 8, 4, GREEN); break;
	case 2: Printstring(L"列表循环", m_width - 8, 4, GREEN); break;
	case 3: Printstring(L"单曲循环", m_width - 8, 4, GREEN); break;
	default: break;
	}

	swprintf(buff, sizeof(buff), L"MusicPlayer V%s 作者：ZY", VERSION);
	Printstring(buff, 0, m_hight - 1,m_width - 37, GRAY);
}

void CPlayer::ShowProgressBar() const
{
	int pos;
	int progress_bar_start{ 5 };
	int progress_bar_length{ m_width - progress_bar_start - 13 };
	wstring progress_bar(progress_bar_length, L'-');		//生成用于表示进度条的由'-'组成的字符串
	//GetCurrentPosition();
	//GetSongLength();
	if (m_song_length_int > 0)
		pos = m_current_position_int * progress_bar_length / m_song_length_int;
	else
		pos = 0;
	if (pos >= progress_bar_length) pos = progress_bar_length - 1;
	progress_bar[pos] = L'*';		//将进度条当前位置的字符替换成*
	Printstring(progress_bar.c_str(), progress_bar_start, 2, pos + 1, PURPLE, DARK_PURPLE);

	//显示>>>>
	int i = m_current_position.sec % 4 + 1;

	if (m_playing == 0) i = 0;
	Printstring(L">>>>", 0, 2, i, GREEN, GRAY);

	//显示歌曲时间
	wchar_t buff[15];
	swprintf(buff, sizeof(buff), L"%d:%.2d/%d:%.2d", m_current_position.min, m_current_position.sec, m_song_length.min, m_song_length.sec);
	ClearString(m_width - 12, 2, 12);
	Printstring(buff, m_width - 12, 2, DARK_YELLOW);
}

void CPlayer::ShowPlaylist() const
{
	int x, y{ 4 }, playlist_width;
	x = m_width < WIDTH_THRESHOLD ? 0 : (m_width / 2 + 1);
	playlist_width = m_width < WIDTH_THRESHOLD ? m_width : (m_width / 2 - 1);
	wchar_t buff[64];
	Printstring(L"播放列表", x, y, GREEN);
	swprintf(buff, sizeof(buff), L"(共%d首)：", m_song_num);
	Printstring(buff, x + 8, y, GREEN);
	Printstring(L"当前路径：", x, y + 1, GRAY);
	Printstring(m_path.c_str(), x + 10, y + 1, playlist_width - 10, GRAY);		//路径最多显示playlist_width - 10个字符
	for (int i{ 0 }; i < m_song_per_page; i++)
		ClearString(x, y + 2 + i, 2);

	if (m_song_num > 0 && m_display_page == m_index / m_song_per_page + 1)		//如果显示的是正在播放歌曲所在页，就显示当前歌曲指示
		Printstring(L"◆", x, y + 2 + m_index%m_song_per_page, CYAN);

	for (int i{ 0 }; i < m_song_per_page; i++)
	{
		int index{ i + m_song_per_page *(m_display_page - 1) };
		if (index < m_song_num)
		{
			PrintInt(index + 1, x + 2, y + 2 + i, GRAY);		//输出序号
			Printstring(m_playlist[index].c_str(), x + 6, y + 2 + i, playlist_width - 13, WHITE);		//输出文件名（最多只输出playlist_width - 13个字符）
			swprintf(buff, sizeof(buff), L"%d:%.2d", m_all_song_length[index].min, m_all_song_length[index].sec);
			if (m_all_song_length[index] > Time{0, 0, 0})
				Printstring(buff, m_width - 6, y + 2 + i, DARK_YELLOW);		//输出音频文件长度
			else
				Printstring(L"-:--", m_width - 6, y + 2 + i, DARK_YELLOW);		//获取不到长度时显示-:--
		}
	}
	//Printstring("按[]翻页", m_width - 14, m_hight - 1, DARK_CYAN);
	swprintf(buff, sizeof(buff), L"按[]翻页 %d/%d", m_display_page, m_total_page);
	Printstring(buff, m_total_page > 99 ? m_width - 16 : m_width - 14, m_hight - 1, DARK_CYAN);

	if (!m_Lyrics.IsEmpty())
	{
		ClearString(m_width - 36, m_hight - 1, 19);
		Printstring(L"歌词编码：", m_width - 36, m_hight - 1, GRAY);
		Printstring(L"UTF8", m_width - 26, m_hight - 1, GRAY);
	}
}

void CPlayer::ShowLyrics(bool force_refresh) const
{
	if (m_width < WIDTH_THRESHOLD)
		ShowLyricsSingleLine();
	else
		ShowLyricsMultiLine(force_refresh);
}

void CPlayer::ShowLyricsSingleLine() const
{
	if (!m_Lyrics.IsEmpty())
	{
		static wstring last_lyric;
		wstring current_lyric{ m_Lyrics.GetLyric(m_current_position, 0) };		//获取当前歌词
		auto length = GetRealPrintLength(current_lyric);
		if (current_lyric != last_lyric)
		{
			last_lyric = current_lyric;
			ClearString(0, 3, m_width);				//只有当前歌词变了才要清除歌词显示区域
		}
		wstring temp;
		int lyric_progress{ static_cast<int>(m_Lyrics.GetLyricProgress(m_current_position)*(length + 1) / 1000) };
		/*lyric_progress为当前歌词的进度所在的字符数，根据GetLyricProgress函数获得的歌词进度和当前歌词的长度计算得到，
		用于以卡拉OK形式显示歌词*/
		int lrc_start, lrc_start_half_width;		//当前歌词从第几个字符开始显示（用于当歌词文本超过指定长度时）
		int x{ static_cast<int>(m_width/2 - length/2) };		//x为输出歌词文本的x位置（用于实现歌词居中显示）
		if (x < 0) x = 0;
		if (length <= m_width)
		{
			Printstring(current_lyric.c_str(), x, 3, lyric_progress, CYAN, DARK_CYAN);
		}
		else				//歌词文本超过指定的长度时滚动显示
		{
			current_lyric.append(L" ");		//在歌词末尾加上一个空格，用于解决有时歌词最后一个字符无法显示的问题
			if (lyric_progress < m_width/2)		//当前歌词进度小于控制台宽度一半时，当前歌词从第0个字符开始显示
			{
				lrc_start = 0;
				lrc_start_half_width = 0;
			}
			else		//当前歌词进度大于等于控制台宽度一半时，需要计算当前歌词从第几个字符开始显示
			{
				lrc_start = lyric_progress - m_width / 2;
				if (lrc_start > length - m_width) lrc_start = length - m_width;
				lrc_start_half_width = lrc_start;
				if (lrc_start < 0) lrc_start = 0;
			}
			temp = current_lyric.substr(lrc_start, m_width);
			Printstring(temp.c_str(), x, 3, m_width, lyric_progress - lrc_start_half_width, CYAN, DARK_CYAN);
		}
	}
	else
		Printstring(L"当前歌曲没有歌词", m_width / 2 - 8, 3, DARK_CYAN);
}

void CPlayer::ShowLyricsMultiLine(bool force_refresh) const
{
	int x{ 0 }, y{ 4 };			//歌词界面的起始x和y坐标
	int lyric_width{ m_width / 2 - 1 };		//歌词显示的宽度
	int lyric_hight{ m_hight - 6 };		//歌词显示的高度
	int lyric_x;		//每一句歌词输出时的起始x坐标
	wstring lyric_text;		//储存每一句歌词文本
	int lyric_length;
	static bool lyric_change_flag{ true };
	if(lyric_change_flag || force_refresh) Printstring(L"歌词秀：", x, y, GREEN);
	if (!m_Lyrics.IsEmpty())
	{
		static int last_lyric_index;
		int current_lyric_index{ m_Lyrics.GetLyricIndex(m_current_position) };		//获取当前歌词编号
		if (current_lyric_index != last_lyric_index)				//如果当前歌词变了，就将歌词区域全部清除
		{
			lyric_change_flag = true;
			last_lyric_index = current_lyric_index;
			for (int i = 0; i < lyric_hight; i++)
				ClearString(x, i + y + 1, lyric_width);
		}
		else
		{
			lyric_change_flag = false;
		}

		//for (int i{ -lyric_hight / 2 + 1 }; i <= lyric_hight / 2; i++)
		for (int i{ -lyric_hight / 2 + 1 }; i + lyric_hight / 2 <= lyric_hight; i++)
		{
			lyric_text = m_Lyrics.GetLyric(m_current_position, i);
			if (lyric_text == L"……") lyric_text.clear();		//多行模式下不显示省略号
			lyric_length = GetRealPrintLength(lyric_text);
			lyric_x = lyric_width / 2 - lyric_length / 2;
			if (lyric_x < 0) lyric_x = 0;
			if (i != 0)		//不是当前歌词，以暗色显示
			{
				if (lyric_change_flag || force_refresh)			//不是当前歌词只有当前歌词变化了或参数要求强制刷新时才刷新，以避免反复刷新歌词，减少闪烁
					Printstring(lyric_text.c_str(), lyric_x, y + lyric_hight / 2 + i, lyric_width - GetFullCharSize(lyric_text, lyric_width), DARK_CYAN);
			}
			else		//i=0时为当前歌词
			{
				int lyric_progress{ static_cast<int>(m_Lyrics.GetLyricProgress(m_current_position)*(lyric_length + 1) / 1000) };		//歌词进度
				if (lyric_length <= lyric_width)		//当前歌词宽度小于歌词显示宽度时直接显示
				{
					Printstring(lyric_text.c_str(), lyric_x, y + lyric_hight / 2 + i, lyric_progress, CYAN, DARK_CYAN);
				}
				else		//当前歌词宽度大于歌词显示宽度时滚动显示
				{
					int lrc_start, split;
					lyric_text.append(L" ");		//在歌词末尾加上一个空格，用于解决有时歌词最后一个字符无法显示的问题
					wstring temp;
					auto half_lyric_width = lyric_width / 2;
					if (lyric_progress < half_lyric_width)		//当前歌词进度小于歌词显示宽度一半时，当前歌词从第0个字符开始显示
					{
						lrc_start = 0;
						split = lyric_progress;
					}
					else		//当前歌词进度大于等于控制台宽度一半时，需要计算当前歌词从第几个字符开始显示
					{
						if(lyric_length - lyric_progress > half_lyric_width) {
							lrc_start = lyric_progress - half_lyric_width;
							split = half_lyric_width;
						}
						else { // lyric_length - lyric_progress < lyric_width
							lrc_start = lyric_length - lyric_width + 1;
							split = lyric_width - (lyric_length - lyric_progress);
						}

					}
					if (lrc_start < 0) lrc_start = 0;
					
					temp = GetStrByReal(lyric_text, lrc_start);
					MoveSplitToFitWChar(temp, split);
					Printstring(temp.c_str(), lyric_x, y + lyric_hight / 2 + i, lyric_width - GetFullCharSize(temp, lyric_width) - lyric_x, split, CYAN, DARK_CYAN);
				}
			}
		}
	}
	else
		Printstring(L"当前歌曲没有歌词", lyric_width / 2 - 8, y + lyric_hight / 2, DARK_CYAN);
}


void CPlayer::SwitchPlaylist(int operation)
{
	if (operation == NEXT)
	{
		m_display_page++;
		if (m_display_page > m_total_page)
			m_display_page = 1;
	}
	if (operation == PREVIOUS)
	{
		m_display_page--;
		if (m_display_page < 1)
			m_display_page = m_total_page;
	}
}

int CPlayer::GetCurrentSecond()
{
	return m_current_position.sec;
}

inline bool CPlayer::IsPlaying() const
{
	return m_playing == 2;
}

bool CPlayer::PlayTrack(int song_track)
{
	switch (m_repeat_mode)
	{
	case 0:		//顺序播放
		if (song_track == NEXT)		//播放下一曲
			song_track = m_index + 1;
		if (song_track == PREVIOUS)		//播放上一曲
			song_track = m_index - 1;
		break;
	case 1:		//随机播放
		if (song_track == NEXT || song_track == PREVIOUS)
		{
			// SYSTEMTIME current_time;
			// GetLocalTime(&current_time);			//获取当前时间
			// srand(current_time.wMilliseconds);		//用当前时间的毫秒数设置产生随机数的种子
			song_track = rand() % m_song_num;
		}
		break;
	case 2:		//列表循环
		if (song_track == NEXT)		//播放下一曲
		{
			song_track = m_index + 1;
			if (song_track >= m_song_num) song_track = 0;
			if (song_track < 0) song_track = m_song_num - 1;
		}
		if (song_track == PREVIOUS)		//播放上一曲
		{
			song_track = m_index - 1;
			if (song_track >= m_song_num) song_track = 0;
			if (song_track < 0) song_track = m_song_num - 1;
		}
		break;
	case 3:		//单曲循环
		if (song_track == NEXT || song_track == PREVIOUS)
			song_track = m_index;
	}

	if (song_track >= 0 && song_track < m_song_num)
	{
		MusicControl(Command::CLOSE);
		m_index = song_track;
		m_display_page = m_index / m_song_per_page + 1;		//切歌时播放列表自动跳转到播放歌曲所在页
		m_current_file_name = m_playlist[m_index];
		MusicControl(Command::OPEN);
		SetVolume();
		IniLyrics();
		MusicControl(Command::PLAY);
		GetCurrentPosition();
		clear();
		ShowInfo();
		ShowProgressBar();
		ShowLyrics(true);
		ShowPlaylist();
		SetTitle();
		SaveConfig();
		return true;
	}
	return false;
}

void CPlayer::ChangePath(const wstring& path, int track)
{
	MusicControl(Command::CLOSE);
	m_path = path;
	if (m_path.empty() || (m_path.back() != L'/'))		//如果输入的新路径为空或末尾没有斜杠，则在末尾加上一个
		m_path.append(1, L'/');
	m_playlist.clear();		//清空播放列表
	m_index = track;
	//初始化播放列表
	IniPlayList();		//根据新路径重新初始化播放列表
	m_current_position_int = 0;
	m_current_position = { 0, 0, 0 };
	SaveConfig();
	SetTitle();
	clear();
	ShowInfo();
	ShowProgressBar();
	ShowPlaylist();
	MusicControl(Command::OPEN);
	SetVolume();
	IniLyrics();
	ShowLyrics(true);
	m_find_result.clear();	//更改路径后清空查找结果
}

void CPlayer::SetPath()
{
	const int x{ 0 }, y{ 4 };
	int hight{ m_hight - 5 };
	wstring path;
	int item_select{ 0 };
	int max_selection;
	wchar_t buff[256];
	for (int i{ 0 }; i < hight; i++)
		ClearString(0, 4 + i, m_width);

	Printstring(L"设置路径", x, y, CYAN);
	Printstring(L"当前路径：", x, y + 1, GRAY);
	Printstring(m_path.c_str(), x + 10, y + 1, m_width - 10, GRAY);		//路径最多显示m_width - 10个字符
	Printstring(L"帮助：方向键选择，回车键确定，D键删除选中的路径，ESC键返回。", x, m_hight - 2, GRAY);
	ClearString(m_width - 16, m_hight - 1, 16);		//清除右下角的“按[]翻页”
	refresh();

	while (true)
	{
		swprintf(buff, sizeof(buff), L"共%d个", static_cast<int>(m_recent_path.size()));
		Printstring(buff, 9, y, DARK_WHITE);
		max_selection = m_recent_path.size() + 1;
		if (max_selection > hight - 3) max_selection = hight - 3;
		for (int i{ 0 }; i < max_selection; i++)
		{
			if (i == item_select)
				Printstring(L"◆", x, y + 2 + i, CYAN);
			else
				ClearString(x, y + 2 + i, 2);
			
			if (i == 0)
			{
				Printstring(L"输入新路径", x + 2, y + 2 + i, YELLOW);
			}
			else
			{
				PrintInt(i, x + 2, y + 2 + i, DARK_YELLOW);		//输出序号
				swprintf(buff, sizeof(buff), L"%s (播放到第%d首)", std::get<PATH>(m_recent_path[i - 1]).c_str(), std::get<TRACK>(m_recent_path[i - 1]) + 1);
				size_t path_length{ std::get<PATH>(m_recent_path[i - 1]).length()};	//路径字符串占的半角字符数
				Printstring(buff, x + 5, y + 2 + i, m_width - 5, path_length, YELLOW, GRAY);
			}
		}
		refresh();

		while(!kbhit()){
			usleep(20000);
		}
		auto k = GetKey();
		switch (k)
		{
		case KEY_UP:
			item_select--;
			if (item_select < 0) item_select = max_selection - 1;
			break;
		case KEY_DOWN:
			item_select++;
			if (item_select >= max_selection) item_select = 0;
			break;
		case 'D':		//按下D键，删除选中路径
			if (item_select > 0 && m_recent_path.size() > 0)	//第0项为输入一个路径，不能执行此操作；同时删除时确保m_recent_path的元素个数大于0
			{
				m_recent_path.erase(m_recent_path.begin() + item_select - 1);
				for (int i{ item_select }; i <= max_selection; i++)
					ClearString(x, y + i + 1, m_width);
				SaveRecentPath();
				max_selection--;
				if (item_select >= max_selection) item_select = max_selection - 1;
			}
			break;
		case '\n':
			if (m_song_num>0) EmplaceCurrentPathToRecent();		//如果当前路径有歌曲，就保存当前路径到最近路径
			SaveRecentPath();
			switch (item_select)
			{
			case 0:		//选择第0项，输入一个新路径
				echo();
				nodelay(stdscr, FALSE);
				CursorVisible(true);
				ClearString(x + 10, y + 1, m_width - 10);
				do
				{
					GotoXY(10, 5);
					char b[256] = {0};
					getnstr(b, 256);
					if(b[0] == '\0') continue;
					path.assign(to_wide_string(string{b}));
				} while (path.empty());
				noecho();
				nodelay(stdscr, TRUE);
				CursorVisible(false);
				if (path.size() > 1)		//路径必须至少2个字符，如果只输入1个字符就忽略它
				{
					ChangePath(path);
					EmplaceCurrentPathToRecent();		//保存新的路径到最近路径
				}
				break;
			default:
				//m_index = std::get<TRACK>(m_recent_path[item_select - 1]);
				ChangePath(std::get<PATH>(m_recent_path[item_select - 1]), std::get<TRACK>(m_recent_path[item_select - 1]));		//设置当前路径为选中的路径
				m_current_position_int = std::get<POSITION>(m_recent_path[item_select - 1]);
				m_current_position = int2time(m_current_position_int);
				MusicControl(Command::SEEK);
				EmplaceCurrentPathToRecent();		//将选中的路径移到第1位
				break;
			}
			SaveRecentPath();
			//按下回车键直接返回，这里没有break。
		case 'Q':
			clear();
			ShowInfo();
			ShowProgressBar();
			ShowLyrics();
			ShowPlaylist();
			refresh();
			return;
		default:
			break;
		}
	}
}

void CPlayer::SetTrack()
{
	std::string track;
	Printstring(L"请输入要播放的歌曲序号（输入0取消）：", 0, 2, DARK_WHITE);
	GotoXY(37, 2);
	CursorVisible(true);
	echo();
	nodelay(stdscr, FALSE);
	refresh();
	do
	{
		char b[256] = {0};
		getnstr(b, 256);
		if(b[0] == '\0') continue;
		track.assign(string{b});
	} while (track.empty());
	noecho();
	nodelay(stdscr, TRUE);
	CursorVisible(false);
	int track_num = 0;
	try{
		track_num = std::stoi(track);
	}catch(...){

	}
	PlayTrack(track_num - 1);
}

void CPlayer::SetRepeatMode()
{
	m_repeat_mode++;
	if (m_repeat_mode > 3)
		m_repeat_mode = 0;
	ShowInfo();
	SaveConfig();
}

inline void CPlayer::SeekTo(int position)
{
	if (position > m_song_length_int)
		position = m_song_length_int;
	m_current_position_int = position;
	m_current_position = int2time(position);
	double pos_sec = static_cast<double>(position) / 1000.0;
	QWORD pos_bytes;
	pos_bytes = BASS_ChannelSeconds2Bytes(m_musicStream, pos_sec);
	BASS_ChannelSetPosition(m_musicStream, pos_bytes, BASS_POS_BYTE);
}

bool CPlayer::ErrorDispose()
{
	m_error_code = BASS_ErrorGetCode();
	if (m_song_num==0)
	{
		Printstring(L"当前路径下没有音频文件，请按任意键重新设置文件路径。", 0, 2, DARK_WHITE);
		refresh();
		getch();
		if (!m_recent_path.empty() && m_path == std::get<PATH>(m_recent_path[0]))
			m_recent_path.erase(m_recent_path.begin());		//当前路径没有音频文件，把最近路径中的没有文件的路径删除
		SetPath();
		if (m_error_code) return false;
		else return true;
	}
	else if (m_musicStream == 0)
	{
		Printstring(L"当前文件无法播放", 0, 2, DARK_WHITE);
		refresh();
		return true;
	}

	else if (m_error_code)
	{
		wchar_t error_info[64];
		swprintf(error_info, 64, L"出现了错误：错误代码：%d", m_error_code);
		Printstring(error_info, 0, 2, DARK_WHITE);
		refresh();
	}
	return true;
}

inline void CPlayer::SetTitle() const
{
	//SetConsoleTitleW((m_current_file_name + " - MusicPlayer").c_str());		//用当前正在播放的歌曲名作为窗口标题
}

void CPlayer::SaveConfig() const
{
	mINI::INIFile file(to_byte_string(m_config_path));
	mINI::INIStructure ini;
	file.read(ini);

	ini["config"]["volume"] = std::to_string(m_volume);
	ini["config"]["repeat_mode"] = std::to_string(m_repeat_mode);
	ini["config"]["window_width"] = std::to_string(m_width);
	ini["config"]["window_hight"] = std::to_string(m_hight);
	ini["config"]["lyric_path"] =  to_byte_string(m_lyric_path);

	file.generate(ini);
}

void CPlayer::LoadConfig()
{	
	if(!FileExist(m_config_path)) {
		m_volume = 100;
		m_repeat_mode = 0;
		m_width = 80;
		m_hight = 25;
		m_lyric_path = L"./lyrics";
		return;
	}

	mINI::INIFile file(to_byte_string(m_config_path));
	mINI::INIStructure ini;
	file.read(ini);
	auto tmp_str = ini["config"]["volume"];
	m_volume = tmp_str.empty() ? 100 : std::stoi(tmp_str);

	tmp_str = ini["config"]["repeat_mode"];
	m_repeat_mode = tmp_str.empty() ? 0 : std::stoi(tmp_str);

	
	tmp_str = ini["config"]["window_width"];
	m_width = tmp_str.empty() ? 80 : std::stoi(tmp_str);

	tmp_str = ini["config"]["window_hight"];
	m_hight = tmp_str.empty() ? 25 : std::stoi(tmp_str);

	tmp_str = ini["config"]["lyric_path"];
	m_lyric_path = tmp_str.empty() ? L"./lyrics/" :  to_wide_string(tmp_str);

}

void CPlayer::GetWindowsSize(int width, int hight)
{
	m_width = width;
	m_hight = hight;
	m_song_per_page = m_hight - 7;
	//重新计算所有需要根据窗口宽度和高度变化而变化的数据
	if (m_song_num == 0)
		m_total_page = 1;
	else
		m_total_page = (m_song_num - 1) / m_song_per_page + 1;
	m_display_page = m_index / m_song_per_page + 1;		//设定播放列表中当前显示的页
}

void CPlayer::Find()
{
	const int x{ 0 }, y{ 4 };
	int hight{ m_hight - 5 };
	int item_select{ 0 };
	int max_selection{ static_cast<int>(m_find_result.size()) };
	wchar_t buff[256];
	wstring key_word;
	bool find_flag{ false };	//执行过一次查找后，find_flag会被置为true

	
	while (true)
	{	
		clear();
		GetCurrentPosition();
		Printstring(L"查找文件", x, y, CYAN);
		Printstring(L"请按空格键后输入要查找的关键词：", x, y + 1, DARK_WHITE);
		ShowInfo();
		ShowProgressBar();
		
		max_selection = m_find_result.size();
		if (max_selection > hight - 4) max_selection = hight - 4;

		if (!m_find_result.empty())
		{
			if (find_flag)
			{
				Printstring(L"查找结果：", x, y + 2, GREEN);
				swprintf(buff, sizeof(buff), L"(共%d个结果)", static_cast<int>(m_find_result.size()));
				Printstring(buff, x + 10, y + 2, GRAY);
			}
			else
			{
				Printstring(L"上次的查找结果：", x, y + 2, GREEN);
				swprintf(buff, sizeof(buff), L"(共%d个结果)", static_cast<int>(m_find_result.size()));
				Printstring(buff, x + 16, y + 2, GRAY);
			}
			for (int i{ 0 }; i < max_selection; i++)
			{
				if (i == item_select)
					Printstring(L"◆", x, y + 3 + i, CYAN);
				else
					ClearString(x, y + 3 + i, 2);

				//查找结果显示格式为：序号 文件名 (分/秒)
				PrintInt(i + 1, x + 2, y + 3 + i, GRAY);
				// FIXME: c_str() 只能取到第一个字符，后面的都被 '\0' 切掉了
				swprintf(buff, sizeof(buff), L"%s (%d:%.2d)", m_playlist[m_find_result[i]].c_str(), m_all_song_length[m_find_result[i]].min, m_all_song_length[m_find_result[i]].sec);
				Printstring(buff, x + 5, y + 3 + i, m_width - 5, m_playlist[m_find_result[i]].length(), WHITE, DARK_YELLOW);
			}
		}
		else if(find_flag)
		{
			Printstring(L"没有找到结果", x, y + 2, YELLOW);
		}

		Printstring(L"帮助：方向键选择查找结果，回车键播放，空格键开始查找，输入“*”取消查找。Q键返回。", x, m_hight - 2, GRAY);
		RemoveString(m_width - 16, m_hight - 1, 16);
		refresh();
		if(kbhit()){
			switch (GetKey())
			{
			case ' ':
				RemoveString(x + 32, y + 1, m_width - x - 32);	//输入前清除上次输入的字符
				CursorVisible(true);
				echo();
				nodelay(stdscr, FALSE);
				do
				{
					GotoXY(x + 32, y + 1);
					char b[256] = {0};
					getnstr(b, 256);//从键盘输入关键词
					if(b[0] == '\n') continue;
					key_word.assign(to_wide_string(b));
				} while (key_word.empty());
				CursorVisible(false);
				noecho();
				nodelay(stdscr, TRUE);
				if (key_word == L"*")		//输入星号取消查找
					break;
				find_flag = true;
				m_find_result.clear();		//查找之前先清除上一次的查找结果
				FindFile(key_word);
				break;
			case KEY_UP:
				item_select--;
				if (item_select < 0) item_select = max_selection - 1;
				break;
			case KEY_DOWN:
				item_select++;
				if (item_select >= max_selection) item_select = 0;
				break;
			case '\n':
				if(!m_find_result.empty())
					PlayTrack(m_find_result[item_select]);
			case 'Q':
				return;
			default:
				break;
			}
		}
		usleep(20000);
	}
}

void CPlayer::FindFile(const wstring & key_word)
{
	int index;
	for (int i{ 0 }; i < m_playlist.size(); i++)
	{
		index = m_playlist[i].find(key_word);
		if (index != wstring::npos)
			m_find_result.push_back(i);
	}
}


void CPlayer::SaveRecentPath() const
{
	//将最近文件列表保存到程序目录下的recent_path.dat中，其中每一行的格式为：路径<曲目序号,播放到的位置(毫秒)>
	ofstream SaveFile{ to_byte_string(m_recent_path_dat_path) };

	char buff_unicode[256];
	for (auto& path_info : m_recent_path)
	{
		snprintf(buff_unicode, sizeof(buff_unicode), "%s<%.4d,%d>\n", to_byte_string(std::get<PATH>(path_info)).c_str(), std::get<TRACK>(path_info), std::get<POSITION>(path_info));
		SaveFile <<  buff_unicode;		//以UTF8格式保存路径
	}
	SaveFile.close();
}

void CPlayer::LoadRecentPath()
{
	ifstream OpenFile{ to_byte_string(m_recent_path_dat_path) };
	if (!FileExist(m_recent_path_dat_path)) return;
	while (!OpenFile.eof())
	{
		string current_line, temp;
		string path;
		int track, position;
		char buff_unicode[256]{ 0 };
		int index;
		std::getline(OpenFile, current_line);		//读取文件中的每一行

		index = current_line.find_first_of('<');
		if (index == string::npos)	//没有找到'<'，说明文件中只保存了路径而没有保存曲目序号和位置
		{
			path = current_line;
			track = 0;
			position = 0;
		}
		else
		{
			path = current_line.substr(0, index);		//读取路径
			temp = current_line.substr(index + 1, 4);	//读取曲目序号（固定4个字符）
			track = std::stoi(temp);
			temp = current_line.substr(index + 6, current_line.size() - index - 5);		//读取位置
			position = std::stoi(temp);
		}
		if (path.empty() || path.size() < 2) continue;		//如果路径为空或路径太短，就忽略它
		if (path.back() != L'/')	//如果读取到的路径末尾没有斜杠，则在末尾加上一个
			path.append(1, L'/');
		m_recent_path.push_back(std::make_tuple(to_wide_string(path), track, position));
	}
	OpenFile.close();

	//从recent_path文件中获取路径、播放到的曲目和位置
	if (!m_recent_path.empty())
	{
		m_path = std::get<PATH>(m_recent_path[0]);
		if (!m_path.empty() && m_path.back() != L'/')		//如果读取到的新路径末尾没有斜杠，则在末尾加上一个
			m_path.push_back(L'/');

		m_index = std::get<TRACK>(m_recent_path[0]);
		m_current_position_int = std::get<POSITION>(m_recent_path[0]);
		m_current_position = int2time(m_current_position_int);
	}
	else
	{
		m_path = L"./songs/";		//默认的路径
	}

}

void CPlayer::EmplaceCurrentPathToRecent()
{
	for (int i{ 0 }; i < m_recent_path.size(); i++)
	{
		if (m_path == std::get<PATH>(m_recent_path[i]))
			m_recent_path.erase(m_recent_path.begin() + i);		//如果当前路径已经在最近路径中，就把它最近路径中删除
	}
	m_recent_path.emplace_front(std::make_tuple(m_path, m_index, m_current_position_int));		//当前路径插入到m_recent_path的前面
}
