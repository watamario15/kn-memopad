#ifndef _UNICODE
#define _UNICODE
#endif
#ifndef UNICODE
#define UNICODE
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>

#include <commctrl.h>
#include <commdlg.h>
#include <tchar.h>

#include <algorithm>

#include "brain-essential/kncedlg/kncedlg.h"
#include "brain-essential/knceutil/knceutil.h"
#include "resource.h"

namespace std {
#ifdef UNICODE
typedef wstring tstring;
#else
typedef string tstring;
#endif
}  // namespace std

enum newline_t { NEWLINE_CRLF, NEWLINE_LF, NEWLINE_CR };
enum charset_t { CHARSET_CP932, CHARSET_UTF8, CHARSET_UTF16LE, CHARSET_CP1252 };

static TBBUTTON tbSTDButton[] = {
    {0, 0, TBSTATE_ENABLED, TBSTYLE_SEP, 0, 0, 0, -1},
    {STD_FILENEW, IDM_FILE_NEW, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0, 0, -1},
    {STD_FILEOPEN, IDM_FILE_OPEN, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0, 0, -1},
    {STD_FILESAVE, IDM_FILE_SAVE, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0, 0, -1},
    {0, 0, TBSTATE_ENABLED, TBSTYLE_SEP, 0, 0, 0, -1},
    {STD_CUT, IDM_EDIT_CUT, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0, 0, -1},
    {STD_COPY, IDM_EDIT_COPY, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0, 0, -1},
    {STD_PASTE, IDM_EDIT_PASTE, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0, 0, -1},
    {STD_DELETE, IDM_EDIT_CLEAR, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0, 0, -1},
    {0, 0, TBSTATE_ENABLED, TBSTYLE_SEP, 0, 0, 0, -1},
    {STD_UNDO, IDM_EDIT_UNDO, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0, 0, -1},
    {0, 0, TBSTATE_ENABLED, TBSTYLE_SEP, 0, 0, 0, -1},
    {STD_FIND, IDM_EDIT_FIND, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0, 0, -1},
};

static LRESULT CALLBACK wndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
static LRESULT CALLBACK editAreaProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
static LRESULT CALLBACK fileNameLabelProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
static void CALLBACK updateStatusBar(HWND hWnd, UINT msg, UINT idTimer, DWORD dwTime);
static BOOL WINAPI findStringDlgProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);
static void onCreate(HWND hWnd);
static void onDestroy(HWND hWnd);
static void onActivate(HWND hWnd, int act);
static void onSize(HWND hWnd, int width, int height);
static void onInitMenuPopup(HWND hWnd);
static void onKeyDown(HWND hWnd);
static void onFileReload(HWND hWnd);
static void onFileNew(HWND hWnd);
static void onFileOpen(HWND hWnd);
static void onFileSave(HWND hWnd);
static void onFileSaveAs(HWND hWnd);
static void onEditUndo(HWND hWnd);
static void onEditBeginSelect(HWND hWnd);
static void onEditEndSelect(HWND hWnd);
static void onEditCut(HWND hWnd);
static void onEditCopy(HWND hWnd);
static void onEditPaste(HWND hWnd);
static void onEditClear(HWND hWnd);
static void onEditFind(HWND hWnd);
static void onEditFindNext(HWND hWnd);
static void onEditSelectAll(HWND hWnd);
static void onToolsLookup(HWND hWnd);
static void onToolsWordwrap(HWND hWnd);
static void onToolsFont(HWND hWnd);
static void onToolsTextColor(HWND hWnd);
static void onToolsBackColor(HWND hWnd);
static void onToolsCharset(HWND hWnd, charset_t charset);
static void onToolsCharsetAutodetect(HWND hWnd);
static void onToolsNewlineCode(HWND hWnd, newline_t newlineCode);
static void onToolsAddBOM(HWND hWnd);
static void onToolsStatusBar(HWND hWnd);
static void onFindStringInitDialog(HWND hDlg);
static void onFindStringOk(HWND hDlg);
static void onFindStringCancel(HWND hDlg);
static void onFindStringTargetBox(HWND hDlg, int event);
static void openFile(HWND hWnd, const std::tstring &fileName, bool isReload);
static bool saveFile(HWND hWnd, bool isOverwrite = true);
static bool checkSaving(HWND hWnd);
static void updateFileNameLabel(HWND hWnd);
static void findString(HWND hWnd);
static const TCHAR *_tcscasestr(const TCHAR *str, const TCHAR *strSearch);
static newline_t convertCRLF(std::tstring &target, newline_t newLine);

static HINSTANCE g_hInstance = NULL;
static HWND g_hEditArea;
static LPTSTR g_lpCmdLine;
static CHOOSECOLOR cc = {0};
static COLORREF custColors[16], crText, crBack;
static HBRUSH hBackBsh;
static INT Row_bef = 0, Col_bef = 0, Charset_bef = -1, CRLF_bef = -1;
static bool AddBOM_bef = false;

static unsigned int g_dicKeyDownMessage = RegisterWindowMessage(_T("DicKeyDown"));

struct MemoPadData {
  HWND hFindStringDialog;
  HWND hCommandBar;
  HWND hStatusBar;
  HFONT hEditAreaFont;
  WNDPROC prevEditAreaProc;
  WNDPROC prevFileNameLabelProc;
  std::tstring editFileName;
  std::vector<int> selectableKeys;
  bool isSelectMode;
  bool isWordwrap;
  bool isStatusBar;
  charset_t charset;
  newline_t crlf;
  bool csAutoDetect;
  bool willAddBOM;
  std::tstring findTargetString;
};

