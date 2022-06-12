//************************************************************
//主函数，程序时序控制，调用播放器类中的成员函数以实现程序各项功能
//************************************************************
#include "Player.h"
#include <ncursesw/curses.h>
#include <iostream>
#include <chrono>

int main(int argc, char* argv[])
{
	setlocale(LC_ALL, "");
	initscr();
	InitColors();
	raw();	
    cbreak();
    noecho();
    scrollok(stdscr, TRUE);
    nodelay(stdscr, TRUE);
	intrflush(stdscr, FALSE);
    keypad(stdscr, TRUE);

	CPlayer my_player;
	bool cmd_line_open{ false };
	if (argc <= 1)
	{
		my_player.Create();
	}
	else
	{
		vector<wstring> files;
		for (int i{ 1 }; i < argc; i++)
			files.push_back(to_wide_string( string{ argv[i] }));
		my_player.Create(files);
		cmd_line_open = true;
	}
	my_player.ShowInfo();
	my_player.ShowPlaylist();
	while(!my_player.ErrorDispose());
	my_player.MusicControl(Command::OPEN);
	my_player.IniLyrics();
	my_player.MusicControl(Command::SEEK);
	my_player.ShowProgressBar();
	my_player.SetVolume();
	if (cmd_line_open)
		my_player.MusicControl(Command::PLAY);
	int sec_current, sec_temp;		//用于控制界面信息刷新频率，当前歌曲进度每过1秒刷新
	sec_temp = my_player.GetCurrentSecond();
	auto last_timepoint = std::chrono::steady_clock::now();		//用于控制歌词刷新频率，歌词每过LYRIC_REFRESH毫秒刷新
	auto interval = std::chrono::milliseconds(LYRIC_REFRESH);
	int width(0), hight(0);
	int width_t, hight_t;
	while (true)
	{
		while(!my_player.ErrorDispose());
		my_player.GetCurrentPosition();
		sec_current = my_player.GetCurrentSecond();
		if (sec_temp != sec_current)		//获取播放时间的秒数，如果秒数变了则刷新一次界面信息
		{
			sec_temp = sec_current;
			my_player.GetCurrentPosition();
			my_player.ShowInfo();
			//my_player.ShowPlaylist();
			my_player.ShowProgressBar();
			//my_player.ShowLyrics();
			//my_player.SaveConfig();
		}
		if (std::chrono::steady_clock::now() - last_timepoint >  interval && my_player.IsPlaying())		//如果当前时间与之前的时间差超过了LYRIC_REFRESH毫秒，并且正在播放，就刷新一次歌词
		{
			last_timepoint = std::chrono::steady_clock::now();
			my_player.ShowLyrics();
		}
		if (my_player.SongIsOver())
		{
			if (!my_player.PlayTrack(NEXT))		//顺序播放模式下列表中的歌曲播放完毕，PlayTrack函数会返回false
				my_player.MusicControl(Command::STOP);		//停止播放
		}
		if (kbhit())
		{
			switch (GetKey())
			{
			//case 'P': my_player.MusicControl(Command::PLAY); break;
			case 'P': case ' ': my_player.MusicControl(Command::PLAY_PAUSE); break;		//播放/暂停
			case 'S': my_player.MusicControl(Command::STOP); break;		//停止
			case 'N': my_player.PlayTrack(NEXT); break;		//下一曲
			case 'V': my_player.PlayTrack(PREVIOUS); break;		//上一曲
			case KEY_RIGHT: my_player.MusicControl(Command::FF); break;		//快进
			case KEY_LEFT:my_player.MusicControl(Command::REW); break;		//快退
			case 'T': my_player.SetPath(); break;		//设置路径
			case KEY_UP: my_player.MusicControl(Command::VOLUME_UP); break;		//音量加
			case KEY_DOWN:my_player.MusicControl(Command::VOLUME_DOWN); break;		//音量减
			case 'K': my_player.SetTrack(); break;		//播放指定歌曲
			case '[': my_player.SwitchPlaylist(PREVIOUS); break;		//播放列表向前翻页
			case ']': my_player.SwitchPlaylist(NEXT); break;			//播放列表向后翻页
			case 'M': my_player.SetRepeatMode(); break;			//切换循环模式
			case 'F': my_player.Find(); break;		//查找文件
			case 'Q':
				//my_player.GetConsoleSetting();
				my_player.SaveConfig();
				my_player.SaveRecentPath();
				goto end;
			}
			clear();
			my_player.ShowInfo();
			my_player.ShowProgressBar();
			my_player.ShowLyrics(true);
			my_player.ShowPlaylist();
		}

		width_t = GetWindowWidth();
		hight_t = GetWindowHight();
		if (width != width_t || hight != hight_t)		//如果控制台窗口大小发生改变
		{
			width = width_t;
			hight = hight_t;
			if (width_t >= MIN_WIDTH && hight_t >= MIN_HIGHT)		//确保设定的窗口大小不会小于最小值
			{
				my_player.GetWindowsSize(width_t, hight_t);
				clear();
				my_player.ShowInfo();
				my_player.ShowProgressBar();
				my_player.ShowLyrics(true);
				my_player.ShowPlaylist();
				my_player.SaveConfig();
			}
		}
		refresh();
		usleep(20000);			// 20ms, 降低主函数循环执行的频率，以减小CPU利用率
	}
end:
	clear();
	endwin();
	return 0;
}