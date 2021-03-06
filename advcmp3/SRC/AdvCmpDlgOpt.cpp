/****************************************************************************
 * AdvCmpDlgOpt.cpp
 *
 * Plugin module for Far Manager 3.0
 *
 * Copyright (c) 2006 Alexey Samlyukov
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
#include "AdvCmpDlgOpt.hpp"

/****************************************************************************
 **************************** ShowOptDialog functions ***********************
 ****************************************************************************/

/****************************************************************************
 * ID-��������� �������
 ****************************************************************************/
enum {
	DlgBORDER = 0,  // 0

	DlgMODE,        // 1
	DlgMODEBOX,     // 2

	DlgCMPCASE,     // 3
	DlgCMPSIZE,     // 4
	DlgCMPTIME,     // 5
	DlgSECONDS,     // 6
	DlgIGNORESEC,   // 7
	DlgPRECISION,   // 8
	DlgTIMEZONE,    // 9
	DlgCMPCONTENTS, //10
	DlgDIFFTIME,    //11
	DlgCACHE,       //12
	DlgCACHEIGNORE, //13
	DlgCACHEUSE,    //14
	DlgCACHECLEAR,  //15
	DlgPARTLY,      //16
	DlgPARTLYFULL,  //17
	DlgLPARTLYKB,   //18
	DlgEPARTLYKB,   //19
	DlgIGNORE,      //20
	DlgIGNORETEMPL, //21

	DlgDUPPATH,     //22
	DlgDUPNAME,     //23
	DlgDUPSIZE,     //24
	DlgDUPCONTENTS, //25
	DlgDUPPIC,      //26
	DlgDUPPICDIFF,  //27
	DlgDUPPICSIZE,  //28
	DlgDUPPICFMT,   //29
	DlgDUPMUSIC,    //30
	DlgDUPMUSICARTIST,//31
	DlgDUPMUSICTITLE, //32
	DlgDUPMUSICDURL,  //33
	DlgDUPMUSICDURE,  //34

	DlgSEP1,        //35
	DlgSUBFOLDER,   //36
	DlgLMAXDEPTH,   //37
	DlgEMAXDEPTH,   //38
	DlgSCANSYMLINK, //39
	DlgLCMPSKIP,    //40
	DlgECMPSKIP,    //41
	DlgSTOPDIFFDUP, //42
	DlgFILTER,      //43
	DlgFILTERBOTTON,//44
	DlgSELECTED,    //45
	DlgSELECTEDNEW, //46
	DlgONLYRIGHT,   //47
	DlgIGNORMISSING,//48
	DlgLIGHTSYNC,   //49
	DlgSHOWMSG,     //50
	DlgSOUND,       //51
	DlgDIALOG,      //52
	DlgTOTALPROCESS,//53

	DlgSEP2,        //54
	DlgOK,          //55
	DlgUNDERCURSOR, //56
	DlgCANCEL,      //57
};

struct ParamStore
{
	int ID;
	wchar_t *OptName;
	int *Option;
} StoreOpt[] = {
	{DlgBORDER,      0,                      0},
	{DlgMODE,        0,                      0},
	{DlgMODEBOX,     L"Mode",                &Opt.Mode},

	{DlgCMPCASE,     L"CmpCase",             &Opt.CmpCase},
	{DlgCMPSIZE,     L"CmpSize",             &Opt.CmpSize},
	{DlgCMPTIME,     L"CmpTime",             &Opt.CmpTime},
	{DlgSECONDS,     L"Seconds",             &Opt.Seconds},
	{DlgIGNORESEC,   0,                      0},
	{DlgPRECISION,   L"LowPrecisionTime",    &Opt.LowPrecisionTime},
	{DlgTIMEZONE,    L"IgnoreTimeZone",      &Opt.IgnoreTimeZone},
	{DlgCMPCONTENTS, L"CmpContents",         &Opt.CmpContents},
	{DlgDIFFTIME,    L"OnlyTimeDiff",        &Opt.OnlyTimeDiff},
	{DlgCACHE,       L"Cache",               &Opt.Cache},
	{DlgCACHEIGNORE, L"CacheIgnore",         &Opt.CacheIgnore},
	{DlgCACHEUSE,    0,                      0},
	{DlgCACHECLEAR,  0,                      0},
	{DlgPARTLY,      L"Partly",              &Opt.Partly},
	{DlgPARTLYFULL,  L"PartlyFull",          &Opt.PartlyFull},
	{DlgLPARTLYKB,   0,                      0},
	{DlgEPARTLYKB,   L"PartlyKbSize",        &Opt.PartlyKbSize},
	{DlgIGNORE,      L"Ignore",              &Opt.Ignore},
	{DlgIGNORETEMPL, L"IgnoreTemplates",     &Opt.IgnoreTemplates},

	{DlgDUPPATH,     0,                      0},
	{DlgDUPNAME,     L"DupName",             &Opt.DupName},
	{DlgDUPSIZE,     L"DupSize",             &Opt.DupSize},
	{DlgDUPCONTENTS, L"DupContents",         &Opt.DupContents},
	{DlgDUPPIC,      L"DupPic",              &Opt.DupPic},
	{DlgDUPPICDIFF,  L"DupPicDiff",          &Opt.DupPicDiff},
	{DlgDUPPICSIZE,  L"DupPicSize",          &Opt.DupPicSize},
	{DlgDUPPICFMT,   L"DupPicFmt",           &Opt.DupPicFmt},
	{DlgDUPMUSIC,    L"DupMusic",            &Opt.DupMusic},
	{DlgDUPMUSICARTIST,L"DupMusicArtist",    &Opt.DupMusicArtist},
	{DlgDUPMUSICTITLE,L"DupMusicTitle",      &Opt.DupMusicTitle},
	{DlgDUPMUSICDURL,L"DupMusicDuration",    &Opt.DupMusicDuration},
	{DlgDUPMUSICDURE,L"DupMusicDurationSec", &Opt.DupMusicDurationSec},

	{DlgSEP1,        0,                      0},
	{DlgSUBFOLDER,   L"Subfolders",          &Opt.Subfolders},
	{DlgLMAXDEPTH,   0,                      0},
	{DlgEMAXDEPTH,   L"MaxScanDepth",        &Opt.MaxScanDepth},
	{DlgSCANSYMLINK, 0,                      0},
	{DlgLCMPSKIP,    L"SkipSubstr",          &Opt.SkipSubstr},
	{DlgECMPSKIP,    0,                      0},
	{DlgSTOPDIFFDUP, L"StopDiffDup",         &Opt.StopDiffDup},
	{DlgFILTER,      L"Filter",              &Opt.Filter},
	{DlgFILTERBOTTON,0,                      0},
	{DlgSELECTED,    L"ProcessSelected",     &Opt.ProcessSelected},
	{DlgSELECTEDNEW, L"SelectedNew",         &Opt.SelectedNew},
	{DlgONLYRIGHT,   L"SyncOnlyRight",       &Opt.SyncOnlyRight},
	{DlgIGNORMISSING,L"IgnoreMissing",       &Opt.IgnoreMissing},
	{DlgLIGHTSYNC,   L"LightSync",           &Opt.LightSync},
	{DlgSHOWMSG,     L"ShowMsg",             &Opt.ShowMsg},
	{DlgSOUND,       L"Sound",               &Opt.Sound},
	{DlgDIALOG,      L"Dialog",              &Opt.Dialog},
	{DlgTOTALPROCESS,L"TotalProgress",       &Opt.TotalProgress}
};


/****************************************************************************
 * ���������� ������� ��� ShowOptDialog
 ****************************************************************************/

intptr_t WINAPI AdvCmpDlgOpt::ShowOptDialogProcThunk(HANDLE hDlg, intptr_t Msg, intptr_t Param1, void *Param2)
{
	AdvCmpDlgOpt* Class=(AdvCmpDlgOpt*)Info.SendDlgMessage(hDlg,DM_GETDLGDATA,0,0);
	return Class->ShowOptDialogProc(hDlg,Msg,Param1,Param2);
}

void AdvCmpDlgOpt::Close()
{
	bSkipSaveOpt=false;
	// ���������� �� ������� �����
	if (Opt.Substr) { free(Opt.Substr); Opt.Substr=NULL; }
	if (Opt.WinMergePath) { free(Opt.WinMergePath); Opt.WinMergePath=NULL; }
	if (Opt.DupPath) { free(Opt.DupPath); Opt.DupPath=NULL; }
	Info.FileFilterControl(Opt.hCustomFilter,FFCTL_FREEFILEFILTER,0,0);
}