static LRESULT CALLBACK wndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
  switch (msg) {
    case WM_CREATE:
      onCreate(hWnd);
      return 0;
    case WM_CLOSE:
      if (!checkSaving(hWnd)) return 0;
      break;
    case WM_DESTROY:
      onDestroy(hWnd);
      return 0;
    case WM_ACTIVATE:
      onActivate(hWnd, LOWORD(wParam));
      return 0;
    case WM_SIZE:
      onSize(hWnd, LOWORD(lParam), HIWORD(lParam));
      return 0;
    case WM_INITMENUPOPUP:
      onInitMenuPopup(hWnd);
      return 0;
    case WM_CTLCOLOREDIT:
      SetTextColor((HDC)wParam, crText);
      SetBkColor((HDC)wParam, crBack);
      return (LRESULT)hBackBsh;
    case WM_COMMAND: {
      int id = LOWORD(wParam);
      int event = HIWORD(wParam);

      switch (id) {
        case IDM_FILE_RELOAD:
          onFileReload(hWnd);
          break;
        case IDM_FILE_NEW:
          onFileNew(hWnd);
          break;
        case IDM_FILE_OPEN:
          onFileOpen(hWnd);
          break;
        case IDM_FILE_SAVE:
          onFileSave(hWnd);
          break;
        case IDM_FILE_SAVE_AS:
          onFileSaveAs(hWnd);
          break;
        case IDM_FILE_EXIT:
          PostMessage(hWnd, WM_CLOSE, 0, 0);
          break;
        case IDM_EDIT_UNDO:
          onEditUndo(hWnd);
          break;
        case IDM_EDIT_BEGIN_SELECT:
          onEditBeginSelect(hWnd);
          break;
        case IDM_EDIT_END_SELECT:
          onEditEndSelect(hWnd);
          break;
        case IDM_EDIT_CUT:
          onEditCut(hWnd);
          break;
        case IDM_EDIT_COPY:
          onEditCopy(hWnd);
          break;
        case IDM_EDIT_PASTE:
          onEditPaste(hWnd);
          break;
        case IDM_EDIT_CLEAR:
          onEditClear(hWnd);
          break;
        case IDM_EDIT_FIND:
          onEditFind(hWnd);
          break;
        case IDM_EDIT_FIND_NEXT:
          onEditFindNext(hWnd);
          break;
        case IDM_EDIT_SELECT_ALL:
          onEditSelectAll(hWnd);
          break;
        case IDM_TOOLS_LOOKUP:
          onToolsLookup(hWnd);
          break;
        case IDM_TOOLS_WORDWRAP:
          onToolsWordwrap(hWnd);
          break;
        case IDM_TOOLS_FONT:
          onToolsFont(hWnd);
          break;
        case IDM_TOOLS_TEXTCOLOR:
          onToolsTextColor(hWnd);
          break;
        case IDM_TOOLS_BACKCOLOR:
          onToolsBackColor(hWnd);
          break;
        case IDM_TOOLS_CHARSET_CP932:
          onToolsCharset(hWnd, CHARSET_CP932);
          break;
        case IDM_TOOLS_CHARSET_UTF8:
          onToolsCharset(hWnd, CHARSET_UTF8);
          break;
        case IDM_TOOLS_CHARSET_UTF16LE:
          onToolsCharset(hWnd, CHARSET_UTF16LE);
          break;
        case IDM_TOOLS_CHARSET_CP1252:
          onToolsCharset(hWnd, CHARSET_CP1252);
          break;
        case IDM_TOOLS_CHARSET_AUTODETECT:
          onToolsCharsetAutodetect(hWnd);
          break;
        case IDM_TOOLS_NEWLINECODE_CRLF:
          onToolsNewlineCode(hWnd, NEWLINE_CRLF);
          break;
        case IDM_TOOLS_NEWLINECODE_LF:
          onToolsNewlineCode(hWnd, NEWLINE_LF);
          break;
        case IDM_TOOLS_NEWLINECODE_CR:
          onToolsNewlineCode(hWnd, NEWLINE_CR);
          break;
        case IDM_TOOLS_ADDBOM:
          onToolsAddBOM(hWnd);
          break;
        case IDM_TOOLS_STATUSBAR:
          onToolsStatusBar(hWnd);
          break;
        case IDM_ABOUT:
          MessageBox(hWnd,
                     APP_NAME _T(" ") APP_VERSION _T("\n\n") APP_COPYRIGHT
                              _T("\n\n")
                              _T("Build date: ") _T(__DATE__),
                     _T("About this software"), MB_OK | MB_ICONINFORMATION);
          break;
      }

      return 0;
    }
  }

  return DefWindowProc(hWnd, msg, wParam, lParam);
}

static LRESULT CALLBACK editAreaProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
  MemoPadData *data = (MemoPadData *)GetWindowLong(GetParent(hWnd), GWL_USERDATA);

  switch (msg) {
    case WM_CHAR:
    case WM_KEYUP:
    case WM_KEYDOWN: {
      int keyCode = wParam;
      if (data->isSelectMode) {
        if (std::find(data->selectableKeys.begin(), data->selectableKeys.end(), keyCode) ==
            data->selectableKeys.end()) {
          return 0;
        }
      }

      break;
    }
  }

  return CallWindowProc(data->prevEditAreaProc, hWnd, msg, wParam, lParam);
}

static LRESULT CALLBACK fileNameLabelProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
  MemoPadData *data = (MemoPadData *)GetWindowLong(GetParent(GetParent(hWnd)), GWL_USERDATA);

  switch (msg) {
    case WM_PAINT: {
      PAINTSTRUCT paintStruct;
      HDC hDC = BeginPaint(hWnd, &paintStruct);

      TCHAR textCStr[256];
      GetWindowText(hWnd, textCStr, 256);

      RECT rect;
      GetClientRect(hWnd, &rect);

      PatBlt(hDC, 0, 0, rect.right, rect.bottom, PATCOPY);

      HPEN hPen = CreatePen(PS_SOLID, 1, 0x00808080);
      HPEN hPrevPen = (HPEN)SelectObject(hDC, hPen);

      MoveToEx(hDC, rect.right - 1, 0, NULL);
      LineTo(hDC, 0, 0);
      LineTo(hDC, 0, rect.bottom - 1);
      LineTo(hDC, rect.right - 1, rect.bottom - 1);
      LineTo(hDC, rect.right - 1, 0);

      SelectObject(hDC, hPrevPen);
      DeleteObject(hPen);

      DrawText(hDC, textCStr, -1, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

      EndPaint(hWnd, &paintStruct);

      return 0;
    }
  }

  return CallWindowProc(data->prevFileNameLabelProc, hWnd, msg, wParam, lParam);
}

static BOOL WINAPI findStringDlgProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam) {
  switch (msg) {
    case WM_INITDIALOG:
      onFindStringInitDialog(hDlg);
      return true;
    case WM_COMMAND: {
      int id = LOWORD(wParam);
      int event = HIWORD(wParam);

      switch (id) {
        case IDOK:
          onFindStringOk(hDlg);
          break;
        case IDCANCEL:
          onFindStringCancel(hDlg);
          break;
        case IDC_FIND_STRING_TARGET_BOX:
          onFindStringTargetBox(hDlg, event);
          break;
        default:
          return false;
      }

      return true;
    }
  }

  return false;
}

