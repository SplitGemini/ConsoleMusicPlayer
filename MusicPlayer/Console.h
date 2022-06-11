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

//在界面的x,y坐标处输出一个数字
void PrintInt(int num, short x, short y, Color color)
{
	int attr;
	if(color <= 7){
		// dark color
		attr = COLOR_PAIR(color) | A_DIM;
	}else attr = COLOR_PAIR(color - 8);
	char str[20];
    snprintf(str, 20, "%d",num);
	attron(attr);
	mvprintw(y, x,"%s",str);
	attroff(attr);
	//FillConsoleOutputAttribute(handle, color, len, pos, &unuse);		//设置颜色
}

//在控制台的x,y坐标处输出一个宽字符串
void Printstring(const char *str, int x, int y, int16_t color)
{	
	int attr;
	if(color <= 7){
		// dark color
		attr = COLOR_PAIR(color) | A_DIM;
	}else attr = COLOR_PAIR(color - 8);

	attron(attr);
	mvaddstr(y, x, str);
	attroff(attr);
	
}

//在控制台的x,y坐标处输出一个指定最大长度的宽字符串
void Printstring(const char* str, short x, short y, size_t length, Color color)
{
	int attr;
	if(color <= 7){
		// dark color
		attr = COLOR_PAIR(color) | A_DIM;
	}else attr = COLOR_PAIR(color - 8);

	attron(attr);
	mvaddnstr(y, x, str, length);
	attroff(attr);
	
}

//在控制台的x,y处输出一个宽字符串，前面的字符显示为color1的颜色，从第split个字符开始显示为color2的颜色
void Printstring(const char* str, short x, short y, int split, Color color1, Color color2)
{
	int attr1;
	if(color1 <= 7){
		// dark color
		attr1 = COLOR_PAIR(color1) | A_DIM;
	}else attr1 = COLOR_PAIR(color1 - 8);

	int attr2;
	if(color2 <= 7){
		// dark color
		attr2 = COLOR_PAIR(color2) | A_DIM;
	}else attr2 = COLOR_PAIR(color2 - 8);

	attron(attr1);
	mvaddnstr(y, x, str, split);
	attroff(attr1);

	attron(attr2);
	mvaddstr(y, x, str + split);
	attroff(attr2);
}

//在控制台的x,y处输出一个宽字符串，前面的字符显示为color1的颜色，从第split个字符开始显示为color2的颜色。同时指定最大长度为length
void Printstring(const char* str, short x, short y, size_t length, int split, Color color1, Color color2)
{
	int attr1;
	if(color1 <= 7){
		// dark color
		attr1 = COLOR_PAIR(color1) | A_DIM;
	}else attr1 = COLOR_PAIR(color1 - 8);

	int attr2;
	if(color2 <= 7){
		// dark color
		attr2 = COLOR_PAIR(color2) | A_DIM;
	}else attr2 = COLOR_PAIR(color2 - 8);

	attron(attr1);
	mvaddnstr(y, x, str, split);
	attroff(attr1);

	attron(attr2);
	mvaddnstr(y, x, str + split, length - split);
	attroff(attr2);
}

//清除控制台的x,y处开始的length个字符
void ClearString(short x, short y, size_t length)
{
	string mask(length, ' ');	//生成length长度的空格
	mvaddnstr(y, x, mask.c_str() , length);
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
	return getmaxx(stdscr) + 1;
}

//获取当前控制台窗口的高度
int GetWindowHight()
{
	return getmaxy(stdscr) + 1;
}
