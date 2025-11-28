// Shooting3.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//
#include <Windows.h>
#include <d2d1_1.h>
#include <dwrite.h>
#include "Shooting3.h"
#include "Graphics.h"
#include "GameController.h"
#include "Timer.h"
#include "framework.h"
#include "Object.h"
#include "GameLevel.h"
#include "Title.h"
#include "Level1.h"
#include "Level2.h"

#pragma comment(lib, "d2d1")
#pragma comment(lib, "dwrite")

#define MAX_LOADSTRING 100

// 전역 변수:
HINSTANCE hInst;                                                            // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING] = L"김서준 Direct 2D 슈팅 게임";             // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING] = L"김서준 Direct 2D 슈팅 게임";       // 기본 창 클래스 이름입니다.
Graphics* graphics;
Object* pPlayerCharacter, * pCharacter, * pEvent, * pPlayerBullet, * pEnemyBullet, * pFarBackGround, * pBackGround, * pForeGround;
HWND globalhWnd = nullptr;                                                  // WndProc에서 사용할 수 있도록 전역 변수

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    if (S_OK != CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE)) return 0; // COINIT_DISABLE_OLE1DDE 추가

    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 여기에 코드를 입력합니다.

    // 전역 문자열을 초기화합니다.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_SHOOTING3, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);
    
    graphics = new Graphics();
    GameLevel::Init(graphics);

    //FarBackGround
    pFarBackGround = new Object();

    //BackGround
    pFarBackGround->addNext(pBackGround = new Object());
    pBackGround->addPrevious(pFarBackGround);

    //Character
    pBackGround->addNext(pCharacter = new Object());
    pCharacter->addPrevious(pBackGround);

    //PlayerCharacter
    pCharacter->addNext(pPlayerCharacter = new Object());
    pPlayerCharacter->addPrevious(pCharacter);

    //Event
    pPlayerCharacter->addNext(pEvent = new Object());
    pEvent->addPrevious(pPlayerCharacter);

    //EnemyBullet
    pEvent->addNext(pEnemyBullet = new Object());
    pEnemyBullet->addPrevious(pEvent);

    //PlayerBullet
    pEnemyBullet->addNext(pPlayerBullet = new Object());
    pPlayerBullet->addPrevious(pEnemyBullet);

    //ForeGround
    pPlayerBullet->addNext(pForeGround = new Object());
    pForeGround->addPrevious(pPlayerBullet);

    // 애플리케이션 초기화를 수행합니다:
    if (!InitInstance (hInstance, nCmdShow))
    {
        if(graphics) {
             delete graphics; graphics = nullptr;
        }
        CoUninitialize();
        return FALSE;
    }

    // InitInstance가 성공적으로 반환된 후 hWnd를 GameController에 전달
    GameController::mainhWnd = globalhWnd;

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SHOOTING3));
    Object::Init(graphics, pPlayerCharacter, pCharacter, pEvent, pPlayerBullet, pEnemyBullet, pFarBackGround, pBackGround, pForeGround);
    GameController::Init();
    GameController::LoadInitialLevel(new Title()); // 여기서 Title의 Load()가 호출되어 초기 비트맵 로드
    
    MSG msg;
    msg.message = WM_NULL;

    while (msg.message != WM_QUIT) {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) { 
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        else {
            if (graphics && graphics->NeedsResourceReload()) {
                OutputDebugString(L"Graphics resources need reload. Calling current level's Unload() and Load(false).\n");
                
                // GameController에서 현재 GameLevel 객체 포인터를 가져오는 함수가 필요합니다.
                // 예: GameLevel* currentLvlObj = GameController::GetCurrentLevelObject();
                // 이 함수가 GameController에 구현되어 있다고 가정합니다.
                GameLevel* currentLvlObj = GameController::GetCurrentLevelObject(); 

                if (currentLvlObj) {
                    currentLvlObj->Unload();        // 기존 리소스 해제
                    currentLvlObj->Load(false);     // 리소스만 다시 로드 (로직 초기화 건너뜀)
                    OutputDebugString(L"Current level's Unload() and Load(false) called successfully.\n");
                } else {
                    OutputDebugString(L"Cannot reload resources: Current level object is null.\n");
                }
                
                graphics->MarkResourcesAsLoaded(); // 리소스 재 로드 시도 완료로 표시
            }

            GameController::Update(); 
            
            if (graphics) { 
                graphics->BeginDraw();
                GameController::Render();
                graphics->EndDraw();
            }
        }
    }
    
    OutputDebugString(L"wWinMain: WM_QUIT received, starting cleanup.\n");
    GameController::CleanupGameResources(); // 게임 로직 관련 리소스 정리

    if (graphics) {
        OutputDebugString(L"wWinMain: Cleaning up Graphics.\n");
        graphics->Cleanup(); 
        delete graphics;
        graphics = nullptr;
    }

    // Object 헤드 노드들 정리 (Shooting3.cpp의 전역 포인터들)
    OutputDebugString(L"wWinMain: Deleting global Object head nodes.\n");
    delete pFarBackGround; pFarBackGround = nullptr;
    delete pBackGround;    pBackGround = nullptr;
    delete pCharacter;     pCharacter = nullptr;
    delete pPlayerCharacter; pPlayerCharacter = nullptr;
    delete pEvent;         pEvent = nullptr;
    delete pEnemyBullet;   pEnemyBullet = nullptr;
    delete pPlayerBullet;  pPlayerBullet = nullptr;
    delete pForeGround;    pForeGround = nullptr;
    
    CoUninitialize();
    OutputDebugString(L"wWinMain: Exiting.\n");
    return (int) msg.wParam;
}

