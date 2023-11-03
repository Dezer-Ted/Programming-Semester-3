// MySecondApplication.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "MySecondApplication.h"

#include <string>
#include <sstream>
#include <Windowsx.h>
#include <format>
#include <vector>


#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
std::vector<POINT> g_Points{};
WPARAM g_LastTitle{0};

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_MYSECONDAPPLICATION, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    const HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MYSECONDAPPLICATION));

    MSG msg;

    // Main message loop:
    while (true)
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            // Process the message
            if (msg.message == WM_QUIT)
                break;

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        Sleep(1);
    }

    return static_cast<int>(msg.wParam);
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MYSECONDAPPLICATION));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_MYSECONDAPPLICATION);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
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
    case WM_ACTIVATE:
    {
        //MessageBoxW(hWnd, L"Windows is active", L"Active", MB_OK);
        if (g_LastTitle == NULL) g_LastTitle = wParam;
        if (wParam != WA_INACTIVE)
            SendMessage(hWnd, WM_SETTEXT, 0, g_LastTitle);
        else 
           SendMessage(hWnd, WM_SETTEXT, 0, (LPARAM)L"Go away");
        
       // if (wParam != WA_INACTIVE)
       //     MessageBox(hWnd,L"W is active", L"Clicked", MB_OK);
    }
    //TODO: Add code for when the windows is not active
    break;
    case WM_PAINT:
        {
            //PAINTSTRUCT ps;
            //HDC hdc = BeginPaint(hWnd, &ps);
            //// TODO: Add any drawing code that uses hdc here...
            //EndPaint(hWnd, &ps);
        PAINTSTRUCT ps;
        RECT rect;
        HDC hdc = BeginPaint(hWnd, &ps);

        GetClientRect(hWnd, &rect); // call window rect

        HPEN bluePen = CreatePen(PS_SOLID, 2, RGB(0, 0, 255)); // create Pen
        HPEN oldPen = (HPEN)SelectObject(hdc, bluePen); // replace and keep old Pen
     
        if(g_Points.size() > 1)
        for (size_t i = 0; i < g_Points.size() -1 ; i++)
        {
            MoveToEx(hdc, g_Points[i].x, g_Points[i].y, 0); // draw line part 1
            LineTo(hdc, g_Points[i+1].x, g_Points[i+1].y); // draw line part 2 
            MoveToEx(hdc, 0, 0, nullptr);
        }
        //MoveToEx(hdc, 0, 0, 0); // draw line part 1
        //LineTo(hdc, rect.right, rect.bottom); // draw line part 2

        EndPaint(hWnd, &ps);
      
        }
        break;
    case WM_LBUTTONDOWN:
    {
        int xPos = GET_X_LPARAM(lParam);
        int yPos = GET_Y_LPARAM(lParam);

        g_Points.push_back(POINT(xPos, yPos));
        InvalidateRect(hWnd, NULL, TRUE);
        //const std::wstring string{ std::format(L"X {} Y {}", xPos, yPos)};

       /* MessageBox
        (
            nullptr,
            (LPCWSTR)string.c_str(),
            (LPCWSTR)L"Mouse Pressed",
            MB_ICONINFORMATION | MB_OK
        );*/
    }
    break;
    case WM_RBUTTONDOWN:
    {
        g_Points.clear();
        InvalidateRect(hWnd, NULL, TRUE);
    }
    break;
    case WM_LBUTTONUP:
    {  
        int xPos{ GET_X_LPARAM(lParam) };
        int yPos{ GET_Y_LPARAM(lParam) };

        std::wstringstream buffer;
        buffer << L"Clicked on: (";
        buffer << xPos;
        buffer << L", ";
        buffer << yPos;
        buffer << L")";

       // MessageBoxW(hWnd, buffer.str().c_str(), L"Clicked", MB_OK);
        break;
    }
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
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
