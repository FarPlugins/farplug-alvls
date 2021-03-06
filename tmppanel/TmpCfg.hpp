#ifndef __TMPCFG_HPP__
#define __TMPCFG_HPP__

typedef struct options_s
{
  int AddToDisksMenu;
  int AddToPluginsMenu;
  int Branch;
  int CommonPanel;
  int SafeModePanel;
  int AnyInPanel;
  int CopyContents;
  int Mode;
  int MenuForFilelist;
  int NewPanelForSearchResults;
  int ShowDir;
  int SelectedShowDir;
  int FullScreenPanel;
  int PanelMode;
  int LastSearchResultsPanel;
  int SelectedCopyContents;
  TCHAR ColumnTypes[64];
  TCHAR ColumnWidths[64];
  TCHAR StatusColumnTypes[64];
  TCHAR StatusColumnWidths[64];
  TCHAR DisksMenuDigit[1];
  TCHAR Mask[512];
  TCHAR Prefix[16];
} options_t;

extern options_t Opt;

#endif /* __TMPCFG_HPP__ */
