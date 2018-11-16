#pragma once
#include <string>
#include <iostream>
#include <stdlib.h>
#include <vector>
#include <iterator>
#include <fstream>
#include <intrin.h>
#include <Tlhelp32.h>
#include <CommCtrl.h>
#include <Wininet.h>
#include <functional>
#include <iostream>
#include <windows.h>
#include <commctrl.h>
#include <Richedit.h>
#include <stdio.h>
#include <crtdbg.h>
#include <tchar.h>

extern "C" {
#include "Lua\lua.h"
#include "Lua\lua.hpp"
#include "Lua\lualib.h"
#include "Lua\lauxlib.h"
#include "Lua\luaconf.h"
#include "Lua\llimits.h"
}

using namespace std;


/*

Written by XTOOLS -> VERSION 1.1.1 HOTFIX

Discord -> http://xtools.cf

*/


/*

[R LUA STATE OFFSETS]

rL + 32 = L->top
rL + 12 = L->base

*/


DWORD MODULEBASE = (DWORD)(GetModuleHandleA(NULL));
#define R_LUA_TNIL 0
#define R_LUA_TLIGHTUSERDATA 1
#define R_LUA_TNUMBER 2
#define R_LUA_TBOOLEAN 3
#define R_LUA_TSTRING 4
#define R_LUA_TTHREAD 5
#define R_LUA_TFUNCTION 6
#define R_LUA_TTABLE 7
#define R_LUA_TUSERDATA 8
#define R_LUA_TPROTO 9
#define R_LUA_TUPVALUE 10
#define r_increasestack(rL) *(DWORD *)(rL + 16) += 16
#define r_growstack(rL, x) *(DWORD*)(rL + 16) += (16 * x)
#define r_savestack(rL, p) ((char *)(p) - (char *)(*(DWORD*)(rL + 20)))
#define r_restorestack(rL, n) (TValue*)(rL + 20) + (n);
#define r_rawPush(rL, v, t) { int v2 = *(DWORD*)(rL + 16); \
					*(DWORD*)(v2 + 8) = t; \
					*(DWORD*)v2 = v; \
					*(DWORD*)(rL + 16) += 16; } 
#define r_ptr_obfus(p, v) *(DWORD*)(p) = (DWORD)v - (p)
#define r_ptr_deobf(p) *(DWORD*)(p) + (p)
#define MODULE_IDABASE 0x400000
#define MODULE_OFFSET(b, x) (MODULEBASE + ((x) - (b)))

int top = 32, base = 12;
typedef DWORD _DWORD;
union r_Value
{
	int b;
	double n;
	void* p;
	void* gc;
};

struct r_TValue
{
	r_Value value;
	int tt;
};


namespace XLIB
{

	DWORD r_index2adr(DWORD rL, signed int idx)
	{
		DWORD result;
		int counter;

		if (idx <= 0)
		{
			if (idx <= -10000)
			{
				switch (idx)
				{
				case -10002:
					result = *(DWORD *)(rL + 64);
					break;
				case -10001:
					result = *(DWORD *)(rL + 80);
					*(DWORD*)(result) = *(DWORD *)(***(DWORD ***)(rL + 28) + 12);
					((DWORD*)result)[2] = 7;
					break;
				case -10000:
					result = *(DWORD *)((*(DWORD *)(rL + 20) ^ (rL + 20)) + 104);
					break;
				default:
					counter = ***(DWORD ***)(rL + 28);
					if (-10002 - idx > *(unsigned __int8 *)(counter + 7))
						result = NULL;
					else
						result = *(DWORD *)(counter + 16 * (-10002 - idx) + 8);
					break;
				}
			}
			else
			{
				result = *(DWORD *)(*(DWORD *)(rL + 32) + 16 * idx);
			}
		}
		else
		{
			result = NULL;
			if ((unsigned int)(16 * idx + *(DWORD *)(rL + 12) - 16) < *(DWORD *)(rL + 32))
				result = *(DWORD *)(16 * idx + *(DWORD *)(rL + 12) - 16);
		}
		return result;
	}

