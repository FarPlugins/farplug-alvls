@echo off
@cls

rem =============== Use Microsoft Visual Studio .NET 2003 ======================

@call "C:\Program Files\Microsoft Visual Studio .NET 2003\Common7\Tools\vsvars32.bat"

rem  ======================== Set name and version ... =========================

@set PlugName=VisRen
@set fileversion=2,0,0,14
@set fileversion_str=2.0 build 14
@set MyDir=%CD%
@set companyname=Eugene Roshal ^& FAR Group
@set filedescription=Visual renaming files for FAR Manager
@set legalcopyright=Copyright � 2007-2010 Alexey Samlyukov


rem  ==================== Make %PlugName%.def file... ==========================

@if not exist %PlugName%.def (
echo Make %PlugName%.def file...

@echo LIBRARY           %PlugName%                         > %PlugName%.def
@echo EXPORTS                                              >> %PlugName%.def
@echo   SetStartupInfoW                                    >> %PlugName%.def
@echo   GetPluginInfoW                                     >> %PlugName%.def
@echo   OpenPluginW                                        >> %PlugName%.def
@echo   GetMinFarVersionW                                  >> %PlugName%.def
@echo   ExitFARW                                           >> %PlugName%.def

@if exist %PlugName%.def echo ... successfully
)

rem  ================== Make %PlugName%.rc file... =============================

@if not exist %PlugName%.rc (
echo Make %PlugName%.rc file...

@echo #define VERSIONINFO_1   1                                 > %PlugName%.rc
@echo.                                                          >> %PlugName%.rc
@echo VERSIONINFO_1  VERSIONINFO                                >> %PlugName%.rc
@echo FILEVERSION    %fileversion%                              >> %PlugName%.rc
@echo PRODUCTVERSION 2,0,0,0                                    >> %PlugName%.rc
@echo FILEFLAGSMASK  0x0                                        >> %PlugName%.rc
@echo FILEFLAGS      0x0                                        >> %PlugName%.rc
@echo FILEOS         0x4                                        >> %PlugName%.rc
@echo FILETYPE       0x2                                        >> %PlugName%.rc
@echo FILESUBTYPE    0x0                                        >> %PlugName%.rc
@echo {                                                         >> %PlugName%.rc
@echo   BLOCK "StringFileInfo"                                  >> %PlugName%.rc
@echo   {                                                       >> %PlugName%.rc
@echo     BLOCK "000004E4"                                      >> %PlugName%.rc
@echo     {                                                     >> %PlugName%.rc
@echo       VALUE "CompanyName",      "%companyname%\0"         >> %PlugName%.rc
@echo       VALUE "FileDescription",  "%filedescription%\0"     >> %PlugName%.rc
@echo       VALUE "FileVersion",      "%fileversion_str%\0"     >> %PlugName%.rc
@echo       VALUE "InternalName",     "%PlugName%\0"            >> %PlugName%.rc
@echo       VALUE "LegalCopyright",   "%legalcopyright%\0"      >> %PlugName%.rc
@echo       VALUE "OriginalFilename", "%PlugName%.dll\0"        >> %PlugName%.rc
@echo       VALUE "ProductName",      "FAR Manager\0"           >> %PlugName%.rc
@echo       VALUE "ProductVersion",   "2.0\0"                   >> %PlugName%.rc
@echo       VALUE "Comments",         "%comments%\0"            >> %PlugName%.rc
@echo     }                                                     >> %PlugName%.rc
@echo   }                                                       >> %PlugName%.rc
@echo   BLOCK "VarFileInfo"                                     >> %PlugName%.rc
@echo   {                                                       >> %PlugName%.rc
@echo     VALUE "Translation", 0, 0x4e4                         >> %PlugName%.rc
@echo   }                                                       >> %PlugName%.rc
@echo }                                                         >> %PlugName%.rc

@if exist %PlugName%.rc echo ... successfully
)

rem  ==================== Compile %PlugName%.dll file...========================

@cd ".."
@if exist %PlugName%.dll del %PlugName%.dll>nul

@echo !!!!!!!  Compile %PlugName%.dll with MSVCRT.dll ...  !!!!!!!

@cd %MyDir%
@rc /l 0x4E4 %PlugName%.rc
@cl /Zp1 /O1i /GF /Gr /GR- /GX- /LD %PlugName%.cpp DList.cpp string.cpp VisRenDlg.cpp VisRenFile.cpp /D "UNICODE" /D "_UNICODE" /link /subsystem:console /machine:I386 /opt:nowin98 /noentry /nodefaultlib /def:%PlugName%.def kernel32.lib advapi32.lib user32.lib shell32.lib MSVCRT60.LIB %PlugName%.res /map:"..\%PlugName%.map" /out:"..\%PlugName%.dll"
rem /merge:.rdata=.text

@if exist *.exp del *.exp>nul
@if exist *.obj del *.obj>nul
@if exist *.lib del *.lib>nul
@if exist *.res del *.res>nul
@if exist *.def del *.def>nul
@if exist *.rc  del *.rc>nul

echo ***************