#include <windows.h>
#include <CommCtrl.h>
#include <Richedit.h>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
using namespace std;

#include "resource.h"

HWND g_main;
#pragma comment( lib, "comctl32.lib" )
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' " "version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

HWND g_hdlg;

struct Question
{
    string q, oa, ob, oc, od;
    char ans;
};
vector<Question> g_questions;
int g_index;


void SetRTF(int rtb, string rtf)
{
    SETTEXTEX st = { 0 };
    st.codepage = 1200;
    st.flags = ST_DEFAULT;
    SendDlgItemMessage(g_hdlg, rtb, EM_SETTEXTEX, (WPARAM)&st, (LPARAM)rtf.c_str());
}
DWORD CALLBACK EditStreamOutCallback(DWORD_PTR dwCookie, LPBYTE pbBuff, LONG cb, LONG *pcb)
{
    std::stringstream *rtf = (std::stringstream*) dwCookie;
    rtf->write((char*)pbBuff, cb);
    *pcb = cb;
    return 0;
}
string GetRTF(int rtb)
{
    std::stringstream rtf;

    EDITSTREAM es = { 0 };
    es.dwCookie = (DWORD_PTR)&rtf;
    es.pfnCallback = &EditStreamOutCallback;
    SendDlgItemMessage(g_hdlg, rtb, EM_STREAMOUT, SF_RTF, (LPARAM)&es);
    return rtf.str();
}
string GetSelectedRtf(int rtb)
{
    std::stringstream rtf;

    EDITSTREAM es = { 0 };
    es.dwCookie = (DWORD_PTR)&rtf;
    es.pfnCallback = &EditStreamOutCallback;
    SendDlgItemMessage(g_hdlg, rtb, EM_STREAMOUT, SF_RTF | SFF_SELECTION, (LPARAM)&es);
    return rtf.str();
}
string GetText(int rtb)
{
    std::stringstream rtf;

    EDITSTREAM es = { 0 };
    es.dwCookie = (DWORD_PTR)&rtf;
    es.pfnCallback = &EditStreamOutCallback;
    SendDlgItemMessage(g_hdlg, rtb, EM_STREAMOUT, SF_TEXT , (LPARAM)&es);
    return rtf.str();
}

bool g_updating = false;
void Update()
{
    g_updating = true;

    SetRTF(IDC_RICHEDIT21, g_questions[g_index].q);
    SetRTF(IDC_RICHEDIT22, g_questions[g_index].oa);
    SetRTF(IDC_RICHEDIT23, g_questions[g_index].ob);
    SetRTF(IDC_RICHEDIT24, g_questions[g_index].oc);
    SetRTF(IDC_RICHEDIT25, g_questions[g_index].od);

    switch (g_questions[g_index].ans)
    {
    case 0:
        CheckRadioButton(g_hdlg, IDC_RADIO1, IDC_RADIO4, IDC_RADIO1);
        break;
    case 1:
        CheckRadioButton(g_hdlg, IDC_RADIO1, IDC_RADIO4, IDC_RADIO2);
        break;
    case 2:
        CheckRadioButton(g_hdlg, IDC_RADIO1, IDC_RADIO4, IDC_RADIO3);
        break;
    case 3:
        CheckRadioButton(g_hdlg, IDC_RADIO1, IDC_RADIO4, IDC_RADIO4);
        break;
    }

    g_updating = false;
}

void Clear()
{
    g_questions[g_index].q = g_questions[g_index].oa = g_questions[g_index].ob = g_questions[g_index].oc = g_questions[g_index].od = "{\\rtf1}";
    g_questions[g_index].ans = 0;
    Update();
}
void Add()
{
    Question q;
    g_questions.push_back(q);
    g_index = g_questions.size() - 1;
    Clear();
}

void ChangeFontSize(int rtb, int size)
{
    int ss, sl;
    SendDlgItemMessage(g_hdlg, rtb, EM_GETSEL, (WPARAM)&ss, (LPARAM)&sl);
    SendDlgItemMessage(g_hdlg, rtb, EM_SETSEL, 0, -1);
    SendDlgItemMessage(g_hdlg, rtb, EM_SETFONTSIZE, size, 0);
    SendDlgItemMessage(g_hdlg, rtb, EM_SETSEL, ss, sl);
}

