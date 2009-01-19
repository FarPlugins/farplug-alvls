/****************************************************************************
 * VisRen4_REN.cpp
 *
 * Plugin module for FAR Manager 1.71
 *
 * Copyright (c) 2007 Alexey Samlyukov
 ****************************************************************************/

enum {
  CASE_NAME_NONE   = 0x00000001,
  CASE_NAME_LOWER  = 0x00000002,
  CASE_NAME_UPPER  = 0x00000004,
  CASE_NAME_FIRST  = 0x00000008,
  CASE_NAME_TITLE  = 0x00000010,
  CASE_EXT_NONE    = 0x00000020,
  CASE_EXT_LOWER   = 0x00000040,
  CASE_EXT_UPPER   = 0x00000080,
  CASE_EXT_FIRST   = 0x00000100,
  CASE_EXT_TITLE   = 0x00000200
};


/****************************************************************************
 * ������� ��������� ��� ������ ��������������
 ****************************************************************************/
static void FreeUndo()
{
  for (int i=sUndoFI.iCount-1; i>=0; i--)
  {
    my_free(sUndoFI.CurFileName[i]);
    my_free(sUndoFI.OldFileName[i]);
  }
  my_free(sUndoFI.CurFileName); sUndoFI.CurFileName=0;
  my_free(sUndoFI.OldFileName); sUndoFI.OldFileName=0;
  my_free(sUndoFI.Dir); sUndoFI.Dir=0;
  sUndoFI.iCount=0;
}

/****************************************************************************
 * ���������� ��������� ���������� ��� ������
 ****************************************************************************/
static bool BuildUndoItem(const TCHAR *CurFileName, const TCHAR *OldFileName, int Count)
{
  if (Count>=sUndoFI.iCount)
  {
    sUndoFI.CurFileName=(TCHAR**)my_realloc(sUndoFI.CurFileName, (Count+1)*sizeof(TCHAR*));
    sUndoFI.OldFileName=(TCHAR**)my_realloc(sUndoFI.OldFileName, (Count+1)*sizeof(TCHAR*));
    if (!sUndoFI.CurFileName || !sUndoFI.OldFileName) return false;
  }
  sUndoFI.CurFileName[Count]=(TCHAR*)my_realloc(sUndoFI.CurFileName[Count], (lstrlen(CurFileName)+1)*sizeof(TCHAR));
  sUndoFI.OldFileName[Count]=(TCHAR*)my_realloc(sUndoFI.OldFileName[Count], (lstrlen(OldFileName)+1)*sizeof(TCHAR));
  if (!sUndoFI.CurFileName[Count] || !sUndoFI.OldFileName[Count]) return false;
  lstrcpy(sUndoFI.CurFileName[Count], CurFileName);
  lstrcpy(sUndoFI.OldFileName[Count], OldFileName);
  return true;
}

/****************************************************************************
 * �������� ����� ����� �� ���������� �������
 ****************************************************************************/
static bool CheckFileName(const TCHAR *str)
{
  static TCHAR Denied[]=_T("<>:\"*?/\\|");

  for (TCHAR *s=(TCHAR *)str; *s; s++)
    for (TCHAR *p=Denied; *p; p++)
      if (*s==*p) return false;
  return true;
}

/****************************************************************************
 * ������ ������ ��� ����� �� ���� � �����
 ****************************************************************************/
static TCHAR *BuildFullFilename(TCHAR *FullFileName, const TCHAR *Dir, const TCHAR *FileName)
{
  FSF.AddEndSlash(lstrcpy(FullFileName, Dir));
  return lstrcat(FullFileName, FileName);
}

/****************************************************************************
 * ��������, ��� ���������� ���� �� �����
 ****************************************************************************/
static bool IsEmpty(const TCHAR *Str)
{
  if (Str)
  {
    for (int i=0; i<lstrlen(Str); i++)
      if ( (Str[i] != _T('\n')) && (Str[i] != _T('\r')) && (Str[i] != _T('\t'))
           && (Str[i] != _T('\0')) && (Str[i] != _T(' ')) )
        return false;
  }
  return true;
}


/****************************************************************************
 * �������������� �� ����� ����� � ���������� ������
 ****************************************************************************/
