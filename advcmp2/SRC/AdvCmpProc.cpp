/****************************************************************************
 * AdvCmpProc.cpp
 *
 * Plugin module for FAR Manager 2.0
 *
 * Copyright (c) 2006-2011 Alexey Samlyukov
 ****************************************************************************/
/*
Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:
1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
3. The name of the authors may not be used to endorse or promote products
   derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#pragma hdrstop
#include "AdvCmpProc.hpp"


AdvCmpProc::AdvCmpProc()
{
	hScreen=Info.SaveScreen(0,0,-1,-1);
	hConInp=CreateFileW(L"CONIN$", GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
	bStartMsg=true;

	Opt.BufSize=65536<<4;
	Opt.Buf[0]=NULL;
	Opt.Buf[1]=NULL;

	// �������� ������ ���������
	if (Opt.CmpContents)
	{
		Opt.Buf[0]=(char*)malloc(Opt.BufSize*sizeof(char));
		Opt.Buf[1]=(char*)malloc(Opt.BufSize*sizeof(char));
	}

	LPanel.hFilter=RPanel.hFilter=INVALID_HANDLE_VALUE;
	Info.FileFilterControl(LPanel.hPlugin,FFCTL_CREATEFILEFILTER,FFT_PANEL,(LONG_PTR)&LPanel.hFilter);
	Info.FileFilterControl(RPanel.hPlugin,FFCTL_CREATEFILEFILTER,FFT_PANEL,(LONG_PTR)&RPanel.hFilter);
	Info.FileFilterControl(LPanel.hFilter,FFCTL_STARTINGTOFILTER,0,0);
	Info.FileFilterControl(RPanel.hFilter,FFCTL_STARTINGTOFILTER,0,0);

	// �� ����� ��������� ������� ��������� ������� ����...
	TitleSaved=GetFarTitle(strFarTitle);
	SetConsoleTitle(GetMsg(MComparingFiles));
}

AdvCmpProc::~AdvCmpProc()
{
	if (Opt.Buf[0]) free(Opt.Buf[0]);
	if (Opt.Buf[1]) free(Opt.Buf[1]);
	if (Opt.Substr) free(Opt.Substr); // ��������� �� ������� �����

	Info.FileFilterControl(Opt.hCustomFilter,FFCTL_FREEFILEFILTER,0,0);  // ��������� �� ������� �����
	Info.FileFilterControl(LPanel.hFilter,FFCTL_FREEFILEFILTER,0,0);
	Info.FileFilterControl(RPanel.hFilter,FFCTL_FREEFILEFILTER,0,0);

	CloseHandle(hConInp);
	Info.RestoreScreen(hScreen);
	// ����������� ��������� ������� ����...
	if (TitleSaved) SetConsoleTitle(strFarTitle);
}


/****************************************************************************
 *************************** ShowMessage functions **************************
 ****************************************************************************/

/****************************************************************************
 * �������� �� Esc. ���������� true, ���� ������������ ����� Esc
 ****************************************************************************/
bool AdvCmpProc::CheckForEsc(void)
{
	if (hConInp == INVALID_HANDLE_VALUE)
		return false;

	static DWORD dwTicks;
	DWORD dwNewTicks = GetTickCount();
	if (dwNewTicks - dwTicks < 500)
		return false;
	dwTicks = dwNewTicks;

	INPUT_RECORD rec;
	DWORD ReadCount;
	while (PeekConsoleInput(hConInp, &rec, 1, &ReadCount) && ReadCount)
	{
		ReadConsoleInput(hConInp, &rec, 1, &ReadCount);
		if ( rec.EventType == KEY_EVENT && rec.Event.KeyEvent.wVirtualKeyCode == VK_ESCAPE && rec.Event.KeyEvent.bKeyDown )
		{
			// ������������ ������������� ���������� �� Esc
			if (Info.AdvControl(Info.ModuleNumber, ACTL_GETCONFIRMATIONS, 0) & FCS_INTERRUPTOPERATION)
			{
				if (YesNoMsg(MEscTitle, MEscBody))
					return bBrokenByEsc = true;
			}
			else
				return bBrokenByEsc = true;
		}
	}
	return false;
}

/****************************************************************************
 * �������� ��������� ������� ����
 ****************************************************************************/
bool AdvCmpProc::GetFarTitle(string &strTitle)
{
	DWORD dwSize=0;
	DWORD dwBufferSize=MAX_PATH;
	wchar_t *lpwszTitle=NULL;
	do
	{
		dwBufferSize <<= 1;
		lpwszTitle = (wchar_t*)realloc(lpwszTitle, dwBufferSize*sizeof(wchar_t));
		dwSize = GetConsoleTitle(lpwszTitle, dwBufferSize);
	}
	while (!dwSize && GetLastError() == ERROR_SUCCESS);

	if (dwSize)
		strTitle=lpwszTitle;
	free(lpwszTitle);
	return dwSize;
}

/****************************************************************************
 * ������� ������ ������� ���� ������ (��� ��������� �������� �����)
 * ��� ����������� ������ � ��������� ���������
 ****************************************************************************/
void AdvCmpProc::TrunCopy(wchar_t *Dest, const wchar_t *Src, bool bDir, const wchar_t *Msg)
{
	string strSrc(Src);
	int iLen=0;
	if (bDir)
	{
		FSF.sprintf(Dest,Msg,FSF.TruncStr(strSrc.get(),WinInfo.TruncLen-wcslen(Msg)+2));
		iLen=wcslen(Dest);
	}
	else
		iLen=wcslen(wcscpy(Dest,FSF.TruncStr(strSrc.get(),WinInfo.TruncLen)));

	if (iLen<WinInfo.TruncLen)
	{
		wmemset(&Dest[iLen],L' ',WinInfo.TruncLen-iLen);
		Dest[WinInfo.TruncLen]=L'\0';
	}
}

/****************************************************************************
 * ����� � ��������� Substr � ������ ������.
 ****************************************************************************/
wchar_t *CutSubstr(string &strSrc, wchar_t *Substr)
{
	if (!Substr) return strSrc.get();
	int len=wcslen(Substr);
	if (!len) return strSrc.get();

	int lenSrc=strSrc.length();
	const wchar_t *src=strSrc.get();
	string strBuf;
	// ������ ������
	{
		HANDLE re;
		int start_offset=0;
		if (!Info.RegExpControl(0,RECTL_CREATE,(LONG_PTR)&re)) return false;

		string Search=L"/";
		if (len>0 && Substr[0]==L'/') 
			Search+=Substr+1;
		else Search+=Substr;
		if (Search.length()>0 && Search[(size_t)(Search.length()-1)]!=L'/')
			Search+=L"/i";
		if (Info.RegExpControl(re,RECTL_COMPILE,(LONG_PTR)Search.get()))
		{
			int brackets=Info.RegExpControl(re,RECTL_BRACKETSCOUNT,0);
			if (!brackets) { Info.RegExpControl(re,RECTL_FREE,0); return false; }
			RegExpMatch *match=(RegExpMatch*)malloc(brackets*sizeof(RegExpMatch));

			for (;;)
			{
				RegExpSearch search= { src,start_offset,lenSrc,match,brackets,0 };

				if (Info.RegExpControl(re,RECTL_SEARCHEX,(LONG_PTR)&search))
				{
					// �������� �� ��������
					for (int i=start_offset; i<match[0].start; i++)
						strBuf+=src[i];

					start_offset=match[0].end;

					if (match[0].start==match[0].end || start_offset>=lenSrc)
						break;
				}
				else
					break;
			}
			free(match);
			Info.RegExpControl(re,RECTL_FREE,0);
		}
		// �������� �� �� ��� �� ����� � �������
		for (int i=start_offset; i<lenSrc; i++)
			strBuf+=src[i];
		if (!FSF.Trim(strBuf.get())) return strSrc.get();
		strSrc=strBuf.get();
	}
	return strSrc.get();
}

/****************************************************************************
 * ����������� int � wchar_t ����������: �� 1234567890 � "1 234 567 890"
 ****************************************************************************/
wchar_t* AdvCmpProc::itoaa(__int64 num, wchar_t *buf)
{
	wchar_t tmp[100];
	FSF.itoa64(num,tmp,10);
	int digits_count=0;
	wchar_t *t=tmp;
	while (*t++)
		digits_count++;
	wchar_t *p=buf+digits_count+(digits_count-1) / 3;
	digits_count=0;
	*p--=L'\0';
	t--;      //������, ��������� �������������� ��������!
	while (p!=buf)
	{
		*p--=*--t;
		if ((++digits_count) % 3 == 0)
			*p-- = L' ';
	}
	*p=*--t;

	return buf;
}


/****************************************************************************
 * ������������� ������ � ���������� �������� ������������
 ****************************************************************************/
void AdvCmpProc::strcentr(wchar_t *Dest, const wchar_t *Src, int len, wchar_t sym)
{
	int iLen, iLen2;
	iLen=wcslen(wcscpy(Dest,Src));
	if (iLen<len)
	{
		iLen2=(len-iLen)/2;
		wmemmove(Dest+iLen2,Dest,iLen);
		wmemset(Dest,sym,iLen2);
		wmemset(Dest+iLen2+iLen,sym,len-iLen2-iLen);
		Dest[len]=L'\0';
	}
}

/****************************************************************************
 * ������ ������-��������
 ****************************************************************************/
void AdvCmpProc::ProgressLine(wchar_t *Dest, unsigned __int64 nCurrent, unsigned __int64 nTotal)
{
	int n=0, len=WinInfo.TruncLen-4;
	if (nTotal>0) n=nCurrent*len / nTotal;
	if (n>len) n=len;
	wchar_t *Buf=(wchar_t*)malloc(WinInfo.TruncLen*sizeof(wchar_t));
	if (Buf)
	{
		wmemset(Buf,0x00002588,n);
		wmemset(&Buf[n],0x00002591,len-n);
		Buf[len]=L'\0';
		FSF.sprintf(Dest,L"%s%3d%%",Buf,nTotal?(nCurrent*100 / nTotal):0);
		free(Buf);
	}
	else
		*Dest=0;
}

/****************************************************************************
 * ���������� ��������� � ��������� ���� ������
 ****************************************************************************/