	DWORD r_type(DWORD rL, signed int idx)
	{
		r_TValue* tv = (r_TValue*)(r_index2adr(rL, idx));
		if (tv)
			return tv->tt;
		else
			return LUA_TNONE;
	}

	void r_settop(int a1, int a2) {
		int i;
		if (a2 < 0)
		{
			*(_DWORD *)(a1 + top) += 16 * a2 + 16;
		}
		else
		{
			for (i = 16 * a2; *(_DWORD *)(a1 + top) < (unsigned int)(i + *(_DWORD *)(a1 + base)); *(_DWORD *)(a1 + top) += 16)
				*(_DWORD *)(*(_DWORD *)(a1 + top) + 8) = 0;
			*(_DWORD *)(a1 + top) = i + *(_DWORD *)(a1 + base);
		}
	}

	void r_pushnil(DWORD a1) {
		*(_DWORD *)(*(_DWORD *)(a1 + top) + 8) = 0;
		*(_DWORD *)(a1 + top) += 16;
	}

	void r_pushboolean(DWORD a1, int a2) {
		_DWORD *v2;
		v2 = *(_DWORD **)(a1 + top);
		*v2 = a2 != 0;
		v2[2] = 3;
		*(_DWORD *)(a1 + top) += 16;
	}

	void r_pushstring(int rState, const char *s) {
		int v3 = *(DWORD *)(rState + top);
		// NEED TO GET OFFSETS *(DWORD *)v3 = r_luaS_newlstr(rState, s, strlen(s));
		*(DWORD *)(v3 + 8) = R_LUA_TSTRING;
		*(DWORD *)(rState + top) += 16;
	}

	void r_pushvalue(DWORD a1, int a2)
	{
		r_TValue *p = (r_TValue*)(r_index2adr(a1, a2));
		int v3 = *(DWORD *)(a1 + top);
		*(DWORD *)v3 = *(DWORD *)p;
		*(DWORD *)(v3 + 4) = *(DWORD *)(p + 4);
		*(DWORD *)(v3 + 8) = p->tt;
		*(DWORD *)(a1 + top) += 16;
	}

	DWORD r_getonstack(DWORD rL, DWORD idx)
	{
		return *(DWORD*)(rL + 8) + 16 * idx;
	}

	double r_ConvertNumber(double Number) {
		void* oldnumber = &(Number);
		double newnumber;
		__asm
		{
			mov edx, oldnumber
			mov eax, XorConstant
			movsd xmm1, qword ptr[edx]
			xorpd xmm1, [eax]
			movsd newnumber, xmm1
		}
		return newnumber;
	}

	int r_fb2int(int x) {
		int e = (x >> 3) & 31;
		if (e == 0) return x;
		else return ((x & 7) + 8) << (e - 1);
	}


	int r_lua_gettop(DWORD a1)
	{
		return (*(_DWORD *)(a1 + top) - *(_DWORD *)(a1 + base)) >> 4;
	}

	int r2_lua_gettop(lua_State *L) {
		int rvalue = 0;
		__asm {
			push ebx
			mov ebx, L
			push edi
			mov edi, [ebx + 1Ch]
			sub edi, [ebx + 10h]
			sar edi, 4
			mov rvalue, edi
			pop edi
			pop ebx
		}
		return rvalue;
	}
	


	/*
	Sehchainfaker
	Eternals sehchain fixed for windows7, windows 8,...
	*/
	void fakeChain(DWORD* chain)
	{
		chain[1] = 0x1555555;
		if ((((DWORD*)chain[0])[1]) != NULL) {
			((DWORD*)chain[0])[1] = 0x1555555;
		}
	}
	void restoreChain(DWORD* chain, DWORD unk, DWORD nextUnk)
	{
		chain[1] = unk;
		if ((((DWORD*)chain[0])[1]) != NULL) {
			((DWORD*)chain[0])[1] = nextUnk;
		}
	}