static void onCreate(HWND hWnd) {
  static int selectKeys[] = {VK_LEFT, VK_UP, VK_RIGHT, VK_DOWN, VK_PRIOR, VK_NEXT, VK_HOME, VK_END};

  int i;

  crBack = RGB(255, 255, 255);
  crText = RGB(0, 0, 0);
  cc.lStructSize = sizeof(CHOOSECOLOR);
  cc.hwndOwner = hWnd;
  cc.rgbResult = RGB(255, 255, 255);
  cc.lpCustColors = custColors;
  cc.Flags = CC_FULLOPEN | CC_RGBINIT;

  MemoPadData *data = new MemoPadData();
  SetWindowLong(hWnd, GWL_USERDATA, (long)data);

  data->hFindStringDialog = NULL;
  data->isSelectMode = false;
  data->isWordwrap = false;
  data->charset = CHARSET_CP932;
  data->willAddBOM = false;
  data->csAutoDetect = true;
  data->crlf = NEWLINE_CRLF;
  data->isStatusBar = true;

  InitCommonControls();

  data->hCommandBar = CommandBar_Create(g_hInstance, hWnd, 1);

  CommandBar_InsertMenubarEx(data->hCommandBar, g_hInstance, _T("MENU"), 0);
  CommandBar_AddBitmap(data->hCommandBar, HINST_COMMCTRL, IDB_STD_SMALL_COLOR, 15, 16, 16);
  CommandBar_AddButtons(data->hCommandBar, sizeof(tbSTDButton) / sizeof(TBBUTTON), tbSTDButton);
  CommandBar_AddAdornments(data->hCommandBar, 0, IDM_FILE_EXIT);
  HMENU hMenu = CommandBar_GetMenu(data->hCommandBar, 0);

  HWND hEditArea = CreateWindow(
      _T("EDIT"), _T(""),
      WS_CHILD | WS_BORDER | WS_HSCROLL | WS_VSCROLL | WS_VISIBLE | ES_AUTOHSCROLL | ES_AUTOVSCROLL | ES_MULTILINE,
      CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, hWnd, (HMENU)IDC_EDIT_AREA, g_hInstance, NULL);

  data->prevEditAreaProc = (WNDPROC)SetWindowLong(hEditArea, GWL_WNDPROC, (DWORD)editAreaProc);
  SendMessage(hEditArea, EM_LIMITTEXT, 65535, 0);
  g_hEditArea = hEditArea;

  HWND hFileNameLabel =
      CreateWindow(_T("STATIC"), _T(""), WS_CHILD | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                   CW_USEDEFAULT, data->hCommandBar, (HMENU)IDC_FILE_NAME_LABEL, g_hInstance, NULL);

  data->prevFileNameLabelProc = (WNDPROC)SetWindowLong(hFileNameLabel, GWL_WNDPROC, (DWORD)fileNameLabelProc);

  bool wordwrap = false;
  std::tstring propFileName = KnceUtil::getCurrentDirectory() + _T("\\settings.dat");
  TCHAR tctemp[256];

  if (GetFileAttributes(propFileName.c_str()) == -1) {
    data->hEditAreaFont = KnceUtil::createFont(_T(""), 90, false, false, KNCE_FONT_PITCH_FIXED);
  } else {
    std::map<std::tstring, std::tstring> props;
    KnceUtil::readPropertyFile(props, propFileName);

    std::tstring fontName = props[_T("editor.fontName")];
    int pointSize = _tcstoul(props[_T("editor.fontSize")].c_str(), NULL, 10);
    bool bold = _tcstol(props[_T("editor.isBold")].c_str(), NULL, 10) != 0;
    bool italic = _tcstol(props[_T("editor.isItalic")].c_str(), NULL, 10) != 0;
    data->hEditAreaFont = KnceUtil::createFont(fontName, pointSize, bold, italic);
    crText = _tcstoul(props[_T("editor.textColor")].c_str(), NULL, 10);
    crBack = _tcstoul(props[_T("editor.backColor")].c_str(), NULL, 10);
    data->charset = (charset_t)_tcstoul(props[_T("editor.charset")].c_str(), NULL, 10);
    if (data->charset > CHARSET_CP1252) data->charset = CHARSET_CP932;
    data->csAutoDetect = _tcstol(props[_T("editor.csAutoDetect")].c_str(), NULL, 10) != 0;
    data->willAddBOM = _tcstol(props[_T("editor.willAddBOM")].c_str(), NULL, 10) != 0;
    data->isStatusBar = _tcstol(props[_T("editor.isStatusBar")].c_str(), NULL, 10) != 0;
    wordwrap = _tcstol(props[_T("editor.isWordwrap")].c_str(), NULL, 10) != 0;
    for (i = 0; i < 16; i++) {
      wsprintf(tctemp, TEXT("editor.custColor%d"), i);
      custColors[i] = _tcstoul(props[tctemp].c_str(), NULL, 10);
    }
  }

  hBackBsh = CreateSolidBrush(crBack);
  SendMessage(hEditArea, WM_SETFONT, (WPARAM)data->hEditAreaFont, false);

  if (wordwrap) onToolsWordwrap(hWnd);

  int numKeys = sizeof(selectKeys) / sizeof(int);
  for (i = 0; i < numKeys; i++) data->selectableKeys.push_back(selectKeys[i]);

  KnceUtil::changeKeyRepeatSpeed(500, 30);

  openFile(hWnd, g_lpCmdLine, false);
  SetFocus(hEditArea);
}

static void onDestroy(HWND hWnd) {
  MemoPadData *data = (MemoPadData *)GetWindowLong(hWnd, GWL_USERDATA);

  if (data->isStatusBar) KillTimer(hWnd, 1);
  KnceUtil::restoreKeyRepeatSpeed();

  std::map<std::tstring, std::tstring> props;
  TCHAR valCStr[256], tctemp[256];

  std::tstring fontName;
  int pointSize = 0;
  bool bold = false;
  bool italic = false;

  KnceUtil::getFontAttributes(data->hEditAreaFont, fontName, pointSize, bold, italic);

  props[_T("editor.fontName")] = fontName;

  _sntprintf(valCStr, 256, _T("%d"), pointSize);
  props[_T("editor.fontSize")] = valCStr;

  props[_T("editor.isBold")] = bold ? _T("1") : _T("0");
  props[_T("editor.isItalic")] = italic ? _T("1") : _T("0");

  props[_T("editor.isWordwrap")] = data->isWordwrap ? _T("1") : _T("0");

  wsprintf(valCStr, TEXT("%lu"), crText);
  props[_T("editor.textColor")] = valCStr;
  wsprintf(valCStr, TEXT("%lu"), crBack);
  props[_T("editor.backColor")] = valCStr;
  wsprintf(valCStr, TEXT("%d"), data->charset);
  props[_T("editor.charset")] = valCStr;
  props[_T("editor.willAddBOM")] = data->willAddBOM ? _T("1") : _T("0");
  props[_T("editor.csAutoDetect")] = data->csAutoDetect ? _T("1") : _T("0");
  props[_T("editor.isStatusBar")] = data->isStatusBar ? _T("1") : _T("0");
  for (int i = 0; i < 16; i++) {
    wsprintf(valCStr, TEXT("%lu"), custColors[i]);
    wsprintf(tctemp, TEXT("editor.custColor%d"), i);
    props[tctemp] = valCStr;
  }

  std::tstring propFileName = KnceUtil::getCurrentDirectory() + _T("\\settings.dat");
  KnceUtil::writePropertyFile(propFileName, props);

  DeleteObject(data->hEditAreaFont);

  PostQuitMessage(0);

  delete data;
}

static void onActivate(HWND hWnd, int act) {
  MemoPadData *data = (MemoPadData *)GetWindowLong(hWnd, GWL_USERDATA);

  if (act == WA_ACTIVE || act == WA_CLICKACTIVE) {
    HWND hEditArea = GetDlgItem(hWnd, IDC_EDIT_AREA);
    SetFocus(hEditArea);
  } else if (data->isSelectMode) {
    onEditEndSelect(hWnd);
  }
}

