// SAManager.cpp : 애플리케이션에 대한 진입점을 정의합니다.
// TODO : 디버그 메세지로 확인해보기

#include "framework.h"
#include "SAManager.h"
#include "ProjectManager.h"


#define MAX_LOADSTRING 100

using namespace std;

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

void DebugMsg(LPCWSTR lpszFormat, ...);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 여기에 코드를 입력합니다.

    // 전역 문자열을 초기화합니다.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_SAMANAGER, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 애플리케이션 초기화를 수행합니다:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SAMANAGER));

    MSG msg;
    
    // 기본 메시지 루프입니다:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//  
//  함수: MyRegisterClass()
//
//  용도: 창 클래스를 등록합니다.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SAMANAGER));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_SAMANAGER);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   함수: InitInstance(HINSTANCE, int)
//
//   용도: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   주석:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   // H2K
   ShowWindow(hWnd, SW_HIDE);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  용도: 주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 애플리케이션 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {

    case WM_CREATE:
    {
    
        CmdManager::GetInst()->ParserCommandLine();
      
        if (ERROR_SUCCESS == CmdManager::GetInst()->CheckCmd())
        {
            DebugMsg(L" EJH %s(%d) : Debug Msg [%s]", __FUNCTIONW__, __LINE__, L"ERROR_SUCCESS_CMDCHECK");

            if(TRUE == CmdManager::GetInst()->CheckSinPath())
            {
                if (ERROR_SUCCESS == ProjectManager::GetInst()->GetSlnList(CmdManager::GetInst()->GetSolutionPath()))
                {
                    DebugMsg(L" EJH %s(%d) : Debug Msg [%s]", __FUNCTIONW__, __LINE__, CmdManager::GetInst()->GetSolutionPath());

                    if (ERROR_SUCCESS == ProjectManager::GetInst()->GetInform())
                    {
                         ProjectManager::GetInst()->MakeCdprojFile(CmdManager::GetInst()->GetOutPath(), CmdManager::GetInst()->GetProductName());
                        
                         if (TRUE == CmdManager::GetInst()->CheckAdd())
                         {
                             ProjectManager::GetInst()->AddPropertiesFile(CmdManager::GetInst()->GetOutPath(), CmdManager::GetInst()->GetProductName());
                         }
                         else
                         {  
                            ProjectManager::GetInst()->MakePropertiesFile(CmdManager::GetInst()->GetOutPath(), CmdManager::GetInst()->GetProductName());
                         }
                    } 
                }
            }
            else if(TRUE == CmdManager::GetInst()->CheckPinPath())
            {
                if (ERROR_SUCCESS == ProjectManager::GetInst()->GetProjectInform(CmdManager::GetInst()->GetProjectPath()))
                {
                    DebugMsg(L" EJH %s(%d) : Debug Msg [%s]", __FUNCTIONW__, __LINE__, CmdManager::GetInst()->GetProjectPath());
                    if (ERROR_SUCCESS == ProjectManager::GetInst()->GetInform())
                    {
                        ProjectManager::GetInst()->MakeCdprojFile(CmdManager::GetInst()->GetOutPath(), CmdManager::GetInst()->GetProductName());
                        
                        if (TRUE == CmdManager::GetInst()->CheckAdd())
                        {
                            ProjectManager::GetInst()->AddPropertiesFile(CmdManager::GetInst()->GetOutPath(), CmdManager::GetInst()->GetProductName());
                        }
                        else
                        {
                            ProjectManager::GetInst()->MakePropertiesFile(CmdManager::GetInst()->GetOutPath(), CmdManager::GetInst()->GetProductName());
                        }
                    }
                }
            }
        }
        else
        {
            DebugMsg(L"%s(%d) : Debug Msg [%s]", __FUNCTIONW__, __LINE__, L"ERROR_CMDCHECK");
        }

        ExitProcess(0);
    }
        break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 메뉴 선택을 구문 분석합니다:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// 정보 대화 상자의 메시지 처리기입니다.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}


// 실행 명령어를 맵에 저장
void CmdManager::ParserCommandLine()
{

    int argc = 0;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLine(), &argc);
    WCHAR szKey[MAX_PATH] = { 0, };
    WCHAR szValue[MAX_PATH] = { 0, };

    for (int i = 0; i < argc; i++)  
    {

        if (0 == i)
            continue;

        if (0 == i % 2)
        {
           wcscpy_s(szValue, argv[i]);
           m_mapCommandLine.insert(pair<wstring, wstring>(szKey, szValue));       // 실행 명령어 m_mapCommandLine에 담음
        }
        else
        {
            wcscpy_s(szKey, argv[i]);
        }
    }

        map<wstring, wstring>::iterator itorator;
        for (itorator = m_mapCommandLine.begin(); itorator != m_mapCommandLine.end(); itorator++)
        {
            DebugMsg(L"%s(%d) : Debug Msg [%s][%s]", __FUNCTIONW__, __LINE__, itorator->first.c_str(), itorator->second.c_str());
        }

    LocalFree(argv);

}