void AdvCmpProc::ShowMessage(const wchar_t *Dir1, const wchar_t *Name1, const wchar_t *Dir2, const wchar_t *Name2, bool bRedraw)
{
	// ��� ����������� �� ���� 3-� ��� � 1 ���.
	if (!bRedraw)
	{
		static DWORD dwTicks;
		DWORD dwNewTicks = GetTickCount();
		if (dwNewTicks - dwTicks < 350)
			return;
		dwTicks = dwNewTicks;
	}

	wchar_t Buf[MAX_PATH], ItemsOut[MAX_PATH];

	wchar_t TruncDir1[MAX_PATH], TruncDir2[MAX_PATH], TruncName1[MAX_PATH], TruncName2[MAX_PATH];
	TrunCopy(TruncDir1, Dir1, true, GetMsg(MComparing));
	TrunCopy(TruncName1, Name1, false, L"");
	TrunCopy(TruncDir2, Dir2, true, GetMsg(MComparingWith));
	TrunCopy(TruncName2, Name2, false, L"");

	wchar_t LDiff[100], RDiff[100], DiffOut[MAX_PATH];
	FSF.sprintf(Buf,GetMsg(MComparingDiffN),itoaa(CmpInfo.LDiff,LDiff),itoaa(CmpInfo.RDiff,RDiff));
	strcentr(DiffOut,Buf,WinInfo.TruncLen,0x00002500);

	wchar_t ProgressLineCur[MAX_PATH], ProgressLineTotal[MAX_PATH];
	if (!Opt.CmpContents || bStartMsg)
		wcscpy(ProgressLineCur,GetMsg(MWait));
	else
		ProgressLine(ProgressLineCur,CmpInfo.CurProcSize,CmpInfo.CurCountSize);

	if (Opt.TotalProcess)
	{
		FSF.sprintf(Buf,GetMsg(MComparingFiles2),CmpInfo.CountSize && !(LPanel.PInfo.Plugin || RPanel.PInfo.Plugin)?(CmpInfo.ProcSize*100/CmpInfo.CountSize):0);
		SetConsoleTitle(Buf);

		wchar_t Count[100], CountSize[100];
		FSF.sprintf(Buf,GetMsg(MComparingN),itoaa(CmpInfo.CountSize,CountSize),itoaa(CmpInfo.Count,Count));
		strcentr(ItemsOut,Buf,WinInfo.TruncLen,0x00002500);

		if (LPanel.PInfo.Plugin || RPanel.PInfo.Plugin)
			wcscpy(ProgressLineTotal,GetMsg(MWait));
		else
			ProgressLine(ProgressLineTotal,CmpInfo.ProcSize,CmpInfo.CountSize);
	}
	strcentr(Buf,L"",WinInfo.TruncLen,0x00002500);  // ������ ���������

	const wchar_t *MsgItems1[] = {
		GetMsg(MCompareTitle),
		TruncDir1,TruncName1,DiffOut,ProgressLineCur,Buf,TruncDir2,TruncName2
	};

	const wchar_t *MsgItems2[] = {
		GetMsg(MCompareTitle),
		TruncDir1,TruncName1,DiffOut,ProgressLineCur,Buf,ProgressLineTotal,ItemsOut,TruncDir2,TruncName2
	};


	Info.Message(Info.ModuleNumber,bStartMsg?FMSG_LEFTALIGN:FMSG_LEFTALIGN|FMSG_KEEPBACKGROUND,0,Opt.TotalProcess?MsgItems2:MsgItems1,
																		Opt.TotalProcess?sizeof(MsgItems2)/sizeof(MsgItems2[0]):sizeof(MsgItems1)/sizeof(MsgItems1[0]),0);
	bStartMsg=false;
}



/****************************************************************************
 ************************** CompareFiles functions **************************
 ****************************************************************************/

void AdvCmpProc::WFD2FFD(WIN32_FIND_DATA &wfd, FAR_FIND_DATA &ffd)
{
	ffd.dwFileAttributes=wfd.dwFileAttributes;
	ffd.ftCreationTime=wfd.ftCreationTime;
	ffd.ftLastAccessTime=wfd.ftLastAccessTime;
	ffd.ftLastWriteTime=wfd.ftLastWriteTime;
	ffd.nFileSize=((__int64)wfd.nFileSizeHigh << 32) | wfd.nFileSizeLow;
	ffd.nPackSize=0;
	ffd.lpwszFileName=(wchar_t*)malloc((wcslen(wfd.cFileName)+1)*sizeof(wchar_t));
	if (ffd.lpwszFileName) wcscpy((wchar_t*)ffd.lpwszFileName,wfd.cFileName);
	ffd.lpwszAlternateFileName=0;
}

/****************************************************************************
 * ������ ��������� ������� Info.GetDirList(). � ������� �� ���� ����������
 * ������ PluginPanelItem
 ****************************************************************************/
int AdvCmpProc::GetDirList(const wchar_t *Dir, struct DirList *pList)
{
	struct FAR_FIND_DATA *FFD;
	int ItemsNumber;
	if (Info.GetDirList(Dir,&FFD,&ItemsNumber))
	{
		pList->Dir=(wchar_t*)malloc((wcslen(Dir)+1)*sizeof(wchar_t));
		if (pList->Dir) wcscpy(pList->Dir,Dir);
		pList->ItemsNumber=ItemsNumber;
		pList->PPI=(PluginPanelItem*)malloc(ItemsNumber*sizeof(PluginPanelItem));
		if (pList->PPI)
		{
			for(int i=0; i<ItemsNumber; i++)
			{
				pList->PPI[i].FindData.dwFileAttributes=FFD[i].dwFileAttributes;
				pList->PPI[i].FindData.ftCreationTime=FFD[i].ftCreationTime;
				pList->PPI[i].FindData.ftLastAccessTime=FFD[i].ftLastAccessTime;
				pList->PPI[i].FindData.ftLastWriteTime=FFD[i].ftLastWriteTime;
				pList->PPI[i].FindData.nFileSize=FFD[i].nFileSize;
				pList->PPI[i].FindData.nPackSize=FFD[i].nPackSize;
				pList->PPI[i].FindData.lpwszFileName=(wchar_t*)malloc((wcslen(FFD[i].lpwszFileName)+1)*sizeof(wchar_t));
				if (pList->PPI[i].FindData.lpwszFileName) wcscpy((wchar_t*)pList->PPI[i].FindData.lpwszFileName,FFD[i].lpwszFileName);
				pList->PPI[i].FindData.lpwszAlternateFileName=0;
			}
		}
		Info.FreeDirList(FFD,ItemsNumber);
		return true;
	}
	return false;
}

/****************************************************************************
 * ������ ��������� ������� Info.GetDirList(). � ������� �� ���� ����������
 * ������ ������ ������ � �������� Dir, ��� ������������.
 * ����� �������� ���������� �� ��������� �� �������� �������.
 ****************************************************************************/
int AdvCmpProc::GetDirList(const wchar_t *Dir, int ScanDepth, bool OnlyInfo, struct DirList *pList)
{
	if (OnlyInfo && bBrokenByEsc)
		return true;

	string strPathMask(Dir);
	strPathMask+=L"\\*";
	WIN32_FIND_DATA wfdFindData;
	HANDLE hFind;
	bool ret=true;

	if (!OnlyInfo) // ��������� DirList
	{
		pList->Dir=(wchar_t*)malloc((wcslen(Dir)+1)*sizeof(wchar_t));
		if (pList->Dir) wcscpy(pList->Dir,Dir);
		pList->PPI=0;
		pList->ItemsNumber=0;
	}

	if ((hFind=FindFirstFileW(strPathMask,&wfdFindData)) != INVALID_HANDLE_VALUE)
	{
		do
		{
			if (OnlyInfo && CheckForEsc())
			{
				ret=true;
				break;
			}

			if (!Opt.ProcessHidden && (wfdFindData.dwFileAttributes&FILE_ATTRIBUTE_HIDDEN))
				continue;
			if ((wfdFindData.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY) &&
					((wfdFindData.cFileName[0]==L'.' && !wfdFindData.cFileName[1]) || (wfdFindData.cFileName[0]==L'.' && wfdFindData.cFileName[1]==L'.' && !wfdFindData.cFileName[2])))
				continue;
			if (OnlyInfo && (wfdFindData.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY))
			{
				if (Opt.ProcessSubfolders==2 && Opt.MaxScanDepth<ScanDepth+1) // �� ������ ��������� ������!
					break;
				if (!Opt.ProcessSubfolders)
					continue;
				strPathMask=Dir;
				if (strPathMask.length()>0 && strPathMask[(size_t)(strPathMask.length()-1)]!=L'\\') strPathMask+=L"\\";
				strPathMask+=wfdFindData.cFileName;
				ret=GetDirList(strPathMask,ScanDepth+1,OnlyInfo,0);
			}
			else
			{
				if (OnlyInfo)
				{
					CmpInfo.Count+=1;
					CmpInfo.CountSize+=((unsigned __int64)wfdFindData.nFileSizeHigh << 32) | wfdFindData.nFileSizeLow;
					ShowMessage(L"*",L"*",L"*",L"*",false);
				}
				else
				{
					struct PluginPanelItem *pPPI=(PluginPanelItem *)realloc(pList->PPI,(pList->ItemsNumber+1)*sizeof(PluginPanelItem));
					if (!pPPI)
					{
						ErrorMsg(MNoMemTitle, MNoMemBody);
						// !!! �������� ��� ��������� �������� �������� � �� ���-��
						ret=false;
						break;
					}
					pList->PPI=pPPI;
					WFD2FFD(wfdFindData,(pList->PPI)[pList->ItemsNumber++].FindData);
				}
			}
		} while (FindNextFile(hFind,&wfdFindData));
		FindClose(hFind);
	}
//	else
//		ret=false;
	return ret;
}

/****************************************************************************
 * ������ ��������� ������� Info.FreeDirList().
 ****************************************************************************/
void AdvCmpProc::FreeDirList(struct DirList *pList)
{
	if (pList->PPI)
	{
		for (int i=0; i<pList->ItemsNumber; i++)
			free((void*)pList->PPI[i].FindData.lpwszFileName);
		free(pList->PPI); pList->PPI=0;
	}
	free(pList->Dir); pList->Dir=0;
	pList->ItemsNumber=0;
}


/****************************************************************************
 * ����� ������? �/��� ���������� ������?
 * ��� ���� Opt.IgnoreWhitespace � Opt.IgnoreNewLines
 ****************************************************************************/
inline bool IsNewLine(int c) {return (c == '\r' || c == '\n');}
inline bool myIsSpace(int c) {return (c == ' ' || c == '\t' || c == '\v' || c == '\f');}
inline bool IsWhiteSpace(int c) {return (c == ' ' || c == '\t' || c == '\v' || c == '\f' || c == '\r' || c == '\n');}

/****************************************************************************
 * ����������� ��������� � ����� ��� ���� Opt.ContentsPercent
 ****************************************************************************/
bool AdvCmpProc::mySetFilePointer(HANDLE hf, __int64 distance, DWORD MoveMethod)
{
	bool bSet = true;
	LARGE_INTEGER li;
	li.QuadPart = distance;
	li.LowPart = SetFilePointer(hf, li.LowPart, &li.HighPart, MoveMethod);
	if (li.LowPart == 0xFFFFFFFF && GetLastError() != NO_ERROR)
		bSet = false;
	return bSet;
}

/****************************************************************************
 * CRC32 �� ����������� ��������� 0xEDB88320.
 ****************************************************************************/