static void onSize(HWND hWnd, int width, int height) {
  const int fileNameLabelWidth = 200;

  MemoPadData *data = (MemoPadData *)GetWindowLong(hWnd, GWL_USERDATA);

  MoveWindow(data->hCommandBar, 0, 0, 0, 0, TRUE);
  int cmdBarHeight = CommandBar_Height(data->hCommandBar);
  int fileNameLabelHeight = cmdBarHeight - 4;
  int statusBarHeight = 0;

  if (data->isStatusBar) {
    KillTimer(hWnd, 1);
    DestroyWindow(data->hStatusBar);
    INT iRight[] = {180, 240, 420};
    data->hStatusBar = CreateStatusWindow(WS_CHILD | WS_VISIBLE | CCS_BOTTOM, _T(""), hWnd, IDC_STATUS_BAR);
    SendMessage(data->hStatusBar, SB_SETPARTS, 3, (LPARAM)iRight);
    Row_bef = Col_bef = 0;
    Charset_bef = CRLF_bef = -1;
    SetTimer(hWnd, 1, 1000 / 30, updateStatusBar);

    RECT rect;
    SendMessage(data->hStatusBar, SB_GETRECT, 0, (LPARAM)&rect);
    INT BorderWidth[3];
    SendMessage(data->hStatusBar, SB_GETBORDERS, 0, (LPARAM)BorderWidth);
    statusBarHeight = rect.bottom - rect.top + BorderWidth[1];
  }

  HWND hFileNameLabel = GetDlgItem(data->hCommandBar, IDC_FILE_NAME_LABEL);
  MoveWindow(hFileNameLabel, width - fileNameLabelWidth - 30, (cmdBarHeight - fileNameLabelHeight) / 2,
             fileNameLabelWidth, fileNameLabelHeight, true);

  HWND hEditArea = GetDlgItem(hWnd, IDC_EDIT_AREA);
  MoveWindow(hEditArea, 0, cmdBarHeight, width, height - cmdBarHeight - statusBarHeight, true);
}

static void onInitMenuPopup(HWND hWnd) {
  MemoPadData *data = (MemoPadData *)GetWindowLong(hWnd, GWL_USERDATA);

  HWND hEditArea = GetDlgItem(hWnd, IDC_EDIT_AREA);

  unsigned int selection = SendMessage(hEditArea, EM_GETSEL, 0, 0);
  bool selected = LOWORD(selection) != HIWORD(selection);

  HMENU hMenu = CommandBar_GetMenu(data->hCommandBar, 0);

  EnableMenuItem(hMenu, IDM_EDIT_UNDO, SendMessage(hEditArea, EM_CANUNDO, 0, 0) ? MF_ENABLED : MF_GRAYED);
  EnableMenuItem(hMenu, IDM_EDIT_BEGIN_SELECT, !data->isSelectMode ? MF_ENABLED : MF_GRAYED);
  EnableMenuItem(hMenu, IDM_EDIT_END_SELECT, data->isSelectMode ? MF_ENABLED : MF_GRAYED);
  EnableMenuItem(hMenu, IDM_EDIT_CUT, selected ? MF_ENABLED : MF_GRAYED);
  EnableMenuItem(hMenu, IDM_EDIT_COPY, selected ? MF_ENABLED : MF_GRAYED);
  EnableMenuItem(hMenu, IDM_EDIT_PASTE, IsClipboardFormatAvailable(CF_UNICODETEXT) ? MF_ENABLED : MF_GRAYED);
  EnableMenuItem(hMenu, IDM_EDIT_CLEAR, selected ? MF_ENABLED : MF_GRAYED);
  EnableMenuItem(hMenu, IDM_TOOLS_ADDBOM, MF_BYCOMMAND | (data->charset == CHARSET_UTF8 ? MF_ENABLED : MF_GRAYED));

  CheckMenuItem(hMenu, IDM_TOOLS_WORDWRAP, MF_BYCOMMAND | (data->isWordwrap ? MF_CHECKED : MF_UNCHECKED));
  CheckMenuRadioItem(hMenu, IDM_TOOLS_CHARSET_CP932, IDM_TOOLS_CHARSET_CP1252, IDM_TOOLS_CHARSET_CP932 + data->charset,
                     MF_BYCOMMAND);
  CheckMenuRadioItem(hMenu, IDM_TOOLS_NEWLINECODE_CRLF, IDM_TOOLS_NEWLINECODE_CR,
                     IDM_TOOLS_NEWLINECODE_CRLF + data->crlf, MF_BYCOMMAND);
  CheckMenuItem(hMenu, IDM_TOOLS_CHARSET_AUTODETECT, MF_BYCOMMAND | (data->csAutoDetect ? MF_CHECKED : MF_UNCHECKED));
  CheckMenuItem(hMenu, IDM_TOOLS_ADDBOM, MF_BYCOMMAND | (data->willAddBOM ? MF_CHECKED : MF_UNCHECKED));
  CheckMenuItem(hMenu, IDM_TOOLS_STATUSBAR, MF_BYCOMMAND | (data->isStatusBar ? MF_CHECKED : MF_UNCHECKED));
}

static void CALLBACK updateStatusBar(HWND hWnd, UINT msg, UINT idTimer, DWORD dwTime) {
  MemoPadData *data = (MemoPadData *)GetWindowLong(hWnd, GWL_USERDATA);

  INT CharIndex = LOWORD(SendMessage(g_hEditArea, EM_GETSEL, 0, 0));
  INT Row = (INT)SendMessage(g_hEditArea, EM_LINEFROMCHAR, -1, 0);
  INT RowStart = (INT)SendMessage(g_hEditArea, EM_LINEINDEX, Row, 0);
  if (Row_bef != Row + 1 || Col_bef != CharIndex - RowStart + 1) {
    TCHAR szBuf[64];
    wsprintf(szBuf, TEXT("Line: %d Column: %d"), Row + 1, CharIndex - RowStart + 1);
    SendMessage(data->hStatusBar, SB_SETTEXT, 0, (LPARAM)szBuf);
    Row_bef = Row + 1;
    Col_bef = CharIndex - RowStart + 1;
  }

  if (data->crlf != CRLF_bef) {
    if (data->crlf == NEWLINE_CRLF) {
      SendMessage(data->hStatusBar, SB_SETTEXT, 1, (LPARAM) _T("CRLF"));
    } else if (data->crlf == NEWLINE_LF) {
      SendMessage(data->hStatusBar, SB_SETTEXT, 1, (LPARAM) _T("LF"));
    } else {
      SendMessage(data->hStatusBar, SB_SETTEXT, 1, (LPARAM) _T("CR"));
    }
    CRLF_bef = data->crlf;
  }

  if (data->charset != Charset_bef || data->willAddBOM != AddBOM_bef) {
    if (data->charset == CHARSET_CP932) {
      SendMessage(data->hStatusBar, SB_SETTEXT, 2, (LPARAM) _T("Shift_JIS (CP932)"));
    } else if (data->charset == CHARSET_UTF8) {
      SendMessage(data->hStatusBar, SB_SETTEXT, 2,
                  data->willAddBOM ? (LPARAM) _T("UTF-8 BOM (Unicode)") : (LPARAM) _T("UTF-8 (Unicode)"));
    } else if (data->charset == CHARSET_UTF16LE) {
      SendMessage(data->hStatusBar, SB_SETTEXT, 2, (LPARAM) _T("UTF-16 LE (Unicode)"));
    } else if (data->charset == CHARSET_CP1252) {
      SendMessage(data->hStatusBar, SB_SETTEXT, 2, (LPARAM) _T("CP1252 (ANSI)"));
    }
    Charset_bef = data->charset;
    AddBOM_bef = data->willAddBOM;
  }
}