CmdManager* CmdManager::GetInst()
{
    static CmdManager Inst;
    return &Inst;
}

CmdManager::CmdManager()
{



}

CmdManager::~CmdManager()
{


}

wstring CmdManager::GetSinPath()
{
    wstring szCmd;
    
    // TODO : 스레드, 메모리, 등등 학습
    
    map<wstring, wstring>::iterator iter;
    iter = m_mapCommandLine.find(L"Sinpath");
    if (iter != m_mapCommandLine.end()) {
        szCmd = iter->first.c_str();

        DebugMsg(L"%s(%d) : Debug Msg [%s]", __FUNCTIONW__, __LINE__, szCmd.c_str());
    }
    
    return szCmd;
}

wstring CmdManager::GetPinPath()
{
    wstring szCmd;
    map<wstring, wstring>::iterator iter;
    iter = m_mapCommandLine.find(L"Pinpath");
    if (iter != m_mapCommandLine.end()) {
        szCmd = iter->first.c_str();

        DebugMsg(L"%s(%d) : Debug Msg [%s]", __FUNCTIONW__, __LINE__, szCmd.c_str());
    }

    return szCmd;
}

wstring CmdManager::GetAdd()
{
    wstring szCmd;
    map<wstring, wstring>::iterator iter;
    iter = m_mapCommandLine.find(L"Add");
    if (iter != m_mapCommandLine.end()) {
        szCmd = iter->first.c_str();

        DebugMsg(L"%s(%d) : Debug Msg [%s]", __FUNCTIONW__, __LINE__, szCmd.c_str());
    }

    return szCmd;
}

WCHAR* CmdManager::GetOutPath()
{
    WCHAR* szCmd = NULL; 
    map<wstring, wstring>::iterator iter;
    iter = m_mapCommandLine.find(L"Outpath");
    if (iter != m_mapCommandLine.end()) {
        szCmd = (WCHAR*)iter->second.c_str();

        DebugMsg(L"%s(%d) : Debug Msg [%s]", __FUNCTIONW__, __LINE__, szCmd);
    }
    return szCmd;
}

WCHAR* CmdManager::GetProductName()
{
    WCHAR* szCmd = NULL;
    map<wstring, wstring>::iterator iter;
    iter = m_mapCommandLine.find(L"Name");
    if (iter != m_mapCommandLine.end()) {
        szCmd = (WCHAR*)iter->second.c_str();

        DebugMsg(L"%s(%d) : Debug Msg [%s]", __FUNCTIONW__, __LINE__, szCmd);
    }
    return szCmd;
}