DWORD AdvCmpProc::ProcessCRC(void *pData, register int iLen, DWORD FileCRC)
{
	register unsigned char *pdata = (unsigned char *)pData;
	register DWORD crc = FileCRC;
	static unsigned TableCRC[256];
	if (!TableCRC[1])
	{ // ������������� CRC32 �������
		unsigned i, j, r;
		for (i = 0; i < 256; i++)
		{
			for (r = i, j = 8; j; j--)
				r = r & 1 ? (r >> 1) ^ 0xEDB88320 : r >> 1;
			TableCRC[i] = r;
		}
	}
	while (iLen--)
	{
		crc = TableCRC[(unsigned char)crc ^ *pdata++] ^ crc >> 8;
		crc ^= 0xD202EF8D;
	}
	return crc;
}

/****************************************************************************
 * ��������� ����������� ��������� "�� �����������".
 ****************************************************************************/
int AdvCmpProc::GetCacheResult(DWORD FullFileName1, DWORD FullFileName2, DWORD64 WriteTime1, DWORD64 WriteTime2)
{
	for (int i=0; i<Cache.ItemsNumber; i++)
	{
		if ( ((FullFileName1==Cache.RCI[i].dwFullFileName[0] && FullFileName2==Cache.RCI[i].dwFullFileName[1]) &&
					(WriteTime1==Cache.RCI[i].dwWriteTime[0] && WriteTime2==Cache.RCI[i].dwWriteTime[1]))
				|| ((FullFileName1==Cache.RCI[i].dwFullFileName[1] && FullFileName2==Cache.RCI[i].dwFullFileName[0]) &&
					(WriteTime1==Cache.RCI[i].dwWriteTime[1] && WriteTime2==Cache.RCI[i].dwWriteTime[0])) )
		{
			return (int)Cache.RCI[i].dwFlags;
		}
	}
	return 0;  // 0 - ��������� �� ���������, �.�. ������� �� ������
}

/****************************************************************************
 * ���������� ���������� ��������� "�� �����������".
 ****************************************************************************/
bool AdvCmpProc::SetCacheResult(DWORD FullFileName1, DWORD FullFileName2, DWORD64 WriteTime1, DWORD64 WriteTime2, DWORD dwFlag)
{
	bool bFoundItem=false;
	for (int i=0; i<Cache.ItemsNumber; i++)
	{
		if ( ((FullFileName1==Cache.RCI[i].dwFullFileName[0] && FullFileName2==Cache.RCI[i].dwFullFileName[1]) &&
					(WriteTime1==Cache.RCI[i].dwWriteTime[0] && WriteTime2==Cache.RCI[i].dwWriteTime[1]))
				|| ((FullFileName1==Cache.RCI[i].dwFullFileName[1] && FullFileName2==Cache.RCI[i].dwFullFileName[0]) &&
					(WriteTime1==Cache.RCI[i].dwWriteTime[1] && WriteTime2==Cache.RCI[i].dwWriteTime[0])) )
		{
			bFoundItem=true;
			Cache.RCI[i].dwFlags=dwFlag; // ������� "����" :-)
			break;
		}
	}

	if (!bFoundItem)
	{
		struct ResultCmpItem *pRCI=(struct ResultCmpItem *)realloc(Cache.RCI,(Cache.ItemsNumber+1)*sizeof(ResultCmpItem));
		if (pRCI)
		{
			Cache.RCI=pRCI;
			struct ResultCmpItem *CurItem=&Cache.RCI[Cache.ItemsNumber++];
			CurItem->dwFullFileName[0]  = FullFileName1;
			CurItem->dwFullFileName[1]  = FullFileName2;
			CurItem->dwWriteTime[0] = WriteTime1;
			CurItem->dwWriteTime[1] = WriteTime2;
			CurItem->dwFlags=dwFlag;
		}
		else
		{
			ErrorMsg(MNoMemTitle, MNoMemBody);
			free(Cache.RCI);
			Cache.RCI=0; Cache.ItemsNumber=0;
			return false;
		}
	}
	return true;
}

/****************************************************************************
 * ��������� ��������� � ������� ��� ���� ���������� ��������� (������ ���
 * ������������).
 * ���������� true, ���� ��� ���������.
 ****************************************************************************/
