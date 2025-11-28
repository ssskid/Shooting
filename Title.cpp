#include "Title.h"
#include "Object.h"
#include "GameController.h" // GameController::GetTimeTotal() 등 사용 시
#include "Graphics.h"     // SCREEN_X, SCREEN_Y 사용 시 (필요하다면)

bool Title::backgroundObjectsCreated = false; // 정적 멤버 초기화

Title::Title()
    : nMenuSelected(0), nMenuInput(-1), 
      logotime(0.0f), fLogoOpaque(0.0f), 
      logoAppearDelay(2.0f), logoFadeInDuration(3.0f), // 초기값 설정
      Settings(false), pPC(nullptr)
{
    OutputDebugString(L"Title Constructor called.\n");
}

// Title 소멸자 추가
Title::~Title() {
    OutputDebugString(L"Title Destructor called.\n");
    if (pPC) { // pPC가 GameController로 전달되지 않고 남아있다면 (예: 게임 시작 안하고 종료)
        delete pPC;
        pPC = nullptr;
    }
}

void Title::initializeLogicState() {
    OutputDebugString(L"Title::initializeLogicState() called.\n");
    nMenuSelected = 0;
    nMenuInput = -1;
    logotime = GameController::GetTimeTotal(); // 로직 초기화 시 기준 시간 설정
    fLogoOpaque = 0.0f; // 로고 투명도 초기화
    GameController::Status = L"";
    GameController::Stage = L"";

    if (!pPC) { // pPC가 아직 없으면 새로 생성
        pPC = new Object(200.0f, 120.0f, 0.0f, 0.0f, 100, OBJECT_NEUTRAL, ACTION_INTRO, SHAPE_CAMEL, 0, 1.0f, 15.0f);
        OutputDebugString(L"Title: pPC created.\n");
    } else {
        pPC->setPosition(200.0f, 120.0f); // 예시
        pPC->reassignObj(OBJECT_NEUTRAL, ACTION_INTRO); // 타이틀 화면용 상태로 되돌림
        OutputDebugString(L"Title: pPC reset.\n");
    }

    if (!backgroundObjectsCreated) {
        OutputDebugString(L"Title: Creating background objects for the first time.\n");
        // 기존 Object::getBackGround() 리스트에 이미 객체가 있는지 확인하고, 없다면 생성
        // 또는, Title 레벨 전용 배경 객체라면 여기서 그냥 생성
        // 예시: Title 화면 전용 배경 객체 생성
        new Object((float)(SCREEN_X >> 1), (float)(SCREEN_Y >> 1) + 10.f, -0.5f, 0.0f, 100, OBJECT_BACKGROUND, ACTION_BACKGROUND, SHAPE_SKY1);
        new Object((float)((SCREEN_X >> 1) + SCREEN_X), (float)(SCREEN_Y >> 1) + 10.f, -0.5f, 0.0f, 100, OBJECT_BACKGROUND, ACTION_BACKGROUND, SHAPE_SKY1);
        backgroundObjectsCreated = true;
    }
}