static void onFileReload(HWND hWnd) {
  if (!checkSaving(hWnd)) return;

  MemoPadData *data = (MemoPadData *)GetWindowLong(hWnd, GWL_USERDATA);
  openFile(hWnd, data->editFileName, true);
}

static void onFileNew(HWND hWnd) {
  if (!checkSaving(hWnd)) return;

  openFile(hWnd, _T(""), false);
}

static void onFileOpen(HWND hWnd) {
  MemoPadData *data = (MemoPadData *)GetWindowLong(hWnd, GWL_USERDATA);

  if (!checkSaving(hWnd)) return;

  KnceChooseFileParams params = {0};
  params.isSaveFile = false;
  params.filters = _T("Text files (*.txt)|*.txt|All files (*.*)|*.*");
  _tcsncpy(params.fileName, data->editFileName.c_str(), MAX_PATH);

  if (!knceChooseFile(hWnd, &params)) return;

  std::tstring fileName = params.fileName;

  openFile(hWnd, fileName, false);
}

static void onFileSave(HWND hWnd) { saveFile(hWnd); }

static void onFileSaveAs(HWND hWnd) { saveFile(hWnd, false); }

static void onEditUndo(HWND hWnd) {
  HWND hEditArea = GetDlgItem(hWnd, IDC_EDIT_AREA);
  SendMessage(hEditArea, EM_UNDO, 0, 0);
}

static void onEditBeginSelect(HWND hWnd) {
  MemoPadData *data = (MemoPadData *)GetWindowLong(hWnd, GWL_USERDATA);

  data->isSelectMode = true;

  keybd_event(VK_SHIFT, 0, 0, 0);
}

static void onEditEndSelect(HWND hWnd) {
  MemoPadData *data = (MemoPadData *)GetWindowLong(hWnd, GWL_USERDATA);

  data->isSelectMode = false;

  keybd_event(VK_SHIFT, 0, KEYEVENTF_KEYUP, 0);
}

static void onEditCut(HWND hWnd) {
  onEditEndSelect(hWnd);

  HWND hEditArea = GetDlgItem(hWnd, IDC_EDIT_AREA);
  SendMessage(hEditArea, WM_CUT, 0, 0);
}

static void onEditCopy(HWND hWnd) {
  onEditEndSelect(hWnd);

  HWND hEditArea = GetDlgItem(hWnd, IDC_EDIT_AREA);
  SendMessage(hEditArea, WM_COPY, 0, 0);
}

static void onEditPaste(HWND hWnd) {
  onEditEndSelect(hWnd);

  HWND hEditArea = GetDlgItem(hWnd, IDC_EDIT_AREA);
  SendMessage(hEditArea, WM_PASTE, 0, 0);
}

static void onEditClear(HWND hWnd) {
  onEditEndSelect(hWnd);

  HWND hEditArea = GetDlgItem(hWnd, IDC_EDIT_AREA);
  SendMessage(hEditArea, WM_CLEAR, 0, 0);
}

static void onEditFind(HWND hWnd) {
  int ret = DialogBox(g_hInstance, _T("FIND_STRING"), hWnd, (DLGPROC)findStringDlgProc);
  if (ret == IDCANCEL) return;

  findString(hWnd);
}

static void onEditFindNext(HWND hWnd) {
  MemoPadData *data = (MemoPadData *)GetWindowLong(hWnd, GWL_USERDATA);

  if (data->findTargetString.empty()) {
    int ret = DialogBox(g_hInstance, _T("FIND_STRING"), hWnd, (DLGPROC)findStringDlgProc);
    if (ret == IDCANCEL) return;
  }

  findString(hWnd);
}

static void onEditSelectAll(HWND hWnd) {
  HWND hEditArea = GetDlgItem(hWnd, IDC_EDIT_AREA);
  SendMessage(hEditArea, EM_SETSEL, 0, -1);
}

static void onToolsLookup(HWND hWnd) {
  int i;

  HWND hEditArea = GetDlgItem(hWnd, IDC_EDIT_AREA);

  int editorSize = GetWindowTextLengthW(hEditArea) + 1;
  TCHAR *editBuff = (TCHAR *)malloc(sizeof(TCHAR) * editorSize);
  GetWindowText(hEditArea, editBuff, editorSize);

  unsigned int selection = SendMessage(hEditArea, EM_GETSEL, 0, 0);
  int from = LOWORD(selection);
  int to = HIWORD(selection);
  int len = to - from;

  if (len > 20) len = 20;

  std::tstring selectedWord(from + editBuff, len);
  free(editBuff);

  std::tstring lookupWord;
  for (i = 0; i < len; i++) {
    TCHAR ch = selectedWord[i];
    if ((ch >= _T('A') && ch <= _T('Z')) || ch >= _T('a') && ch <= _T('z')) {
      lookupWord += _totlower(ch);
    }
  }

  if (lookupWord.empty()) return;

  HWND hEdWindow = FindWindow(_T("SHARP SIM"), NULL);
  if (hEdWindow == NULL) return;

  SendMessage(hEdWindow, g_dicKeyDownMessage, 0x4003, 0);

  len = lookupWord.length();
  for (i = 0; i < len; i++) {
    int code = lookupWord[i] - 'a' + 0x1000;
    SendMessage(hEdWindow, g_dicKeyDownMessage, code, 0);
  }

  SendMessage(hEdWindow, g_dicKeyDownMessage, 0x2008, 0);

  SetForegroundWindow(hEdWindow);
}

static void onToolsStatusBar(HWND hWnd) {
  MemoPadData *data = (MemoPadData *)GetWindowLong(hWnd, GWL_USERDATA);
  data->isStatusBar = !data->isStatusBar;
  if (!data->isStatusBar) {
    KillTimer(hWnd, 1);
    DestroyWindow(data->hStatusBar);
  }

  RECT rect;
  GetClientRect(hWnd, &rect);
  SendMessage(hWnd, WM_SIZE, SIZE_RESTORED, ((rect.bottom - rect.top) << 16) + (rect.right - rect.left));
}