void Changed()
{
    if (g_updating) return;

    Question &q = g_questions[g_index];
    q.q = GetRTF(IDC_RICHEDIT21);
    q.oa = GetRTF(IDC_RICHEDIT22);
    q.ob = GetRTF(IDC_RICHEDIT23);
    q.oc = GetRTF(IDC_RICHEDIT24);
    q.od = GetRTF(IDC_RICHEDIT25);
    if (IsDlgButtonChecked(g_hdlg, IDC_RADIO1)) q.ans = 0;
    else if (IsDlgButtonChecked(g_hdlg, IDC_RADIO2)) q.ans = 1;
    else if (IsDlgButtonChecked(g_hdlg, IDC_RADIO3)) q.ans = 2;
    else if (IsDlgButtonChecked(g_hdlg, IDC_RADIO4)) q.ans = 3;
}

void Save()
{
    TCHAR szFileName[MAX_PATH] = L"";
    OPENFILENAME ofn = { 0 };
    ofn.lStructSize = sizeof(ofn); 
    ofn.hwndOwner = g_main;
    ofn.lpstrFilter = L"Question Files (*.fbq)\0*.fbq";
    ofn.lpstrFile = szFileName;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
    ofn.lpstrDefExt = L"fbq";

    if (GetSaveFileName(&ofn))
    {
        std::fstream file;
        file.open(szFileName, std::ios::out | std::ios::binary);
        unsigned int size = g_questions.size();
        file.write((char*)&size, sizeof(size));
        for (unsigned int i = 0; i < size; ++i)
        {
            int length = (int)g_questions[i].q.size();
            file.write((char*)&length, sizeof(length));
            file.write(g_questions[i].q.c_str(), length);

            length = g_questions[i].oa.size();
            file.write((char*)&length, sizeof(length));
            file.write(g_questions[i].oa.c_str(), length);

            length = g_questions[i].ob.size();
            file.write((char*)&length, sizeof(length));
            file.write(g_questions[i].ob.c_str(), length);

            length = g_questions[i].oc.size();
            file.write((char*)&length, sizeof(length));
            file.write(g_questions[i].oc.c_str(), length);

            length = g_questions[i].od.size();
            file.write((char*)&length, sizeof(length));
            file.write(g_questions[i].od.c_str(), length);

            file.write(&g_questions[i].ans, sizeof(g_questions[i].ans));
        }
        file.close();
    }
}

void Open()
{
    TCHAR szFileName[MAX_PATH] = L"";
    OPENFILENAME ofn = { 0 };
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = g_main;
    ofn.lpstrFilter = L"Question Files (*.fbq)\0*.fbq";
    ofn.lpstrFile = szFileName;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
    ofn.lpstrDefExt = L"fbq";

    if (GetOpenFileName(&ofn))
    {
        std::fstream file;
        file.open(szFileName, std::ios::in | std::ios::binary);
        unsigned int size;
        file.read((char*)&size, sizeof(size));
        g_questions.resize(size);
        for (unsigned int i = 0; i < size; ++i)
        {
            for (int j = 0; j < 5; ++j)
            {
                int length;
                file.read((char*)&length, sizeof(length));
                char * str = new char[length + 1];
                file.read(str, length);
                str[length] = '\0';
                if (j == 0) g_questions[i].q = str;
                else if (j == 1) g_questions[i].oa = str;
                else if (j == 2) g_questions[i].ob = str;
                else if (j == 3) g_questions[i].oc = str;
                else if (j == 4) g_questions[i].od = str;
                delete[] str;
            }
            file.read(&g_questions[i].ans, sizeof(g_questions[i].ans));
        }
        file.close();
        Update();
    }
}

void Select(int rtb, int start, int end)
{
    CHARRANGE cr;
    cr.cpMin = start; cr.cpMax = end;
    SendDlgItemMessage(g_hdlg, rtb, EM_EXSETSEL, NULL, (LPARAM)&cr);
}
void SmartPaste()
{
    SendDlgItemMessage(g_hdlg, IDC_RICHEDIT21, WM_PASTE, 0, 0);
    string text = GetText(IDC_RICHEDIT21);
    text.erase(std::remove(text.begin(), text.end(), '\r'), text.end());

    int id = text.find("\ta)");
    Select(IDC_RICHEDIT21, 0, id);
    string q = GetSelectedRtf(IDC_RICHEDIT21);

    int nid = text.find("\tb)");
    Select(IDC_RICHEDIT21, id + 4, nid);
    SetRTF(IDC_RICHEDIT22, GetSelectedRtf(IDC_RICHEDIT21));

    id = nid;
    nid = text.find("\tc)");
    Select(IDC_RICHEDIT21, id + 4, nid);
    SetRTF(IDC_RICHEDIT23, GetSelectedRtf(IDC_RICHEDIT21));

    id = nid;
    nid = text.find("\td)");
    Select(IDC_RICHEDIT21, id + 4, nid);
    SetRTF(IDC_RICHEDIT24, GetSelectedRtf(IDC_RICHEDIT21));

    id = nid;
    nid = text.size();
    Select(IDC_RICHEDIT21, id + 4, nid);
    SetRTF(IDC_RICHEDIT25, GetSelectedRtf(IDC_RICHEDIT21));

    SetRTF(IDC_RICHEDIT21, q);


    ChangeFontSize(IDC_RICHEDIT21, 2);
    ChangeFontSize(IDC_RICHEDIT22, 2);
    ChangeFontSize(IDC_RICHEDIT23, 2);
    ChangeFontSize(IDC_RICHEDIT24, 2);
    ChangeFontSize(IDC_RICHEDIT25, 2);
}