static bool GetNewNameExt(const TCHAR *src, TCHAR *destName, TCHAR *destExt,
                          unsigned ItemIndex, DWORD *dwCase, FILETIME ftLastWriteTime)
{
  SYSTEMTIME modific; FILETIME local;
  FileTimeToLocalFileTime(&ftLastWriteTime, &local);
  FileTimeToSystemTime(&local, &modific);

  bool bIsMP3=Info.CmpName(_T("*.mp3"), src, true);
  ID3TagInternal *pInternalTag=0;
  if (bIsMP3)
  {
    static TCHAR FullFilename[MAX_PATH];
    GetCurrentDirectory(sizeof(FullFilename), FullFilename);
    BuildFullFilename(FullFilename, FullFilename, src);
    pInternalTag=AnalyseFile(FullFilename);
  }

  TCHAR Name[NM], Ext[NM];
  lstrcpy(Name, src);
  TCHAR *ptr=strrchr(Name, _T('.'));

  if (ptr) { *ptr=_T('\0'); lstrcpy(Ext, ++ptr); }
  else Ext[0]=_T('\0');

  for (int Index=0; Index<=1; Index++)
  {
    TCHAR *pMask=(Index==0 ? Opt.MaskName : Opt.MaskExt);
    TCHAR buf[512]={_T('\0')};
    ptr=buf;
    while (*pMask)
    {
      if (!strncmp(pMask, _T("[[]"), 3))
      {
        *ptr++=*pMask++; pMask+=2;
      }
      else if (!strncmp(pMask, _T("[]]"), 3))
      {
        pMask+=2; *ptr++=*pMask++;
      }
      else if (!strncmp(pMask, _T("[N"), 2) || !strncmp(pMask, _T("[E"), 2))
      {
        bool bName=!strncmp(pMask, _T("[N"), 2);
        int len=lstrlen(bName?Name:Ext);

        if (*(pMask+2)==_T(']'))  // [N] ��� [E]
        {
          lstrcpy(ptr, bName?Name:Ext); pMask+=3; ptr+=len;
        }
        else // [N#-#] ��� [E#-#]
        {
          TCHAR buf2[512], Start[512], End[512];
          int i, iStart, iEnd;
          bool bFromEnd_Start=false, bFromEnd_End=false;
          pMask+=2;
          // ����������� ����� � ����� �����
          if (*pMask==_T('-')) { bFromEnd_Start=true; pMask++; }
          // �����
          for (i=0; *pMask>=_T('0') && *pMask<=_T('9'); i++, pMask++) Start[i]=*pMask;
          Start[i]=_T('\0'); iStart=FSF.atoi(Start);
          if (iStart==0)  return false;
          if (bFromEnd_Start)
            iStart=len-iStart;
          else
          {
            iStart--;
            if (iStart>len) iStart=len;
          }
          // �������� ������...
          if (*pMask==_T(']'))
          {
            if (iStart<0) iStart=len;   //< bFromEnd_Start==true
            *ptr++=*((bName?Name:Ext)+iStart);
            pMask++;
          }
          // �����, ��������� ������...
          else
          {
            if (iStart<0) iStart=0;     //< bFromEnd_Start==true

            if (*pMask==_T(','))
            {
              pMask++;
              for (i=0; *pMask>=_T('0') && *pMask<=_T('9'); i++, pMask++) End[i]=*pMask;
              End[i]=_T('\0'); iEnd=FSF.atoi(End);
              if (!iEnd || *pMask!=_T(']')) return false;
              lstrcpyn(ptr, (bName?Name:Ext)+iStart, iEnd+1);
              ptr+=lstrlen(ptr); pMask++;
            }
            // �������� �������� ��������...
            else if (*pMask==_T('-'))
            {
              pMask++;
              if (*pMask==_T(']'))
              {
                lstrcpy(ptr, (bName?Name:Ext)+iStart);
                ptr+=lstrlen(ptr); pMask++;
              }
              // ��� ��������
              else
              {
                if (bFromEnd_Start) bFromEnd_End=true;
                if (*pMask==_T('-'))
                  if (bFromEnd_End) return false;
                  else { bFromEnd_End=true; pMask++; }
                for (i=0; *pMask>=_T('0') && *pMask<=_T('9'); i++, pMask++) End[i]=*pMask;
                End[i]=_T('\0'); iEnd=FSF.atoi(End);
                if (!iEnd || *pMask!=_T(']')) return false;
                if (bFromEnd_End)
                {
                  iEnd=len-(iEnd-1);
                  if (iEnd<iStart) iEnd=iStart;
                }
                else
                  if (iEnd<=iStart) return false;
                lstrcpyn(buf2, (bName?Name:Ext), iEnd+1); lstrcpy(ptr, buf2+iStart);
                ptr+=lstrlen(ptr); pMask++;
              }
            }
            else return false;
          }
        }
      }
      else if (!strncmp(pMask, _T("[C"), 2))
      {
        static TCHAR Start[512], Width[512], Step[512];
        unsigned i, iStart, iWidth, iStep;
        pMask+=2;
        for (i=0; *pMask>=_T('0') && *pMask<=_T('9'); i++, pMask++) Start[i]=*pMask;
        if (!i || i>9 || *pMask!=_T('+')) return false;
        Start[i]=_T('\0'); iStart=FSF.atoi(Start);
        iWidth=i; pMask++;
        for (i=0; *pMask>=_T('0') && *pMask<=_T('9'); i++, pMask++) Step[i]=*pMask;
        if (!i || *pMask!=_T(']')) return false;
        Step[i]=_T('\0'); iStep=FSF.atoi(Step);
        FSF.sprintf(ptr, _T("%0*d"), iWidth, iStart+ItemIndex*iStep);
        ptr+=lstrlen(ptr); pMask++;
      }
      else if (!strncmp(pMask, _T("[L]"), 3))
      {
        *dwCase|=(Index==0 ? CASE_NAME_LOWER : CASE_EXT_LOWER);
        pMask+=3;
      }
      else if (!strncmp(pMask, _T("[U]"), 3))
      {
        *dwCase|=(Index==0 ? CASE_NAME_UPPER : CASE_EXT_UPPER);
        pMask+=3;
      }
      else if (!strncmp(pMask, _T("[F]"), 3))
      {
        *dwCase|=(Index==0 ? CASE_NAME_FIRST : CASE_EXT_FIRST);
        pMask+=3;
      }
      else if (!strncmp(pMask, _T("[T]"), 3))
      {
        *dwCase|=(Index==0 ? CASE_NAME_TITLE : CASE_EXT_TITLE);
        pMask+=3;
      }
      else if (!strncmp(pMask, _T("[#]"), 3))
      {
        if (pInternalTag && !IsEmpty(pInternalTag->pEntry[TAG_TRACK]))
        {
          lstrcpy(ptr, pInternalTag->pEntry[TAG_TRACK]);
          ptr+=lstrlen(pInternalTag->pEntry[TAG_TRACK]);
        }
        pMask+=3;
      }
      else if (!strncmp(pMask, _T("[t]"), 3))
      {
        if ( pInternalTag &&
             !IsEmpty(pInternalTag->pEntry[TAG_TITLE]) &&
             CheckFileName(pInternalTag->pEntry[TAG_TITLE]) )
        {
          FSF.Trim(pInternalTag->pEntry[TAG_TITLE]);
          lstrcpy(ptr, pInternalTag->pEntry[TAG_TITLE]);
          ptr+=lstrlen(pInternalTag->pEntry[TAG_TITLE]);
        }
        pMask+=3;
      }
      else if (!strncmp(pMask, _T("[a]"), 3))
      {
        if ( pInternalTag &&
             !IsEmpty(pInternalTag->pEntry[TAG_ARTIST]) &&
             CheckFileName(pInternalTag->pEntry[TAG_ARTIST]) )
        {
          FSF.Trim(pInternalTag->pEntry[TAG_ARTIST]);
          lstrcpy(ptr, pInternalTag->pEntry[TAG_ARTIST]);
          ptr+=lstrlen(pInternalTag->pEntry[TAG_ARTIST]);
        }
        pMask+=3;
      }
      else if (!strncmp(pMask, _T("[l]"), 3))
      {
        if ( pInternalTag &&
             !IsEmpty(pInternalTag->pEntry[TAG_ALBUM]) &&
             CheckFileName(pInternalTag->pEntry[TAG_ALBUM]) )
        {
          FSF.Trim(pInternalTag->pEntry[TAG_ALBUM]);
          lstrcpy(ptr, pInternalTag->pEntry[TAG_ALBUM]);
          ptr+=lstrlen(pInternalTag->pEntry[TAG_ALBUM]);
        }
        pMask+=3;
      }
      else if (!strncmp(pMask, _T("[y]"), 3))
      {
        if (pInternalTag && !IsEmpty(pInternalTag->pEntry[TAG_YEAR]))
        {
          lstrcpy(ptr, pInternalTag->pEntry[TAG_YEAR]);
          ptr+=lstrlen(pInternalTag->pEntry[TAG_YEAR]);
        }
        pMask+=3;
      }
      else if (!strncmp(pMask, _T("[g]"), 3))
      {
        if ( pInternalTag &&
             !IsEmpty(pInternalTag->pEntry[TAG_GENRE]) &&
             CheckFileName(pInternalTag->pEntry[TAG_GENRE]) )
        {
          lstrcpy(ptr, pInternalTag->pEntry[TAG_GENRE]);
          ptr+=lstrlen(pInternalTag->pEntry[TAG_GENRE]);
        }
        pMask+=3;
      }
      else if (!strncmp(pMask, _T("[DM]"), 4))
      {
        FSF.sprintf(ptr, "%02d.%02d.%04d", modific.wDay, modific.wMonth, modific.wYear);
        pMask+=4; ptr+=10;
      }
      else if (!strncmp(pMask, _T("[TM]"), 4))
      {
        FSF.sprintf(ptr, "%02d-%02d", modific.wHour, modific.wMinute);
        pMask+=4; ptr+=5;
      }
      else if (*pMask==_T('[') || *pMask==_T(']')) return false;
      else *ptr++=*pMask++;
    }
    *ptr=_T('\0');
    if (lstrlen(buf)>=MAX_PATH) return false;
    lstrcpy(Index==0?destName:destExt, buf);
  }

  if (pInternalTag)
  {
    for (int i=0; i<pInternalTag->nEntryCount; i++)
      my_free(pInternalTag->pEntry[i]);
    delete pInternalTag;
  }

  return true;
}