static void onToolsWordwrap(HWND hWnd) {
  MemoPadData *data = (MemoPadData *)GetWindowLong(hWnd, GWL_USERDATA);

  data->isWordwrap = !data->isWordwrap;

  HWND hEditArea = GetDlgItem(hWnd, IDC_EDIT_AREA);

  int editBuffSize = GetWindowTextLength(hEditArea) + 1;
  TCHAR *editBuff = new TCHAR[editBuffSize];
  GetWindowText(hEditArea, editBuff, editBuffSize);

  int modified = SendMessage(hEditArea, EM_GETMODIFY, 0, 0);

  DestroyWindow(hEditArea);

  hEditArea = CreateWindow(_T("EDIT"), _T(""),
                           WS_CHILD | WS_BORDER | WS_VSCROLL | WS_VISIBLE | ES_AUTOVSCROLL |
                               (data->isWordwrap ? 0 : WS_HSCROLL | ES_AUTOHSCROLL) | ES_MULTILINE,
                           CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, hWnd, (HMENU)IDC_EDIT_AREA,
                           g_hInstance, NULL);

  data->prevEditAreaProc = (WNDPROC)SetWindowLong(hEditArea, GWL_WNDPROC, (DWORD)editAreaProc);
  SendMessage(hEditArea, EM_LIMITTEXT, 65535, 0);
  g_hEditArea = hEditArea;

  SendMessage(hEditArea, WM_SETFONT, (WPARAM)data->hEditAreaFont, true);

  RECT rect;
  GetClientRect(hWnd, &rect);
  onSize(hWnd, rect.right - rect.left, rect.bottom - rect.top);

  SetWindowText(hEditArea, editBuff);

  delete[] editBuff;

  SendMessage(hEditArea, EM_SETMODIFY, modified != 0, 0);

  SetFocus(hEditArea);
}

static void onToolsFont(HWND hWnd) {
  MemoPadData *data = (MemoPadData *)GetWindowLong(hWnd, GWL_USERDATA);

  KnceChooseFontParams params = {0};
  params.hFont = data->hEditAreaFont;

  if (knceChooseFont(hWnd, &params)) {
    DeleteObject(data->hEditAreaFont);

    data->hEditAreaFont = params.hFont;

    HWND hEditArea = GetDlgItem(hWnd, IDC_EDIT_AREA);
    SendMessage(hEditArea, WM_SETFONT, (WPARAM)data->hEditAreaFont, true);
  }
}

static void onToolsTextColor(HWND hWnd) {
  if (!ChooseColor(&cc)) return;
  crText = cc.rgbResult;
  InvalidateRect(g_hEditArea, NULL, TRUE);
}

static void onToolsBackColor(HWND hWnd) {
  if (!ChooseColor(&cc)) return;
  crBack = cc.rgbResult;
  DeleteObject(hBackBsh);
  hBackBsh = CreateSolidBrush(cc.rgbResult);
  InvalidateRect(g_hEditArea, NULL, TRUE);
}

static void onToolsCharset(HWND hWnd, charset_t charset) {
  MemoPadData *data = (MemoPadData *)GetWindowLong(hWnd, GWL_USERDATA);
  data->charset = charset;
}

static void onToolsCharsetAutodetect(HWND hWnd) {
  MemoPadData *data = (MemoPadData *)GetWindowLong(hWnd, GWL_USERDATA);
  data->csAutoDetect = !data->csAutoDetect;
}

static void onToolsNewlineCode(HWND hWnd, newline_t newlineCode) {
  MemoPadData *data = (MemoPadData *)GetWindowLong(hWnd, GWL_USERDATA);
  data->crlf = newlineCode;
}

static void onToolsAddBOM(HWND hWnd) {
  MemoPadData *data = (MemoPadData *)GetWindowLong(hWnd, GWL_USERDATA);
  data->willAddBOM = !data->willAddBOM;
}

static void onFindStringInitDialog(HWND hDlg) {
  MemoPadData *data = (MemoPadData *)GetWindowLong(GetWindow(hDlg, GW_OWNER), GWL_USERDATA);

  KnceUtil::adjustDialogLayout(hDlg);

  HWND hTargetBox = GetDlgItem(hDlg, IDC_FIND_STRING_TARGET_BOX);

  SetWindowText(hTargetBox, data->findTargetString.c_str());
  SendMessage(hTargetBox, EM_SETSEL, 0, -1);
}

static void onFindStringOk(HWND hDlg) {
  MemoPadData *data = (MemoPadData *)GetWindowLong(GetWindow(hDlg, GW_OWNER), GWL_USERDATA);

  HWND hTargetBox = GetDlgItem(hDlg, IDC_FIND_STRING_TARGET_BOX);

  TCHAR findTargetCStr[1024];
  GetWindowText(hTargetBox, findTargetCStr, 1024);
  data->findTargetString = findTargetCStr;

  EndDialog(hDlg, IDOK);
}

static void onFindStringCancel(HWND hDlg) { EndDialog(hDlg, IDCANCEL); }

static void onFindStringTargetBox(HWND hDlg, int event) {
  if (event != EN_CHANGE) return;

  HWND hOkButton = GetDlgItem(hDlg, IDOK);
  HWND hTargetBox = GetDlgItem(hDlg, IDC_FIND_STRING_TARGET_BOX);

  EnableWindow(hOkButton, GetWindowTextLength(hTargetBox) > 0);
}

