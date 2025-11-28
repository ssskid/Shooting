#include "Level2.h"
#include "GameController.h" // GameController::GetTimeTotal() 등 사용 시
#include "Object.h"
#include "Graphics.h"     // SCREEN_X, SCREEN_Y 등 사용 시 (필요하다면)

Level2::Level2()
    : sequence(0), player_x_speed(0.0f), player_y_speed(0.0f)
{
    OutputDebugString(L"Level2 Constructor called.\n");
}

Level2::~Level2()
{
  OutputDebugString(L"Level2 Destructor called.\n");
  if (playerCharacter) {
    OutputDebugString(L"Level2 Destructor: Deleting playerCharacter.\n");
    delete playerCharacter;
    playerCharacter = nullptr;
  }
}

// Level2의 로직 상태를 초기화하는 함수
void Level2::initializeLogicState() {
    OutputDebugString(L"Level2::initializeLogicState() called.\n");
    // Level2에 필요한 로직 상태 변수 초기화
    // 예: level2_stagestart_time = GameController::GetTimeTotal();
    // GameController::Stage = L"S T A G E  2"; // 필요하다면 스테이지 이름 설정
    sequence = 0; // 예시: 시퀀스 변수 초기화
    // 기존 Object 정리 로직 (필요한 경우)
}

void Level2::Load(bool reinitializeLogic /*= true*/) {
    if (reinitializeLogic) {
        OutputDebugString(L"Level2::Load() called with reinitializeLogic = true.\n");
        initializeLogicState(); // 게임 로직 상태 초기화
    } else {
        OutputDebugString(L"Level2::Load() called with reinitializeLogic = false (Resource Reload Only).\n");
    }

    // 그래픽 리소스 로드는 항상 수행
    OutputDebugString(L"Level2: Loading graphics resources...\n");
    if (!gfx) {
        OutputDebugString(L"Error: gfx is null in Level2::Load.\n");
        PostQuitMessage(0); return;
    }
    // 모든 MyLoadimage 호출 (기존 Level2::Load() 내용과 동일)
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
    OutputDebugString(L"Level2: Graphics resources loaded.\n");
}

void Level2::Unload() {
    OutputDebugString(L"Level2::Unload() called.\n");
    if (!gfx) return;
    // 모든 MyUnLoadimage 호출 (기존 Level2::Unload() 내용과 동일)
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
}

void Level2::CLS() {
    if(gfx) gfx->ClearScreen(0.6f, 0.6f, 0.8f);
}

void Level2::GLRender() {
    if(!gfx) return;
    // Level2에 맞는 렌더링 (현재는 Stage, LifeandPoint, Status 텍스트만 그림)
    gfx->OnPaintText(GameController::Stage, 280.0f, 170.f, 2); // 위치나 텍스트 내용 변경 가능
    gfx->OnPaintText(GameController::LifeandPoint, 420.0f, 20.f, 18);
    gfx->OnPaintText(GameController::Status, 20.0f, 20.f, 10);
}

void Level2::GLUpdate(float timeTotal, float timeDelta) {
    // Level2의 게임 로직 업데이트 (현재는 비어 있음)
    // 만약 initializeLogicState에서 level2_stagestart_time 등을 설정했다면,
    // 여기서 timeTotal - level2_stagestart_time 을 사용하여 시간 기반 이벤트 처리 가능
    return;
}

Object* Level2::getPlayerCharacterObject() {
  return playerCharacter;
}