// 실행명령어 순서는 상관 x
// 실행불가능한 명령어일 경우 프로그램 종료 
// Sinpath, Pinpath, Outpath, Name, Add 실행명령어가 아닐경우 프로그램 종료
// 명령어 조합 체크 ex)  Sinpath + Outpath + Name + (Add), Pinpath + Outpath + Name + (Add)
int CmdManager::CheckCmd()
{
    
    map<wstring, wstring>::iterator SinpathIter;
    SinpathIter = m_mapCommandLine.find(L"Sinpath");
    map<wstring, wstring>::iterator PinpathIter;
    PinpathIter = m_mapCommandLine.find(L"Pinpath");
    map<wstring, wstring>::iterator OutpathIter;
    OutpathIter = m_mapCommandLine.find(L"Outpath");
    map<wstring, wstring>::iterator NameIter;
    NameIter = m_mapCommandLine.find(L"Name");
    map<wstring, wstring>::iterator AddIter;
    AddIter = m_mapCommandLine.find(L"Add");

    if (SinpathIter != m_mapCommandLine.end())
    {
        DebugMsg(L"%s(%d) : Debug Msg [%s]", __FUNCTIONW__, __LINE__, SinpathIter->first.c_str());

        if (OutpathIter != m_mapCommandLine.end())
        {
            DebugMsg(L"%s(%d) : Debug Msg [%s]", __FUNCTIONW__, __LINE__, OutpathIter->first.c_str());

            if (NameIter != m_mapCommandLine.end())
            {
                DebugMsg(L"%s(%d) : Debug Msg [%s]", __FUNCTIONW__, __LINE__, NameIter->first.c_str());
                
                if (AddIter != m_mapCommandLine.end())
                {
                    DebugMsg(L"%s(%d) : Debug Msg [%s]", __FUNCTIONW__, __LINE__, AddIter->first.c_str());

                    return ERROR_SUCCESS;
                }
               
                return ERROR_SUCCESS;
            }
            else
            {
                DebugMsg(L"%s(%d) : Debug Msg [%s]", __FUNCTIONW__, __LINE__, L"ERROR");
                return ERROR_INVALID_FUNCTION;
            }
        }
        else
        {
            DebugMsg(L"%s(%d) : Debug Msg [%s]", __FUNCTIONW__, __LINE__, L"ERROR");
            return ERROR_INVALID_FUNCTION;
        }
    }
    else if (PinpathIter != m_mapCommandLine.end())
    {
        DebugMsg(L"%s(%d) : Debug Msg [%s]", __FUNCTIONW__, __LINE__, PinpathIter->first.c_str());
        if (OutpathIter != m_mapCommandLine.end())
        {
            DebugMsg(L"%s(%d) : Debug Msg [%s]", __FUNCTIONW__, __LINE__, OutpathIter->first.c_str());

            if (NameIter != m_mapCommandLine.end())
            {
                DebugMsg(L"%s(%d) : Debug Msg [%s]", __FUNCTIONW__, __LINE__, NameIter->first.c_str());
                
                if (AddIter != m_mapCommandLine.end())
                {
                    DebugMsg(L"%s(%d) : Debug Msg [%s]", __FUNCTIONW__, __LINE__, AddIter->first.c_str());
                    return ERROR_SUCCESS;
                }

                return ERROR_SUCCESS;

            }
            else
            {
                DebugMsg(L"%s(%d) : Debug Msg [%s]", __FUNCTIONW__, __LINE__, L"ERROR");
                return ERROR_INVALID_FUNCTION;
            }
        }
        else
        {
            DebugMsg(L"%s(%d) : Debug Msg [%s]", __FUNCTIONW__, __LINE__, L"ERROR");
            return ERROR_INVALID_FUNCTION;
        }
    }
    else
    {
        DebugMsg(L"%s(%d) : Debug Msg [%s]", __FUNCTIONW__, __LINE__, "ERROR");
        return ERROR_INVALID_FUNCTION;
    }
}

BOOL CmdManager::CheckSinPath()
{ 
    wstring inpath = L"Sinpath";
    if (inpath == CmdManager::GetInst()->GetSinPath())
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

BOOL CmdManager::CheckPinPath()
{
    wstring inpath = L"Pinpath";
    if (inpath == CmdManager::GetInst()->GetPinPath())
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

BOOL CmdManager::CheckAdd()
{
    int argc = 0;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLine(), &argc);

    for (int i = 0; i < argc; i++)
    {
        DebugMsg(L" EJH Add %s(%d) : Debug Msg [%s]", __FUNCTIONW__, __LINE__, argv[i]);

        if (0 == wcscmp(argv[i],L"Add"))
        {
            return TRUE;
        }
    }
    return FALSE;
}

WCHAR* CmdManager::GetSolutionPath()
{
    WCHAR* szCmd = {0, };

    map<wstring, wstring>::iterator iter;
    iter = m_mapCommandLine.find(L"Sinpath");
    if (iter != m_mapCommandLine.end()) {
        szCmd = (WCHAR*)iter->second.c_str();

        DebugMsg(L"%s(%d) : Debug Msg [%s][%s]", __FUNCTIONW__, __LINE__, szCmd, L"GetSlnPath_SUCCESS");
    }

    return szCmd;
}

WCHAR* CmdManager::GetProjectPath()
{
    WCHAR* szCmd = { 0, };

    map<wstring, wstring>::iterator iter;
    iter = m_mapCommandLine.find(L"Pinpath");
    if (iter != m_mapCommandLine.end()) {
        szCmd = (WCHAR*)iter->second.c_str();

        DebugMsg(L"%s(%d) : Debug Msg [%s][%s]", __FUNCTIONW__, __LINE__, szCmd, L"GetProPath_SUCCESS");
    }

    return szCmd;
}