	string FileRead(string file_path) {
		std::ifstream ifs;
		ifs.open(file_path, std::ifstream::in);
		string fdata = "";
		char c = ifs.get();
		while (ifs.good()) {
			fdata += c;
			c = ifs.get();
		}
		ifs.close();
		return fdata;
	}

	string wstr_to_str(wstring wstr) {
		string str = "";
		for (char c : wstr)
			if ((BYTE)c >= 0x20 && (BYTE)c <= 0x7A)
				str += c;
			else break;
		return str;
	}
	/*

This code requires UNINCODE in PROJECT SETTINGS

	string most_recent_log() {
		WCHAR fname[MAX_PATH];
		wstring wfpath, logspath;
		GetModuleFileName(GetModuleHandle(NULL), fname, sizeof(fname));
		wfpath = wstring(fname);
		for (int i = 0; i < wfpath.length(); i++)
			if ((BYTE)wfpath[i] > 0x20 && (BYTE)wfpath[i] < 0x7F) { // valid characters only
				if ((i + 8) < wfpath.length())
					if (wfpath.substr(i, 8) == L"Versions")
						break;
					else logspath += wfpath[i];
			}
			else break;
		logspath += L"logs"; // Lets check them logs

		WIN32_FIND_DATA FindFileData;
		HANDLE hFind;
		TCHAR patter[MAX_PATH];
		FILETIME bestDate = { 0, 0 };
		FILETIME curDate;
		string filename = "no file";
		memset(patter, 0x00, MAX_PATH);
		_stprintf(patter, TEXT("%s\\*.txt"), logspath.c_str());
		hFind = FindFirstFile(patter, &FindFileData);
		if (hFind == INVALID_HANDLE_VALUE) {
			printf("Find file failed (%d)\n", GetLastError());
			return "";
		}
		else {
			do {
				//ignore current and parent directories
				if (_tcscmp(FindFileData.cFileName, TEXT(".")) == 0 || _tcscmp(FindFileData.cFileName, TEXT("..")) == 0)
					continue;
				if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
					//ignore directories
				}
				else {
					//list the Files
					string filepath = (wstr_to_str(logspath) + "\\" + wstr_to_str(FindFileData.cFileName));
					curDate = FindFileData.ftCreationTime;
					if (CompareFileTime(&curDate, &bestDate) > 0) {
						bestDate = curDate;
						// Put the Path to the file first.
						filename = filepath;
					}
				}
			} while (FindNextFile(hFind, &FindFileData));
			FindClose(hFind);
		}

		return filename;
	}
	*/

	/*

	This code requires UNINCODE in PROJECT SETTINGS

	DWORD to_addr(string& str) {
		string addr("0x" + str);
		DWORD m_dwIP = 0x00000000;
		istringstream ss(addr);
		ss >> hex >> m_dwIP;
		return m_dwIP;
	}
	*/

	string sub(string str, int at, int amount) {
		if ((at + amount) > str.length()) return "";
		return str.substr(at, amount);
	}

	int strmatch(string str1, string str2) {
		if (str1.find(str2) != string::npos)
			return (int)str1.find(str2);
		else
			return -1;
	}


	auto r_backjump(DWORD jmp) // credit to Avocado
	{
		jmp = MODULE_OFFSET(MODULE_IDABASE, jmp);
		DWORD OldProtect;
		BYTE OldJump = *(BYTE*)(jmp);
		VirtualProtect((LPVOID)(jmp), 1, PAGE_EXECUTE_READWRITE, &OldProtect);
		*(BYTE*)(jmp) = 0xEB;
		return [&]() {
			*(BYTE*)(jmp) = OldJump;
			VirtualProtect((LPVOID)(jmp), 1, OldProtect, &OldProtect);
		};
	}


}