intptr_t WINAPI AdvCmpDlgOpt::ShowOptDialogProc(HANDLE hDlg, intptr_t Msg, intptr_t Param1, void *Param2)
{

	switch (Msg)
	{
		case DN_INITDIALOG:
			{
				bool CheckSelect=false;
				if (LPanel.PInfo.SelectedItemsNumber)
				{
					FarGetPluginPanelItem FGPPI={sizeof(FarGetPluginPanelItem),0,0};
					FGPPI.Item=(PluginPanelItem*)malloc(FGPPI.Size=Info.PanelControl(LPanel.hPanel,FCTL_GETSELECTEDPANELITEM,0,&FGPPI));
					if (FGPPI.Item)
					{
						Info.PanelControl(LPanel.hPanel,FCTL_GETSELECTEDPANELITEM,0,&FGPPI);
						if (FGPPI.Item->Flags&PPIF_SELECTED) CheckSelect=true;
						free(FGPPI.Item);
					}
				}
				if (!CheckSelect && RPanel.PInfo.SelectedItemsNumber)
				{
					FarGetPluginPanelItem FGPPI={sizeof(FarGetPluginPanelItem),0,0};
					FGPPI.Item=(PluginPanelItem*)malloc(FGPPI.Size=Info.PanelControl(RPanel.hPanel,FCTL_GETSELECTEDPANELITEM,0,&FGPPI));
					if (FGPPI.Item)
					{
						Info.PanelControl(RPanel.hPanel,FCTL_GETSELECTEDPANELITEM,0,&FGPPI);
						if (FGPPI.Item->Flags&PPIF_SELECTED) CheckSelect=true;
						free(FGPPI.Item);
					}
				}
				if (!CheckSelect)
				{
					Info.SendDlgMessage(hDlg,DM_SETCHECK,DlgSELECTED,(void*)BSTATE_UNCHECKED);
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgSELECTED,(void*)false);
				}
				//-------------
				if ((LPanel.PInfo.Flags&PFLAGS_PLUGIN) || (RPanel.PInfo.Flags&PFLAGS_PLUGIN))
				{
					Opt.Mode=MODE_CMP;
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgMODEBOX,(void*)false);
				}
				//-------------
				if (Opt.Mode==MODE_CMP)
				{
					for(int i=DlgDUPPATH; i<=DlgDUPMUSICDURE; i++)
						Info.SendDlgMessage(hDlg,DM_SHOWITEM,i,0);

					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgONLYRIGHT,(void*)false);
					Info.SendDlgMessage(hDlg,DM_SETCHECK,DlgONLYRIGHT,(void*)BSTATE_UNCHECKED);
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgLIGHTSYNC,(void*)false);
					Info.SendDlgMessage(hDlg,DM_SETCHECK,DlgLIGHTSYNC,(void*)BSTATE_UNCHECKED);
				}
				else
				{
					if (Opt.Mode==MODE_SYNC)
					{
						for(int i=DlgDUPPATH; i<=DlgDUPMUSICDURE; i++)
							Info.SendDlgMessage(hDlg,DM_SHOWITEM,i,0);
						Info.SendDlgMessage(hDlg,DM_ENABLE,DlgSTOPDIFFDUP,(void*)false);
						Info.SendDlgMessage(hDlg,DM_SETCHECK,DlgSTOPDIFFDUP,(void*)BSTATE_UNCHECKED);
					}
					else if (Opt.Mode==MODE_DUP) // ���������
					{
						for(int i=DlgCMPCASE; i<=DlgIGNORETEMPL; i++)
							Info.SendDlgMessage(hDlg,DM_SHOWITEM,i,0);

						Info.SendDlgMessage(hDlg,DM_ENABLE,DlgONLYRIGHT,(void*)false);
						Info.SendDlgMessage(hDlg,DM_SETCHECK,DlgONLYRIGHT,(void*)BSTATE_UNCHECKED);
						Info.SendDlgMessage(hDlg,DM_ENABLE,DlgIGNORMISSING,(void*)false);
						Info.SendDlgMessage(hDlg,DM_SETCHECK,DlgIGNORMISSING,(void*)BSTATE_UNCHECKED);
						Info.SendDlgMessage(hDlg,DM_ENABLE,DlgLIGHTSYNC,(void*)false);
						Info.SendDlgMessage(hDlg,DM_SETCHECK,DlgLIGHTSYNC,(void*)BSTATE_UNCHECKED);
					}
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgSCANSYMLINK,(void*)false);
					Info.SendDlgMessage(hDlg,DM_SETCHECK,DlgSCANSYMLINK,(void*)BSTATE_UNCHECKED);
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgLCMPSKIP,(void*)false);
					Info.SendDlgMessage(hDlg,DM_SETCHECK,DlgLCMPSKIP,(void*)BSTATE_UNCHECKED);
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgECMPSKIP,(void*)false);
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgSELECTEDNEW,(void*)false);
					Info.SendDlgMessage(hDlg,DM_SETCHECK,DlgSELECTEDNEW,(void*)BSTATE_UNCHECKED);
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgDIALOG,(void*)false);
					Info.SendDlgMessage(hDlg,DM_SETCHECK,DlgDIALOG,(void*)BSTATE_CHECKED);
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgUNDERCURSOR,(void*)false);
				}
				//------------������� �����! ���� �� ������� ����� ������
				if (Opt.LowPrecisionTime)
					Info.SendDlgMessage(hDlg,DM_SETCHECK,DlgPRECISION,(void*)BSTATE_CHECKED);
				else
					Info.SendDlgMessage(hDlg,DM_SETCHECK,DlgIGNORESEC,(void*)BSTATE_CHECKED);
				if (!Opt.Seconds)
				{
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgIGNORESEC,(void*)false);
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgPRECISION,(void*)false);
				}
				if (!Opt.CmpTime)
				{
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgSECONDS,(void*)false);
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgIGNORESEC,(void*)false);
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgPRECISION,(void*)false);
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgTIMEZONE,(void*)false);
				}

				//--------
				if ( !( ((LPanel.PInfo.Flags&PFLAGS_REALNAMES) && (RPanel.PInfo.Flags&PFLAGS_REALNAMES)) ||
								((LPanel.PInfo.Flags&PFLAGS_REALNAMES) && RPanel.bARC) ||
								((RPanel.PInfo.Flags&PFLAGS_REALNAMES) && LPanel.bARC) ||
								(LPanel.bARC && RPanel.bARC) ) )
				{
					Opt.CmpContents=0;
					Opt.Cache=0;
					Info.SendDlgMessage(hDlg,DM_SETCHECK,DlgCMPCONTENTS,(void*)BSTATE_UNCHECKED);
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgCMPCONTENTS,(void*)false);
				}
				if (Opt.CmpContents && (LPanel.bARC || RPanel.bARC))
				{
					Opt.Partly=0;
					Opt.Ignore=0;
					Opt.Cache=0;
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgPARTLY,(void*)false);
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgIGNORE,(void*)false);
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgCACHE,(void*)false);
				}
				//------------ ������� �����! ���� �� ������� ����� ������
				if (Opt.PartlyFull)
				{
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgEPARTLYKB,(void*)false);
					Info.SendDlgMessage(hDlg,DM_SETCHECK,DlgPARTLYFULL,(void*)BSTATE_CHECKED);
				}
				else
					Info.SendDlgMessage(hDlg,DM_SETCHECK,DlgLPARTLYKB,(void*)BSTATE_CHECKED);

				if (!Opt.Partly)
				{
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgPARTLYFULL,(void*)false);
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgLPARTLYKB,(void*)false);
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgEPARTLYKB,(void*)false);
				}
				if (!Opt.Ignore)
				{
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgIGNORETEMPL,(void*)false);
				}
				//------------������� �����! ���� �� ������� ����� ������
				if (Opt.CacheIgnore)
					Info.SendDlgMessage(hDlg,DM_SETCHECK,DlgCACHEIGNORE,(void*)BSTATE_CHECKED);
				else
					Info.SendDlgMessage(hDlg,DM_SETCHECK,DlgCACHEUSE,(void*)BSTATE_CHECKED);

				if (!Opt.Cache)
				{
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgCACHEIGNORE,(void*)false);
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgCACHEUSE,(void*)false);
				}

				if (!Opt.CmpContents)
				{
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgDIFFTIME,(void*)false);
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgPARTLY,(void*)false);
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgPARTLYFULL,(void*)false);
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgLPARTLYKB,(void*)false);
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgEPARTLYKB,(void*)false);
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgIGNORE,(void*)false);
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgIGNORETEMPL,(void*)false);
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgCACHE,(void*)false);
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgCACHEIGNORE,(void*)false);
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgCACHEUSE,(void*)false);
				}
				//-----------
				if (!Cache.ItemsNumber)
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgCACHECLEAR,(void*)false);
				//------------
				if (LPanel.bTMP || RPanel.bTMP || (!LPanel.bDir && !RPanel.bDir))
				{
					Opt.Subfolders=0;
					Info.SendDlgMessage(hDlg,DM_SETCHECK,DlgSUBFOLDER,(void*)BSTATE_UNCHECKED);
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgSUBFOLDER,(void*)false);
				}
				if (Opt.Subfolders!=2)
				{
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgLMAXDEPTH,(void*)false);
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgEMAXDEPTH,(void*)false);
				}
				//------
				if (!Opt.DupPic || !bGflLoaded)
				{
					if (!bGflLoaded)
					{
						Info.SendDlgMessage(hDlg,DM_SETCHECK,DlgDUPPIC,(void*)BSTATE_UNCHECKED);
						Info.SendDlgMessage(hDlg,DM_ENABLE,DlgDUPPIC,(void*)false);
					}
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgDUPPICDIFF,(void*)false);
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgDUPPICSIZE,(void*)false);
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgDUPPICFMT,(void*)false);
				}
				if (!Opt.DupMusicDuration)
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgDUPMUSICDURE,(void*)false);
				if (!Opt.DupMusic || !bBASSLoaded)
				{
					if (!bBASSLoaded)
					{
						Info.SendDlgMessage(hDlg,DM_SETCHECK,DlgDUPMUSIC,(void*)BSTATE_UNCHECKED);
						Info.SendDlgMessage(hDlg,DM_ENABLE,DlgDUPMUSIC,(void*)false);
					}
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgDUPMUSICARTIST,(void*)false);
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgDUPMUSICTITLE,(void*)false);
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgDUPMUSICDURL,(void*)false);
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgDUPMUSICDURE,(void*)false);
				}
				if (!Opt.DupContents)
				{
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgDUPPIC,(void*)false);
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgDUPPICDIFF,(void*)false);
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgDUPPICSIZE,(void*)false);
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgDUPPICFMT,(void*)false);
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgDUPMUSIC,(void*)false);
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgDUPMUSICARTIST,(void*)false);
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgDUPMUSICTITLE,(void*)false);
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgDUPMUSICDURL,(void*)false);
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgDUPMUSICDURE,(void*)false);
				}
				//------
				Opt.ScanSymlink=GetFarSetting(FSSF_SYSTEM,L"ScanJunction")?true:false;
				Info.SendDlgMessage(hDlg,DM_SETCHECK,DlgSCANSYMLINK,(void*)(Opt.ScanSymlink?BSTATE_CHECKED:BSTATE_UNCHECKED));
				if (!Opt.Subfolders || Opt.Mode!=MODE_CMP || (LPanel.PInfo.Flags&PFLAGS_PLUGIN) || (RPanel.PInfo.Flags&PFLAGS_PLUGIN))
				{
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgSCANSYMLINK,(void*)false);
					Info.SendDlgMessage(hDlg,DM_SETCHECK,DlgSCANSYMLINK,(void*)BSTATE_UNCHECKED);
				}
				//------------
				if (!Opt.Filter)
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgFILTERBOTTON,(void*)false);
				//------------
				if (!Opt.SkipSubstr)
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgECMPSKIP,(void*)false);
				//------------
				if (LPanel.bTMP || RPanel.bTMP)
				{
					Info.SendDlgMessage(hDlg,DM_SETCHECK,DlgDIALOG,(void*)BSTATE_UNCHECKED);
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgDIALOG,(void*)false);
				}
				//------------
				if (!LPanel.bCurFile || !RPanel.bCurFile || Opt.Mode!=MODE_CMP)
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgUNDERCURSOR,(void*)false);

				// ��������� ��������� �����...
				Opt.ProcessHidden=GetFarSetting(FSSF_PANEL,L"ShowHidden")?true:false;
				Opt.hCustomFilter=INVALID_HANDLE_VALUE;

				bSkipSaveOpt=false;

				return true;
			}

	/************************************************************************/

		case DN_LISTHOTKEY:  // ��� ������� ���
		case DN_LISTCHANGE:  //  ���� � ����
			if (Param1 == DlgMODEBOX)
			{
				if (Param2 == (void*)MODE_DUP)
				{
					for(int i=DlgCMPCASE; i<=DlgIGNORETEMPL; i++)
						Info.SendDlgMessage(hDlg,DM_SHOWITEM,i,0);
					for(int i=DlgDUPPATH; i<=DlgDUPMUSICDURE; i++)
						Info.SendDlgMessage(hDlg,DM_SHOWITEM,i,(void*)1);

					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgSCANSYMLINK,(void*)false);
					Info.SendDlgMessage(hDlg,DM_SETCHECK,DlgSCANSYMLINK,(void*)BSTATE_UNCHECKED);
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgLCMPSKIP,(void*)false);
					Info.SendDlgMessage(hDlg,DM_SETCHECK,DlgLCMPSKIP,(void*)BSTATE_UNCHECKED);
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgECMPSKIP,(void*)false);
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgSTOPDIFFDUP,(void*)true);
					Info.SendDlgMessage(hDlg,DM_SETCHECK,DlgSTOPDIFFDUP,(void*)(Opt.StopDiffDup?BSTATE_CHECKED:BSTATE_UNCHECKED));
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgSELECTEDNEW,(void*)false);
					Info.SendDlgMessage(hDlg,DM_SETCHECK,DlgSELECTEDNEW,(void*)BSTATE_UNCHECKED);
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgIGNORMISSING,(void*)false);
					Info.SendDlgMessage(hDlg,DM_SETCHECK,DlgIGNORMISSING,(void*)BSTATE_UNCHECKED);
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgDIALOG,(void*)false);
					Info.SendDlgMessage(hDlg,DM_SETCHECK,DlgDIALOG,(void*)BSTATE_CHECKED);
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgUNDERCURSOR,(void*)false);
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgONLYRIGHT,(void*)false);
					Info.SendDlgMessage(hDlg,DM_SETCHECK,DlgONLYRIGHT,(void*)BSTATE_UNCHECKED);
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgLIGHTSYNC,(void*)false);
					Info.SendDlgMessage(hDlg,DM_SETCHECK,DlgLIGHTSYNC,(void*)BSTATE_UNCHECKED);
				}
				else
				{
					for(int i=DlgCMPCASE; i<=DlgIGNORETEMPL; i++)
						Info.SendDlgMessage(hDlg,DM_SHOWITEM,i,(void*)1);
					for(int i=DlgDUPPATH; i<=DlgDUPMUSICDURE; i++)
						Info.SendDlgMessage(hDlg,DM_SHOWITEM,i,0);

					if (Param2 == (void*)MODE_CMP)
					{
						Info.SendDlgMessage(hDlg,DM_ENABLE,DlgSCANSYMLINK,(void*)(Info.SendDlgMessage(hDlg,DM_GETCHECK,DlgSUBFOLDER,0) && !((LPanel.PInfo.Flags&PFLAGS_PLUGIN) || (RPanel.PInfo.Flags&PFLAGS_PLUGIN))) );
						Info.SendDlgMessage(hDlg,DM_SETCHECK,DlgSCANSYMLINK,(void*)(Opt.ScanSymlink && Info.SendDlgMessage(hDlg,DM_GETCHECK,DlgSUBFOLDER,0)?BSTATE_CHECKED:BSTATE_UNCHECKED));
						Info.SendDlgMessage(hDlg,DM_ENABLE,DlgLCMPSKIP,(void*)true);
						Info.SendDlgMessage(hDlg,DM_SETCHECK,DlgLCMPSKIP,(void*)(Opt.SkipSubstr?BSTATE_CHECKED:BSTATE_UNCHECKED));
						Info.SendDlgMessage(hDlg,DM_ENABLE,DlgECMPSKIP,(void*)(Opt.SkipSubstr?true:false));
						Info.SendDlgMessage(hDlg,DM_ENABLE,DlgSTOPDIFFDUP,(void*)true);
						Info.SendDlgMessage(hDlg,DM_SETCHECK,DlgSTOPDIFFDUP,(void*)(Opt.StopDiffDup?BSTATE_CHECKED:BSTATE_UNCHECKED));
						Info.SendDlgMessage(hDlg,DM_ENABLE,DlgSELECTEDNEW,(void*)true);
						Info.SendDlgMessage(hDlg,DM_SETCHECK,DlgSELECTEDNEW,(void*)(Opt.SelectedNew?BSTATE_CHECKED:BSTATE_UNCHECKED));
						Info.SendDlgMessage(hDlg,DM_ENABLE,DlgIGNORMISSING,(void*)true);
						Info.SendDlgMessage(hDlg,DM_SETCHECK,DlgIGNORMISSING,(void*)(Opt.IgnoreMissing==2?BSTATE_3STATE:(Opt.IgnoreMissing?BSTATE_CHECKED:BSTATE_UNCHECKED)));
						Info.SendDlgMessage(hDlg,DM_ENABLE,DlgDIALOG,(void*)true);
						Info.SendDlgMessage(hDlg,DM_SETCHECK,DlgDIALOG,(void*)(Opt.Dialog?BSTATE_CHECKED:BSTATE_UNCHECKED));
						Info.SendDlgMessage(hDlg,DM_ENABLE,DlgUNDERCURSOR,(void*)(!LPanel.bCurFile || !RPanel.bCurFile?false:true));
						Info.SendDlgMessage(hDlg,DM_ENABLE,DlgONLYRIGHT,(void*)false);
						Info.SendDlgMessage(hDlg,DM_SETCHECK,DlgONLYRIGHT,(void*)BSTATE_UNCHECKED);
						Info.SendDlgMessage(hDlg,DM_ENABLE,DlgLIGHTSYNC,(void*)false);
						Info.SendDlgMessage(hDlg,DM_SETCHECK,DlgLIGHTSYNC,(void*)BSTATE_UNCHECKED);
					}
					else
					{
						Info.SendDlgMessage(hDlg,DM_ENABLE,DlgSCANSYMLINK,(void*)false);
						Info.SendDlgMessage(hDlg,DM_SETCHECK,DlgSCANSYMLINK,(void*)BSTATE_UNCHECKED);
						Info.SendDlgMessage(hDlg,DM_ENABLE,DlgLCMPSKIP,(void*)false);
						Info.SendDlgMessage(hDlg,DM_SETCHECK,DlgLCMPSKIP,(void*)BSTATE_UNCHECKED);
						Info.SendDlgMessage(hDlg,DM_ENABLE,DlgECMPSKIP,(void*)false);
						Info.SendDlgMessage(hDlg,DM_ENABLE,DlgSTOPDIFFDUP,(void*)false);
						Info.SendDlgMessage(hDlg,DM_SETCHECK,DlgSTOPDIFFDUP,(void*)BSTATE_UNCHECKED);
						Info.SendDlgMessage(hDlg,DM_ENABLE,DlgSELECTEDNEW,(void*)false);
						Info.SendDlgMessage(hDlg,DM_SETCHECK,DlgSELECTEDNEW,(void*)BSTATE_UNCHECKED);
						Info.SendDlgMessage(hDlg,DM_ENABLE,DlgIGNORMISSING,(void*)true);
						Info.SendDlgMessage(hDlg,DM_SETCHECK,DlgIGNORMISSING,(void*)(Opt.IgnoreMissing==2?BSTATE_3STATE:(Opt.IgnoreMissing?BSTATE_CHECKED:BSTATE_UNCHECKED)));
						Info.SendDlgMessage(hDlg,DM_ENABLE,DlgDIALOG,(void*)false);
						Info.SendDlgMessage(hDlg,DM_SETCHECK,DlgDIALOG,(void*)BSTATE_CHECKED);
						Info.SendDlgMessage(hDlg,DM_ENABLE,DlgUNDERCURSOR,(void*)false);
						Info.SendDlgMessage(hDlg,DM_ENABLE,DlgONLYRIGHT,(void*)true);
						Info.SendDlgMessage(hDlg,DM_SETCHECK,DlgONLYRIGHT,(void*)(Opt.SyncOnlyRight==2?BSTATE_3STATE:(Opt.SyncOnlyRight?BSTATE_CHECKED:BSTATE_UNCHECKED)));
						Info.SendDlgMessage(hDlg,DM_ENABLE,DlgLIGHTSYNC,(void*)true);
					Info.SendDlgMessage(hDlg,DM_SETCHECK,DlgLIGHTSYNC,(void*)(Opt.LightSync?BSTATE_CHECKED:BSTATE_UNCHECKED));
					}
				}
				return Msg==DN_LISTHOTKEY?false:true;    // ��� ������� ������� �������� - ������� ������
			}
			break;

	/************************************************************************/

		case DN_BTNCLICK:
			if (Param1 == DlgCMPTIME)
			{
				if (Param2)
				{
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgSECONDS,(void*)true);
					if (Info.SendDlgMessage(hDlg,DM_GETCHECK,DlgSECONDS,0))
					{
						Info.SendDlgMessage(hDlg,DM_ENABLE,DlgIGNORESEC,(void*)true);
						Info.SendDlgMessage(hDlg,DM_ENABLE,DlgPRECISION,(void*)true);
					}
					else
					{
						Info.SendDlgMessage(hDlg,DM_ENABLE,DlgIGNORESEC,(void*)false);
						Info.SendDlgMessage(hDlg,DM_ENABLE,DlgPRECISION,(void*)false);
					}
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgTIMEZONE,(void*)true);
				}
				else
				{
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgSECONDS,(void*)false);
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgIGNORESEC,(void*)false);
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgPRECISION,(void*)false);
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgTIMEZONE,(void*)false);
				}
			}
			//------------
			else if (Param1 == DlgSECONDS)
			{
				if (Param2)
				{
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgIGNORESEC,(void*)true);
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgPRECISION,(void*)true);
				}
				else
				{
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgIGNORESEC,(void*)false);
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgPRECISION,(void*)false);
				}
			}
			//-----------
			else if (Param1 == DlgCMPCONTENTS)
			{
				if (Param2)
				{
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgDIFFTIME,(void*)true);

					if (!(LPanel.bARC || RPanel.bARC))
						Info.SendDlgMessage(hDlg,DM_ENABLE,DlgPARTLY,(void*)true);
					if (Info.SendDlgMessage(hDlg,DM_GETCHECK,DlgPARTLY,0))
					{
						Info.SendDlgMessage(hDlg,DM_ENABLE,DlgPARTLYFULL,(void*)true);
						Info.SendDlgMessage(hDlg,DM_ENABLE,DlgLPARTLYKB,(void*)true);
						if (Info.SendDlgMessage(hDlg,DM_GETCHECK,DlgLPARTLYKB,0))
							Info.SendDlgMessage(hDlg,DM_ENABLE,DlgEPARTLYKB,(void*)true);
						else
							Info.SendDlgMessage(hDlg,DM_ENABLE,DlgEPARTLYKB,(void*)false);
					}
					else
					{
						Info.SendDlgMessage(hDlg,DM_ENABLE,DlgPARTLYFULL,(void*)false);
						Info.SendDlgMessage(hDlg,DM_ENABLE,DlgLPARTLYKB,(void*)false);
						Info.SendDlgMessage(hDlg,DM_ENABLE,DlgEPARTLYKB,(void*)false);
					}

					if (!(LPanel.bARC || RPanel.bARC))
						Info.SendDlgMessage(hDlg,DM_ENABLE,DlgIGNORE,(void*)true);
					if (Info.SendDlgMessage(hDlg,DM_GETCHECK,DlgIGNORE,0))
						Info.SendDlgMessage(hDlg,DM_ENABLE,DlgIGNORETEMPL,(void*)true);
					else
						Info.SendDlgMessage(hDlg,DM_ENABLE,DlgIGNORETEMPL,(void*)false);

					if (!(LPanel.bARC || RPanel.bARC))
						Info.SendDlgMessage(hDlg,DM_ENABLE,DlgCACHE,(void*)true);
					if (Info.SendDlgMessage(hDlg,DM_GETCHECK,DlgCACHE,0))
					{
						Info.SendDlgMessage(hDlg,DM_ENABLE,DlgCACHEIGNORE,(void*)true);
						Info.SendDlgMessage(hDlg,DM_ENABLE,DlgCACHEUSE,(void*)true);
					}
					else
					{
						Info.SendDlgMessage(hDlg,DM_ENABLE,DlgCACHEIGNORE,(void*)false);
						Info.SendDlgMessage(hDlg,DM_ENABLE,DlgCACHEUSE,(void*)false);
					}
				}
				else
				{
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgDIFFTIME,(void*)false);
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgCACHE,(void*)false);
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgCACHEIGNORE,(void*)false);
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgCACHEUSE,(void*)false);
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgPARTLY,(void*)false);
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgPARTLYFULL,(void*)false);
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgLPARTLYKB,(void*)false);
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgEPARTLYKB,(void*)false);
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgIGNORE,(void*)false);
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgIGNORETEMPL,(void*)false);
				}
			}
			//------------
			else if (Param1 == DlgCACHE)
			{
				if (Param2)
				{
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgCACHEIGNORE,(void*)true);
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgCACHEUSE,(void*)true);
				}
				else
				{
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgCACHEIGNORE,(void*)false);
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgCACHEUSE,(void*)false);
				}
			}
			//------------
			else if (Param1 == DlgPARTLY)
			{
				if (Param2)
				{
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgPARTLYFULL,(void*)true);
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgLPARTLYKB,(void*)true);
					if (Info.SendDlgMessage(hDlg,DM_GETCHECK,DlgLPARTLYKB,0))
						Info.SendDlgMessage(hDlg,DM_ENABLE,DlgEPARTLYKB,(void*)true);
					else
						Info.SendDlgMessage(hDlg,DM_ENABLE,DlgEPARTLYKB,(void*)false);
				}
				else
				{
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgPARTLYFULL,(void*)false);
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgLPARTLYKB,(void*)false);
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgEPARTLYKB,(void*)false);
				}
			}
			//------------
			else if (Param1 == DlgLPARTLYKB)
			{
				if (Param2)
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgEPARTLYKB,(void*)true);
				else
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgEPARTLYKB,(void*)false);
			}
			//------------
			else if (Param1 == DlgIGNORE)
			{
				if (Param2)
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgIGNORETEMPL,(void*)true);
				else
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgIGNORETEMPL,(void*)false);
			}
			//------------
			else if (Param1 == DlgSUBFOLDER)
			{
				if ((int)Param2 == MODE_SYNC)
				{
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgLMAXDEPTH,(void*)false);
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgEMAXDEPTH,(void*)false);
					if (!((LPanel.PInfo.Flags&PFLAGS_PLUGIN) || (RPanel.PInfo.Flags&PFLAGS_PLUGIN)) && !Info.SendDlgMessage(hDlg,DM_LISTGETCURPOS,DlgMODEBOX,0))
					{
						Info.SendDlgMessage(hDlg,DM_ENABLE,DlgSCANSYMLINK,(void*)true);
						Info.SendDlgMessage(hDlg,DM_SETCHECK,DlgSCANSYMLINK,(void*)(Opt.ScanSymlink?BSTATE_CHECKED:BSTATE_UNCHECKED));
					}
					else
					{
						Info.SendDlgMessage(hDlg,DM_ENABLE,DlgSCANSYMLINK,(void*)false);
						Info.SendDlgMessage(hDlg,DM_SETCHECK,DlgSCANSYMLINK,(void*)BSTATE_UNCHECKED);
					}
				}
				else if ((int)Param2 == MODE_DUP)
				{
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgLMAXDEPTH,(void*)true);
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgEMAXDEPTH,(void*)true);
					if (!((LPanel.PInfo.Flags&PFLAGS_PLUGIN) || (RPanel.PInfo.Flags&PFLAGS_PLUGIN)) && !Info.SendDlgMessage(hDlg,DM_LISTGETCURPOS,DlgMODEBOX,0))
					{
						Info.SendDlgMessage(hDlg,DM_ENABLE,DlgSCANSYMLINK,(void*)true);
						Info.SendDlgMessage(hDlg,DM_SETCHECK,DlgSCANSYMLINK,(void*)(Opt.ScanSymlink?BSTATE_CHECKED:BSTATE_UNCHECKED));
					}
					else
					{
						Info.SendDlgMessage(hDlg,DM_ENABLE,DlgSCANSYMLINK,(void*)false);
						Info.SendDlgMessage(hDlg,DM_SETCHECK,DlgSCANSYMLINK,(void*)BSTATE_UNCHECKED);
					}
				}
				else
				{
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgLMAXDEPTH,(void*)false);
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgEMAXDEPTH,(void*)false);
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgSCANSYMLINK,(void*)false);
					Info.SendDlgMessage(hDlg,DM_SETCHECK,DlgSCANSYMLINK,(void*)BSTATE_UNCHECKED);
				}
			}
			//------------
			else if (Param1 == DlgFILTER)
			{
				if (Param2)
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgFILTERBOTTON,(void*)true);
				else
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgFILTERBOTTON,(void*)false);
			}
			//------------
			else if (Param1 == DlgLCMPSKIP)
			{
				if (Param2)
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgECMPSKIP,(void*)true);
				else
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgECMPSKIP,(void*)false);
			}
			else if (Param1 == DlgDUPCONTENTS)
			{
				if (Param2)
				{
					if (bGflLoaded)
						Info.SendDlgMessage(hDlg,DM_ENABLE,DlgDUPPIC,(void*)true);
					if (bGflLoaded && Info.SendDlgMessage(hDlg,DM_GETCHECK,DlgDUPPIC,0))
					{
						Info.SendDlgMessage(hDlg,DM_ENABLE,DlgDUPPICDIFF,(void*)true);
						Info.SendDlgMessage(hDlg,DM_ENABLE,DlgDUPPICSIZE,(void*)true);
						Info.SendDlgMessage(hDlg,DM_ENABLE,DlgDUPPICFMT,(void*)true);
					}
					else
					{
						Info.SendDlgMessage(hDlg,DM_ENABLE,DlgDUPPICDIFF,(void*)false);
						Info.SendDlgMessage(hDlg,DM_ENABLE,DlgDUPPICSIZE,(void*)false);
						Info.SendDlgMessage(hDlg,DM_ENABLE,DlgDUPPICFMT,(void*)false);
					}

					if (bBASSLoaded)
						Info.SendDlgMessage(hDlg,DM_ENABLE,DlgDUPMUSIC,(void*)true);
					if (bBASSLoaded && Info.SendDlgMessage(hDlg,DM_GETCHECK,DlgDUPMUSIC,0))
					{
						Info.SendDlgMessage(hDlg,DM_ENABLE,DlgDUPMUSICARTIST,(void*)true);
						Info.SendDlgMessage(hDlg,DM_ENABLE,DlgDUPMUSICTITLE,(void*)true);
						Info.SendDlgMessage(hDlg,DM_ENABLE,DlgDUPMUSICDURL,(void*)true);
						Info.SendDlgMessage(hDlg,DM_ENABLE,DlgDUPMUSICDURE,(void*)true);
					}
					else
					{
						Info.SendDlgMessage(hDlg,DM_ENABLE,DlgDUPMUSICARTIST,(void*)false);
						Info.SendDlgMessage(hDlg,DM_ENABLE,DlgDUPMUSICTITLE,(void*)false);
						Info.SendDlgMessage(hDlg,DM_ENABLE,DlgDUPMUSICDURL,(void*)false);
						Info.SendDlgMessage(hDlg,DM_ENABLE,DlgDUPMUSICDURE,(void*)false);
					}
				}
				else
				{
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgDUPPIC,(void*)false);
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgDUPPICDIFF,(void*)false);
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgDUPPICSIZE,(void*)false);
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgDUPPICFMT,(void*)false);
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgDUPMUSIC,(void*)false);
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgDUPMUSICARTIST,(void*)false);
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgDUPMUSICTITLE,(void*)false);
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgDUPMUSICDURL,(void*)false);
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgDUPMUSICDURE,(void*)false);
				}
			}
			else if (Param1 == DlgDUPPIC)
			{
				if (Param2)
				{
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgDUPPICDIFF,(void*)true);
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgDUPPICSIZE,(void*)true);
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgDUPPICFMT,(void*)true);
				}
				else
				{
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgDUPPICDIFF,(void*)false);
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgDUPPICSIZE,(void*)false);
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgDUPPICFMT,(void*)false);
				}
			}
			else if (Param1 == DlgDUPMUSIC)
			{
				if (Param2)
				{
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgDUPMUSICARTIST,(void*)true);
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgDUPMUSICTITLE,(void*)true);
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgDUPMUSICDURL,(void*)true);
					if (Info.SendDlgMessage(hDlg,DM_GETCHECK,DlgDUPMUSICDURL,0))
						Info.SendDlgMessage(hDlg,DM_ENABLE,DlgDUPMUSICDURE,(void*)true);
				}
				else
				{
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgDUPMUSICARTIST,(void*)false);
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgDUPMUSICTITLE,(void*)false);
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgDUPMUSICDURL,(void*)false);
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgDUPMUSICDURE,(void*)false);
				}
			}
			else if (Param1 == DlgDUPMUSICDURL)
			{
				if (Param2)
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgDUPMUSICDURE,(void*)true);
				else
					Info.SendDlgMessage(hDlg,DM_ENABLE,DlgDUPMUSICDURE,(void*)false);
			}
			break;


	/************************************************************************/

		case DN_CONTROLINPUT:
		{
			const INPUT_RECORD* record=(const INPUT_RECORD *)Param2;

			if (record->EventType==KEY_EVENT && record->Event.KeyEvent.bKeyDown)
			{
				WORD vk=record->Event.KeyEvent.wVirtualKeyCode;
				if (IsNone(record))
				{
					if (vk == VK_F4 && Info.SendDlgMessage(hDlg,DM_GETCHECK,DlgFILTER,0))
						Info.SendDlgMessage(hDlg,DM_CLOSE,DlgFILTERBOTTON,0);
					else if (Opt.Mode!=MODE_DUP && vk == VK_F8 && Info.SendDlgMessage(hDlg,DM_ENABLE,DlgCACHECLEAR,(void*)-1))
						Info.SendDlgMessage(hDlg,DM_CLOSE,DlgCACHECLEAR,0);
					else if (vk == VK_F2 && Info.SendDlgMessage(hDlg,DM_ENABLE,DlgUNDERCURSOR,(void*)-1))
						Info.SendDlgMessage(hDlg,DM_CLOSE,DlgUNDERCURSOR,0);
				}
				else if (IsCtrl(record) && vk==VK_RETURN)
				{
					bSkipSaveOpt=true;
					Info.SendDlgMessage(hDlg,DM_CLOSE,DlgOK,0);
				}
			}
		}
		break;

	/************************************************************************/

		case DN_CLOSE:
			if (Param1 == DlgFILTERBOTTON)
			{
				if (Opt.hCustomFilter == INVALID_HANDLE_VALUE)
					Opt.Filter=Info.FileFilterControl(PANEL_NONE,FFCTL_CREATEFILEFILTER,FFT_CUSTOM,&Opt.hCustomFilter);
				if (Opt.hCustomFilter != INVALID_HANDLE_VALUE)
					Info.FileFilterControl(Opt.hCustomFilter,FFCTL_OPENFILTERSMENU,0,0);
				return false;
			}
			else if (Param1 == DlgCACHECLEAR)
			{
				if (Cache.RCI)
				{
					if (GetFarSetting(FSSF_CONFIRMATIONS,L"Delete"))
					{
						wchar_t buf[100]; FSF.sprintf(buf,GetMsg(MClearCacheItems),Cache.ItemsNumber);
						const wchar_t *MsgItems[]={ GetMsg(MClearCacheTitle),buf,GetMsg(MClearCacheBody) };
						if (!Info.Message(&MainGuid,&ClearCacheMsgGuid,FMSG_WARNING|FMSG_MB_YESNO,0,MsgItems,3,0))
							goto ClearCache;
					}
					else
					ClearCache:
					{
						free(Cache.RCI);
						Cache.RCI=0;
						Cache.ItemsNumber=0;
						Info.SendDlgMessage(hDlg,DM_ENABLE,DlgCACHECLEAR,(void*)false);
					}
				}
				return false;
			}
			else if (Param1 == DlgUNDERCURSOR)
			{
				for (int i=DlgMODEBOX; i<=DlgIGNORETEMPL; i++)
				{
					if (StoreOpt[i].OptName)
					{
						if (i==DlgEPARTLYKB)
							*StoreOpt[i].Option=FSF.atoi((const wchar_t *)Info.SendDlgMessage(hDlg,DM_GETCONSTTEXTPTR,i,0));
						else if (i==DlgMODEBOX || i==DlgIGNORETEMPL)
							*StoreOpt[i].Option=Info.SendDlgMessage(hDlg,DM_LISTGETCURPOS,i,0);
						else
							*StoreOpt[i].Option=Info.SendDlgMessage(hDlg,DM_GETCHECK,i,0);
					}
				}
				FarSettingsCreate settings={sizeof(FarSettingsCreate),MainGuid,INVALID_HANDLE_VALUE};
				if (Info.SettingsControl(INVALID_HANDLE_VALUE,SCTL_CREATE,0,&settings))
				{
					size_t Root=0; // ������ �����
					for (int i=DlgMODEBOX; i<=DlgIGNORETEMPL; i++)
					{
						if (StoreOpt[i].OptName && Info.SendDlgMessage(hDlg,DM_ENABLE,i,(void*)-1))
						{
							FarSettingsItem item={sizeof(FarSettingsItem),Root,StoreOpt[i].OptName,FST_QWORD};
							item.Number=*StoreOpt[i].Option;
							Info.SettingsControl(settings.Handle,SCTL_SET,0,&item);
						}
					}
					Info.SettingsControl(settings.Handle,SCTL_FREE,0,0);
				}
				return true;
			}
			else if (Param1 == DlgOK)
			{
				for (int i=DlgMODEBOX; i<(sizeof(StoreOpt)/sizeof(StoreOpt[0])); i++)
				{
					if (StoreOpt[i].OptName)
					{
						if (i==DlgEPARTLYKB || i==DlgEMAXDEPTH || i==DlgDUPMUSICDURE)
							*StoreOpt[i].Option=FSF.atoi((const wchar_t *)Info.SendDlgMessage(hDlg,DM_GETCONSTTEXTPTR,i,0));
						else if (i==DlgMODEBOX || i==DlgIGNORETEMPL || i==DlgDUPPICDIFF)
							*StoreOpt[i].Option=Info.SendDlgMessage(hDlg,DM_LISTGETCURPOS,i,0);
						else
							*StoreOpt[i].Option=Info.SendDlgMessage(hDlg,DM_GETCHECK,i,0);
					}
				}

				Opt.ScanSymlink=Info.SendDlgMessage(hDlg,DM_GETCHECK,DlgSCANSYMLINK,0);

				Opt.Substr=NULL;
				int len=Info.SendDlgMessage(hDlg,DM_GETTEXT,DlgECMPSKIP,0);
				if (len)
				{
					Opt.Substr=(wchar_t*)malloc((len+1)*sizeof(wchar_t));
					if (Opt.Substr)
					{
						FarDialogItemData item={sizeof(FarDialogItemData),len,Opt.Substr};
						Info.SendDlgMessage(hDlg,DM_GETTEXT,DlgECMPSKIP,&item);
					}
					else
						Opt.SkipSubstr=0;
				}
				else
				{
					Opt.SkipSubstr=0;
				}

				Opt.DupPath=NULL;
				len=Info.SendDlgMessage(hDlg,DM_GETTEXT,DlgDUPPATH,0);
				if (len)
				{
					Opt.DupPath=(wchar_t*)malloc((len+1)*sizeof(wchar_t));
					if (Opt.DupPath)
					{
						FarDialogItemData item={sizeof(FarDialogItemData),len,Opt.DupPath};
						Info.SendDlgMessage(hDlg,DM_GETTEXT,DlgDUPPATH,&item);
					}
				}

				if (Opt.Filter && Opt.hCustomFilter == INVALID_HANDLE_VALUE)
					Opt.Filter=Info.FileFilterControl(PANEL_NONE,FFCTL_CREATEFILEFILTER,FFT_CUSTOM,&Opt.hCustomFilter);

				if (!bSkipSaveOpt)
				{
					FarSettingsCreate settings={sizeof(FarSettingsCreate),MainGuid,INVALID_HANDLE_VALUE};
					if (Info.SettingsControl(INVALID_HANDLE_VALUE,SCTL_CREATE,0,&settings))
					{
						size_t Root=0; // ������ �����
						for (int i=DlgMODEBOX; i<sizeof(StoreOpt)/sizeof(StoreOpt[0]); i++)
						{
							if (StoreOpt[i].OptName && Info.SendDlgMessage(hDlg,DM_ENABLE,i,(void*)-1))
							{
								FarSettingsItem item={sizeof(FarSettingsItem),Root,StoreOpt[i].OptName,FST_QWORD};
								item.Number=*StoreOpt[i].Option;
								Info.SettingsControl(settings.Handle,SCTL_SET,0,&item);
							}
						}
						Info.SettingsControl(settings.Handle,SCTL_FREE,0,0);
					}
				}

				// ������� ����� ��� Opt.DupContents
				if (Opt.DupContents && (Opt.DupPic==1 || Opt.DupMusic==1))
				{
					if (Opt.DupPic==1 && Opt.DupMusic==2) Opt.DupMusic=1;
					else if (Opt.DupPic==2 && Opt.DupMusic==1) Opt.DupPic=1;
				}
				return true;
			}
			break;

	}
	return Info.DefDlgProc(hDlg, Msg, Param1, Param2);
}