//
//  함수: MyRegisterClass()
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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1); // nullptr로 설정하여 배경을 직접 그리도록 할 수도 있음
    wcex.lpszMenuName   = NULL; // 메뉴 사용 안 함
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_ICON1));

    return RegisterClassExW(&wcex);
}

//
//   함수: InitInstance(HINSTANCE, int)
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

   // 초기 창 크기 설정 (클라이언트 영역 기준)
   RECT rect = { 0, 0, SCREEN_X, SCREEN_Y };
   AdjustWindowRectEx(&rect, WS_OVERLAPPEDWINDOW, FALSE, 0);

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, CW_USEDEFAULT, rect.right - rect.left, rect.bottom - rect.top, nullptr, nullptr, hInstance, nullptr);

   globalhWnd = hWnd; // 전역 변수에 할당

   if (!hWnd)
   {
      return FALSE;
   }

   if (!graphics->Init(hWnd)) {
       return FALSE;
   }

   // <<< 추가: 시작 시 전체 화면으로 설정 >>>
   graphics->SetFullscreen(true); 
   // ShowWindow 및 UpdateWindow는 SetFullscreen 내부에서 처리될 수 있으나,
   // 명시적으로 호출하는 것이 안전할 수 있습니다.
   // SetFullscreen이 내부적으로 ShowWindow를 호출하지 않는다면 여기서 호출 필요.
   // 현재 Graphics::SetFullscreen(false) 부분에 SetWindowPos가 있으므로,
   // 전체 화면 설정 후 ShowWindow가 필요할 수 있습니다.
   // 하지만 DXGI 전체 화면은 일반적으로 ShowWindow가 필요 없을 수 있습니다.
   // 테스트 후 필요시 아래 주석 해제:
   // ShowWindow(hWnd, nCmdShow); // nCmdShow는 SW_SHOWDEFAULT 등일 수 있음
   // UpdateWindow(hWnd);

   return TRUE;
}

//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_SIZE: // 창 크기 변경 메시지 처리
        if (graphics && wParam != SIZE_MINIMIZED) { // 최소화 상태가 아닐 때만
            OutputDebugString(L"WM_SIZE received.\n");
            graphics->Resize(LOWORD(lParam), HIWORD(lParam));
        }
        return 0; // DefWindowProc 호출 안 함 (DXGI 스왑체인이 처리)

    case WM_SYSKEYDOWN: // Alt 키 조합 등 시스템 키 입력 처리
        if (wParam == VK_RETURN && (lParam & (1 << 29))) // Alt + Enter
        {
            OutputDebugString(L"Alt+Enter pressed.\n");
            if (graphics) {
                graphics->SetFullscreen(!graphics->IsFullscreen());
            }
            return 0; // 메시지 처리됨
        }
        break; // DefWindowProc으로 넘어감

    case WM_KEYDOWN:
        if (wParam == VK_F11) { // F11 키로 전체화면 토글
            OutputDebugString(L"F11 pressed.\n");
            if (graphics) {
                graphics->SetFullscreen(!graphics->IsFullscreen());
            }
            return 0; // 메시지 처리됨
        }
        switch (wParam) {
        case VK_LEFT: GameController::KeyLeft(); break;
        case VK_RIGHT: GameController::KeyRight(); break;
        case VK_UP: GameController::KeyUp(); break;
        case VK_DOWN: GameController::KeyDown(); break;
        case VK_RETURN: GameController::KeyReturn(); break; // Enter 키
        case VK_SPACE: GameController::KeyShoot(); break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam); // 처리 안 된 키는 기본 처리
        }
        break; // WM_KEYDOWN 처리 완료

    case WM_KEYUP:
        switch (wParam) {
        case VK_LEFT: GameController::UnKeyLeft(); break;
        case VK_RIGHT: GameController::UnKeyRight(); break;
        case VK_UP: GameController::UnKeyUp(); break;
        case VK_DOWN: GameController::UnKeyDown(); break;
        case VK_RETURN: GameController::UnKeyReturn(); break;
        case VK_SPACE: GameController::UnKeyShoot(); break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam); // 처리 안 된 키는 기본 처리
        }
        break; // WM_KEYUP 처리 완료

    case WM_ACTIVATEAPP: // 애플리케이션 활성화/비활성화 시
        if (graphics && graphics->IsFullscreen()) {
            if (wParam == FALSE) { // 비활성화 (예: Alt+Tab)
                // 전체화면 모드에서 비활성화될 때 창 모드로 전환하거나 최소화하는 등의 처리 가능
            } else { // 활성화
            }
        }
        break; // DefWindowProc으로 넘어감

    case WM_DESTROY:
        OutputDebugString(L"WM_DESTROY received.\n");
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0; // 대부분의 메시지를 처리했으면 0을 반환
}