BOOL CALLBACK DlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDC_RICHEDIT21:
        case IDC_RICHEDIT22:
        case IDC_RICHEDIT23:
        case IDC_RICHEDIT24:
        case IDC_RICHEDIT25:
            if (HIWORD(wParam) == EN_CHANGE)
                Changed();
            break;
        case IDC_RADIO1:
        case IDC_RADIO2:
        case IDC_RADIO3:
        case IDC_RADIO4:
            if (HIWORD(wParam) == BN_CLICKED)
                Changed();
            break;
        case IDC_NEXT:
            if (g_index < (int)g_questions.size() - 1) ++g_index;
            Update();
            break;
        case IDC_PRE: 
            if (g_index > 0) --g_index;
            Update();
            break;
        case IDC_ADD:
            Add();
            break;
        case IDC_SAVE:
            Save();
            break;
        case IDC_OPEN:
            Open();
            break;
        case IDC_SMART:
            SmartPaste();
            break;
        case IDC_CLEAR:
            Clear();
            break;
        }
        break;

    case WM_CLOSE: 
        DestroyWindow(g_hdlg);
        return TRUE;
    case WM_DESTROY:
        PostQuitMessage(0);
        return TRUE;
    }
    return FALSE;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPInst, char* line, int show)
{
    LoadLibrary(L"Msftedit.DLL");
    InitCommonControls();
 
    g_hdlg = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DlgProc);
    SendDlgItemMessage(g_hdlg, IDC_RICHEDIT21, EM_SETEVENTMASK, 0, ENM_CHANGE);
    SendDlgItemMessage(g_hdlg, IDC_RICHEDIT22, EM_SETEVENTMASK, 0, ENM_CHANGE);
    SendDlgItemMessage(g_hdlg, IDC_RICHEDIT23, EM_SETEVENTMASK, 0, ENM_CHANGE);
    SendDlgItemMessage(g_hdlg, IDC_RICHEDIT24, EM_SETEVENTMASK, 0, ENM_CHANGE);
    SendDlgItemMessage(g_hdlg, IDC_RICHEDIT25, EM_SETEVENTMASK, 0, ENM_CHANGE);

    ShowWindow(g_hdlg, show);
    CHARFORMAT cf = { 0 };
    cf.cbSize = sizeof(CHARFORMAT);
    cf.dwMask = CFM_SIZE;
    cf.yHeight = 20*11;
    SendDlgItemMessage(g_hdlg, IDC_RICHEDIT21, EM_SETCHARFORMAT, SCF_DEFAULT, (LPARAM)&cf);
    SendDlgItemMessage(g_hdlg, IDC_RICHEDIT22, EM_SETCHARFORMAT, SCF_DEFAULT, (LPARAM)&cf);
    SendDlgItemMessage(g_hdlg, IDC_RICHEDIT23, EM_SETCHARFORMAT, SCF_DEFAULT, (LPARAM)&cf);
    SendDlgItemMessage(g_hdlg, IDC_RICHEDIT24, EM_SETCHARFORMAT, SCF_DEFAULT, (LPARAM)&cf);
    SendDlgItemMessage(g_hdlg, IDC_RICHEDIT25, EM_SETCHARFORMAT, SCF_DEFAULT, (LPARAM)&cf);

    Add();

    BOOL ret;
    MSG msg;
    while ((ret = GetMessage(&msg, 0, 0, 0)) != 0) {
        if (ret == -1) 
            return -1;

        if (!IsDialogMessage(g_hdlg, &msg)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg); 
        }
    }
    return 0;
}

