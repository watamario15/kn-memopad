#include <algorithm>
#include <windows.h>
#include <tchar.h>
#include <commctrl.h>
#include <commdlg.h>
#include "knceutil-0.12/knceutil.h"
#include "kncedlg-0.10/kncedlg.h"

#ifdef UNICODE
namespace std { typedef wstring tstring; }
#else
namespace std { typedef string tstring; }
#endif

using namespace std;

enum {
    IDC_EDIT_AREA = 101,
    IDC_FILE_NAME_LABEL = 102,
    IDM_FILE_NEW = 201,
    IDM_FILE_OPEN = 202,
    IDM_FILE_SAVE = 203,
    IDM_FILE_SAVE_AS = 204,
    IDM_FILE_EXIT = 205,
    IDM_EDIT_UNDO = 221,
    IDM_EDIT_BEGIN_SELECT = 222,
    IDM_EDIT_END_SELECT = 223,
    IDM_EDIT_CUT = 224,
    IDM_EDIT_COPY = 225,
    IDM_EDIT_PASTE = 226,
    IDM_EDIT_CLEAR = 227,
    IDM_EDIT_FIND = 228,
    IDM_EDIT_FIND_NEXT = 229,
    IDM_EDIT_SELECT_ALL = 230,
    IDM_TOOLS_LOOKUP = 241,
    IDM_TOOLS_WORDWRAP = 242,
    IDM_TOOLS_FONT = 243,
    IDM_TOOLS_TEXTCOLOR = 244,
    IDM_TOOLS_BACKCOLOR = 245,
    IDC_FIND_STRING_TARGET_BOX = 101,
    MAX_EDIT_BUFFER = 1024 * 64
};
typedef BOOL (*DLL_ChooseColor)(LPCHOOSECOLOR lpcc);

static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);
static LRESULT CALLBACK editAreaProc(HWND hWnd, UINT msg, WPARAM wParam,
    LPARAM lParam);
static LRESULT CALLBACK fileNameLabelProc(HWND hWnd, UINT msg, WPARAM wParam,
    LPARAM lParam);
static BOOL WINAPI findStringDlgProc(HWND hDlg, UINT msg, WPARAM wParam,
    LPARAM lParam);
static void onCreate(HWND hWnd);
static void onDestroy(HWND hWnd);
static void onActivate(HWND hWnd, int act);
static void onSize(HWND hWnd, int width, int height);
static void onInitMenuPopup(HWND hWnd);
static void onKeyDown(HWND hWnd);
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
static void onFindStringInitDialog(HWND hDlg);
static void onFindStringOk(HWND hDlg);
static void onFindStringCancel(HWND hDlg);
static void onFindStringTargetBox(HWND hDlg, int event);
static void openFile(HWND hWnd, const tstring &fileName);
static bool saveFile(HWND hWnd, bool isOverwrite = true);
static bool checkSaving(HWND hWnd);
static void updateFileNameLabel(HWND hWnd);
static void findString(HWND hWnd);
static const TCHAR *_tcscasestr(const TCHAR *str, const TCHAR *strSearch);

HINSTANCE g_hInstance = NULL;
HWND g_hEditArea;
CHOOSECOLOR cc = {0};
COLORREF CustColors[16], crText, crBack;
HBRUSH hBackBsh;
DLL_ChooseColor dll_ChooseColor;

static unsigned int g_dicKeyDownMessage =
    RegisterWindowMessage(_T("DicKeyDown"));

struct MemoPadData {
    HWND hFindStringDialog;
    HWND hCommandBar;
    HFONT hEditAreaFont;
    WNDPROC prevEditAreaProc;
    WNDPROC prevFileNameLabelProc;
    tstring editFileName;
    vector<int> selectableKeys;
    bool isSelectMode;
    bool isWordwrap;
    tstring findTargetString;
};

static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam,
    LPARAM lParam) {

	switch (msg) {
    case WM_CREATE:
        onCreate(hWnd);
        return 0;
    case WM_CLOSE:
        if (!checkSaving(hWnd))
            return 0;
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
    case WM_COMMAND:
    {
        int id    = LOWORD(wParam); 
        int event = HIWORD(wParam);

        switch (id) {
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
        }

        return 0;
    }
	}

    return DefWindowProc(hWnd, msg, wParam, lParam);
}