static void openFile(HWND hWnd, const std::tstring &fileName, bool isReload) {
  MemoPadData *data = (MemoPadData *)GetWindowLong(hWnd, GWL_USERDATA);

  HWND hEditArea = GetDlgItem(hWnd, IDC_EDIT_AREA);

  if (fileName.empty()) {
    SetWindowText(hEditArea, _T(""));
    SendMessage(hEditArea, EM_SETMODIFY, false, 0);
    data->editFileName = fileName;
    updateFileNameLabel(hWnd);
    return;
  }

  HANDLE hFile =
      CreateFile(fileName.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
  if (hFile == INVALID_HANDLE_VALUE) {
    MessageBox(hWnd, (_T("Cannot open file: ") + fileName).c_str(), _T("Error"), MB_ICONEXCLAMATION);
    return;
  }

  DWORD dwTemp, dwSize;
  dwSize = GetFileSize(hFile, NULL);
  if (dwSize > 65535 * 2) {
    MessageBox(hWnd, (_T("Too large file: ") + fileName).c_str(), _T("Error"), MB_ICONEXCLAMATION);
    return;
  }

  // Determine the charset
  if (data->csAutoDetect && !isReload) {
    CHAR fileBuf[3] = {0};
    ReadFile(hFile, fileBuf, 3, &dwTemp, NULL);
    if (dwSize >= 3 && fileBuf[0] == '\xEF' && fileBuf[1] == '\xBB' && fileBuf[2] == '\xBF') {
      data->charset = CHARSET_UTF8;
    } else if (dwSize >= 2 && fileBuf[0] == '\xFF' && fileBuf[1] == '\xFE') {
      data->charset = CHARSET_UTF16LE;
    } else if (data->charset == CHARSET_UTF16LE) {
      data->charset = CHARSET_CP932;
    }
    SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
  }
  
  bool isTooLarge = false;
  std::wstring wstrBuf;
  switch (data->charset) {
    case CHARSET_CP932: {
      PSTR fileBuf = (PSTR)calloc(dwSize + 1, sizeof(CHAR));
      ReadFile(hFile, fileBuf, dwSize, &dwTemp, NULL);

      INT length = MultiByteToWideChar(932, 0, fileBuf, dwSize, NULL, 0);
      if (length > 65535) {
        isTooLarge = true;
        free(fileBuf);
        break;
      }
      PWSTR buf = (PWSTR)calloc(length + 1, sizeof(WCHAR));
      MultiByteToWideChar(932, 0, fileBuf, dwSize, buf, length);
      free(fileBuf);
      wstrBuf = buf;
      free(buf);

      data->crlf = convertCRLF(wstrBuf, NEWLINE_CRLF);
      break;
    }

    case CHARSET_UTF8: {
      PSTR fileBuf = (PSTR)calloc(dwSize + 1, sizeof(CHAR));
      ReadFile(hFile, fileBuf, dwSize, &dwTemp, NULL);

      int padding;
      if (fileBuf[0] == '\xEF' && fileBuf[1] == '\xBB' && fileBuf[2] == '\xBF') {
        padding = 3;
        data->willAddBOM = true;
      } else {
        padding = 0;
        data->willAddBOM = false;
      }

      INT length = MultiByteToWideChar(CP_UTF8, 0, fileBuf + padding, dwSize - padding, NULL, 0);
      if (length > 65535) {
        isTooLarge = true;
        free(fileBuf);
        break;
      }
      PWSTR buf = (PWSTR)calloc(length + 1, sizeof(WCHAR));
      MultiByteToWideChar(CP_UTF8, 0, fileBuf + padding, dwSize - padding, buf, length);
      free(fileBuf);
      wstrBuf = buf;
      free(buf);

      data->crlf = convertCRLF(wstrBuf, NEWLINE_CRLF);
      break;
    }

    case CHARSET_UTF16LE: {
      PWSTR buf = (PWSTR)calloc(dwSize + 1, sizeof(WCHAR));
      ReadFile(hFile, buf, dwSize, &dwTemp, NULL);
      wstrBuf = buf + (buf[0] == L'\xFEFF');  // Skip a BOM by pointer addition
      free(buf);

      data->crlf = convertCRLF(wstrBuf, NEWLINE_CRLF);
      break;
    }

    default: {
      PSTR fileBuf = (PSTR)calloc(dwSize + 1, sizeof(CHAR));
      ReadFile(hFile, fileBuf, dwSize, &dwTemp, NULL);

      INT length = MultiByteToWideChar(1252, 0, fileBuf, dwSize, NULL, 0);
      if (length > 65535) {
        isTooLarge = true;
        free(fileBuf);
        break;
      }
      PWSTR buf = (PWSTR)calloc(length + 1, sizeof(WCHAR));
      MultiByteToWideChar(1252, 0, fileBuf, dwSize, buf, length);
      free(fileBuf);
      wstrBuf = buf;
      free(buf);

      data->crlf = convertCRLF(wstrBuf, NEWLINE_CRLF);
    }
  }

  CloseHandle(hFile);
  if (isTooLarge) {
    MessageBox(hWnd, (_T("Too large file: ") + fileName).c_str(), _T("Error"), MB_ICONEXCLAMATION);
    return;
  }

  SetWindowText(hEditArea, wstrBuf.c_str());
  SendMessage(hEditArea, EM_SETMODIFY, false, 0);
  data->editFileName = fileName;
  updateFileNameLabel(hWnd);
}

static bool saveFile(HWND hWnd, bool isOverwrite) {
  MemoPadData *data = (MemoPadData *)GetWindowLong(hWnd, GWL_USERDATA);

  std::tstring fileName = data->editFileName;
  if (!isOverwrite || data->editFileName.empty()) {
    KnceChooseFileParams params = {0};
    params.isSaveFile = true;
    params.filters = _T("Text files (*.txt)|*.txt|All files (*.*)|*.*");
    _tcsncpy(params.fileName, fileName.c_str(), MAX_PATH);
    if (!knceChooseFile(hWnd, &params)) return false;
    fileName = params.fileName;
  }

  HWND hEditArea = GetDlgItem(hWnd, IDC_EDIT_AREA);
  int editorSize = GetWindowTextLengthW(hEditArea) + 1;
  WCHAR *editorBuf = (WCHAR *)malloc(sizeof(WCHAR) * editorSize);
  GetWindowText(hEditArea, editorBuf, editorSize);
  std::wstring wstrBuf = editorBuf;
  free(editorBuf);

  HANDLE hFile =
      CreateFile(fileName.c_str(), GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
  if (hFile == INVALID_HANDLE_VALUE) {
    MessageBox(hWnd, (_T("Cannot write file: ") + fileName).c_str(), _T("Error"), MB_OK | MB_ICONWARNING);
    return false;
  }

  if (data->charset == CHARSET_UTF8 && data->willAddBOM) {
    DWORD dwtemp;
    WriteFile(hFile, "\xEF\xBB\xBF", sizeof("\xEF\xBB\xBF") - sizeof(CHAR), &dwtemp, NULL);
  }

  if (data->charset == CHARSET_UTF16LE) {
    DWORD dwtemp;
    WriteFile(hFile, L"\xFEFF", sizeof(L"\xFEFF") - sizeof(WCHAR), &dwtemp, NULL);
  }

  if (data->crlf != NEWLINE_CRLF) convertCRLF(wstrBuf, data->crlf);

  switch (data->charset) {
    case CHARSET_CP932: {
      INT length = WideCharToMultiByte(932, 0, wstrBuf.c_str(), -1, NULL, 0, NULL, NULL);
      PSTR mbBuf = (PSTR)malloc(length * sizeof(CHAR));
      WideCharToMultiByte(932, 0, wstrBuf.c_str(), -1, mbBuf, length, NULL, NULL);

      DWORD dwTemp;
      WriteFile(hFile, mbBuf, length - 1, &dwTemp, NULL);

      free(mbBuf);
      break;
    }

    case CHARSET_UTF8: {
      INT length = WideCharToMultiByte(CP_UTF8, 0, wstrBuf.c_str(), -1, NULL, 0, NULL, NULL);
      PSTR mbBuf = (PSTR)malloc(length * sizeof(CHAR));
      WideCharToMultiByte(CP_UTF8, 0, wstrBuf.c_str(), -1, mbBuf, length, NULL, NULL);

      DWORD dwTemp;
      WriteFile(hFile, mbBuf, length - 1, &dwTemp, NULL);

      free(mbBuf);
      break;
    }

    case CHARSET_UTF16LE: {
      DWORD dwTemp;
      WriteFile(hFile, wstrBuf.c_str(), wstrBuf.length() * sizeof(WCHAR), &dwTemp, NULL);
      break;
    }

    default: {
      INT length = WideCharToMultiByte(1252, 0, wstrBuf.c_str(), -1, NULL, 0, NULL, NULL);
      PSTR mbBuf = (PSTR)malloc(length * sizeof(CHAR));
      WideCharToMultiByte(1252, 0, wstrBuf.c_str(), -1, mbBuf, length, NULL, NULL);

      DWORD dwTemp;
      WriteFile(hFile, mbBuf, length - 1, &dwTemp, NULL);

      free(mbBuf);
    }
  }

  CloseHandle(hFile);
  SendMessage(hEditArea, EM_SETMODIFY, false, 0);

  data->editFileName = fileName;
  updateFileNameLabel(hWnd);

  return true;
}

static bool checkSaving(HWND hWnd) {
  MemoPadData *data = (MemoPadData *)GetWindowLong(hWnd, GWL_USERDATA);

  HWND hEditArea = GetDlgItem(hWnd, IDC_EDIT_AREA);
  if (SendMessage(hEditArea, EM_GETMODIFY, 0, 0) == 0) return true;

  std::tstring fileName;
  if (data->editFileName.empty())
    fileName = _T("untitled");
  else {
    fileName = data->editFileName.substr(data->editFileName.rfind(_T('\\')) + 1);
  }

  std::tstring msg = _T("Save changes to '") + fileName + _T("'?");

  int ret = MessageBox(hWnd, msg.c_str(), _T("Confirm"), MB_YESNOCANCEL | MB_ICONQUESTION);

  return ret == IDCANCEL ? false : ret == IDYES ? saveFile(hWnd) : true;
}

static void updateFileNameLabel(HWND hWnd) {
  MemoPadData *data = (MemoPadData *)GetWindowLong(hWnd, GWL_USERDATA);

  std::tstring fileName;
  if (!data->editFileName.empty()) {
    fileName = data->editFileName.substr(data->editFileName.rfind(_T('\\')) + 1);
  }

  HWND hFileNameLabel = GetDlgItem(data->hCommandBar, IDC_FILE_NAME_LABEL);
  if (fileName.empty()) {
    SetWindowText(hFileNameLabel, _T("KN MemoPad"));
    SetWindowText(hWnd, _T("KN MemoPad"));
  } else {
    SetWindowText(hFileNameLabel, fileName.c_str());
    SetWindowText(hWnd, (fileName + _T(" - KN MemoPad")).c_str());
  }
}

static void findString(HWND hWnd) {
  MemoPadData *data = (MemoPadData *)GetWindowLong(hWnd, GWL_USERDATA);

  HWND hEditArea = GetDlgItem(hWnd, IDC_EDIT_AREA);

  int editorSize = GetWindowTextLengthW(hEditArea) + 1;
  TCHAR *editBuff = (TCHAR *)malloc(sizeof(TCHAR) * editorSize);
  GetWindowText(hEditArea, editBuff, editorSize);

  int count = GetWindowTextLength(hEditArea);

  unsigned int selection = SendMessage(hEditArea, EM_GETSEL, 0, 0);
  int pos = LOWORD(selection);

  if (pos < count) pos++;

  const TCHAR *foundPtr = _tcscasestr(editBuff + pos, data->findTargetString.c_str());

  if (foundPtr == NULL) {
    foundPtr = _tcscasestr(editBuff, data->findTargetString.c_str());

    if (foundPtr == NULL) {
      std::tstring msg = _T("Cannot find string: ") + data->findTargetString;
      MessageBox(hWnd, msg.c_str(), _T("Error"), MB_ICONEXCLAMATION);

      return;
    }
  }

  pos = foundPtr - editBuff;
  free(editBuff);
  SendMessage(hEditArea, EM_SETSEL, pos, pos + data->findTargetString.length());
  SendMessage(hEditArea, EM_SCROLLCARET, 0, 0);
}

static const TCHAR *_tcscasestr(const TCHAR *str, const TCHAR *strSearch) {
  int strpos = 0, strSearchpos = 0;

  while (str[strpos] != _T('\0')) {
    if (tolower(str[strpos]) == tolower(strSearch[0])) {
      strSearchpos = 1;

      while ((strSearch[strSearchpos]) && (tolower(str[strpos + strSearchpos]) == tolower(strSearch[strSearchpos]))) {
        ++strSearchpos;
      }

      if (!strSearch[strSearchpos]) return str + strpos;
    }

    ++strpos;
  }

  return NULL;
}

// Translates newlines and returns the previous newline code.
// `_target`: A `std::tstring` to operate on, `_newLine`: A desired newline code.
static newline_t convertCRLF(std::tstring &target, newline_t newLine) {
  std::tstring::iterator iter = target.begin();
  std::tstring::iterator iterEnd = target.end();
  std::tstring temp;
  size_t CRs = 0, LFs = 0, CRLFs = 0;
  const wchar_t *nl = newLine == NEWLINE_LF ? L"\n" : newLine == NEWLINE_CR ? L"\r" : L"\r\n";

  if (0 < target.size()) {
    wchar_t bNextChar = *iter++;

    while (true) {
      if (L'\r' == bNextChar) {
        temp += nl;                  // Newline
        if (iter == iterEnd) break;  // EOF
        bNextChar = *iter++;         // Retrive a character
        if (L'\n' == bNextChar) {
          if (iter == iterEnd) break;  // EOF
          bNextChar = *iter++;         // Retrive a character
          CRLFs++;
        } else {
          CRs++;
        }
      } else if (L'\n' == bNextChar) {
        temp += nl;                    // Newline
        if (iter == iterEnd) break;    // EOF
        bNextChar = *iter++;           // Retrive a character
        if (L'\r' == bNextChar) {      // Broken LFCR, so don't count
          if (iter == iterEnd) break;  // EOF
          bNextChar = *iter++;         // Retrive a character
        } else {
          LFs++;
        }
      } else {
        temp += bNextChar;           // Not a newline
        if (iter == iterEnd) break;  // EOF
        bNextChar = *iter++;         // Retrive a character
      }
    }
  }

  target = temp;

  return LFs > CRLFs && LFs >= CRs ? NEWLINE_LF : CRs > LFs && CRs > CRLFs ? NEWLINE_CR : NEWLINE_CRLF;
}

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPTSTR lpCmd, int nShow) {
  const TCHAR *className = _T("KNMemoPad");

  g_hInstance = hInst;
  g_lpCmdLine = lpCmd;

  WNDCLASS windowClass;
  memset(&windowClass, 0, sizeof(WNDCLASS));
  windowClass.style = CS_HREDRAW | CS_VREDRAW;
  windowClass.lpfnWndProc = wndProc;
  windowClass.hInstance = hInst;
  windowClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
  windowClass.lpszClassName = className;
  RegisterClass(&windowClass);

  HWND hWnd = CreateWindow(className, _T("KN MemoPad"), WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                           CW_USEDEFAULT, NULL, NULL, hInst, NULL);

  ACCEL accel;
  accel.fVirt = FVIRTKEY;
  accel.key = HARD_KEY_CONTENTS2;
  accel.cmd = IDM_TOOLS_LOOKUP;

  HACCEL hAccel = CreateAcceleratorTable(&accel, 1);

  ShowWindow(hWnd, SW_SHOWMAXIMIZED);
  UpdateWindow(hWnd);

  MSG msg;
  while (GetMessage(&msg, NULL, 0, 0)) {
    if (!TranslateAccelerator(hWnd, hAccel, &msg)) {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
  }

  DestroyAcceleratorTable(hAccel);

  UnregisterClass(className, hInst);

  return msg.wParam;
}