/****************************************************************************
 * ����� � ������ � ������ ������ (�����������������, �����������).
 ****************************************************************************/
static bool Replase(const TCHAR *src, TCHAR *dest)
{
  int lenSearch=lstrlen(Opt.Search);
  if (!lenSearch) return true;

  int lenSrc=lstrlen(src);
  int lenReplace=lstrlen(Opt.Replace);
  int j=0;
  TCHAR buf[512];
  // ������ ������
  if (!Opt.RegEx)
  {
    for (int i=0; i<lenSrc; )
    {
      if (!(Opt.CaseSensitive?strncmp(src+i, Opt.Search, lenSearch)
                             :FSF.LStrnicmp(src+i, Opt.Search, lenSearch) ))
      {
        for (int k=0; k<lenReplace; k++)
          buf[j++]=Opt.Replace[k];
        i+=lenSearch;
      }
      else
        buf[j++]=src[i++];
    }
    buf[j]=_T('\0');
    if (j>=MAX_PATH || !CheckFileName(buf)) return false;
    lstrcpy(dest, buf);
  }
  else
  {
    pcre *re;
    //pcre_extra *re_ext;
    const char *Error;
    int ErrPtr;
    int matchCount;
    int start_offset=0;
    const int OvectorSize=99;
    int Ovector[OvectorSize];
    TCHAR AnsiSrc[MAX_PATH], AnsiSearch[512];
    OemToChar(src, AnsiSrc);  OemToChar(Opt.Search, AnsiSearch);

    if (!(re=pcre_compile(AnsiSearch, Opt.CaseSensitive?0:PCRE_CASELESS,
                           &Error, &ErrPtr, 0))) return false;
    //if (!(re_ext=pcre_study(re, 0, &Error))) { pcre_free(re); return false; }

    while ((matchCount=pcre_exec(re, /*re_ext*/0, AnsiSrc, lenSrc, start_offset,
             PCRE_NOTEMPTY, Ovector, OvectorSize))>0)
    {
      // �������� �� ��������
      for (int i=start_offset; i<Ovector[0]; i++)
        buf[j++]=src[i];
      // ����� �������. ��������� ���������� Opt.Replace
      for (int i=0; i<lenReplace; i++)
      {
        // ����� '\' ��������� ��� ���������
        if (Opt.Replace[i]==_T('\\') && i+1<lenReplace)
        {
          buf[j++]=Opt.Replace[++i]; continue;
        }
        // ��������� �� ��������� ������������
        if (Opt.Replace[i]==_T('$') && i+1<lenReplace)
        {
          TCHAR *p=Opt.Replace+(i+1), Digit[10];
          unsigned int k, iDigit;
          for (k=0; *p>=_T('0') && *p<=_T('9') && k<9; k++, p++)
            Digit[k]=*p;
          if (k)
          {
            Digit[k]=_T('\0');
            iDigit=FSF.atoi(Digit);
            if (iDigit<matchCount && iDigit<=99)
            {
              i+=k;
              for (k=Ovector[iDigit*2]; k<Ovector[iDigit*2+1]; k++)
                buf[j++]=src[k];
            }
            else i+=k;
          }
          else buf[j++]=Opt.Replace[i];
        }
        else buf[j++]=Opt.Replace[i];
      }
      start_offset=Ovector[1];
    }
    pcre_free(re);
    //pcre_free(re_ext);
    // �������� �� �� ��� �� ����� � �������
    for (int i=start_offset; i<lenSrc; i++)
      buf[j++]=src[i];
    buf[j]=_T('\0');
    if (j>=MAX_PATH || !CheckFileName(buf)) return false;
    lstrcpy(dest, buf);
  }
  return true;
}


