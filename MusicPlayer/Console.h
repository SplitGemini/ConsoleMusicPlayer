//************************************************************
//				本文件为控制台相关全局函数的定义
//************************************************************
#pragma once
#include <ncursesw/curses.h>
#include <unistd.h>  /* only for sleep() */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "Common.h"

int GetColorAttr(Color color){
	if(color <= 7){
		// dark color
		return COLOR_PAIR(color) | A_DIM;
	}else return COLOR_PAIR(color - 8);
}

//在界面的x,y坐标处输出一个数字
void PrintInt(int num, short x, short y, Color color)
{
	int attr = GetColorAttr(color);
	wchar_t str[20];
    swprintf(str, 20, L"%d", num);
	attron(attr);
	mvaddnwstr(y, x, str, 20);
	attroff(attr);
}

//在控制台的x,y坐标处输出一个宽字符串
void Printstring(const wchar_t *str, int x, int y, Color color)
{	
	int attr = GetColorAttr(color);

	attron(attr);
	mvaddwstr(y, x, str);
	
	attroff(attr);
	
}

//在控制台的x,y坐标处输出一个指定最大长度的宽字符串
void Printstring(const wchar_t* str, short x, short y, size_t length, Color color)
{
	int attr = GetColorAttr(color);

	attron(attr);
	mvaddnwstr(y, x, str, length);
	attroff(attr);
	
}

//在控制台的x,y处输出一个宽字符串，前面的字符显示为color1的颜色，从第split个字符开始显示为color2的颜色
void Printstring(const wchar_t* str, short x, short y, int split, Color color1, Color color2)
{
	int attr1 = GetColorAttr(color1);
	int attr2 = GetColorAttr(color2);

	attron(attr2);
	mvaddwstr(y, x, str);
	attroff(attr2);

	mvchgat(y, x, split, attr1, (color1 <= 7 ? color1 : color1 - 8), NULL);
}

//在控制台的x,y处输出一个宽字符串，前面的字符显示为color1的颜色，从第split个字符开始显示为color2的颜色。同时指定最大长度为length
void Printstring(const wchar_t* str, short x, short y, size_t length, int split, Color color1, Color color2)
{
	int attr1 = GetColorAttr(color1);
	int attr2 = GetColorAttr(color2);

	attron(attr2);
	mvaddnwstr(y, x, str, length);
	attroff(attr2);
	
	mvchgat(y, x , split, attr1, (color2 <= 7 ? color2 : color2 - 8), NULL);
}

//清除控制台的x,y处开始的length个字符
void ClearString(short x, short y, size_t length)
{
	wstring mask(length, ' ');	//生成length长度的空格
	mvaddnwstr(y, x, mask.c_str(), length);
}

//清除控制台的x,y处开始的length个字符
void RemoveString(short x, short y, size_t length)
{
	for(auto i = 0; i < length; i++)
		mvwdelch(stdscr, y, x + i);
}

//光标移动到x,y坐标
void GotoXY(short x, short y)
{
	move(y, x);
}

//显示或隐藏光标
void CursorVisible(bool visible)
{
	curs_set(visible ? 2 : 0);
}

//获取当前控制台窗口的宽度
int GetWindowWidth()
{
	return getmaxx(stdscr);
}

//获取当前控制台窗口的高度
int GetWindowHight()
{
	return getmaxy(stdscr);
}
