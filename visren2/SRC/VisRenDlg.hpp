/****************************************************************************
 * VisRenDlg.hpp
 *
 * Plugin module for FAR Manager 2.0
 *
 * Copyright (c) 2007-2010 Alexey Samlyukov
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

#pragma once
#include "VisRen.hpp"
#include "VisRenFile.hpp"

class VisRenDlg: public RenFile
{
		string PluginRootKey;
		int StartPosX;
		int SaveItemFocus;
		int Focus;

		void GetDlgSize();
		void DlgResize(HANDLE hDlg, bool bF5=false);
		void LenItems(const wchar_t *FileName, bool bDest);
		bool UpdateFarList(HANDLE hDlg, bool bFull, bool bUndoList);
		bool SetMask(HANDLE hDlg, DWORD IdMask, DWORD IdTempl);
		void MouseDragDrop(HANDLE hDlg, DWORD dwMousePosY);
		int ListMouseRightClick(HANDLE hDlg, DWORD dwMousePosX, DWORD dwMousePosY);
		void ShowName(int Pos);
		static LONG_PTR WINAPI ShowDialogProcThunk(HANDLE hDlg, int Msg, int Param1, LONG_PTR Param2);
		LONG_PTR WINAPI ShowDialogProc(HANDLE hDlg, int Msg, int Param1, LONG_PTR Param2);

	public:
		VisRenDlg() { }
		~VisRenDlg() { }
		int ShowDialog();
};