/****************************************************************************
 * ��������� �������� ���� ������.
 ****************************************************************************/
static void Case(TCHAR *Name, TCHAR *Ext, DWORD dwCase)
{
  if (!dwCase) return;
  // ������� �����
  if (dwCase&CASE_NAME_LOWER)
    FSF.LStrlwr(Name);
  else if (dwCase&CASE_NAME_UPPER)
    FSF.LStrupr(Name);
  else if (dwCase&CASE_NAME_FIRST)
  {
    *Name = (char)FSF.LUpper((BYTE)*Name);
     FSF.LStrlwr(Name+1);
  }
  else if (dwCase&CASE_NAME_TITLE)
    for (int i=0; Name[i]; i++)
    {
      if (!i || memchr(Opt.WordDiv, Name[i-1], lstrlen(Opt.WordDiv)))
        Name[i]=(char)FSF.LUpper((BYTE)Name[i]);
      else
        Name[i]=(char)FSF.LLower((BYTE)Name[i]);
    }
  // ������� ����������
  if (dwCase&CASE_EXT_LOWER)
    FSF.LStrlwr(Ext);
  else if (dwCase&CASE_EXT_UPPER)
    FSF.LStrupr(Ext);
  else if (dwCase&CASE_EXT_FIRST)
  {
    *Ext = (char)FSF.LUpper((BYTE)*Ext);
     FSF.LStrlwr(Ext+1);
  }
  else if (dwCase&CASE_EXT_TITLE)
    for (int i=0; Ext[i]; i++)
    {
      if (!i || memchr(Opt.WordDiv, Ext[i-1], lstrlen(Opt.WordDiv)))
        Ext[i]=(char)FSF.LUpper((BYTE)Ext[i]);
      else
        Ext[i]=(char)FSF.LLower((BYTE)Ext[i]);
    }
}

