//************************************************************
//		文本件为歌词类的定义，用于实现歌词显示
//************************************************************
#pragma once
#include<string>
#include<vector>
#include<fstream>
#include<iostream>
#include<algorithm>
#include"Common.h"

using std::ifstream;
using std::wstring;
using std::string;
using std::vector;

class CLyrics
{
private:
	struct Lyric
	{
		Time time;
		wstring text;
		bool operator<(const Lyric& lyric) const	//重载小于号运算符，用于对歌词按时间标签排序
		{
			return lyric.time > time;
		}
	};

	wstring m_file;		//歌词文件的文件名
	vector<Lyric> m_lyrics;		//储存每一句歌词（包含时间标签和文本）
	vector<wstring> m_lyrics_str;	//储存未拆分时间标签的每一句歌词
	wstring m_ti;		//歌词中的ti标签

	void DivideLyrics();		//将歌词文件拆分成若干句歌词，并保存在m_lyrics_str中
	void DisposeLyric();		//获得歌词中的时间标签和歌词文本，并将文本从string类型转换成string类型，保存在m_lyrics中

public:
	CLyrics(wstring& file_name);
	CLyrics(){}
	bool IsEmpty() const;		//判断是否有歌词
	wstring GetLyric(Time time, int offset) const;		//根据时间返回一句歌词。第2个参数如果是0，则返回当前时间对应的歌词，如果是-1则返回当前时间的前一句歌词，1则返回后一句歌词，以此类推。
	int GetLyricProgress(Time time) const;		//根据时间返回该时间所对应的歌词的进度（0~1000）（用于使歌词以卡拉OK样式显示）
	int GetLyricIndex(Time time) const;			//根据时间返回该时间对应的歌词序号（用于判断歌词是否有变化）
};

CLyrics::CLyrics(wstring& file_name) : m_file{ file_name }
{
	DivideLyrics();
	DisposeLyric();
	std::sort(m_lyrics.begin(), m_lyrics.end());		//将歌词按时间标签排序
}

void CLyrics::DivideLyrics()
{
	ifstream OpenFile{ to_byte_string(m_file) };
	string current_line;
	if (OpenFile.good())
	{
		while(std::getline(OpenFile, current_line))		//从歌词文件中获取一行歌词
			m_lyrics_str.push_back(to_wide_string(current_line));
	}
}

void CLyrics::DisposeLyric()
{
	int index;
	wstring temp;
	Lyric lyric;
	for (int i{ 0 }; i < m_lyrics_str.size(); i++)
	{
		if (i <= 2)
		{
			//在前3句歌词中查找ti:标签
			index = m_lyrics_str[i].find(L"ti:");
			int index2 = m_lyrics_str[i].find_first_of(L']');
			if (index != wstring::npos && index2 != wstring::npos)
			{
				temp = m_lyrics_str[i].substr(index + 3, index2 - index - 3);
				m_ti = temp;
			}
		}

		//获取歌词文本
		index = m_lyrics_str[i].find_last_of(L']');		//查找最后一个']'，后面的字符即为歌词文本
		if (index == wstring::npos) continue;
		temp = m_lyrics_str[i].substr(index + 1, m_lyrics_str[i].size() - index - 1);
		//将获取到的歌词文本转换成Unicode
		if (temp.empty())		//如果时间标签后没有文本，显示为“……”
			lyric.text = L"……";
		else
			lyric.text = temp;
		if (lyric.text.back() <= 32) lyric.text.pop_back();		//删除歌词末尾的一个控制字符或空格

		//获取时间标签
		index = -1;
		while (true)
		{
			index = m_lyrics_str[i].find_first_of(L'[', index + 1);		//查找第1个左中括号
			if (index == wstring::npos) break;		//没有找到左中括号，退出循环
			else if (index > m_lyrics_str[i].size() - 9) break;		//找到了左中括号，但是左中括号在字符串的倒数第9个字符以后，也退出循环
			else if (m_lyrics_str[i][index + 1] > L'9' || m_lyrics_str[i][index + 1] < L'0') break;		//找到了左中括号，但是左中括号后面不是数字，也退出循环
			temp = m_lyrics_str[i].substr(index + 1, 2);		//获取时间标签的分钟数
			lyric.time.min = std::stoi(temp);
			temp = m_lyrics_str[i].substr(index + 4, 2);		//获取时间标签的秒钟数
			lyric.time.sec = std::stoi(temp);
			if (m_lyrics_str[i][index + 8] == L']')			//如果从左中括号往右数第8个字符就是右中括号了，说明这个时间标签的毫秒数只有1位
			{
				lyric.time.msec = m_lyrics_str[i][index + 7] - L'0';
				lyric.time.msec *= 100;
			}
			else
			{
				temp = m_lyrics_str[i].substr(index + 7, 2);		//获取时间标签的毫秒数（这里只取两位，乘以10后得到毫秒数）
				lyric.time.msec = std::stoi(temp) * 10;
			}
			m_lyrics.push_back(lyric);
		}
	}
}

bool CLyrics::IsEmpty() const
{
	return (m_lyrics.size() == 0);
}

wstring CLyrics::GetLyric(Time time, int offset) const
{
	for (int i{ 0 }; i < m_lyrics.size(); i++)
	{
		if (m_lyrics[i].time>time)		//如果找到第一个时间标签比要显示的时间大，则该时间标签的前一句歌词即为当前歌词
		{
			if (i + offset - 1 < -1) return wstring{};
			else if (i + offset - 1 == -1) return m_ti;		//时间在第一个时间标签前面，返回ti标签的值
			else if (i + offset - 1 < m_lyrics.size()) return m_lyrics[i + offset - 1].text;
			else return wstring{};
		}
	}
	if (m_lyrics.size() + offset - 1 < m_lyrics.size())
		return m_lyrics[m_lyrics.size() + offset - 1].text;		//如果没有时间标签比要显示的时间大，当前歌词就是最后一句歌词
	else
		return wstring{};
}

int CLyrics::GetLyricProgress(Time time) const
{
	int lyric_last_time{ 1 };		//time时间所在的歌词持续的时间
	int lyric_current_time{ 0 };		//当前歌词在time时间时已经持续的时间
	for (int i{ 0 }; i < m_lyrics.size(); i++)
	{
		if (m_lyrics[i].time>time)
		{
			if (i == 0)
			{
				lyric_current_time = 0;
				lyric_last_time = 1;
			}
			else
			{
				lyric_last_time = m_lyrics[i].time - m_lyrics[i - 1].time;
				lyric_current_time = time - m_lyrics[i - 1].time;
			}
			if (lyric_last_time == 0) lyric_last_time = 1;
			return lyric_current_time * 1000 / lyric_last_time;
		}
	}
	//如果最后一句歌词之后已经没有时间标签，该句歌词默认显示20秒
	lyric_current_time = time - m_lyrics[m_lyrics.size() - 1].time;
	lyric_last_time = 20000;
	return lyric_current_time * 1000 / lyric_last_time;
}

int CLyrics::GetLyricIndex(Time time) const
{
	for (int i{ 0 }; i < m_lyrics.size(); i++)
	{
		if (m_lyrics[i].time>time)
			return i - 1;
	}
	return m_lyrics.size() - 1;
}