static LRESULT CALLBACK editAreaProc(HWND hWnd, UINT msg, WPARAM wParam,
    LPARAM lParam)
{
    MemoPadData *data = (MemoPadData *)GetWindowLong(GetParent(hWnd),
        GWL_USERDATA);

	switch (msg) {
    case WM_CHAR:
    case WM_KEYUP:
    case WM_KEYDOWN:
    {
        int keyCode = wParam;
        if (data->isSelectMode) {
            if (find(data->selectableKeys.begin(), data->selectableKeys.end(),
                keyCode) == data->selectableKeys.end())
            {
                return 0;
            }
        }

        break;
    }
    }

    return CallWindowProc(data->prevEditAreaProc, hWnd, msg, wParam,
        lParam);
}

static LRESULT CALLBACK fileNameLabelProc(HWND hWnd, UINT msg, WPARAM wParam,
    LPARAM lParam) {

    MemoPadData *data = (MemoPadData *)GetWindowLong(GetParent(
        GetParent(hWnd)), GWL_USERDATA);

	switch (msg) {
    case WM_PAINT:
    {
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

        DrawText(hDC, textCStr, -1, &rect, DT_CENTER | DT_VCENTER |
            DT_SINGLELINE);

        EndPaint(hWnd, &paintStruct);

        return 0;
    }
    }

    return CallWindowProc(data->prevFileNameLabelProc, hWnd, msg, wParam,
        lParam);
}