void Title::Load(bool reinitializeLogic /*= true*/) {
    if (reinitializeLogic) {
        OutputDebugString(L"Title::Load() called with reinitializeLogic = true.\n");
        initializeLogicState(); 
    } else {
        OutputDebugString(L"Title::Load() called with reinitializeLogic = false (Resource Reload Only).\n");
    }

    OutputDebugString(L"Title: Loading graphics resources...\n");
    if (!gfx) { OutputDebugString(L"Error: gfx is null in Title::Load.\n"); PostQuitMessage(0); return; }

    if (!gfx->MyLoadimage(0, 0, L"1.png")) { OutputDebugString(L"Failed to load 1.png\n"); PostQuitMessage(0); return; }
    if (!gfx->MyLoadimage(0, 1, L"2.png")) { OutputDebugString(L"Failed to load 2.png\n"); PostQuitMessage(0); return; }
    if (!gfx->MyLoadimage(0, 2, L"3.png")) { OutputDebugString(L"Failed to load 3.png\n"); PostQuitMessage(0); return; }
    if (!gfx->MyLoadimage(1, 0, L"4.png")) { OutputDebugString(L"Failed to load 4.png\n"); PostQuitMessage(0); return; }
    if (!gfx->MyLoadimage(1, 1, L"5.png")) { OutputDebugString(L"Failed to load 5.png\n"); PostQuitMessage(0); return; }
    if (!gfx->MyLoadimage(1, 2, L"6.png")) { OutputDebugString(L"Failed to load 6.png\n"); PostQuitMessage(0); return; }
    if (!gfx->MyLoadimage(2, 0, L"20.png")) { OutputDebugString(L"Failed to load 20.png\n"); PostQuitMessage(0); return; }
    if (!gfx->MyLoadimage(3, 0, L"30.png")) { OutputDebugString(L"Failed to load 30.png\n"); PostQuitMessage(0); return; }
    if (!gfx->MyLoadimage(4, 0, L"40.png")) { OutputDebugString(L"Failed to load 40.png\n"); PostQuitMessage(0); return; }
    if (!gfx->MyLoadimage(6, 0, L"60.png")) { OutputDebugString(L"Failed to load 60.png\n"); PostQuitMessage(0); return; }
    if (!gfx->MyLoadimage(6, 1, L"61.png")) { OutputDebugString(L"Failed to load 61.png\n"); PostQuitMessage(0); return; }
    if (!gfx->MyLoadimage(6, 2, L"62.png")) { OutputDebugString(L"Failed to load 62.png\n"); PostQuitMessage(0); return; }
    if (!gfx->MyLoadimage(6, 3, L"63.png")) { OutputDebugString(L"Failed to load 63.png\n"); PostQuitMessage(0); return; }
    if (!gfx->MyLoadimage(6, 4, L"64.png")) { OutputDebugString(L"Failed to load 64.png\n"); PostQuitMessage(0); return; }
    if (!gfx->MyLoadimage(6, 5, L"65.png")) { OutputDebugString(L"Failed to load 65.png\n"); PostQuitMessage(0); return; }
    if (!gfx->MyLoadimage(6, 6, L"66.png")) { OutputDebugString(L"Failed to load 66.png\n"); PostQuitMessage(0); return; }
    if (!gfx->MyLoadimage(6, 7, L"67.png")) { OutputDebugString(L"Failed to load 67.png\n"); PostQuitMessage(0); return; }
    if (!gfx->MyLoadimage(7, 0, L"70.png")) { OutputDebugString(L"Failed to load 70.png\n"); PostQuitMessage(0); return; }
    if (!gfx->MyLoadimage(7, 1, L"71.png")) { OutputDebugString(L"Failed to load 71.png\n"); PostQuitMessage(0); return; }
    if (!gfx->MyLoadimage(7, 2, L"72.png")) { OutputDebugString(L"Failed to load 72.png\n"); PostQuitMessage(0); return; }
    if (!gfx->MyLoadimage(7, 3, L"73.png")) { OutputDebugString(L"Failed to load 73.png\n"); PostQuitMessage(0); return; }
    if (!gfx->MyLoadimage(7, 4, L"74.png")) { OutputDebugString(L"Failed to load 74.png\n"); PostQuitMessage(0); return; }
    if (!gfx->MyLoadimage(7, 5, L"75.png")) { OutputDebugString(L"Failed to load 75.png\n"); PostQuitMessage(0); return; }
    if (!gfx->MyLoadimage(7, 6, L"76.png")) { OutputDebugString(L"Failed to load 76.png\n"); PostQuitMessage(0); return; }
    if (!gfx->MyLoadimage(7, 7, L"77.png")) { OutputDebugString(L"Failed to load 77.png\n"); PostQuitMessage(0); return; }
    if (!gfx->MyLoadimage(8, 0, L"80.png")) { OutputDebugString(L"Failed to load 80.png\n"); PostQuitMessage(0); return; }
    if (!gfx->MyLoadimage(8, 1, L"81.png")) { OutputDebugString(L"Failed to load 81.png\n"); PostQuitMessage(0); return; }
    if (!gfx->MyLoadimage(8, 2, L"82.png")) { OutputDebugString(L"Failed to load 82.png\n"); PostQuitMessage(0); return; }
    if (!gfx->MyLoadimage(8, 3, L"83.png")) { OutputDebugString(L"Failed to load 83.png\n"); PostQuitMessage(0); return; }
    if (!gfx->MyLoadimage(8, 4, L"84.png")) { OutputDebugString(L"Failed to load 84.png\n"); PostQuitMessage(0); return; }
    if (!gfx->MyLoadimage(8, 5, L"85.png")) { OutputDebugString(L"Failed to load 85.png\n"); PostQuitMessage(0); return; }
    if (!gfx->MyLoadimage(8, 6, L"86.png")) { OutputDebugString(L"Failed to load 86.png\n"); PostQuitMessage(0); return; }
    if (!gfx->MyLoadimage(8, 7, L"87.png")) { OutputDebugString(L"Failed to load 87.png\n"); PostQuitMessage(0); return; }
    if (!gfx->MyLoadimage(10, 0, L"bg1.png")) { OutputDebugString(L"Failed to load bg1.png\n"); PostQuitMessage(0); return; }
    if (!gfx->MyLoadimage(11, 0, L"Logo.png")) { OutputDebugString(L"Failed to load Logo.png\n"); PostQuitMessage(0); return; }
    OutputDebugString(L"Title: Graphics resources loaded.\n");
}