/****************************************************************************
 * �������� ������� �� ��������� � �������� ����� ���� ������.
 ****************************************************************************/
static bool ProcessFileName()
{
  if (!CheckFileName(Opt.MaskName) || !CheckFileName(Opt.MaskExt))
    return false;

  HANDLE hScreen=Info.SaveScreen(0,0,-1,-1);
  const TCHAR *MsgItems[]={ GetMsg(MVRenTitle), GetMsg(MLoadFiles) };
  TCHAR NewName[NM], NewExt[NM];
  DWORD dwCase=0, dwTicks=GetTickCount();
  bool bRet=true;

  for (unsigned Index=0; Index<sFI.iCount; Index++)
  {
    TCHAR *src  = sFI.ppi[Index].FindData.cFileName,
          *dest = sFI.DestFileName[Index];

    if (GetTickCount()-dwTicks>1000)
      Info.Message(Info.ModuleNumber, 0, 0, MsgItems, 2, 0);
    if (!GetNewNameExt(src, NewName, NewExt, Index, &dwCase, sFI.ppi[Index].FindData.ftLastWriteTime))
    {
      bRet=false; break;
    }
    lstrcpy(dest, NewName);
    if (NewExt[0])
    {
      if (lstrlen(NewExt)+1>=MAX_PATH)
      {
        bRet=false; break;
      }
      else
        lstrcat(lstrcat(dest, _T(".")), NewExt);
    }
    if (!Replase(dest, dest))
    {
      bRet=false; break;
    }
    lstrcpy(NewName, dest);
    TCHAR *ptr=strrchr(NewName, _T('.'));
    if (ptr) { *ptr=_T('\0'); lstrcpy(NewExt, ++ptr); }
    else NewExt[0]=_T('\0');

    Case(NewName, NewExt, dwCase);
    lstrcpy(dest, NewName);
    if (NewExt[0]) lstrcat(lstrcat(dest, _T(".")), NewExt);
  }

  Info.RestoreScreen(hScreen);
  return bRet;
}