/****************************************************************************
 * ������ ��������� �� �������, ���������� ������ � ������� ���������,
 * ��������� ��������� Opt, ��������� (���� ����) ����� ��������� � �������,
 * ���������� true, ���� ������������ ����� OK
 ****************************************************************************/
int AdvCmpDlgOpt::ShowOptDialog()
{
	const intptr_t dW = 68;   // ������
	const intptr_t dH = 25;   // ������

	struct FarDialogItem DialogItems[] = {
		//			Type	X1	Y1	X2	Y2		Selected	History					Mask															Flags	Data	MaxLen	UserParam
		/* 0*/{DI_DOUBLEBOX,  0, 0,  dW,  dH, 0, 0,                   0,                                0, GetMsg(MCmpTitle),0,0},

		/* 1*/{DI_TEXT,       0, 1,  33,   0, 0, 0,                   0,                                0, GetMsg(MMode),0,0},
		/* 2*/{DI_COMBOBOX,  35, 1,dW-3,   0, 0, 0,                   0,DIF_LISTAUTOHIGHLIGHT|DIF_DROPDOWNLIST|DIF_LISTWRAPMODE|DIF_FOCUS, L"",0,0},
					/// ������ "���������" � "�������������"
		/* 3*/{DI_CHECKBOX,   2, 2,   0,   0, 0, 0,                   0,                                0, GetMsg(MCmpCase),0,0},
		/* 4*/{DI_CHECKBOX,   2, 3,   0,   0, 1, 0,                   0,                                0, GetMsg(MCmpSize),0,0},
		/* 5*/{DI_CHECKBOX,   2, 4,   0,   0, 1, 0,                   0,                                0, GetMsg(MCmpTime),0,0},
		/* 6*/{DI_CHECKBOX,   6, 5,   0,   0, 0, 0,                   0,                                0, GetMsg(MCmpSeconds),0,0},
		/* 7*/{DI_RADIOBUTTON,0, 5,  33,   0, 0, 0,                   0,                        DIF_GROUP, GetMsg(MCmpIgnoreSeconds),0,0},
		/* 8*/{DI_RADIOBUTTON,35,5,   0,   0, 1, 0,                   0,                                0, GetMsg(MCmpLowPrecision),0,0},
		/* 9*/{DI_CHECKBOX,   6, 6,   0,   0, 0, 0,                   0,                                0, GetMsg(MCmpIgnoreTimeZone),0,0},
		/*10*/{DI_CHECKBOX,   2, 7,   0,   0, 0, 0,                   0,                                0, GetMsg(MCmpContents),0,0},
		/*11*/{DI_CHECKBOX,   6, 8,   0,   0, 0, 0,                   0,                                0, GetMsg(MCmpOnlyTimeDiff),0,0},
		/*12*/{DI_CHECKBOX,   6, 9,   0,   0, 0, 0,                   0,                                0, GetMsg(MCmpCache),0,0},
		/*13*/{DI_RADIOBUTTON,0, 9,  33,   0, 0, 0,                   0,                        DIF_GROUP, GetMsg(MCmpCacheIgnore),0,0},
		/*14*/{DI_RADIOBUTTON,35,9,  44,   0, 1, 0,                   0,                                0, GetMsg(MCmpCacheUse),0,0},
		/*15*/{DI_BUTTON,     0, 9,dW-3,   0, 0, 0,                   0,                                0, GetMsg(MCmpCacheClearBotton),0,0},
		/*16*/{DI_CHECKBOX,   6,10,   0,   0, 0, 0,                   0,                                0, GetMsg(MCmpPartly),0,0},
		/*17*/{DI_RADIOBUTTON,0,10,  33,   0, 1, 0,                   0,                        DIF_GROUP, GetMsg(MCmpPartlyFull),0,0},
		/*18*/{DI_RADIOBUTTON,35,10, 52,   0, 0, 0,                   0,                                0, GetMsg(MCmpPartlyKb),0,0},
		/*19*/{DI_FIXEDIT,   54,10,dW-3,   0, 0, 0,       L"##########",                     DIF_MASKEDIT, L"64",0,0},
		/*20*/{DI_CHECKBOX,   6,11,  22,   0, 0, 0,                   0,                                0, GetMsg(MCmpIgnore),0,0},
		/*21*/{DI_COMBOBOX,   0,11,dW-3,   0, 0, 0,                   0,DIF_LISTAUTOHIGHLIGHT|DIF_DROPDOWNLIST|DIF_LISTWRAPMODE, L"",0,0},
					/// ����� "���������"
		/*22*/{DI_EDIT,       2, 2,dW-3,   0, 0, L"AdvCmpDupPath",    0,                      DIF_HISTORY, (LPanel.PInfo.Flags&PFLAGS_FOCUS?LPanel.Dir:RPanel.Dir),0,0},
		/*23*/{DI_CHECKBOX,   2, 3,   0,   0, 1, 0,                   0,                       DIF_3STATE, GetMsg(MDupName),0,0},
		/*24*/{DI_CHECKBOX,   2, 4,   0,   0, 0, 0,                   0,                                0, GetMsg(MDupSize),0,0},
		/*25*/{DI_CHECKBOX,   2, 5,   0,   0, 0, 0,                   0,                                0, GetMsg(MDupContents),0,0},
		/*26*/{DI_CHECKBOX,   6, 6,   0,   0, 0, 0,                   0,                       DIF_3STATE, GetMsg(MDupPic),0,0},
		/*27*/{DI_COMBOBOX,   0, 6,dW-3,   0, 0, 0,                   0,DIF_LISTAUTOHIGHLIGHT|DIF_DROPDOWNLIST|DIF_LISTWRAPMODE, L"",0,0},
		/*28*/{DI_CHECKBOX,  10, 7,   0,   0, 0, 0,                   0,                                0, GetMsg(MDupPicSize),0,0},
		/*29*/{DI_CHECKBOX,  35, 7,   0,   0, 0, 0,                   0,                                0, GetMsg(MDupPicFmt),0,0},
		/*30*/{DI_CHECKBOX,   6, 8,   0,   0, 0, 0,                   0,                       DIF_3STATE, GetMsg(MDupMusic),0,0},
		/*31*/{DI_CHECKBOX,  10, 9,   0,   0, 0, 0,                   0,                       DIF_3STATE, GetMsg(MDupMusicArtist),0,0},
		/*32*/{DI_CHECKBOX,  35, 9,   0,   0, 0, 0,                   0,                       DIF_3STATE, GetMsg(MDupMusicTitle),0,0},
		/*33*/{DI_CHECKBOX,  10,10,   0,   0, 0, 0,                   0,                                0, GetMsg(MDupMusicDuration),0,0},
		/*34*/{DI_FIXEDIT,    0,10,   2,   0, 0, 0,              L"999",                     DIF_MASKEDIT, L"3",0,0},
					/// ������ �����
		/*35*/{DI_TEXT,      -1,12,   0,   0, 0, 0,                   0,                    DIF_SEPARATOR, GetMsg(MTitleOptions),0,0},
		/*36*/{DI_CHECKBOX,   2,13,   0,   0, 0, 0,                   0,                       DIF_3STATE, GetMsg(MSubfolders),0,0},
		/*37*/{DI_TEXT,       0,13,   0,   0, 0, 0,                   0,                                0, GetMsg(MMaxScanDepth),0,0},
		/*38*/{DI_FIXEDIT,    0,13,   2,   0, 0, 0,              L"999",                     DIF_MASKEDIT, L"10",0,0},
		/*39*/{DI_CHECKBOX,   2,14,   0,   0, 0, 0,                   0,                                0, GetMsg(MScanSymlink),0,0},
		/*40*/{DI_CHECKBOX,   2,15,   0,   0, 0, 0,                   0,                                0, GetMsg(MSkipSubstr),0,0},
		/*41*/{DI_EDIT,       0,15,dW-3,   0, 0, L"AdvCmpSkipSubstr", 0,   DIF_USELASTHISTORY|DIF_HISTORY, L"",0,0},
		/*42*/{DI_CHECKBOX,   2,16,  33,   0, 1, 0,                   0,                                0, GetMsg(MStopDiffDup),0,0},
		/*43*/{DI_CHECKBOX,  35,16,dW-9,   0, 0, 0,                   0,                                0, GetMsg(MFilter),0,0},
		/*44*/{DI_BUTTON,  dW-8,16,dW-3,   0, 0, 0,                   0,                                0, GetMsg(MFilterBotton),0,0},
		/*45*/{DI_CHECKBOX,   2,17,  33,   0, 0, 0,                   0,                                0, GetMsg(MProcessSelected),0,0},
		/*46*/{DI_CHECKBOX,  35,17,   0,   0, 0, 0,                   0,                                0, GetMsg(MSelectedNew),0,0},
		/*47*/{DI_CHECKBOX,   2,18,  33,   0, 0, 0,                   0,                       DIF_3STATE, GetMsg(MSyncOnlyRight),0,0},
		/*48*/{DI_CHECKBOX,  35,18,   0,   0, 0, 0,                   0,                       DIF_3STATE, GetMsg(MIgnoreMissing),0,0},
		/*49*/{DI_CHECKBOX,   2,19,   0,   0, 0, 0,                   0,                                0, GetMsg(MLightSync),0,0},
		/*50*/{DI_CHECKBOX,   2,20,   0,   0, 1, 0,                   0,                                0, GetMsg(MShowMsg),0,0},
		/*51*/{DI_CHECKBOX,  35,20,   0,   0, 0, 0,                   0,                                0, GetMsg(MSound),0,0},
		/*52*/{DI_CHECKBOX,   2,21,   0,   0, 0, 0,                   0,                                0, GetMsg(MDialog),0,0},
		/*53*/{DI_CHECKBOX,  35,21,   0,   0, 0, 0,                   0,                                0, GetMsg(MTotalProgress),0,0},

		/*54*/{DI_TEXT,      -1,22,   0,   0, 0, 0,                   0,                    DIF_SEPARATOR, L"",0,0},
		/*55*/{DI_BUTTON,     0,23,   0,   0, 0, 0,                   0,DIF_CENTERGROUP|DIF_DEFAULTBUTTON, GetMsg(MOK),0,0},
		/*56*/{DI_BUTTON,     0,23,   0,   0, 0, 0,                   0,                  DIF_CENTERGROUP, GetMsg(MUnderCursorBotton),0,0},
		/*57*/{DI_BUTTON,     0,23,   0,   0, 0, 0,                   0,                  DIF_CENTERGROUP, GetMsg(MCancel),0,0}
	};

	// ������������ ���������� ��� �����
	DialogItems[DlgMODE].X1 = DialogItems[DlgMODE].X2 - wcslen(DialogItems[DlgMODE].Data) + (wcschr(DialogItems[DlgMODE].Data, L'&')?1:0);
	DialogItems[DlgIGNORESEC].X1 = DialogItems[DlgSECONDS].X1 + wcslen(DialogItems[DlgSECONDS].Data) - (wcschr(DialogItems[DlgSECONDS].Data, L'&')?1:0) + 5;
	DialogItems[DlgECMPSKIP].X1 = DialogItems[DlgLCMPSKIP].X1 + wcslen(DialogItems[DlgLCMPSKIP].Data) - (wcschr(DialogItems[DlgLCMPSKIP].Data, L'&')?1:0) + 5;
	DialogItems[DlgLMAXDEPTH].X1 = DialogItems[DlgSUBFOLDER].X1 + wcslen(DialogItems[DlgSUBFOLDER].Data) - (wcschr(DialogItems[DlgSUBFOLDER].Data, L'&')?1:0) + 5;
	DialogItems[DlgLMAXDEPTH].X2 += DialogItems[DlgLMAXDEPTH].X1;
	DialogItems[DlgEMAXDEPTH].X1 = DialogItems[DlgLMAXDEPTH].X1 + wcslen(DialogItems[DlgLMAXDEPTH].Data) - (wcschr(DialogItems[DlgLMAXDEPTH].Data, L'&')?1:0) + 1;
	DialogItems[DlgEMAXDEPTH].X2 += DialogItems[DlgEMAXDEPTH].X1;
	DialogItems[DlgCACHECLEAR].X1 = DialogItems[DlgCACHECLEAR].X2 - wcslen(DialogItems[DlgCACHECLEAR].Data) + (wcschr(DialogItems[DlgCACHECLEAR].Data, L'&')?1:0) - 3;
	DialogItems[DlgPARTLYFULL].X1 = DialogItems[DlgPARTLY].X1 + wcslen(DialogItems[DlgPARTLY].Data) - (wcschr(DialogItems[DlgPARTLY].Data, L'&')?1:0) + 5;
	DialogItems[DlgIGNORETEMPL].X1 = DialogItems[DlgIGNORE].X1 + wcslen(DialogItems[DlgIGNORE].Data) - (wcschr(DialogItems[DlgIGNORE].Data, L'&')?1:0) + 5;
	DialogItems[DlgCACHEIGNORE].X1 = DialogItems[DlgCACHE].X1 + wcslen(DialogItems[DlgCACHE].Data) - (wcschr(DialogItems[DlgCACHE].Data, L'&')?1:0) + 5;
	DialogItems[DlgDUPPICDIFF].X1 = DialogItems[DlgDUPPIC].X1 + wcslen(DialogItems[DlgDUPPIC].Data) - (wcschr(DialogItems[DlgDUPPIC].Data, L'&')?1:0) + 5;
	DialogItems[DlgDUPMUSICDURE].X1 = DialogItems[DlgDUPMUSICDURL].X1 + wcslen(DialogItems[DlgDUPMUSICDURL].Data) - (wcschr(DialogItems[DlgDUPMUSICDURL].Data, L'&')?1:0) + 5;
	DialogItems[DlgDUPMUSICDURE].X2 += DialogItems[DlgDUPMUSICDURE].X1;

	// ��������� ����� � �������
	FarSettingsCreate settings={sizeof(FarSettingsCreate),MainGuid,INVALID_HANDLE_VALUE};
	if (Info.SettingsControl(INVALID_HANDLE_VALUE,SCTL_CREATE,0,&settings))
	{
		int Root=0; // ������ �����
		for (int i=DlgMODEBOX; i<sizeof(StoreOpt)/sizeof(StoreOpt[0]); i++)
		{
			FarSettingsItem item={sizeof(FarSettingsItem),Root,StoreOpt[i].OptName,FST_QWORD};
			if (StoreOpt[i].OptName && Info.SettingsControl(settings.Handle,SCTL_GET,0,&item))
			{
				*StoreOpt[i].Option=(int)item.Number;
				if (i==DlgEPARTLYKB)
				{
					static wchar_t buf[20];
					FSF.itoa64(item.Number,buf,10);
					DialogItems[i].Data=buf;
				}
				else if (i==DlgEMAXDEPTH)
				{
					static wchar_t buf[5];
					FSF.itoa64(item.Number,buf,10);
					DialogItems[i].Data=buf;
				}
				else if (i==DlgDUPMUSICDURE)
				{
					static wchar_t buf[5];
					FSF.itoa64(item.Number,buf,10);
					DialogItems[i].Data=buf;
				}
				else
					DialogItems[i].Selected=(int)item.Number;
			}
		}
		// ������ ���������������� ���� �� WinMerge
		Opt.WinMergePath=NULL;
		FarSettingsItem item={sizeof(FarSettingsItem),Root,L"WinMergePath",FST_STRING};
		if (Info.SettingsControl(settings.Handle,SCTL_GET,0,&item))
		{
			Opt.WinMergePath=(wchar_t*)malloc((wcslen(item.String)+1)*sizeof(wchar_t));
			if (Opt.WinMergePath) wcscpy(Opt.WinMergePath,item.String);
		}
		else
		{
			Opt.WinMergePath=(wchar_t*)malloc(40*sizeof(wchar_t));
			if (Opt.WinMergePath) wcscpy(Opt.WinMergePath,L"%ProgramFiles%\\WinMerge\\WinMergeU.exe");
		}
		Info.SettingsControl(settings.Handle,SCTL_FREE,0,0);
	}

	// ��������������� ������ � �������� ���������
	FarListItem itemModeBox[3]={};
	int n = sizeof(itemModeBox) / sizeof(itemModeBox[0]);
	for (int i=0; i<n; i++)
	{
		itemModeBox[i].Flags = 0;
		// !!! ������� ��������� � ����: ������ �������� �������� LIF_SELECTED �� DN_INITDIALOG
		if ((LPanel.PInfo.Flags&PFLAGS_PLUGIN) || (RPanel.PInfo.Flags&PFLAGS_PLUGIN))
			itemModeBox[0].Flags |= LIF_SELECTED;
		else
		{
			if (i==Opt.Mode)
			itemModeBox[i].Flags |= LIF_SELECTED;
		}
		itemModeBox[i].Text=GetMsg(MModeCmp+i);
	}
	FarList ModeBox={sizeof(FarList),n,itemModeBox};
	DialogItems[DlgMODEBOX].ListItems = &ModeBox;

	// ��������������� ������ � ��������� ������� �� ���������
	FarListItem itemIgnoreTemplates[3]={};
	n = sizeof(itemIgnoreTemplates) / sizeof(itemIgnoreTemplates[0]);
	for (int i=0; i<n; i++)
	{
		itemIgnoreTemplates[i].Flags = 0;
		if (i==Opt.IgnoreTemplates)
			itemIgnoreTemplates[i].Flags |= LIF_SELECTED;
		itemIgnoreTemplates[i].Text=GetMsg(MCmpIgnoreAllWhitespace+i);
	}
	FarList IgnoreTemplates={sizeof(FarList),n,itemIgnoreTemplates};
	DialogItems[DlgIGNORETEMPL].ListItems = &IgnoreTemplates;

	// ��������������� ������ � �������� �������� ��������
	FarListItem itemPicDiffBox[5]={};
	n = sizeof(itemPicDiffBox) / sizeof(itemPicDiffBox[0]);
	for (int i=0; i<n; i++)
	{
		itemPicDiffBox[i].Flags = 0;
		if (i==Opt.DupPicDiff)
			itemPicDiffBox[i].Flags |= LIF_SELECTED;
		itemPicDiffBox[i].Text=GetMsg(MDupPicDiffNone+i);
	}
	FarList PicDiffBox={sizeof(FarList),n,itemPicDiffBox};
	DialogItems[DlgDUPPICDIFF].ListItems = &PicDiffBox;

	HANDLE hDlg=Info.DialogInit( &MainGuid, &OptDlgGuid,
                                -1, -1, dW, dH,
                                L"Contents",
                                DialogItems,
                                sizeof(DialogItems)/sizeof(DialogItems[0]),
                                0, FDLG_SMALLDIALOG,
                                ShowOptDialogProcThunk,
                                this );

	int ExitCode=-1;

	if (hDlg != INVALID_HANDLE_VALUE)
	{
		ExitCode=(int)Info.DialogRun(hDlg);
		Info.DialogFree(hDlg);
	}
	return ExitCode;
}