static BOOL WINAPI findStringDlgProc(HWND hDlg, UINT msg, WPARAM wParam,
    LPARAM lParam)
{
	switch (msg) {
    case WM_INITDIALOG:
        onFindStringInitDialog(hDlg);
        return true;
    case WM_COMMAND:
    {
        int id    = LOWORD(wParam); 
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
    static int selectKeys[] = {VK_LEFT, VK_UP, VK_RIGHT, VK_DOWN, VK_PRIOR,
        VK_NEXT, VK_HOME, VK_END};

    int i;

    dll_ChooseColor = (DLL_ChooseColor)GetProcAddress(LoadLibrary(TEXT("commdlg.dll")), TEXT("ChooseColor"));

    crBack = RGB(255, 255, 255);
    crText = RGB(0, 0, 0);
    cc.lStructSize = sizeof(CHOOSECOLOR);
    cc.hwndOwner = hWnd;
    cc.rgbResult = RGB(255, 255, 255);
    cc.lpCustColors = CustColors;
    cc.Flags = CC_FULLOPEN | CC_RGBINIT;

    MemoPadData *data = new MemoPadData();
    SetWindowLong(hWnd, GWL_USERDATA, (long)data);

    data->hFindStringDialog = NULL;
    data->isSelectMode = false;
    data->isWordwrap = false;

    InitCommonControls();

	data->hCommandBar = CommandBar_Create(g_hInstance, hWnd, 1);
	CommandBar_InsertMenubarEx(data->hCommandBar, g_hInstance, _T("MENU"), 0);

    HWND hEditArea = CreateWindow(_T("EDIT"), _T(""),
        WS_CHILD | WS_BORDER | WS_HSCROLL | WS_VSCROLL | WS_VISIBLE |
        ES_AUTOHSCROLL | ES_AUTOVSCROLL | ES_MULTILINE,
        CW_USEDEFAULT, CW_USEDEFAULT,
        CW_USEDEFAULT, CW_USEDEFAULT, hWnd,
        (HMENU)IDC_EDIT_AREA, g_hInstance, NULL);

    data->prevEditAreaProc = (WNDPROC)SetWindowLong(hEditArea, GWL_WNDPROC,
        (DWORD)editAreaProc);
    SendMessage(hEditArea, EM_LIMITTEXT, 0, 0);
    g_hEditArea = hEditArea;

    HWND hFileNameLabel = CreateWindow(_T("STATIC"), _T(""),
        WS_CHILD | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT,
        CW_USEDEFAULT, CW_USEDEFAULT, data->hCommandBar,
        (HMENU)IDC_FILE_NAME_LABEL, g_hInstance, NULL);

    data->prevFileNameLabelProc = (WNDPROC)SetWindowLong(hFileNameLabel, GWL_WNDPROC,
        (DWORD)fileNameLabelProc);

    bool wordwrap = false;
    tstring propFileName = _T("\\Nand2\\.knmemopad_wm.dat");

    if (GetFileAttributes(propFileName.c_str()) == -1) {
        data->hEditAreaFont = KnceUtil::createFont(_T(""), 90, false, false,
            KNCE_FONT_PITCH_FIXED);
    }
    else {
        map<tstring, tstring> props;
        KnceUtil::readPropertyFile(props, propFileName);

        tstring fontName = props[_T("editor.fontName")];
        int pointSize = _ttoi(props[_T("editor.fontSize")].c_str());
        bool bold = _ttoi(props[_T("editor.isBold")].c_str()) != 0;
        bool italic = _ttoi(props[_T("editor.isItalic")].c_str()) != 0;
        crText = _ttoi64(props[_T("editor.textColor")].c_str());
        crBack = _ttoi64(props[_T("editor.backColor")].c_str());
        data->hEditAreaFont = KnceUtil::createFont(fontName, pointSize, bold,
            italic);

        wordwrap = _ttoi(props[_T("editor.isWordwrap")].c_str()) != 0;
    }
    hBackBsh = CreateSolidBrush(crBack);

    SendMessage(hEditArea, WM_SETFONT, (WPARAM)data->hEditAreaFont, false);

    if (wordwrap)
        onToolsWordwrap(hWnd);

    int numKeys = sizeof(selectKeys) / sizeof(int);
    for (i = 0; i < numKeys; i++)
        data->selectableKeys.push_back(selectKeys[i]);

    KnceUtil::changeKeyRepeatSpeed(500, 30);

    openFile(hWnd, _T(""));

    SetFocus(hEditArea);
}

static void onDestroy(HWND hWnd) {
    MemoPadData *data = (MemoPadData *)GetWindowLong(hWnd, GWL_USERDATA);

    KnceUtil::restoreKeyRepeatSpeed();

    map<tstring, tstring> props;
    TCHAR valCStr[256];

    tstring fontName;
    int pointSize = 0;
    bool bold = false;
    bool italic = false;

    KnceUtil::getFontAttributes(data->hEditAreaFont, fontName, pointSize, bold,
        italic);

    props[_T("editor.fontName")] = fontName;

    _sntprintf(valCStr, 256, _T("%d"), pointSize);
    props[_T("editor.fontSize")] = valCStr;

    props[_T("editor.isBold")] = bold ? _T("1") : _T("0");
    props[_T("editor.isItalic")] = italic ? _T("1") : _T("0");

    props[_T("editor.isWordwrap")] = data->isWordwrap  ? _T("1") : _T("0");

    wsprintf(valCStr, TEXT("%u"), crText);
    props[_T("editor.textColor")] = valCStr;
    wsprintf(valCStr, TEXT("%u"), crBack);
    props[_T("editor.backColor")] = valCStr;

    tstring propFileName = _T("\\Nand2\\.knmemopad_wm.dat");
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
    }
    else {
        if (data->isSelectMode)
            onEditEndSelect(hWnd);
    }
}

static void onSize(HWND hWnd, int width, int height) {
    const int fileNameLabelWidth = 200;

    MemoPadData *data = (MemoPadData *)GetWindowLong(hWnd, GWL_USERDATA);

    int cmdBarHeight = CommandBar_Height(data->hCommandBar);
    int fileNameLabelHeight = cmdBarHeight - 4;

    HWND hFileNameLabel = GetDlgItem(data->hCommandBar, IDC_FILE_NAME_LABEL);
    MoveWindow(hFileNameLabel, width - fileNameLabelWidth - 30,
        (cmdBarHeight - fileNameLabelHeight) / 2,
        fileNameLabelWidth, fileNameLabelHeight, true);

    HWND hEditArea = GetDlgItem(hWnd, IDC_EDIT_AREA);
    MoveWindow(hEditArea, 0, cmdBarHeight, width, height - cmdBarHeight,
        true);
}

static void onInitMenuPopup(HWND hWnd) {
    MemoPadData *data = (MemoPadData *)GetWindowLong(hWnd, GWL_USERDATA);

    HWND hEditArea = GetDlgItem(hWnd, IDC_EDIT_AREA);

    unsigned int selection = SendMessage(hEditArea, EM_GETSEL, 0, 0);
    bool selected = LOWORD(selection) != HIWORD(selection);

    HMENU hMenu = CommandBar_GetMenu(data->hCommandBar, 0);

    EnableMenuItem(hMenu, IDM_EDIT_UNDO, SendMessage(hEditArea, EM_CANUNDO,
        0, 0) ? MF_ENABLED : MF_GRAYED);
    EnableMenuItem(hMenu, IDM_EDIT_BEGIN_SELECT, !data->isSelectMode ?
        MF_ENABLED : MF_GRAYED);
    EnableMenuItem(hMenu, IDM_EDIT_END_SELECT, data->isSelectMode ?
        MF_ENABLED : MF_GRAYED);
    EnableMenuItem(hMenu, IDM_EDIT_CUT, selected ? MF_ENABLED : MF_GRAYED);
    EnableMenuItem(hMenu, IDM_EDIT_CUT, selected ? MF_ENABLED : MF_GRAYED);
    EnableMenuItem(hMenu, IDM_EDIT_COPY, selected ? MF_ENABLED : MF_GRAYED);
    EnableMenuItem(hMenu, IDM_EDIT_PASTE, IsClipboardFormatAvailable(
        CF_UNICODETEXT) ?  MF_ENABLED : MF_GRAYED);
    EnableMenuItem(hMenu, IDM_EDIT_CLEAR, selected ? MF_ENABLED : MF_GRAYED);

    CheckMenuItem(hMenu, IDM_TOOLS_WORDWRAP, MF_BYCOMMAND |
        (data->isWordwrap ? MF_CHECKED : MF_UNCHECKED));
}

static void onFileNew(HWND hWnd) {
    if (!checkSaving(hWnd))
        return;

    openFile(hWnd, _T(""));
}

static void onFileOpen(HWND hWnd) {
    MemoPadData *data = (MemoPadData *)GetWindowLong(hWnd, GWL_USERDATA);

    if (!checkSaving(hWnd))
        return;

    KnceChooseFileParams params = {0};
    params.isSaveFile = false;
    params.filters = _T("Text files (*.txt)|*.txt|All files (*.*)|*.*");
    _tcsncpy(params.fileName, data->editFileName.c_str(), MAX_PATH);

    if (!knceChooseFile(hWnd, &params))
        return;

    tstring fileName = params.fileName;

    openFile(hWnd, fileName);
}

static void onFileSave(HWND hWnd) {
    saveFile(hWnd);
}

static void onFileSaveAs(HWND hWnd) {
    saveFile(hWnd, false);
}

static void onEditUndo(HWND hWnd) {
    HWND hEditArea = GetDlgItem(hWnd, IDC_EDIT_AREA);
    SendMessage(hEditArea, EM_UNDO, 0, 0);
}

static void onEditBeginSelect(HWND hWnd) {
    MemoPadData *data = (MemoPadData *)GetWindowLong(hWnd, GWL_USERDATA);

    data->isSelectMode = true;

    keybd_event(VK_SHIFT, 0, 0, NULL);
}

static void onEditEndSelect(HWND hWnd) {
    MemoPadData *data = (MemoPadData *)GetWindowLong(hWnd, GWL_USERDATA);

    data->isSelectMode = false;

    keybd_event(VK_SHIFT, 0, KEYEVENTF_KEYUP, NULL);
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
    int ret = DialogBox(g_hInstance, _T("FIND_STRING"),
        hWnd, (DLGPROC)findStringDlgProc);
    if (ret == IDCANCEL)
        return;
 
    findString(hWnd);
}

static void onEditFindNext(HWND hWnd) {
    MemoPadData *data = (MemoPadData *)GetWindowLong(hWnd, GWL_USERDATA);

    if (data->findTargetString.empty()) {
        int ret = DialogBox(g_hInstance, _T("FIND_STRING"),
            hWnd, (DLGPROC)findStringDlgProc);
        if (ret == IDCANCEL)
            return;
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

    static TCHAR editBuff[MAX_EDIT_BUFFER];
    GetWindowText(hEditArea, editBuff, MAX_EDIT_BUFFER);

    unsigned int selection = SendMessage(hEditArea, EM_GETSEL, 0, 0);
    int from = LOWORD(selection);
    int to = HIWORD(selection);
    int len = to - from;

    if (len > 20)
        len = 20;

    tstring selectedWord(from + editBuff, len);

    tstring lookupWord;
    for (i = 0; i < len; i++) {
        TCHAR ch = selectedWord[i];
        if ((ch >= _T('A') && ch <= _T('Z')) ||
            ch >= _T('a') && ch <= _T('z')) {

            lookupWord += _totlower(ch);
        }
    }

    if (lookupWord.empty())
        return;

    HWND hEdWindow = FindWindow(_T("SHARP SIM"), NULL);
    if (hEdWindow == NULL)
        return;

    SendMessage(hEdWindow, g_dicKeyDownMessage, 0x4003, 0);

    len = lookupWord.length();
    for (i = 0; i < len; i++) {
        int code = lookupWord[i] - 'a' + 0x1000;
        SendMessage(hEdWindow, g_dicKeyDownMessage, code, 0);
    }

    SendMessage(hEdWindow, g_dicKeyDownMessage, 0x2008, 0);

    SetForegroundWindow(hEdWindow);
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
        (data->isWordwrap ? 0 : WS_HSCROLL | ES_AUTOHSCROLL) |
        ES_MULTILINE,
        CW_USEDEFAULT, CW_USEDEFAULT,
        CW_USEDEFAULT, CW_USEDEFAULT, hWnd,
        (HMENU)IDC_EDIT_AREA, g_hInstance, NULL);

    data->prevEditAreaProc = (WNDPROC)SetWindowLong(hEditArea, GWL_WNDPROC,
        (DWORD)editAreaProc);
    SendMessage(hEditArea, EM_LIMITTEXT, 0, 0);
    g_hEditArea = hEditArea;

    SendMessage(hEditArea, WM_SETFONT, (WPARAM)data->hEditAreaFont, true);

    RECT rect;
    GetClientRect(hWnd, &rect);
    onSize(hWnd, rect.right - rect.left , rect.bottom - rect.top);

    SetWindowText(hEditArea, editBuff);

    delete [] editBuff;

    SendMessage(hEditArea, EM_SETMODIFY, modified != 0, 0);

    SetFocus(hEditArea);
}

static void onToolsFont(HWND hWnd) {
    MemoPadData *data = (MemoPadData *)GetWindowLong(hWnd, GWL_USERDATA);

    KnceChooseFontParams params = {0};
    params.hFont = data->hEditAreaFont;
    params.isFixedOnly = true;

    if (knceChooseFont(hWnd, &params)) {
        DeleteObject(data->hEditAreaFont);

        data->hEditAreaFont = params.hFont;

        HWND hEditArea = GetDlgItem(hWnd, IDC_EDIT_AREA);
        SendMessage(hEditArea, WM_SETFONT, (WPARAM)data->hEditAreaFont, true);
    }
}

static void onToolsTextColor(HWND hWnd) {
	if (!dll_ChooseColor(&cc)) return;
    crText = cc.rgbResult;
	InvalidateRect(g_hEditArea, NULL, TRUE);
}

static void onToolsBackColor(HWND hWnd) {
	if (!dll_ChooseColor(&cc)) return;
    crBack = cc.rgbResult;
    DeleteObject(hBackBsh);
    hBackBsh = CreateSolidBrush(cc.rgbResult);
	InvalidateRect(g_hEditArea, NULL, TRUE);
}

static void onFindStringInitDialog(HWND hDlg) {
    MemoPadData *data = (MemoPadData *)GetWindowLong(GetWindow(hDlg, GW_OWNER),
        GWL_USERDATA);

    KnceUtil::adjustDialogLayout(hDlg);

    HWND hTargetBox = GetDlgItem(hDlg, IDC_FIND_STRING_TARGET_BOX);

    SetWindowText(hTargetBox, data->findTargetString.c_str());
    SendMessage(hTargetBox, EM_SETSEL, 0, -1);
}

static void onFindStringOk(HWND hDlg) {
    MemoPadData *data = (MemoPadData *)GetWindowLong(GetWindow(hDlg, GW_OWNER),
        GWL_USERDATA);

    HWND hTargetBox = GetDlgItem(hDlg, IDC_FIND_STRING_TARGET_BOX);

    TCHAR findTargetCStr[1024];
    GetWindowText(hTargetBox, findTargetCStr, 1024);
    data->findTargetString = findTargetCStr;

    EndDialog(hDlg, IDOK);
}

static void onFindStringCancel(HWND hDlg) {
    EndDialog(hDlg, IDCANCEL);
}

static void onFindStringTargetBox(HWND hDlg, int event) {
    if (event != EN_CHANGE)
        return;

    HWND hOkButton = GetDlgItem(hDlg, IDOK);
    HWND hTargetBox = GetDlgItem(hDlg, IDC_FIND_STRING_TARGET_BOX);

    if (GetWindowTextLength(hTargetBox) > 0)
        EnableWindow(hOkButton, true);
    else
        EnableWindow(hOkButton, false);
}

static void openFile(HWND hWnd, const tstring &fileName) {
    const int lineBuffSize = 1024;

    MemoPadData *data = (MemoPadData *)GetWindowLong(hWnd, GWL_USERDATA);

    HWND hEditArea = GetDlgItem(hWnd, IDC_EDIT_AREA);

    if (fileName.empty())
        SetWindowText(hEditArea, _T(""));
    else {
        FILE *file = _tfopen(fileName.c_str(), _T("r"));
        if (file == NULL) {
            tstring msg = _T("Cannot open file: ") + fileName;
            MessageBox(hWnd, msg.c_str(), _T("Error"),
                MB_ICONEXCLAMATION);

            return;
        }
 
        tstring editStream;
        TCHAR lineBuff[lineBuffSize];
        char mbLineBuff[lineBuffSize * 2];

        while (true) {
            if (fgets(mbLineBuff, lineBuffSize * 2, file) == NULL)
                break;

            char *cr = strchr(mbLineBuff, '\n');
            if (cr != NULL)
                *cr = '\0';

            MultiByteToWideChar(932, 0, mbLineBuff, -1, lineBuff,
                lineBuffSize);

            editStream += lineBuff;
            editStream += _T("\r\n");
        }

        fclose(file);

        SetWindowText(hEditArea, editStream.c_str());
    }

    SendMessage(hEditArea, EM_SETMODIFY, false, 0);

    data->editFileName = fileName;
    updateFileNameLabel(hWnd);
}

static bool saveFile(HWND hWnd, bool isOverwrite) {
    const int lineBuffSize = 1024;

    MemoPadData *data = (MemoPadData *)GetWindowLong(hWnd, GWL_USERDATA);

    tstring fileName = data->editFileName;
    if (!isOverwrite || data->editFileName.empty()) {
        KnceChooseFileParams params = {0};
        params.isSaveFile = true;
        params.filters = _T("Text files (*.txt)|*.txt|All files (*.*)|*.*");
        _tcsncpy(params.fileName, fileName.c_str(), MAX_PATH);

        if (!knceChooseFile(hWnd, &params))
            return false;

        fileName = params.fileName;
    }

    HWND hEditArea = GetDlgItem(hWnd, IDC_EDIT_AREA);

    static TCHAR editBuff[MAX_EDIT_BUFFER];
    GetWindowText(hEditArea, editBuff, MAX_EDIT_BUFFER);

    FILE *file = _tfopen(fileName.c_str(), _T("w"));
    if (file == NULL) {
        tstring msg = _T("Cannot write file: ") + fileName;
        MessageBox(hWnd, msg.c_str(), _T("Error"),
            MB_ICONEXCLAMATION);

        return false;
    }

    tstring line;
    char mbLineBuff[lineBuffSize * 2];
    TCHAR *curPtr = editBuff;

    while (*curPtr != _T('\0')) {
        TCHAR *sepPtr = _tcschr(curPtr, '\r');

        if (sepPtr == NULL) {
            line = curPtr;
            curPtr = _tcschr(curPtr, '\0');
        }
        else {
            line = tstring(curPtr, sepPtr - curPtr);
            curPtr = sepPtr + 2;
        }

        line += _T("\n");

        WideCharToMultiByte(932, 0, line.c_str(), -1, mbLineBuff,
            lineBuffSize * 2, NULL, NULL);

        fputs(mbLineBuff, file);
    }

    fclose(file);

    SendMessage(hEditArea, EM_SETMODIFY, false, 0);

    data->editFileName = fileName;
    updateFileNameLabel(hWnd);

    return true;
}

static bool checkSaving(HWND hWnd) {
    MemoPadData *data = (MemoPadData *)GetWindowLong(hWnd, GWL_USERDATA);

    HWND hEditArea = GetDlgItem(hWnd, IDC_EDIT_AREA);
    if (SendMessage(hEditArea, EM_GETMODIFY, 0, 0) == 0)
        return true;

    tstring fileName;
    if (data->editFileName.empty())
        fileName = _T("untitled");
    else {
        fileName = data->editFileName.substr(
            data->editFileName.rfind(_T('\\')) + 1);
    }

    tstring msg = _T("Save changes to '") + fileName + _T("'?");

    int ret = MessageBox(hWnd, msg.c_str(), _T("Confirm"),
        MB_YESNOCANCEL | MB_ICONQUESTION);

    switch (ret) {
    case IDCANCEL:
        return false;
    case IDYES:
        if (saveFile(hWnd))
            return true;
        else
            return false;
    case IDNO:
        return true;
    }

    // not reached
    return true;
}

static void updateFileNameLabel(HWND hWnd) {
    MemoPadData *data = (MemoPadData *)GetWindowLong(hWnd, GWL_USERDATA);

    tstring fileName;
    if (!data->editFileName.empty()) {
        fileName = data->editFileName.substr(
            data->editFileName.rfind(_T('\\')) + 1);
    }

    HWND hFileNameLabel = GetDlgItem(data->hCommandBar, IDC_FILE_NAME_LABEL);
    if (fileName.empty()) {
        SetWindowText(hFileNameLabel, _T("KN MemoPad"));
        SetWindowText(hWnd, _T("KN MemoPad"));
    }
    else {
        SetWindowText(hFileNameLabel, fileName.c_str());
        SetWindowText(hWnd, (fileName + _T(" - KN MemoPad")).c_str());
    }
}

static void findString(HWND hWnd) {
    MemoPadData *data = (MemoPadData *)GetWindowLong(hWnd, GWL_USERDATA);

    HWND hEditArea = GetDlgItem(hWnd, IDC_EDIT_AREA);

    static TCHAR editBuff[MAX_EDIT_BUFFER];
    GetWindowText(hEditArea, editBuff, MAX_EDIT_BUFFER);

    int count = GetWindowTextLength(hEditArea);

    unsigned int selection = SendMessage(hEditArea, EM_GETSEL, 0, 0);
    int pos = LOWORD(selection);

    if (pos < count)
        pos++;

    const TCHAR *foundPtr = _tcscasestr(editBuff + pos,
        data->findTargetString.c_str());

    if (foundPtr == NULL) {
        foundPtr = _tcscasestr(editBuff, data->findTargetString.c_str());

        if (foundPtr == NULL) {
            tstring msg = _T("Cannot find string: ") + data->findTargetString;
            MessageBox(hWnd, msg.c_str(), _T("Error"),
                MB_ICONEXCLAMATION);

            return;
        }
    }

    pos = foundPtr - editBuff;
    SendMessage(hEditArea, EM_SETSEL, pos,
        pos + data->findTargetString.length());
    SendMessage(hEditArea, EM_SCROLLCARET, 0, 0);
}

static const TCHAR *_tcscasestr(const TCHAR *str, const TCHAR *strSearch) {
    int strpos = 0, strSearchpos = 0;

    while (str[strpos] != _T('\0')) {
        if (tolower(str[strpos]) == tolower(strSearch[0])) {
            strSearchpos = 1;

            while ((strSearch[strSearchpos]) &&
                (tolower(str[strpos + strSearchpos]) ==
                tolower(strSearch[strSearchpos])) )
            {
                ++strSearchpos;
            }

            if (!strSearch[strSearchpos])
                return str + strpos;
        }

        ++strpos;
    }

    return NULL;
}

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPTSTR lpCmd, int nShow)
{
    const TCHAR *className = _T("KNMemoPad");

    g_hInstance = hInst;

    WNDCLASS windowClass;
    memset(&windowClass, 0, sizeof(WNDCLASS));
    windowClass.style = CS_HREDRAW | CS_VREDRAW;
    windowClass.lpfnWndProc = WndProc;
    windowClass.hInstance = hInst;
    windowClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    windowClass.lpszClassName = className;
    RegisterClass(&windowClass);

    HWND hWnd = CreateWindow(
        className, _T("KN MemoPad"),
        WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT,
        CW_USEDEFAULT, CW_USEDEFAULT,
        NULL, NULL, hInst, NULL);

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