/****************************************************************************
 * ������� �� �������������� ������. ������ �������� ��������������.
 * ���������� ��������������, ���� ��������� ������ ��� �������������� �����.
 * ���������� ���-�� ������������ ������ �� ������ ����� ����������.
 ****************************************************************************/
static bool RenameFile(PanelInfo *PInfo)
{
  TCHAR srcFull[MAX_PATH], destFull[MAX_PATH];
  bool bSkipAll=false;
  int i, j, Count, iRen=0, iUndo=0;
  PanelRedrawInfo RInfo={0,0};
  SetLastError(ERROR_SUCCESS);

  // ������� ������ ��������� �� ������
  for (i=PInfo->ItemsNumber-1; i>=0; i--)
    PInfo->PanelItems[i].Flags &= ~PPIF_SELECTED;

  if (!Opt.Undo)
  {
    Count=sFI.iCount;

    for (i=0; i<Count; i++)
    {
      TCHAR *src =sFI.ppi[i].FindData.cFileName,
            *dest=sFI.DestFileName[i];

      // ����� ��������� - ��������� ��������������
      if (!lstrcmp(src, dest)) continue;
 RETRY_1:
      // ���������������
      if (MoveFile(src, dest))
      {
        iRen++;
        // �������� � Undo
        if (Opt.LogRen && !BuildUndoItem(dest, src, iUndo++))
        {
          Opt.LogRen=0;
          ErrorMsg(MErrorCreateLog, MErrorNoMem);
        }
        continue;
      }
      // ���� ��� ������ ��������� ��������� - ��������� ��� ����
      if (GetLastError()==ERROR_FILE_NOT_FOUND) continue;
      // �� ������������� - �������
      if (bSkipAll)
      {
        for (j=0; j<PInfo->ItemsNumber; j++)
          if (!FSF.LStricmp(PInfo->PanelItems[j].FindData.cFileName, src))
          {
            PInfo->PanelItems[j].Flags |= PPIF_SELECTED;
            break;
          }
        continue;
      }
      // ������ � ����������-������� ��������������
      const TCHAR *MsgItems[]={
        GetMsg(MVRenTitle),
        GetMsg(MRenameFail),
        src,
        GetMsg(MTo),
        dest,
        GetMsg(MSkip), GetMsg(MSkipAll), GetMsg(MRetry), GetMsg(MCancel)
      };
      int Ret=Info.Message( Info.ModuleNumber, FMSG_WARNING|FMSG_ERRORTYPE, 0,
                            MsgItems, sizeof(MsgItems) / sizeof(MsgItems[0]), 4 );
      switch (Ret)
      {
        case 2:     // ���������
          goto RETRY_1;
        case 1:     // ���������� ���
          bSkipAll=true;
        case 0:     // ����������
          // �� ������������� - �������
          for (j=0; j<PInfo->ItemsNumber; j++)
            if (!FSF.LStricmp(PInfo->PanelItems[j].FindData.cFileName, src))
            {
              PInfo->PanelItems[j].Flags |= PPIF_SELECTED;
              break;
            }
          break;
        default:    // ��������
          // �� ������������� - �������
          for ( ; i<Count; i++)
            for (j=0; j<PInfo->ItemsNumber; j++)
              if (!FSF.LStricmp(PInfo->PanelItems[j].FindData.cFileName, sFI.ppi[i].FindData.cFileName))
              {
                PInfo->PanelItems[j].Flags |= PPIF_SELECTED;
                break;
              }
          goto NEXT;
      }
    }
  }
  else
  {
    Count=sUndoFI.iCount;

    for (i=Count-1; i>=0; i--)
    {
      TCHAR *src =BuildFullFilename(srcFull, sUndoFI.Dir, sUndoFI.CurFileName[i]),
            *dest=BuildFullFilename(destFull, sUndoFI.Dir, sUndoFI.OldFileName[i]);
 RETRY_2:
      if (MoveFile(src, dest))  { iRen++; continue; }

      if (GetLastError()==ERROR_FILE_NOT_FOUND || bSkipAll) continue;

      const TCHAR *MsgItems[]={
        GetMsg(MVRenTitle),
        GetMsg(MRenameFail),
        src,
        GetMsg(MTo),
        dest,
        GetMsg(MSkip), GetMsg(MSkipAll), GetMsg(MRetry), GetMsg(MCancel)
      };
      int Ret=Info.Message( Info.ModuleNumber, FMSG_WARNING|FMSG_ERRORTYPE, 0,
                            MsgItems, sizeof(MsgItems) / sizeof(MsgItems[0]), 4 );
      switch (Ret)
      {
        case 2:     // ���������
          goto RETRY_2;
        case 1:     // ���������� ���
          bSkipAll=true;
        case 0:     // ����������
          break;
        default:    // ��������
          goto BREAK;
      }
    }
 BREAK:
    if (i<0) i=0;
    // ��������� �������
    Info.Control(INVALID_HANDLE_VALUE, FCTL_SETPANELDIR, (void *)sUndoFI.Dir);
    // ������� �����
    Info.Control(INVALID_HANDLE_VALUE, FCTL_GETPANELINFO, PInfo);
    for (int k=i; k<Count; k++)
      for (j=0; j<PInfo->ItemsNumber; j++)
        if (!FSF.LStricmp(PInfo->PanelItems[j].FindData.cFileName, sUndoFI.OldFileName[k]))
        {
          if (k==i) RInfo.TopPanelItem=RInfo.CurrentItem=j;
          PInfo->PanelItems[j].Flags |= PPIF_SELECTED;
          break;
        }
  }

 NEXT:
  // ������ �� ��������� ������ �������� (�� ������� Undo)
  for (i=sUndoFI.iCount-1; i>iUndo-1; i--)
  {
    my_free(sUndoFI.CurFileName[i]); sUndoFI.CurFileName[i]=0;
    my_free(sUndoFI.OldFileName[i]); sUndoFI.OldFileName[i]=0;
  }

  // ���-�� ��������� � Undo...
  if (sUndoFI.iCount=iUndo)
  {
    // ...�������� ����� � �������
    if (!(sUndoFI.Dir=(TCHAR*)my_realloc(sUndoFI.Dir, (lstrlen(PInfo->CurDir)+1)*sizeof(TCHAR))) )
    {
      FreeUndo();
      ErrorMsg(MVRenTitle, MErrorCreateLog);
    }
    else
      lstrcpy(sUndoFI.Dir, PInfo->CurDir);
  }

  if (iRen)
  {
    TCHAR buf[80];
    FSF.sprintf(buf, GetMsg(MProcessedFmt), iRen, Count);
    const TCHAR *MsgItems[]={GetMsg(MVRenTitle), buf, GetMsg(MOK)};
    Info.Message( Info.ModuleNumber, 0, 0, MsgItems,
                  sizeof(MsgItems) / sizeof(MsgItems[0]), 1 );
  }

  Info.Control(INVALID_HANDLE_VALUE, FCTL_SETSELECTION, PInfo);
  Info.Control(INVALID_HANDLE_VALUE, FCTL_REDRAWPANEL, Opt.Undo?&RInfo:0);

  return true;
}