void Title::Unload() {
    OutputDebugString(L"Title::Unload() called.\n");
    if (!gfx) return;

    gfx->MyUnLoadimage(0, 0);
    gfx->MyUnLoadimage(0, 1);
    gfx->MyUnLoadimage(0, 2);
    gfx->MyUnLoadimage(1, 0);
    gfx->MyUnLoadimage(1, 1);
    gfx->MyUnLoadimage(1, 2);
    gfx->MyUnLoadimage(2, 0);
    gfx->MyUnLoadimage(3, 0);
    gfx->MyUnLoadimage(4, 0);
    gfx->MyUnLoadimage(6, 0);
    gfx->MyUnLoadimage(6, 1);
    gfx->MyUnLoadimage(6, 2);
    gfx->MyUnLoadimage(6, 3);
    gfx->MyUnLoadimage(6, 4);
    gfx->MyUnLoadimage(6, 5);
    gfx->MyUnLoadimage(6, 6);
    gfx->MyUnLoadimage(6, 7);
    gfx->MyUnLoadimage(7, 0);
    gfx->MyUnLoadimage(7, 1);
    gfx->MyUnLoadimage(7, 2);
    gfx->MyUnLoadimage(7, 3);
    gfx->MyUnLoadimage(7, 4);
    gfx->MyUnLoadimage(7, 5);
    gfx->MyUnLoadimage(7, 6);
    gfx->MyUnLoadimage(7, 7);
    gfx->MyUnLoadimage(8, 0);
    gfx->MyUnLoadimage(8, 1);
    gfx->MyUnLoadimage(8, 2);
    gfx->MyUnLoadimage(8, 3);
    gfx->MyUnLoadimage(8, 4);
    gfx->MyUnLoadimage(8, 5);
    gfx->MyUnLoadimage(8, 6);
    gfx->MyUnLoadimage(8, 7);
    gfx->MyUnLoadimage(10, 0);
    gfx->MyUnLoadimage(11, 0);

    // pPC 객체는 소멸자에서 관리하거나, GameController로 소유권이 넘어갔다면 여기서 delete하지 않음
}

void Title::CLS() {
    if (gfx) gfx->ClearScreen(0.6f, 0.6f, 0.8f);
}