bool AdvCmpProc::CompareFiles(const wchar_t *LDir, const PluginPanelItem *pLPPI, const wchar_t *RDir, const PluginPanelItem *pRPPI, int ScanDepth)
{
	if (pLPPI->FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
	{
		// ����� ���������� ��� �����������
		if (Opt.ProcessSubfolders)
		{
			if (Opt.ProcessSubfolders==2 && Opt.MaxScanDepth<ScanDepth+1)
				return true;

			if (ScanDepth>0 && (LPanel.bARC || RPanel.bARC))
				return true;

			string strLFullDir(LDir), strRFullDir(RDir);
			if ((strLFullDir.length()>0 && strLFullDir[(size_t)(strLFullDir.length()-1)]!=L'\\') || !strLFullDir.length()) strLFullDir+=L"\\";
			if ((strRFullDir.length()>0 && strRFullDir[(size_t)(strRFullDir.length()-1)]!=L'\\') || !strRFullDir.length()) strRFullDir+=L"\\";
			strLFullDir+=pLPPI->FindData.lpwszFileName;
			strRFullDir+=pRPPI->FindData.lpwszFileName;

			// �������� ������ ��������� � ������������
			struct DirList LList, RList;
			bool bEqual = true;

			if (!(LPanel.bARC || RPanel.bARC))
			{
				if (!GetDirList(strLFullDir,ScanDepth,false,&LList) ||!GetDirList(strRFullDir,ScanDepth,false,&RList))
				{
					bBrokenByEsc=true; // �� �� ���� �������, �� �� ������ ������
					bEqual=false; // ��������� ���������
					bOpenFail=true;
				}
			}
			else if (LPanel.bARC && RPanel.bARC)
			{
				LList.Dir=(wchar_t*)malloc((wcslen(LDir)+1)*sizeof(wchar_t));
				if (LList.Dir) wcscpy(LList.Dir,LDir);

				RList.Dir=(wchar_t*)malloc((wcslen(RDir)+1)*sizeof(wchar_t));
				if (RList.Dir) wcscpy(RList.Dir,RDir);

				if (!Info.GetPluginDirList(Info.ModuleNumber,LPanel.hPlugin,pLPPI->FindData.lpwszFileName,&LList.PPI,&LList.ItemsNumber) ||
						!Info.GetPluginDirList(Info.ModuleNumber,RPanel.hPlugin,pRPPI->FindData.lpwszFileName,&RList.PPI,&RList.ItemsNumber) )
				{
					bBrokenByEsc=true; // �� �� ���� �������, �� �� ������ ������
					bEqual=false; // ��������� ���������
					bOpenFail=true;
				}
			}
			else if (!LPanel.bARC && RPanel.bARC)
			{
				RList.Dir=(wchar_t*)malloc((wcslen(RDir)+1)*sizeof(wchar_t));
				if (RList.Dir) wcscpy(RList.Dir,RDir);
//				DebugMsg(L"RPanel.bARC",RList.Dir);
				if (!GetDirList(strLFullDir,&LList) ||
						!Info.GetPluginDirList(Info.ModuleNumber,RPanel.hPlugin,pRPPI->FindData.lpwszFileName,&RList.PPI,&RList.ItemsNumber) )
				{
					bBrokenByEsc=true; // �� �� ���� �������, �� �� ������ ������
					bEqual=false; // ��������� ���������
					bOpenFail=true;
				}
//				for (int i=0; bEqual && i<LList.ItemsNumber; i++)
//				DebugMsg(L"!LPanel.bARC",(wchar_t*)LList.PPI[i].FindData.lpwszFileName,i);

//				for (int i=0; bEqual && i<RList.ItemsNumber; i++)
//				DebugMsg(L"RPanel.bARC",(wchar_t*)RList.PPI[i].FindData.lpwszFileName,i);

			}
			else if (LPanel.bARC && !RPanel.bARC)
			{
				LList.Dir=(wchar_t*)malloc((wcslen(LDir)+1)*sizeof(wchar_t));
				if (LList.Dir) wcscpy(LList.Dir,LDir);

				if (!GetDirList(strRFullDir,&RList) ||
						!Info.GetPluginDirList(Info.ModuleNumber,LPanel.hPlugin,pLPPI->FindData.lpwszFileName,&LList.PPI,&LList.ItemsNumber) )
				{
					bBrokenByEsc=true; // �� �� ���� �������, �� �� ������ ������
					bEqual=false; // ��������� ���������
					bOpenFail=true;
				}
//				for (int i=0; bEqual && i<RList.ItemsNumber; i++)
//				DebugMsg(L"RPanel.bARC",(wchar_t*)RList.PPI[i].FindData.lpwszFileName);
			}
//			DebugMsg(L"Posle Info.GetPluginDirList");

			if (bEqual)
				bEqual=CompareDirs(&LList,&RList,!Opt.Panel,ScanDepth+1);  // Opt.Panel==0 �� �� ������� � �����������, ��� ������ � �������

			if (!(LPanel.bARC || RPanel.bARC))
			{
				FreeDirList(&LList);
				FreeDirList(&RList);
			}
			else if (LPanel.bARC && RPanel.bARC)
			{
				free(LList.Dir);
				free(RList.Dir);
				Info.FreePluginDirList(LList.PPI,LList.ItemsNumber);
				Info.FreePluginDirList(RList.PPI,RList.ItemsNumber);
			}
			else if (!LPanel.bARC && RPanel.bARC)
			{
				FreeDirList(&LList);
				free(RList.Dir);
				Info.FreePluginDirList(RList.PPI,RList.ItemsNumber);
			}
			else if (LPanel.bARC && !RPanel.bARC)
			{
				FreeDirList(&RList);
				free(LList.Dir);
				Info.FreePluginDirList(LList.PPI,LList.ItemsNumber);
			}

			return bEqual;
		}
	}
	else
	// ����� ���������� ��� �����
	{
		CmpInfo.CurCountSize=pLPPI->FindData.nFileSize+pRPPI->FindData.nFileSize;
		CmpInfo.CurProcSize=0;

		// ������� "������" �� ��������� :)
		if (!Opt.CmpContents)  // ���������� - ������ ������...
			CmpInfo.ProcSize+=CmpInfo.CurCountSize;

		string strLFileName(LPanel.bTMP?FSF.PointToName(pLPPI->FindData.lpwszFileName):pLPPI->FindData.lpwszFileName);
		string strRFileName(RPanel.bTMP?FSF.PointToName(pRPPI->FindData.lpwszFileName):pRPPI->FindData.lpwszFileName);

		// ������� ����
		if (Opt.CmpCase &&
				Strncmp(Opt.SkipSubstr?CutSubstr(strLFileName,Opt.Substr):strLFileName.get(),
								Opt.SkipSubstr?CutSubstr(strRFileName,Opt.Substr):strRFileName.get())
			 )
		{
			return false;
		}
		//===========================================================================
		// ������
		if (Opt.CmpSize && (pLPPI->FindData.nFileSize != pRPPI->FindData.nFileSize))
		{
			return false;
		}
		//===========================================================================
		// �����
		if (Opt.CmpTime)
		{
			if (Opt.LowPrecisionTime || Opt.IgnoreTimeZone)
			{
				union {
					__int64 num;
					struct {
						DWORD lo;
						DWORD hi;
					} hilo;
				} Precision, Difference, TimeDelta, temp;

				Precision.hilo.hi = 0;
				Precision.hilo.lo = Opt.LowPrecisionTime ? 20000000 : 0; //2s or 0s
				Difference.num = __int64(9000000000); //15m

				if (pLPPI->FindData.ftLastWriteTime.dwHighDateTime > pRPPI->FindData.ftLastWriteTime.dwHighDateTime)
				{
					TimeDelta.hilo.hi=pLPPI->FindData.ftLastWriteTime.dwHighDateTime - pRPPI->FindData.ftLastWriteTime.dwHighDateTime;
					TimeDelta.hilo.lo=pLPPI->FindData.ftLastWriteTime.dwLowDateTime - pRPPI->FindData.ftLastWriteTime.dwLowDateTime;
					if (TimeDelta.hilo.lo > pLPPI->FindData.ftLastWriteTime.dwLowDateTime)
						--TimeDelta.hilo.hi;
				}
				else
				{
					if (pLPPI->FindData.ftLastWriteTime.dwHighDateTime == pRPPI->FindData.ftLastWriteTime.dwHighDateTime)
					{
						TimeDelta.hilo.hi=0;
						TimeDelta.hilo.lo=max(pRPPI->FindData.ftLastWriteTime.dwLowDateTime,pLPPI->FindData.ftLastWriteTime.dwLowDateTime)-
															min(pRPPI->FindData.ftLastWriteTime.dwLowDateTime,pLPPI->FindData.ftLastWriteTime.dwLowDateTime);
					}
					else
					{
						TimeDelta.hilo.hi=pRPPI->FindData.ftLastWriteTime.dwHighDateTime - pLPPI->FindData.ftLastWriteTime.dwHighDateTime;
						TimeDelta.hilo.lo=pRPPI->FindData.ftLastWriteTime.dwLowDateTime - pLPPI->FindData.ftLastWriteTime.dwLowDateTime;
						if (TimeDelta.hilo.lo > pRPPI->FindData.ftLastWriteTime.dwLowDateTime)
							--TimeDelta.hilo.hi;
					}
				}

				//������������ �������� �� ������ ��� 26 �����.
				if (Opt.IgnoreTimeZone)
				{
					int counter = 0;
					while (TimeDelta.hilo.hi > Difference.hilo.hi && counter<=26*4)
					{
						temp.hilo.lo = TimeDelta.hilo.lo - Difference.hilo.lo;
						temp.hilo.hi = TimeDelta.hilo.hi - Difference.hilo.hi;
						if (temp.hilo.lo > TimeDelta.hilo.lo)
							--temp.hilo.hi;
						TimeDelta.hilo.lo = temp.hilo.lo;
						TimeDelta.hilo.hi = temp.hilo.hi;
						++counter;
					}
					if (counter<=26*4 && TimeDelta.hilo.hi == Difference.hilo.hi)
					{
						TimeDelta.hilo.hi = 0;
						TimeDelta.hilo.lo = max(TimeDelta.hilo.lo,Difference.hilo.lo) - min(TimeDelta.hilo.lo,Difference.hilo.lo);
					}
				}

				if ( Precision.hilo.hi < TimeDelta.hilo.hi ||
						(Precision.hilo.hi == TimeDelta.hilo.hi && Precision.hilo.lo < TimeDelta.hilo.lo))
				{
					return false;
				}
			}
			else if (pLPPI->FindData.ftLastWriteTime.dwLowDateTime != pRPPI->FindData.ftLastWriteTime.dwLowDateTime ||
							 pLPPI->FindData.ftLastWriteTime.dwHighDateTime != pRPPI->FindData.ftLastWriteTime.dwHighDateTime)
			{
				return false;
			}
		}
		//===========================================================================
		// ����������
		if (Opt.CmpContents)
		{

			// ��������-���������: ������� ������ ������
			if (!Opt.Ignore && (pLPPI->FindData.nFileSize != pRPPI->FindData.nFileSize))
			{
				CmpInfo.ProcSize+=CmpInfo.CurCountSize;
				return false;
			}

			// ��������-���������: ����� ������� - ������ "����������"
			if ( Opt.OnlyTimeDiff &&
					 (pLPPI->FindData.ftLastWriteTime.dwLowDateTime == pRPPI->FindData.ftLastWriteTime.dwLowDateTime &&
						pLPPI->FindData.ftLastWriteTime.dwHighDateTime == pRPPI->FindData.ftLastWriteTime.dwHighDateTime)
					)
			{
				CmpInfo.ProcSize+=CmpInfo.CurCountSize;
				return true;
			}

			// ������� 2-� �������� ������
			if (LPanel.bARC && RPanel.bARC)
			{
				CmpInfo.ProcSize+=CmpInfo.CurCountSize;
//        wchar_t buf[40];
//        FSF.sprintf(buf, L"L - %X R- %X", pLPPI->CRC32,pRPPI->CRC32);
//        DebugMsg(buf, L"");
				if (pLPPI->CRC32 == pRPPI->CRC32) return true;
				else return false;
			}

			string strLFullFileName(LDir), strRFullFileName(RDir);
			if ((strLFullFileName.length()>0 && strLFullFileName[(size_t)(strLFullFileName.length()-1)]!=L'\\') || !strLFullFileName.length()) strLFullFileName+=L"\\";
			if ((strRFullFileName.length()>0 && strRFullFileName[(size_t)(strRFullFileName.length()-1)]!=L'\\') || !strRFullFileName.length()) strRFullFileName+=L"\\";
			strLFullFileName+=pLPPI->FindData.lpwszFileName;
			strRFullFileName+=pRPPI->FindData.lpwszFileName;

			// ������ � �����
			DWORD dwLFileName, dwRFileName;

			if (Opt.Cache && !Opt.Ignore && !(LPanel.bARC || RPanel.bARC))
			{
				dwLFileName=ProcessCRC((void *)strLFullFileName.get(),strLFullFileName.length()*2,0);
				dwRFileName=ProcessCRC((void *)strRFullFileName.get(),strRFullFileName.length()*2,0);

				// ���������� ������������ ������
				if (!Opt.CacheIgnore )
				{
					int Result=GetCacheResult(dwLFileName,dwRFileName,
																		((__int64)pLPPI->FindData.ftLastWriteTime.dwHighDateTime << 32) | pLPPI->FindData.ftLastWriteTime.dwLowDateTime,
																		((__int64)pRPPI->FindData.ftLastWriteTime.dwHighDateTime << 32) | pRPPI->FindData.ftLastWriteTime.dwLowDateTime);
//              wchar_t buf[200];
//              FSF.sprintf(buf, L"GetCacheResult: L - %X R- %X", dwLFileName, dwRFileName);
//              DebugMsg(buf,(wchar_t*)pLPPI->FindData.lpwszFileName,Result?(Result==RCIF_EQUAL?RCIF_EQUAL:RCIF_DIFFER):0);

					if (Result == RCIF_EQUAL)
					{
						CmpInfo.ProcSize+=CmpInfo.CurCountSize;
						return true;
					}
					else if (Result == RCIF_DIFFER)
					{
						CmpInfo.ProcSize+=CmpInfo.CurCountSize;
						return false;
					}
				}
			}

			HANDLE hLFile, hRFile;
			FILETIME LAccess, RAccess;

			if (!LPanel.bARC)
			{
				if ((hLFile=CreateFileW(strLFullFileName, GENERIC_READ, FILE_SHARE_READ, 0,
                                 OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, 0)) == INVALID_HANDLE_VALUE)
				{
					CmpInfo.ProcSize+=CmpInfo.CurCountSize;
					bOpenFail=true;
					return false;
				}
				// �������� ����� ���������� ������� � �����
				LAccess=pLPPI->FindData.ftLastAccessTime;
			}

			if (!RPanel.bARC)
			{
				if ((hRFile=CreateFileW(strRFullFileName, GENERIC_READ, FILE_SHARE_READ, 0,
                                 OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, 0)) == INVALID_HANDLE_VALUE)
				{
					if (hLFile) CloseHandle(hLFile);
					CmpInfo.ProcSize+=CmpInfo.CurCountSize;
					bOpenFail=true;
					return false;
				}
				RAccess=pRPPI->FindData.ftLastAccessTime;
			}

			//---------------------------------------------------------------------------

			ShowMessage(LDir,pLPPI->FindData.lpwszFileName,RDir,pRPPI->FindData.lpwszFileName,true);

			DWORD LReadSize=1, RReadSize=1;
			DWORD LBufPos=1, RBufPos=1;     // ������� � Opt.Buf
			const DWORD ReadBlock=65536;
			__int64 LFilePos=0, RFilePos=0;  // ������� � �����
			bool bEqual=true;

			{
				char *LPtr=Opt.Buf[0]+LBufPos, *RPtr=Opt.Buf[1]+RBufPos;
				bool bLExpectNewLine=false, bRExpectNewLine=false;
				SHFILEINFO shinfo;
				bool bExe=0;
				if (!(LPanel.bARC || RPanel.bARC))
					bExe=(SHGetFileInfoW(strLFullFileName,0,&shinfo,sizeof(shinfo),SHGFI_EXETYPE) ||
								SHGetFileInfoW(strRFullFileName,0,&shinfo,sizeof(shinfo),SHGFI_EXETYPE));

				DWORD dwFileCRC=0;
				__int64 PartlyKbSize=(__int64)Opt.PartlyKbSize*1024;
				// ��������� ���������
				bool bPartlyFull=( Opt.Partly && !Opt.Ignore && !(LPanel.bARC || RPanel.bARC) &&
													(Opt.PartlyFull && pLPPI->FindData.nFileSize > Opt.BufSize) );

				bool bPartlyKb=( Opt.Partly && !Opt.Ignore && !(LPanel.bARC || RPanel.bARC) &&
												(!Opt.PartlyFull && PartlyKbSize && pLPPI->FindData.nFileSize > abs(PartlyKbSize)) );

				unsigned int BlockIndex=pLPPI->FindData.nFileSize / Opt.BufSize;
				unsigned int LCurBlockIndex=0, RCurBlockIndex=0;

				// ���� � �������, ��������� � ����� �����
				bool bFromEnd=(bPartlyKb && abs(PartlyKbSize)!=PartlyKbSize);
				if (bFromEnd)
				{
					if (!mySetFilePointer(hLFile,PartlyKbSize,FILE_END) || !mySetFilePointer(hRFile,PartlyKbSize,FILE_END))
						bEqual=false;
				}

				while (1)
				{
					// ��������� ���������, ���������� ������� �� Opt.BufSize
					if (bPartlyFull)
					{
						if (!mySetFilePointer(hLFile,Opt.BufSize,FILE_CURRENT) || !mySetFilePointer(hRFile,Opt.BufSize,FILE_CURRENT))
						{
							bEqual = false;
							break;
						}
//						else 	DebugMsg(L"skip",L"",Opt.BufSize);
					}

					// ������ ���� � �������� ������
					if (!LPanel.bARC && LPtr >= Opt.Buf[0]+LBufPos)
					{
						LBufPos=0;
						LPtr=Opt.Buf[0];
						// ������ ������ Opt.BufSize
						while (LBufPos < Opt.BufSize)
						{
							if (CheckForEsc() || 
									!ReadFile(hLFile,Opt.Buf[0]+LBufPos,(!bPartlyKb || bFromEnd || LFilePos+ReadBlock<=PartlyKbSize)?ReadBlock:(PartlyKbSize-LFilePos),&LReadSize,0))
							{
								bEqual=false;
								break;
							}
							LBufPos+=LReadSize;
							LFilePos+=LReadSize;
//					DebugMsg(L"LReadSize",L"",LReadSize);
							CmpInfo.CurProcSize+=LReadSize;
							CmpInfo.ProcSize+=LReadSize;
							if (LReadSize < ReadBlock) break;
						}
					}
					if (!bEqual)
						break;

					// ������ ���� � ��������� ������
					if (!RPanel.bARC && RPtr >= Opt.Buf[1]+RBufPos)
					{
						RBufPos=0;
						RPtr=Opt.Buf[1];
						// ������ ������ Opt.BufSize
						while (RBufPos < Opt.BufSize)
						{
							if (CheckForEsc() || 
									!ReadFile(hRFile,Opt.Buf[1]+RBufPos,(!bPartlyKb || bFromEnd || RFilePos+ReadBlock<=PartlyKbSize)?ReadBlock:(PartlyKbSize-RFilePos),&RReadSize,0))
							{
								bEqual=false;
								break;
							}
							RBufPos+=RReadSize;
							RFilePos+=RReadSize;
//					DebugMsg(L"RReadSize",L"",RReadSize);

							CmpInfo.CurProcSize+=RReadSize;
							CmpInfo.ProcSize+=RReadSize;
							if (RReadSize < ReadBlock) break;
						}
					}
					if (!bEqual)
						break;

					ShowMessage(LDir,pLPPI->FindData.lpwszFileName,RDir,pRPPI->FindData.lpwszFileName,false);

					// ���������� � �������
					if (RPanel.bARC)
					{
						dwFileCRC=ProcessCRC(Opt.Buf[0],LBufPos,dwFileCRC);
						LPtr+=LBufPos;
						CmpInfo.CurProcSize+=LBufPos;
						CmpInfo.ProcSize+=LBufPos;
					}
					else if (LPanel.bARC)
					{
						dwFileCRC=ProcessCRC(Opt.Buf[1],RBufPos,dwFileCRC);
						RPtr+=RBufPos;
						CmpInfo.CurProcSize+=RBufPos;
						CmpInfo.ProcSize+=RBufPos;
					}

					if (LPanel.bARC || RPanel.bARC)
					{
						if ((RPanel.bARC && LBufPos != Opt.BufSize) || (LPanel.bARC && RBufPos != Opt.BufSize))
						{
							if (!LPanel.bARC && RPanel.bARC && dwFileCRC != pRPPI->CRC32)
								bEqual=false;
							else if (LPanel.bARC && !RPanel.bARC && dwFileCRC != pLPPI->CRC32)
								bEqual = false;
//              wchar_t buf[40];
//              FSF.sprintf(buf, L"L - %X R- %X", LPanel.bARC?pLPPI->CRC32:dwFileCRC, RPanel.bARC?pRPPI->CRC32:dwFileCRC);
//							if ((LPanel.bARC?pLPPI->CRC32:dwFileCRC)!=(RPanel.bARC?pRPPI->CRC32:dwFileCRC))
//              DebugMsg(buf,(wchar_t*)pLPPI->FindData.lpwszFileName,(LPanel.bARC?pLPPI->CRC32:dwFileCRC)!=(RPanel.bARC?pRPPI->CRC32:dwFileCRC));
							break;
						}
						else
							continue;
					}

					// ������� ��������� (������ �������� ��� ����� ����������)
					if (!Opt.Ignore || bExe)
					{
						if (memcmp(Opt.Buf[0], Opt.Buf[1], LBufPos))
						{
							bEqual=false;
							break;
						}
						LPtr += LBufPos;
						RPtr += RBufPos;

						// ������� ��, �������
						if (LBufPos != Opt.BufSize || RBufPos != Opt.BufSize)
							break;
					}
					else
					// ������ �������
					{
						if (Opt.IgnoreTemplates == 0)      // '\n' & ' '
						{
							while (LPtr < Opt.Buf[0]+LBufPos && RPtr < Opt.Buf[1]+RBufPos && !IsWhiteSpace(*LPtr) && !IsWhiteSpace(*RPtr))
							{
								if (*LPtr != *RPtr)
								{
									bEqual=false;
									break;
								}
								++LPtr;
								++RPtr;
							}
							if (!bEqual)
								break;

							while (LPtr < Opt.Buf[0]+LBufPos && IsWhiteSpace(*LPtr))
								++LPtr;

							while (RPtr < Opt.Buf[1]+RBufPos && IsWhiteSpace(*RPtr))
								++RPtr;
						}
						else if (Opt.IgnoreTemplates == 1)  // '\n'
						{
							if (bLExpectNewLine)
							{
								bLExpectNewLine=false;
								if (LPtr < Opt.Buf[0]+LBufPos && *LPtr == '\n')
									++LPtr;
							}

							if (bRExpectNewLine)
							{
								bRExpectNewLine=false;
								if (RPtr < Opt.Buf[1]+RBufPos && *RPtr == '\n')
									++RPtr;
							}

							while (LPtr < Opt.Buf[0]+LBufPos && RPtr < Opt.Buf[1]+RBufPos && !IsNewLine(*LPtr) && !IsNewLine(*RPtr))
							{
								if (*LPtr != *RPtr)
								{
									bEqual = false;
									break;
								}
								++LPtr;
								++RPtr;
							}
							if (!bEqual)
								break;

							if (LPtr < Opt.Buf[0]+LBufPos && RPtr < Opt.Buf[1]+RBufPos && (!IsNewLine(*LPtr) || !IsNewLine(*RPtr)))
							{
								bEqual = false;
								break;
							}

							if (LPtr < Opt.Buf[0]+LBufPos && RPtr < Opt.Buf[1]+RBufPos)
							{
								if (*LPtr == '\r')
									bLExpectNewLine=true;

								if (*RPtr == '\r')
									bRExpectNewLine=true;

								++LPtr;
								++RPtr;
							}
						}
						else if (Opt.IgnoreTemplates == 2)  // ' '
						{
							while (LPtr < Opt.Buf[0]+LBufPos && RPtr < Opt.Buf[1]+RBufPos && !myIsSpace(*LPtr) && !myIsSpace(*RPtr))
							{
								if (*LPtr != *RPtr)
								{
									bEqual=false;
									break;
								}
								++LPtr;
								++RPtr;
							}
							if (!bEqual)
								break;

							while (LPtr < Opt.Buf[0]+LBufPos && myIsSpace(*LPtr))
								++LPtr;

							while (RPtr < Opt.Buf[1]+RBufPos && myIsSpace(*RPtr))
								++RPtr;
						}
						if (!LReadSize && RReadSize || LReadSize && !RReadSize)
						{
							bEqual = false;
							break;
						}
					}
					if (!LReadSize && !RReadSize)
						break;
				}

				// �������� � ��� ���������
				if (Opt.Cache && !(LPanel.bARC || RPanel.bARC) && (!Opt.Ignore || bExe) && !Opt.Partly)
				{
					Opt.Cache=SetCacheResult(dwLFileName,dwRFileName,
																	((__int64)pLPPI->FindData.ftLastWriteTime.dwHighDateTime << 32) | pLPPI->FindData.ftLastWriteTime.dwLowDateTime,
																	((__int64)pRPPI->FindData.ftLastWriteTime.dwHighDateTime << 32) | pRPPI->FindData.ftLastWriteTime.dwLowDateTime,
																	bEqual?RCIF_EQUAL:RCIF_DIFFER);
//              DebugMsg(L"SetCacheResult",(wchar_t*)pLPPI->FindData.lpwszFileName,bEqual?RCIF_EQUAL:RCIF_DIFFER);
//              wchar_t buf[200];
//              FSF.sprintf(buf, L"SetCacheResult: L - %X R- %X", dwLFileName, dwRFileName);
//              DebugMsg(buf,(wchar_t*)pLPPI->FindData.lpwszFileName,bEqual?RCIF_EQUAL:RCIF_DIFFER);

				}
			}
			if (!LPanel.bARC)
			{
				CloseHandle(hLFile);
				if ((hLFile=CreateFileW(strLFullFileName, FILE_WRITE_ATTRIBUTES, FILE_SHARE_READ | FILE_SHARE_WRITE, 0,
                                OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, 0)) != INVALID_HANDLE_VALUE)
				{
					SetFileTime(hLFile,0,&LAccess,0);
					CloseHandle(hLFile);
				}
			}
			if (!RPanel.bARC)
			{
				CloseHandle(hRFile);
				if ((hRFile=CreateFileW(strRFullFileName, FILE_WRITE_ATTRIBUTES, FILE_SHARE_READ | FILE_SHARE_WRITE, 0,
                                OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, 0)) != INVALID_HANDLE_VALUE)
				{
					SetFileTime(hRFile,0,&RAccess,0);
					CloseHandle(hRFile);
				}
			}
			if (!bEqual)
			{
				CmpInfo.ProcSize+=CmpInfo.CurCountSize-CmpInfo.CurProcSize;
				return false;
			}
		}
	}
	return true;
}


/****************************************************************************
 *************************** CompareDirs functions **************************
 ****************************************************************************/


/****************************************************************************
 * ������� ��������� ��� ������ � ���� ���������� PluginPanelItem
 * ��� ���� qsort()
 ****************************************************************************/
int __cdecl PICompare(const void *el1, const void *el2)
{
	const PluginPanelItem *ppi1 = *(const PluginPanelItem **)el1, *ppi2 = *(const PluginPanelItem **)el2;

	if (ppi1->FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
	{
		if (!(ppi2->FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			return 1;
	}
	else
	{
		if (ppi2->FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			return -1;
	}

	string strLFileName(LPanel.bTMP||RPanel.bTMP?FSF.PointToName(ppi1->FindData.lpwszFileName):ppi1->FindData.lpwszFileName);
	string strRFileName(LPanel.bTMP||RPanel.bTMP?FSF.PointToName(ppi2->FindData.lpwszFileName):ppi2->FindData.lpwszFileName);

	int i=FSF.LStricmp(Opt.SkipSubstr?CutSubstr(strLFileName,Opt.Substr):strLFileName.get(),
											Opt.SkipSubstr?CutSubstr(strRFileName,Opt.Substr):strRFileName.get());
//			DebugMsg(strLFileName.get(),L"PICompare-strLFileName",i);
//			DebugMsg(strRFileName.get(),L"PICompare-strRFileName",i);
  return i;
}

/****************************************************************************
 * ������� ���������, ������ �� ���� �� ������ � �������� ������� �����������
 ****************************************************************************/
bool AdvCmpProc::CheckScanDepth(const wchar_t *FileName, int ScanDepth)
{
	int i=0;
	while (*FileName++)
		if (*FileName==L'\\') i++;
	return  i<ScanDepth;
}

/****************************************************************************
 * ���������� �������������� ������ ��������� ��� �������� ���������
 ****************************************************************************/
bool AdvCmpProc::BuildItemsIndex(bool bLeftPanel,const struct DirList *pList,struct ItemsIndex *pIndex,int ScanDepth)
{
	pIndex->pPPI=0;
	pIndex->iCount=pList->ItemsNumber;

	if (!pIndex->iCount)
		return true;
	if (!(pIndex->pPPI=(PluginPanelItem **)malloc(pIndex->iCount * sizeof(pIndex->pPPI[0]))))
		return false;

	int j = 0;
	for (int i=pIndex->iCount-1; i>=0 && j<pIndex->iCount; i--)
	{
					// �������� ��������� �����... ���� ����
		if ( (Opt.ProcessSubfolders || !(pList->PPI[i].FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) &&
					// �������� ������ ���������� ��������... ���� ���� :)
				(!(Opt.ProcessSelected && ScanDepth==0) || (pList->PPI[i].Flags & PPIF_SELECTED))
			 )
		{
			if ( (pList->PPI[i].FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) &&
					 ((pList->PPI[i].FindData.lpwszFileName[0]==L'.' && !pList->PPI[i].FindData.lpwszFileName[1]) || 
					 (pList->PPI[i].FindData.lpwszFileName[1]==L'.' && !pList->PPI[i].FindData.lpwszFileName[2]))
					)
				continue;
			if ( (bLeftPanel?LPanel.hFilter:RPanel.hFilter)!=INVALID_HANDLE_VALUE &&
					 !Info.FileFilterControl((bLeftPanel?LPanel.hFilter:RPanel.hFilter),FFCTL_ISFILEINFILTER,0,(LONG_PTR)&pList->PPI[i].FindData))
				continue;
			if (Opt.Filter && !Info.FileFilterControl(Opt.hCustomFilter,FFCTL_ISFILEINFILTER,0,(LONG_PTR)&pList->PPI[i].FindData))
				continue;

			if (ScanDepth)
			{
				// ����� + ������ || ������ + ����� (������� � ������)
				if ((LPanel.bARC && !RPanel.bARC && !bLeftPanel) || (!LPanel.bARC && RPanel.bARC && bLeftPanel))
				{
					string srtFileName(pList->PPI[i].FindData.lpwszFileName+4);
					string strSubstr;
					wchar_t *p=pList->Dir+4;
					while (*p++) // ��� ������������� ������������ � ���������
					{
						if (*p==L'\\' || *p==L'[' || *p==L']' || *p==L'+' || *p==L'{' || *p==L'}')
							strSubstr+=L"\\";
						strSubstr+=*p;
					}
					// ������� pList->Dir �� ����� �����, �.�. ���� �� ������� ����� (� ���� �����) ��� �� �����
					wcscpy((wchar_t*)pList->PPI[i].FindData.lpwszFileName,CutSubstr(srtFileName,strSubstr.get())+2);

					if (Opt.ProcessSubfolders==2 && !CheckScanDepth(pList->PPI[i].FindData.lpwszFileName, Opt.MaxScanDepth))
						continue;
				}

				// ����� + ������ || ������ + ����� (������� � ������)
				else if ((LPanel.bARC && !RPanel.bARC && bLeftPanel) || (!LPanel.bARC && RPanel.bARC && !bLeftPanel))
				{
					string srtFileName(pList->PPI[i].FindData.lpwszFileName);
					// ������� �� �� ������� �����, ��� ��� ���� ����� ��� �� �����
					wcscpy((wchar_t*)pList->PPI[i].FindData.lpwszFileName,CutSubstr(srtFileName,L"^(.*?)\\\\"));

					if (Opt.ProcessSubfolders==2 && !CheckScanDepth(pList->PPI[i].FindData.lpwszFileName, Opt.MaxScanDepth))
						continue;
				}
			}

			pIndex->pPPI[j++]=&pList->PPI[i];
		}
	}

	if (pIndex->iCount=j)
	{
			FSF.qsort(pIndex->pPPI,j,sizeof(pIndex->pPPI[0]),PICompare);
	}
	else
	{
		free(pIndex->pPPI);
		pIndex->pPPI=0;
	}

	return true;
}

/****************************************************************************
 * ������������ ������
 ****************************************************************************/
void AdvCmpProc::FreeItemsIndex(struct ItemsIndex *pIndex)
{
	if (pIndex->pPPI)
		free(pIndex->pPPI);
	pIndex->pPPI=0;
	pIndex->iCount=0;
}

/****************************************************************************
 * ���������� ������� ���������, ��� ������� � ������������ ���������
 ****************************************************************************/
bool AdvCmpProc::BuildFileList(const wchar_t *LDir,const PluginPanelItem *pLPPI,const wchar_t *RDir,const PluginPanelItem *pRPPI,DWORD dwFlag)
{
	if (pLPPI && pRPPI && (pLPPI->FindData.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY) && (pRPPI->FindData.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY))
		return true;

	File add;
	add.dwFlags=dwFlag;
	add.strLDir=LDir;
	add.strRDir=RDir;
	if (pLPPI)
	{
		add.strFileName=pLPPI->FindData.lpwszFileName;
		add.dwAttributes=pLPPI->FindData.dwFileAttributes;
		add.ftLLastWriteTime.dwLowDateTime=pLPPI->FindData.ftLastWriteTime.dwLowDateTime;
		add.ftLLastWriteTime.dwHighDateTime=pLPPI->FindData.ftLastWriteTime.dwHighDateTime;
		add.nLFileSize=pLPPI->FindData.nFileSize;
	}
	if (pRPPI)
	{
		if (!add.strFileName.length()) add.strFileName=pRPPI->FindData.lpwszFileName;
		add.dwAttributes=pRPPI->FindData.dwFileAttributes;
		add.ftRLastWriteTime.dwLowDateTime=pRPPI->FindData.ftLastWriteTime.dwLowDateTime;
		add.ftRLastWriteTime.dwHighDateTime=pRPPI->FindData.ftLastWriteTime.dwHighDateTime;
		add.nRFileSize=pRPPI->FindData.nFileSize;
	}
	FileList.Push(&add);
	return true;
}

/****************************************************************************
 * ��������� ���� ���������, ��������� ����������� AInfo � PInfo.
 * ���������� true, ���� ��� ���������.
 * �������� bCompareAll ����������,
 * ���� �� ���������� ��� ����� � �������� PPIF_SELECTED (bCompareAll == true)
 * ��� ������ ������� false ��� ������ ������������ (bCompareAll == false).
 ****************************************************************************/
bool AdvCmpProc::CompareDirs(const struct DirList *pLList,const struct DirList *pRList,bool bCompareAll,int ScanDepth)
{
	// �������� � ���������� � ���������
	ShowMessage(pLList->Dir,L"*",pRList->Dir,L"*",true);

	// ������� ����������, ������� ��������� ����� ���������� � �� ������
	if (ScanDepth==0 && Opt.TotalProcess)
	{
		if (!LPanel.PInfo.Plugin)
		{
			if (!Opt.ProcessSelected)
				GetDirList(pLList->Dir,ScanDepth,true);
			else
			{
				string strDir;
				for (int i=0; !bBrokenByEsc && i<pLList->ItemsNumber; i++)
				{
					if (pLList->PPI[i].Flags&PPIF_SELECTED)
					{
						if (pLList->PPI[i].FindData.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)
						{
							strDir=pLList->Dir;
							if (strDir.length()>0 && strDir[(size_t)(strDir.length()-1)]!=L'\\') strDir+=L"\\";
							strDir+=pLList->PPI[i].FindData.lpwszFileName;
							GetDirList(strDir,ScanDepth,true);
						}
						else
						{
							CmpInfo.Count+=1;
							CmpInfo.CountSize+=(unsigned __int64)pLList->PPI[i].FindData.nFileSize;
						}
					}
				}
			}
		}

		if (bBrokenByEsc)
			return true;

		if (!RPanel.PInfo.Plugin)
		{
			if (!Opt.ProcessSelected)
				GetDirList(pRList->Dir,ScanDepth,true);
			else
			{
				string strDir;
				for (int i=0; !bBrokenByEsc && i<pRList->ItemsNumber; i++)
				{
					if (pRList->PPI[i].Flags&PPIF_SELECTED)
					{
						if (pRList->PPI[i].FindData.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)
						{
							strDir=pRList->Dir;
							if (strDir.length()>0 && strDir[(size_t)(strDir.length()-1)]!=L'\\') strDir+=L"\\";
							strDir+=pRList->PPI[i].FindData.lpwszFileName;
							GetDirList(strDir,ScanDepth,true);
						}
						else
						{
							CmpInfo.Count+=1;
							CmpInfo.CountSize+=(unsigned __int64)pRList->PPI[i].FindData.nFileSize;
						}
					}
				}
			}
		}

		if (bBrokenByEsc)
			return true;
	}

	// ������ ������� ���������, ��� ���������� ���������
	struct ItemsIndex LII, RII;
	if (!BuildItemsIndex(true,pLList,&LII,ScanDepth) || !BuildItemsIndex(false,pRList,&RII,ScanDepth))
	{
		ErrorMsg(MNoMemTitle,MNoMemBody);
		bBrokenByEsc=true;
		FreeItemsIndex(&LII);
		FreeItemsIndex(&RII);
		return true;
	}

	// ��������-��������� ���������� ��������
	if (ScanDepth && Opt.Panel && !Opt.IgnoreMissing && LII.iCount!=RII.iCount)
	{
		FreeItemsIndex(&LII);
		FreeItemsIndex(&RII);
		return false;
	}

	int i, j;

	// ������� ������ ��������� �� �������
	if (ScanDepth==0)
	{
		for (i=0; i<pLList->ItemsNumber; i++)
			pLList->PPI[i].Flags &= ~PPIF_SELECTED;
		for (i=0; i<pRList->ItemsNumber; i++)
			pRList->PPI[i].Flags &= ~PPIF_SELECTED;
	}

	// �������� ���������� "����" ��������...
	bool bDifferenceNotFound=true;
	i=0; j=0;
	DWORD dwFlag=0;

	while (i<LII.iCount && j<RII.iCount && (bDifferenceNotFound || bCompareAll) && !bBrokenByEsc)
	{
		// �������� �� ESC
		const int iMaxCounter=256;
		static int iCounter=iMaxCounter;
		if (!--iCounter)
		{
			iCounter=iMaxCounter;
			if (CheckForEsc())
				break;
		}

		bool bNextItem;
		dwFlag=RCIF_DIFFER;

		switch (PICompare(&LII.pPPI[i], &RII.pPPI[j]))
		{

			/******************************************************************************/

			case 0: // ����� ������� - ��������� �� ���������
			{
//wchar_t buf[512];
//FSF.sprintf(buf,L"Left: %s Right: %s, %d + %d", LII.pPPI[i]->FindData.lpwszFileName, RII.pPPI[j]->FindData.lpwszFileName, i,j);
//			 DebugMsg(buf,L"case 0",bDifferenceNotFound);

				if (CompareFiles(pLList->Dir,LII.pPPI[i],pRList->Dir,RII.pPPI[j],ScanDepth))
				{// � ��������� �������
					i++; j++;
					dwFlag=RCIF_EQUAL;
				}
				else
				{
					bDifferenceNotFound=false;
					// ������, ����� ���?
					if (Opt.SelectedNew || !Opt.Panel)
					{
						__int64 Delta=(((__int64)LII.pPPI[i]->FindData.ftLastWriteTime.dwHighDateTime << 32) | LII.pPPI[i]->FindData.ftLastWriteTime.dwLowDateTime) -
													(((__int64)RII.pPPI[j]->FindData.ftLastWriteTime.dwHighDateTime << 32) | RII.pPPI[j]->FindData.ftLastWriteTime.dwLowDateTime);

						if (Delta>0)
						{
							LII.pPPI[i]->Flags |= PPIF_SELECTED;
							dwFlag=RCIF_LNEW;
						}
						else if (Delta<0)
						{
							RII.pPPI[j]->Flags |= PPIF_SELECTED;
							dwFlag=RCIF_RNEW;
						}
						else
						{
							LII.pPPI[i]->Flags |= PPIF_SELECTED;
							RII.pPPI[j]->Flags |= PPIF_SELECTED;
						}
					}
					else
					{
						LII.pPPI[i]->Flags |= PPIF_SELECTED;
						RII.pPPI[j]->Flags |= PPIF_SELECTED;
					}
					i++; j++;
					CmpInfo.LDiff++; CmpInfo.RDiff++;
					if (Opt.ProcessTillFirstDiff && !bBrokenByEsc)
					{     // ����� �� ���������� ����������
						bCompareAll=(Opt.ShowMsg && !YesNoMsg(MFirstDiffTitle, MFirstDiffBody));
						Opt.ProcessTillFirstDiff=0;
					}
				}
				CmpInfo.Proc+=2;
				// ������� ������� � ������ �����������
				if (!Opt.Panel)
					BuildFileList(pLList->Dir,LII.pPPI[i-1],pRList->Dir,RII.pPPI[j-1],dwFlag);
				break;
			}

			/******************************************************************************/

			case -1: // ������� LII.pPPI[i] �� ����� ���������� � RII.pPPI
			{
//wchar_t buf2[512];
//FSF.sprintf(buf2,L"Left: %s Right: %s", LII.pPPI[i]->FindData.lpwszFileName, RII.pPPI[j]->FindData.lpwszFileName);
//			 DebugMsg(buf2,L"case -1",bDifferenceNotFound);

				CmpContinueL:
				dwFlag=RCIF_DIFFER;
				if (!Opt.IgnoreMissing)
				{
					if (!LPanel.bTMP)
					{
						bNextItem=true;
						goto FoundDiffL;
					}
					else
					{ // ...�� ���� � ����-������, �� �������� � ��������� RII.pPPI
						bNextItem=false;
						for (int k=0; k<RII.iCount; k++)
						{
							if (!PICompare(&LII.pPPI[i], &RII.pPPI[k]))
							{
								bNextItem = true;
								if (CompareFiles(pLList->Dir,LII.pPPI[i],pRList->Dir,RII.pPPI[k],ScanDepth))
								{
									i++;
									break;
								}
								else
								FoundDiffL:
								{
									bDifferenceNotFound=false;
									LII.pPPI[i]->Flags |= PPIF_SELECTED;
									dwFlag=RCIF_LNEW;
									i++; CmpInfo.LDiff++;
									if (LPanel.bTMP && k<RII.iCount)
									{
										RII.pPPI[k]->Flags |= PPIF_SELECTED;
										CmpInfo.RDiff++;
									}
									if (Opt.ProcessTillFirstDiff && !bBrokenByEsc)
									{
										bCompareAll=(Opt.ShowMsg && !YesNoMsg(MFirstDiffTitle, MFirstDiffBody));
										Opt.ProcessTillFirstDiff=0;
									}
									break;
								}
							}
						}
						if (!bNextItem)
						{
							bNextItem=true;
							goto FoundDiffL;
						}
					}
				}
				else
				{
					i++;
				}
				CmpInfo.Proc++;
				// ������� ������� � ������ �����������
				if (!Opt.Panel)
					BuildFileList(pLList->Dir,LII.pPPI[i-1],pRList->Dir,NULL,dwFlag);
				break;
			}

			/******************************************************************************/

			case 1: // ������� RII.pPPI[j] �� ����� ���������� � LII.pPPI
			{
//wchar_t buf3[512];
//FSF.sprintf(buf3,L"Left: %s Right: %s", LII.pPPI[i]->FindData.lpwszFileName, RII.pPPI[j]->FindData.lpwszFileName);
//			 DebugMsg(buf3,L"case 1",bDifferenceNotFound);

				CmpContinueR:
				dwFlag=RCIF_DIFFER;
				if (!Opt.IgnoreMissing)
				{
					if (!RPanel.bTMP)
					{
						bNextItem=true;
						goto FoundDiffR;
					}
					else
					{ // ...�� ���� � ����-������, �� �������� � ��������� LII.pPPI
						bNextItem=false;
						for (int k=0; k<LII.iCount; k++)
						{
							if (!PICompare(&LII.pPPI[k], &RII.pPPI[j]))
							{
								bNextItem = true;
								if (CompareFiles(pLList->Dir,LII.pPPI[k],pRList->Dir,RII.pPPI[j],ScanDepth))
								{
									j++;
									break;
								}
								else
								FoundDiffR:
								{
									bDifferenceNotFound=false;
									RII.pPPI[j]->Flags |= PPIF_SELECTED;
									dwFlag=RCIF_RNEW;
									j++; CmpInfo.RDiff++;
									if (RPanel.bTMP && k<LII.iCount)
									{
										LII.pPPI[k]->Flags |= PPIF_SELECTED;
										CmpInfo.LDiff++;
									}
									if (Opt.ProcessTillFirstDiff && !bBrokenByEsc)
									{
										bCompareAll=(Opt.ShowMsg && !YesNoMsg(MFirstDiffTitle, MFirstDiffBody));
										Opt.ProcessTillFirstDiff=0;
									}
									break;
								}
							}
						}
						if (!bNextItem)
						{
							bNextItem = true;
							goto FoundDiffR;
						}
					}
				}
				else
				{
					j++;
				}
				CmpInfo.Proc++;
				// ������� ������� � ������ �����������
				if (!Opt.Panel)
					BuildFileList(pLList->Dir,NULL,pRList->Dir,RII.pPPI[j-1],dwFlag);
				break;
			}
		}
	}

	if (!bBrokenByEsc)
	{
		// ���������� ��������� ��������. ������� ��, ��� �������� �������������� � ��������
		if (!Opt.IgnoreMissing && i<LII.iCount)
		{
			if (!LPanel.bTMP)
				bDifferenceNotFound=false;
			if (bCompareAll)
				goto CmpContinueL;
		}
		if (!Opt.IgnoreMissing && j<RII.iCount)
		{
			if (!RPanel.bTMP)
				bDifferenceNotFound=false;
			if (bCompareAll)
				goto CmpContinueR;
		}
	}
//			 DebugMsg(L"LII.iCount",L"",LII.iCount);
//			 DebugMsg(L"RII.iCount",L"",RII.iCount);

	FreeItemsIndex(&LII);
	FreeItemsIndex(&RII);

	return bDifferenceNotFound;
}

int GetPosToName(wchar_t *FileName)
{
	if (FileName[0]==L'\\' && FileName[1]==L'\\' && FileName[2]==L'?')
	{
		if (FileName[5]==L':')
			return 4;
		else if (FileName[5]==L'N')
			return 7;
	}
	return 0;
}

/***************************************************************************
 * ���������/���������� ����� ������ � �������
 ***************************************************************************/
bool UpdateFarList(HANDLE hDlg, DList<File> *pFileList, bool bInit=false)
{
	// �������� ����������
	FarListInfo ListInfo;
	Info.SendDlgMessage(hDlg,DM_LISTINFO,0,(LONG_PTR)&ListInfo);

	if (ListInfo.ItemsNumber)
		Info.SendDlgMessage(hDlg,DM_LISTDELETE,0,0);

	wchar_t *buf=(wchar_t *)malloc(65536*sizeof(wchar_t));

	for (File *cur=pFileList->First(); cur; cur=pFileList->Next(cur))
	{
		// ������� �������-�����. ������ ��� ������ ���������� ������!
		File *next=pFileList->Next(cur);
		if (bInit && next && !(next->dwAttributes&FILE_ATTRIBUTE_DIRECTORY) && FSF.LStricmp(cur->strLDir.get(),next->strLDir.get()))
		{
			File add;
			add.strLDir=next->strLDir;
			add.strRDir=next->strRDir;
			add.dwAttributes|=FILE_ATTRIBUTE_DIRECTORY;
			pFileList->InsertBefore(next,&add);
		}

		wchar_t LTime[18]={0}, RTime[18]={0};
		SYSTEMTIME ModificTime;
		FILETIME local;
		if (cur->ftLLastWriteTime.dwLowDateTime || cur->ftLLastWriteTime.dwHighDateTime)
		{
			FileTimeToLocalFileTime(&(cur->ftLLastWriteTime), &local);
			FileTimeToSystemTime(&local, &ModificTime);
			FSF.sprintf(LTime, L"%02d.%02d.%02d %02d:%02d:%02d",ModificTime.wDay,ModificTime.wMonth,ModificTime.wYear%100,ModificTime.wHour,ModificTime.wMinute,ModificTime.wSecond);
		}
		if (cur->ftRLastWriteTime.dwLowDateTime || cur->ftRLastWriteTime.dwHighDateTime)
		{
			FileTimeToLocalFileTime(&(cur->ftRLastWriteTime), &local);
			FileTimeToSystemTime(&local, &ModificTime);
			FSF.sprintf(RTime, L"%02d.%02d.%02d %02d:%02d:%02d",ModificTime.wDay,ModificTime.wMonth,ModificTime.wYear%100,ModificTime.wHour,ModificTime.wMinute,ModificTime.wSecond);
		}

		wchar_t LSize[65]={0}, RSize[65]={0};
		if (!(cur->dwAttributes&FILE_ATTRIBUTE_DIRECTORY))
		{
			if (LTime[0]) FSF.itoa64(cur->nLFileSize,LSize,10);
			if (RTime[0]) FSF.itoa64(cur->nRFileSize,RSize,10);
		}

		wchar_t Mark;
		switch (cur->dwFlags)
		{
			case RCIF_EQUAL:
				Mark=L'='; break;
			case RCIF_DIFFER:
				Mark=0x2260; break;
			case RCIF_LNEW:
				Mark=0x2192; break;
			case RCIF_RNEW:
				Mark=0x2190; break;
			default:
				Mark=L' '; break;
		}

		if (cur->dwAttributes&FILE_ATTRIBUTE_DIRECTORY)
		{
			int len=WinInfo.Con.Right/2-3;
			string strTmp1(GetMsg(MFolder));
			if (LTime[0] || !cur->dwFlags)
			{
				strTmp1=cur->strLDir.get()+GetPosToName(cur->strLDir.get());
				if (strTmp1.length()>0 && strTmp1[(size_t)(strTmp1.length()-1)]!=L'\\') strTmp1+=L"\\";
				strTmp1+=cur->strFileName;
				if (strTmp1.length()>0 && strTmp1[(size_t)(strTmp1.length()-1)]!=L'\\') strTmp1+=L"\\";
				FSF.TruncPathStr(strTmp1.get(),len);
				strTmp1.updsize();
			}
			string strTmp2(GetMsg(MFolder));
			if (RTime[0] || !cur->dwFlags)
			{
				strTmp2=cur->strRDir.get()+GetPosToName(cur->strRDir.get());
				if (strTmp2.length()>0 && strTmp2[(size_t)(strTmp2.length()-1)]!=L'\\') strTmp2+=L"\\";
				strTmp2+=cur->strFileName;
				if (strTmp2.length()>0 && strTmp2[(size_t)(strTmp2.length()-1)]!=L'\\') strTmp2+=L"\\";
				FSF.TruncPathStr(strTmp2.get(),len);
				strTmp2.updsize();
			}
			FSF.sprintf(buf, L"%-*.*s  %-*.*s",len,len,strTmp1.get(),len,len,strTmp2.get());
		}
		else
			FSF.sprintf(buf, L"%*.*s%c%*.*s%c%*.*s%c%*.*s%c%c%c%s",14,14,LSize,0x2502,17,17,LTime,0x2551,17,17,RTime,0x2502,14,14,RSize,0x2551,Mark,0x2502,cur->strFileName.get());

		struct FarList List;
		List.ItemsNumber=1;
		struct FarListItem Item;
		Item.Flags=0;
		if (cur->dwAttributes&FILE_ATTRIBUTE_DIRECTORY)
		{
			Item.Flags=0x2b; 
			Item.Flags|=LIF_CHECKED;
		}
		else if (cur->dwFlags==RCIF_EQUAL) Item.Flags|=LIF_GRAYED;
		Item.Text=buf;
		Item.Reserved[0]=Item.Reserved[1]=Item.Reserved[2]=0;
		List.Items=&Item;
		Info.SendDlgMessage(hDlg,DM_LISTADD,0,(LONG_PTR)&List);
	}

	if (buf) free(buf);

	FarListPos ListPos;
	ListPos.SelectPos=0/*ListInfo.SelectPos*/;
	ListPos.TopPos=-1/*ListInfo.TopPos*/;
	Info.SendDlgMessage(hDlg,DM_LISTSETCURPOS,0,(LONG_PTR)&ListPos);

	Info.SendDlgMessage(hDlg,DM_LISTSETMOUSEREACTION,0,(LONG_PTR)LMRT_NEVER);
	return true;
}


LONG_PTR WINAPI ShowCmpDialogProc(HANDLE hDlg,int Msg,int Param1,LONG_PTR Param2)
{
	DList<File> *pFileList=(DList<File> *)Info.SendDlgMessage(hDlg,DM_GETDLGDATA,0,0);
	switch(Msg)
	{
		case DN_INITDIALOG:
			UpdateFarList(hDlg,pFileList,true);
			break;

		case DN_RESIZECONSOLE:
		{
			COORD c=(*(COORD*)Param2);
			Info.SendDlgMessage(hDlg,DM_ENABLEREDRAW,false,0);
			Info.SendDlgMessage(hDlg,DM_RESIZEDIALOG,0,(LONG_PTR)&c);
			WinInfo.Con.Right=c.X-1;
			WinInfo.Con.Bottom=c.Y-1;
			Info.SendDlgMessage(hDlg,DM_SETITEMPOSITION,0,(LONG_PTR)&WinInfo.Con);
			c.X=c.Y=-1;
			Info.SendDlgMessage(hDlg,DM_MOVEDIALOG,true,(LONG_PTR)&c);
			Info.SendDlgMessage(hDlg,DM_LISTSETMOUSEREACTION,0,(LONG_PTR)LMRT_NEVER);
			Info.SendDlgMessage(hDlg,DM_ENABLEREDRAW,true,0);
			return true;
		}

		case DN_CTLCOLORDLGLIST:
		if (Param1==0)
		{
			static BYTE ColorDlgList[15];
			BYTE ColorIndex[]=
			{
				COL_PANELTEXT,
				COL_PANELBOX,
				COL_PANELBOX,
				COL_PANELTEXT,
				COL_PANELSELECTEDTEXT,
				COL_PANELBOX,
				COL_PANELCURSOR,
				COL_PANELSELECTEDCURSOR,
				COL_PANELSCROLLBAR,
				COL_PANELHIGHLIGHTTEXT,
				COL_PANELSELECTEDTEXT,
				COL_PANELSELECTEDCURSOR,
				COL_PANELSELECTEDTEXT,
				COL_PANELHIGHLIGHTTEXT,
				COL_PANELCURSOR
			};
			for (int i=0; i<sizeof(ColorDlgList); i++)
				ColorDlgList[i]=Info.AdvControl(Info.ModuleNumber,ACTL_GETCOLOR,(void *)ColorIndex[i]);

			FarListColors *flc=(FarListColors *)Param2;
			flc->Flags=flc->Reserved=0;
			flc->ColorCount=sizeof(ColorDlgList);
			flc->Colors=ColorDlgList;
			return true;
		}

		case DN_MOUSECLICK:
		{
			if (Param1==0 &&
							((MOUSE_EVENT_RECORD *)Param2)->dwButtonState==FROM_LEFT_1ST_BUTTON_PRESSED &&
							((MOUSE_EVENT_RECORD *)Param2)->dwEventFlags==DOUBLE_CLICK )
				goto GOTOFILE;
			return false;
		}

		case DN_KEY:
		{
			if (Param2==KEY_ENTER || Param2==KEY_NUMENTER)
			{
GOTOFILE:
				int Pos=Info.SendDlgMessage(hDlg,DM_LISTGETCURPOS,0,0);
				File *cur=NULL; unsigned index;
				for (cur=pFileList->First(), index=0; cur && index<Pos; cur=pFileList->Next(cur), index++)
					;
				if (cur)
				{
					if ((cur->dwAttributes&FILE_ATTRIBUTE_DIRECTORY) ||
							!(cur->ftLLastWriteTime.dwLowDateTime || cur->ftLLastWriteTime.dwHighDateTime) ||
							!(cur->ftRLastWriteTime.dwLowDateTime || cur->ftRLastWriteTime.dwHighDateTime))
						MessageBeep(MB_OK);
					else if (pCompareFiles)
					{
						string strLFullFileName=cur->strLDir.get();
						if (strLFullFileName.length()>0 && strLFullFileName[(size_t)(strLFullFileName.length()-1)]!=L'\\') strLFullFileName+=L"\\";
						strLFullFileName+=cur->strFileName;
						string strRFullFileName=cur->strRDir.get();
						if (strRFullFileName.length()>0 && strRFullFileName[(size_t)(strRFullFileName.length()-1)]!=L'\\') strRFullFileName+=L"\\";
						strRFullFileName+=cur->strFileName;
						pCompareFiles(strLFullFileName.get(),strRFullFileName.get(),0);
					}
				}
				return true;
			}
		}

	}
	return Info.DefDlgProc(hDlg,Msg,Param1,Param2);
}


int AdvCmpProc::ShowCmpDialog(const struct DirList *pLList,const struct DirList *pRList)
{
	FarDialogItem DialogItems[1];
	memset(DialogItems,0,sizeof(DialogItems));

	DialogItems[0].Type=DI_LISTBOX;
	DialogItems[0].X1=0; DialogItems[0].X2=WinInfo.Con.Right;
	DialogItems[0].Y1=0; DialogItems[0].Y2=WinInfo.Con.Bottom-WinInfo.Con.Top;
	DialogItems[0].Flags=DIF_LISTNOCLOSE;

	int len=WinInfo.Con.Right/2-2;

	int size=Info.Control(LPanel.hPlugin,FCTL_GETPANELHOSTFILE,0,0);
	string strTmp;
	if (size>1)
	{
		strTmp.get(size);
		Info.Control(LPanel.hPlugin,FCTL_GETPANELHOSTFILE,size,(LONG_PTR)strTmp.get());
	}
	else
		strTmp=pLList->Dir+GetPosToName(pLList->Dir);
	FSF.TruncPathStr(strTmp.get(),len-2);
	strTmp.updsize();
	string strTmp2=L" ";
	strTmp2+=strTmp+L" ";
	wchar_t buf1[MAX_PATH];
	strcentr(buf1,strTmp2.get(),len,0x00002550);

	size=Info.Control(RPanel.hPlugin,FCTL_GETPANELHOSTFILE,0,0);
	if (size>1)
	{
		strTmp.get(size);
		Info.Control(RPanel.hPlugin,FCTL_GETPANELHOSTFILE,size,(LONG_PTR)strTmp.get());
	}
	else
		strTmp=pRList->Dir+GetPosToName(pRList->Dir);
	FSF.TruncPathStr(strTmp.get(),len-2);
	strTmp.updsize();
	strTmp2=L" ";
	strTmp2+=strTmp+L" ";
	wchar_t buf2[MAX_PATH];
	strcentr(buf2,strTmp2.get(),len,0x00002550);

	wchar_t *buf=(wchar_t*)malloc(WinInfo.Con.Right*sizeof(wchar_t));
	if (buf)
	{
		FSF.sprintf(buf, L"%s %s",buf1,buf2);
		DialogItems[0].PtrData=buf;
	}

	HANDLE hDlg=Info.DialogInit(Info.ModuleNumber,0,0,WinInfo.Con.Right,WinInfo.Con.Bottom-WinInfo.Con.Top,L"Contents",DialogItems,
															sizeof(DialogItems)/sizeof(DialogItems[0]),0,FDLG_SMALLDIALOG|FDLG_KEEPCONSOLETITLE,ShowCmpDialogProc,(LONG_PTR)&FileList);
	if (hDlg != INVALID_HANDLE_VALUE)
	{
		Info.DialogRun(hDlg);
		Info.DialogFree(hDlg);
	}
	if (buf) free(buf);
	return 1;
}