void Title::GLUpdate(float timeTotal, float timeDelta) {
    float elapsedTimeSinceLoad = timeTotal - logotime; // 로직 초기화 후 경과 시간

    if (elapsedTimeSinceLoad >= logoAppearDelay) {
        float fadeInProgress = (elapsedTimeSinceLoad - logoAppearDelay) / logoFadeInDuration;
        if (fadeInProgress < 0.0f) fadeInProgress = 0.0f;
        if (fadeInProgress > 1.0f) fadeInProgress = 1.0f;
        fLogoOpaque = fadeInProgress;
    } 
    else {
        fLogoOpaque = 0.0f;
    }

    if (nMenuInput >= 0) {
        switch (nMenuInput)
        {
        case 0: // Game Start
            GameController::setPlayerLife(2);
            GameController::setPlayerPoint(0);

            if (pPC) {
                OutputDebugString(L"Title: Game Start selected. Reassigning pPC and passing to GameController.\n");
                pPC->reassignObj(OBJECT_PLAYABLE_CHARACTER, ACTION_PLAYER); // 플레이어 상태로 변경
                GameController::SetPendingPlayerObject(pPC); // GameController로 객체 전달
                pPC = nullptr; // Title은 더 이상 이 객체를 소유하지 않음 (메모리 해제 책임 없음)
            } else {
                OutputDebugString(L"Title: pPC is null, cannot start game with existing object.\n");
                // 필요하다면 여기서 새 플레이어 객체를 만들어 전달할 수도 있음
                // Object* newPlayer = new Object(...); GameController::SetPendingPlayerObject(newPlayer);
            }
            
            GameController::setPlayerStarted();
            GameController::changeCurrentLevel(1);
            nMenuInput = -1;
            break;
        case 1:
            GameController::Status = L"Settings Not Supported Yet";
            nMenuInput = -1;
            break;
        case 2:
            GameController::Status = L"Staff Not Supported Yet";
            nMenuInput = -1;
            break;
        case 3:
            nMenuInput = -1;
            PostQuitMessage(0);
            break;
        default:
            break;
        }
    }
}

void Title::GLRender() {
    if (!gfx) return;
    gfx->OnPaintRenderTarget(320.0f, 100.0f, 11, 0, fLogoOpaque); 
    if (fLogoOpaque >= 1.0f) {
      gfx->OnPaintText(L"Game Start", 400.0f, 180.f, nMenuSelected == 0 ? 0 : 1);
      gfx->OnPaintText(L"Settings", 400.0f, 220.f, nMenuSelected == 1 ? 0 : 1);
      gfx->OnPaintText(L"Staff", 400.0f, 260.f, nMenuSelected == 2 ? 0 : 1);
      gfx->OnPaintText(L"Exit", 400.0f, 300.f, nMenuSelected == 3 ? 0 : 1);
      gfx->OnPaintText(GameController::Status, 20.0f, 20.f, 10);
    }
}

void Title::GLKeyUp() {
    if (fLogoOpaque < 1.0f) return;
    nMenuSelected--;
    if (nMenuSelected < 0) nMenuSelected = 3;
}

void Title::GLKeyDown() {
    if (fLogoOpaque < 1.0f) return;
    nMenuSelected++;
    if (nMenuSelected >= 4) nMenuSelected = 0;
}

void Title::GLKeyReturn() {
    if (fLogoOpaque < 1.0f) return;
    nMenuInput = nMenuSelected;
}

void Title::GLKeyShoot() {
    if (fLogoOpaque < 1.0f) return; 
    nMenuInput = nMenuSelected;
}

void Title::GLUnKeyUp() { }
void Title::GLUnKeyDown() { }
void Title::GLUnKeyReturn() { }
void Title::GLUnKeyShoot() { }

Object* Title::getPlayerCharacterObject() {
    return pPC; // 만약 pPC가 GameController에서 알아야 할 플레이어 객체라면
}
