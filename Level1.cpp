#include "Level1.h"
#include "GameController.h"
#include "Object.h" // Object 클래스 정의 포함

Level1::Level1() : ReentryTimer(0.0f), GameoverTimer(0.0f), playerCharacter(nullptr) {
    for (int i = 0; i < 5; ++i) effect[i] = 0.0f;
    OutputDebugString(L"Level1 Constructor called.\n");
}

Level1::~Level1() {
    OutputDebugString(L"Level1 Destructor called.\n");
    if (playerCharacter) {
        OutputDebugString(L"Level1 Destructor: Deleting playerCharacter.\n");
        delete playerCharacter;
        playerCharacter = nullptr;
    }
}

void Level1::init_game_logic_state() {
    OutputDebugString(L"Level1::init_game_logic_state() called.\n");
    stagestart = GameController::GetTimeTotal();
    GameController::Stage = L"S T A G E  1";
    Gameover = false;
    StageClear = false;
    ReentryTimer = 0.0f;
    GameoverTimer = 0.0f;
    flag15ActivationTime = 0.0f; // 초기화
    for (int i = 0; i < 20; ++i) flag[i] = false;

    if (!playerCharacter) {
        playerCharacter = GameController::GetPendingPlayerObjectAndClear();
        if (playerCharacter) {
            OutputDebugString(L"Level1: Received player object from GameController.\n");
        } else {
            OutputDebugString(L"Level1: No pending player object from GameController. Creating new one.\n");
        }
    }
    if (!GameController::getPlayerAlive() && playerCharacter) {
         GameController::setPlayerStarted();
    }
}

void Level1::Load(bool reinitializeLogic /*= true*/) {
    if (reinitializeLogic) {
        OutputDebugString(L"Level1::Load() called with reinitializeLogic = true.\n");
        init_game_logic_state();
    } else {
        OutputDebugString(L"Level1::Load() called with reinitializeLogic = false (Resource Reload Only).\n");
    }

    OutputDebugString(L"Level1: Loading graphics resources...\n");
    if (!gfx) {
        OutputDebugString(L"Error: gfx is null in Level1::Load.\n");
        PostQuitMessage(0);
        return;
    }
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
    OutputDebugString(L"Level1: Graphics resources loaded.\n");
}

void Level1::Unload() {
    OutputDebugString(L"Level1::Unload() called.\n");
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
}

void Level1::CLS() {
    if (gfx) gfx->ClearScreen(0.6f, 0.6f, 0.8f);
}

void Level1::GLRender() {
    if (!gfx) return;
    gfx->OnPaintText(GameController::Stage, 0.0f, 0.0f, 26);
    gfx->OnPaintText(GameController::LifeandPoint, 420.0f, 20.f, 18);
    gfx->OnPaintText(GameController::Status, 20.0f, 20.f, 10);
}

void Level1::GLUpdate(float timeTotal, float timeDelta) {
    // 플레이어 생존 여부 확인 (GameController::getPlayerAlive()는 플레이어가 조작 가능한 상태인지)
    // playerCharacter 포인터는 현재 레벨의 플레이어 객체를 가리킴 (추락 중인 EFFECT 객체일 수도 있음)
    if (!GameController::getPlayerAlive()) { // 플레이어가 조작 불가능한 상태 (사망했거나, 아직 등장 전)
        if (!Gameover && GameController::IsThisGameOver()) { // 라이프가 0 미만이면 게임오버
            Gameover = true;
            GameController::Stage = L"G a m e O v e r";
            GameoverTimer = timeTotal + 2.5f; // 게임오버 메시지 표시 후 타이틀로 돌아갈 시간
            OutputDebugString(L"Level1: Game Over condition met.\n");
            // 게임오버 시에는 더 이상 플레이어 리스폰 없음
        }
        // 플레이어가 사망했고 (fPlayerShottime > 0), 게임오버가 아니며,
        // 현재 playerCharacter 객체가 (추락 후) 화면 밖으로 50px 이상 벗어났거나, 또는 askIsbeingdeleted()가 true일 때 리스폰 시도
        else if (!Gameover && GameController::getPlayerShottime() > 0.0f &&
                 (playerCharacter == nullptr || 
                  (playerCharacter->getObjectType() == OBJECT_EFFECT && 
                   (playerCharacter->askIsbeingdeleted() || 
                    playerCharacter->getXPos() < -50.0f || playerCharacter->getXPos() > SCREEN_X + 50.0f ||
                    playerCharacter->getYPos() < -50.0f || playerCharacter->getYPos() > SCREEN_Y + 50.0f
                   )
                  )
                 ) &&
                 GameController::getPlayerShottime() + 1.5f <= timeTotal) { // 사망 후 1.5초 뒤 리스폰 시도

            // 이전 playerCharacter (추락하던 EFFECT 객체)가 삭제되었는지 확인 또는 화면 밖으로 나갔는지 확인
            if (playerCharacter && (playerCharacter->askIsbeingdeleted() || 
                                   playerCharacter->getXPos() < -50.0f || playerCharacter->getXPos() > SCREEN_X + 50.0f ||
                                   playerCharacter->getYPos() < -50.0f || playerCharacter->getYPos() > SCREEN_Y + 50.0f)) {
                // GameController::Update()에서 delete temp; 로 처리되므로 여기서 delete 안 함.
                // 다만, playerCharacter 포인터는 nullptr로 만들어야 새 객체를 할당받을 수 있음.
                // askIsbeingdeleted()가 true가 되도록 Object.cpp에서 설정하거나, 여기서 직접 delete
                if (!playerCharacter->askIsbeingdeleted()) { // 아직 삭제 요청이 안된 상태면 강제 삭제
                    OutputDebugString(L"Level1: Player effect object went off-screen (beyond 50px). Deleting.\n");
                    delete playerCharacter; // 직접 삭제
                }
                playerCharacter = nullptr;
                OutputDebugString(L"Level1: Previous player effect object is now null (deleted or marked for deletion).\n");
            }
            
            if (playerCharacter == nullptr) { // 확실히 이전 객체가 정리된 후 새 객체 생성
                OutputDebugString(L"Level1: Player died, attempting to respawn.\n");
                // 새 플레이어 객체를 화면 좌측 중-상단에 생성 (OBJECT_EFFECT, ACTION_IDLE로 등장 애니메이션)
                // 초기 위치는 화면 밖 좌측, 약간 위쪽으로 설정하여 날아 들어오는 효과
                playerCharacter = new Object(-50.0f, 100.0f, PSPEEDF, 0.0f, 10, OBJECT_EFFECT, ACTION_IDLE, SHAPE_CAMEL, 0, 0.5f, 15.0f);
                playerCharacter->resetcreationtime(timeTotal); // 등장 애니메이션용 시간 기록
                // playerCharacter->setBlip(); // 등장 시 바로 깜빡이게 할 수도 있음
                OutputDebugString(L"Level1: New player (as EFFECT for entry) created.\n");
            }
        }
        // 등장 애니메이션 (OBJECT_EFFECT, ACTION_IDLE) 후 플레이 가능 상태로 전환
        else if (!Gameover && playerCharacter && playerCharacter->getObjectType() == OBJECT_EFFECT &&
                 playerCharacter->getCreationTime() + 1.0f <= timeTotal) { // 등장 애니메이션 1초 후
            
            // 실제 플레이 가능한 위치로 이동 (예: 화면 좌측 안쪽)
            playerCharacter->setPosition(50.0f, 180.0f); // 원하는 등장 완료 위치
            playerCharacter->setSpeed(0.0f, 0.0f); // 정지

            OutputDebugString(L"Level1: Respawned player (EFFECT) becoming active PLAYABLE_CHARACTER.\n");
            GameController::setPlayerStarted(); // GameController의 bPlayerActive = true; (조작 가능 상태)
            playerCharacter->reassignObj(OBJECT_PLAYABLE_CHARACTER, ACTION_PLAYER);
            playerCharacter->setBlip(); // 무적 깜빡임 시작
            playerCharacter->changeOpaque(1.0f);
            // playerCharacter->resetcreationtime(timeTotal); // 무적 시간용으로 재설정할 수도 있음
        }
    }

    // 스테이지 시작 메시지 표시 시간 제어: stagestart로부터 2초 후 메시지 제거
    if (!flag[0] && (timeTotal - stagestart >= 2.0f)) {
      flag[0] = true;
      GameController::Stage = L"\0"; // 스테이지 메시지 제거
    }

    if (!flag[2] && timeTotal - stagestart >= 1.9f) {
        flag[2] = true;
        new Object(670.0f, 80.0f, -2.0f, 0.00f, 10, OBJECT_ENEMY, ACTION_FIGHTER, SHAPE_FOKKER, 0, 1.0f, 15.0f);
        new Object(670.0f, 200.0f, -2.0f, -0.03f, 10, OBJECT_ENEMY, ACTION_FIGHTER, SHAPE_FOKKER, 0, 1.0f, 15.0f);
    }
    else if (flag[2] && GameController::getPlayerAlive() && GameController::getEnemycount() == 0 && !flag[3]) {
        flag[3] = true;
        new Object(670.0f, 80.0f, -2.0f, 0.30f, 10, OBJECT_ENEMY, ACTION_FIGHTER, SHAPE_FOKKER, 0, 1.0f, 15.0f);
        new Object(670.0f, 100.0f, -2.0f, 0.03f, 10, OBJECT_ENEMY, ACTION_FIGHTER, SHAPE_FOKKER, 0, 1.0f, 15.0f);
        new Object(670.0f, 200.0f, -2.0f, -0.03f, 10, OBJECT_ENEMY, ACTION_FIGHTER, SHAPE_FOKKER, 0, 1.0f, 15.0f);
        new Object(670.0f, 360.0f, -2.0f, -0.40f, 10, OBJECT_ENEMY, ACTION_FIGHTER, SHAPE_FOKKER, 0, 1.0f, 15.0f);
    }
    else if (flag[3] && GameController::getPlayerAlive() && GameController::getEnemycount() == 0 && !flag[4]) {
        flag[4] = true;
        new Object(670.0f, 80.0f, -2.0f, 0.00f, 10, OBJECT_ENEMY, ACTION_FIGHTER, SHAPE_FOKKER, 0, 1.0f, 15.0f);
        new Object(670.0f, 100.0f, -2.3f, 0.10f, 10, OBJECT_ENEMY, ACTION_TRAINED, SHAPE_FOKKER, 0, 1.0f, 15.0f);
        new Object(670.0f, 200.0f, -2.3f, 0.00f, 10, OBJECT_ENEMY, ACTION_TRAINED, SHAPE_FOKKER, 0, 1.0f, 15.0f);
        new Object(670.0f, 360.0f, -2.0f, -0.40f, 10, OBJECT_ENEMY, ACTION_FIGHTER, SHAPE_FOKKER, 0, 1.0f, 15.0f);
    }
    else if (flag[4] && GameController::getPlayerAlive() && GameController::getEnemycount() <= 1 && !flag[5]) {
        flag[5] = true;
        new Object(670.0f, 80.0f, -2.3f, 0.00f, 10, OBJECT_ENEMY, ACTION_TRAINED, SHAPE_FOKKER, 0, 1.0f, 15.0f);
        new Object(670.0f, 100.0f, -2.3f, 0.10f, 10, OBJECT_ENEMY, ACTION_VETERAN, SHAPE_FOKKER, 0, 1.0f, 15.0f);
        new Object(670.0f, 200.0f, -2.3f, 0.00f, 10, OBJECT_ENEMY, ACTION_TRAINED, SHAPE_FOKKER, 0, 1.0f, 15.0f);
        new Object(670.0f, 360.0f, -2.3f, -0.40f, 10, OBJECT_ENEMY, ACTION_FIGHTER, SHAPE_FOKKER, 0, 1.0f, 15.0f);
    }
    else if (flag[5] && GameController::getPlayerAlive() && GameController::getEnemycount() == 0 && !flag[6]) {
        flag[6] = true;
        new Object(670.0f, 10.0f, -2.0f, 0.90f, 10, OBJECT_ENEMY, ACTION_TRAINED, SHAPE_FOKKER, 0, 1.0f, 15.0f);
        new Object(670.0f, 100.0f, -1.0f, 0.00f, 300, OBJECT_ENEMY, ACTION_BALLOON8, SHAPE_AIRSHIP, 0, 1.0f, 15.0f);
        new Object(670.0f, 350.0f, -2.0f, -0.90f, 10, OBJECT_ENEMY, ACTION_TRAINED, SHAPE_FOKKER, 0, 1.0f, 15.0f);
    }
    else if (flag[6] && GameController::getPlayerAlive() && GameController::getEnemycount() < 3 && !flag[7]) {
        flag[7] = true;
        new Object(670.0f, 180.0f, -2.0f, 0.00f, 30, OBJECT_ENEMY, ACTION_KAMIKAZE, SHAPE_FOKKER, 0, 1.0f, 15.0f);
    }
    else if (flag[7] && GameController::getPlayerAlive() && GameController::getEnemycount() < 3 && !flag[8]) {
        flag[8] = true;
        new Object(670.0f, 180.0f, -2.3f, 0.00f, 30, OBJECT_ENEMY, ACTION_VETERAN, SHAPE_FOKKER, 0, 1.0f, 15.0f);
    }
    else if (flag[8] && GameController::getPlayerAlive() && GameController::getEnemycount() == 0 && !flag[9]) {
        flag[9] = true;
        new Object(670.0f, 80.0f, -2.3f, 0.80f, 10, OBJECT_ENEMY, ACTION_TRAINED, SHAPE_FOKKER, 0, 1.0f, 15.0f);
        new Object(670.0f, 100.0f, -2.3f, 0.10f, 10, OBJECT_ENEMY, ACTION_VETERAN, SHAPE_FOKKER, 0, 1.0f, 15.0f);
        new Object(670.0f, 220.0f, -2.3f, -0.10f, 10, OBJECT_ENEMY, ACTION_TRAINED, SHAPE_FOKKER, 0, 1.0f, 15.0f);
        new Object(670.0f, 320.0f, -2.3f, -0.80f, 10, OBJECT_ENEMY, ACTION_TRAINED, SHAPE_FOKKER, 0, 1.0f, 15.0f);
    }
    else if (flag[9] && GameController::getPlayerAlive() && GameController::getEnemycount() == 0 && !flag[10]) {
        flag[10] = true;
        new Object(670.0f, 40.0f, -2.3f, 0.80f, 10, OBJECT_ENEMY, ACTION_TRAINED, SHAPE_FOKKER, 0, 1.0f, 15.0f);
        new Object(670.0f, 100.0f, -1.0f, 0.00f, 300, OBJECT_ENEMY, ACTION_BALLOON8, SHAPE_AIRSHIP, 0, 1.0f, 15.0f);
        new Object(670.0f, 200.0f, -2.3f, 0.00f, 10, OBJECT_ENEMY, ACTION_TRAINED, SHAPE_FOKKER, 0, 1.0f, 15.0f);
        new Object(670.0f, 320.0f, -2.3f, -0.80f, 10, OBJECT_ENEMY, ACTION_TRAINED, SHAPE_FOKKER, 0, 1.0f, 15.0f);
    }
    else if (flag[10] && GameController::getPlayerAlive() && GameController::getEnemycount() == 0 && !flag[11]) {
        flag[11] = true;
        new Object(670.0f, 40.0f, -2.3f, 1.50f, 10, OBJECT_ENEMY, ACTION_TRAINED, SHAPE_FOKKER, 0, 1.0f, 15.0f);
        new Object(670.0f, 100.0f, -1.0f, 0.00f, 300, OBJECT_ENEMY, ACTION_BALLOON8, SHAPE_AIRSHIP, 0, 1.0f, 15.0f);
        new Object(670.0f, 320.0f, -2.3f, -1.50f, 10, OBJECT_ENEMY, ACTION_TRAINED, SHAPE_FOKKER, 0, 1.0f, 15.0f);
    }
    else if (flag[11] && GameController::getPlayerAlive() && GameController::getEnemycount() < 3 && !flag[12]) {
        flag[12] = true;
        new Object(670.0f, 180.0f, -2.0f, 0.00f, 30, OBJECT_ENEMY, ACTION_KAMIKAZE, SHAPE_FOKKER, 0, 1.0f, 15.0f);
    }
    else if (flag[12] && GameController::getPlayerAlive() && GameController::getEnemycount() < 3 && !flag[13]) {
        flag[13] = true;
        new Object(670.0f, 180.0f, -2.3f, 0.00f, 10, OBJECT_ENEMY, ACTION_VETERAN, SHAPE_FOKKER, 0, 1.0f, 15.0f);
    }
    else if (flag[13] && GameController::getPlayerAlive() && GameController::getEnemycount() == 0 && !flag[14]) {
        flag[14] = true;
        new Object(670.0f, 80.0f, -2.3f, 1.50f, 10, OBJECT_ENEMY, ACTION_TRAINED, SHAPE_FOKKER, 0, 1.0f, 15.0f);
        new Object(670.0f, 100.0f, -2.3f, 0.10f, 10, OBJECT_ENEMY, ACTION_VETERAN, SHAPE_FOKKER, 0, 1.0f, 15.0f);
        new Object(670.0f, 220.0f, -2.3f, -0.10f, 10, OBJECT_ENEMY, ACTION_TRAINED, SHAPE_FOKKER, 0, 1.0f, 15.0f);
        new Object(670.0f, 320.0f, -2.3f, -1.50f, 10, OBJECT_ENEMY, ACTION_TRAINED, SHAPE_FOKKER, 0, 1.0f, 15.0f);
    }
    else if (flag[14] && GameController::getPlayerAlive() && GameController::getEnemycount() == 0 && !flag[15]) {
        flag[15] = true;
        flag15ActivationTime = timeTotal; // flag[15] 활성화 시간 기록
        OutputDebugString(L"Level1: Flag 15 activated. Final wave incoming.\n");
        new Object(670.0f, 40.0f, -2.3f, 1.50f, 10, OBJECT_ENEMY, ACTION_VETERAN, SHAPE_FOKKER, 0, 1.0f, 15.0f);
        new Object(670.0f, 100.0f, -1.0f, 0.00f, 500, OBJECT_ENEMY, ACTION_SWIRL, SHAPE_AIRSHIP, 0, 1.0f, 15.0f);
        new Object(670.0f, 200.0f, -2.3f, 0.00f, 10, OBJECT_ENEMY, ACTION_VETERAN, SHAPE_FOKKER, 0, 1.0f, 15.0f);
        new Object(670.0f, 320.0f, -2.0f, -1.50f, 10, OBJECT_ENEMY, ACTION_VETERAN, SHAPE_FOKKER, 0, 1.0f, 15.0f);
    }

    // 보스 격파 후 처리 (flag[15]가 켜지고 10초가 지난 후, 적이 없으면 클리어)
    if (flag[15] && (timeTotal >= flag15ActivationTime + 10.0f) && GameController::getEnemycount() == 0 && !StageClear && !Gameover) {
        OutputDebugString(L"Level1: Final boss wave defeated (after 10s delay). Stage Clear sequence initiated.\n");
        GameController::Stage = L"S T A G E  C L E A R !";
        StageClear = true; // 스테이지 클리어 상태로 변경
        GameoverTimer = timeTotal + 5.0f; // 타이틀로 돌아가기까지의 시간 (예: 5초)

        if (playerCharacter) {
            OutputDebugString(L"Level1: Setting player to non-blinking event object.\n");
            playerCharacter->setSpeed(0.0f, 0.0f); // 움직임 정지
            playerCharacter->reassignObj(OBJECT_EVENT, ACTION_STILL); // 이벤트 캐릭터로 전환, 행동은 STILL
            playerCharacter->setInvincible(false); // 혹시 모를 무적 상태 해제 (setInvincible은 blips도 제어하므로)
            playerCharacter->changeOpaque(1.0f); // 투명도 원상복구
            // PlayerRender는 OBJECT_PLAYABLE_CHARACTER일 때만 호출되므로, OBJECT_EVENT로 바뀌면 AllRender가 호출되어 깜빡이지 않음.
        }
    }

    // 스테이지 클리어 상태이고, 설정된 시간이 지나면 타이틀 화면으로 돌아감
    if (StageClear && timeTotal >= GameoverTimer) {
        OutputDebugString(L"Level1: Stage Clear time up. Returning to Title.\n");

        // 플레이어 객체 먼저 처리 (GameController로 전달하거나 삭제)
        if (playerCharacter) {
            // playerCharacter->setInvincible(false); // 이미 위에서 처리됨
            // 타이틀에서는 새로운 플레이어 객체를 사용하므로, 현재 플레이어 객체는 삭제.
            OutputDebugString(L"Level1: Deleting playerCharacter before returning to Title (Stage Clear).\n");
            delete playerCharacter;
            playerCharacter = nullptr;
        }
        
        // GameController가 레벨 전환 시 나머지 객체를 정리하도록 GameController::changeCurrentLevel 호출
        GameController::changeCurrentLevel(static_cast<int>(LEVEL_TITLE));
        return; 
    }

    // 게임오버 처리 (스테이지 클리어와는 별개)
    if (!StageClear && Gameover && timeTotal >= GameoverTimer) {
        OutputDebugString(L"Level1: Game Over time up. Returning to Title.\n");
        if (playerCharacter) { // playerCharacter가 아직 남아있다면 (예: 추락 중인 EFFECT 객체)
            OutputDebugString(L"Level1: Deleting playerCharacter before returning to Title (Game Over).\n");
            delete playerCharacter; // 이 시점에서 리스트에서 제거됨
            playerCharacter = nullptr;
        }
        // GameController::SetPendingPlayerObject(nullptr); // 이미 playerCharacter가 정리되었으므로 불필요
        
        // 나머지 모든 게임 내 Object들은 GameController::SwitchLevel에서 이전 레벨(Level1)이
        // delete될 때, Level1의 소멸자에서 정리되거나, 또는 GameController::CleanupGameResources에서
        // 일괄적으로 정리될 수 있습니다.
        // 여기서는 Level1이 직접 생성하고 관리하던 playerCharacter만 확실히 정리합니다.
        // 다른 객체들은 Object의 정적 리스트에 연결되어 있으며,
        // GameController::Update()의 루프에서 Isbeingdeleted 플래그를 보고 삭제하거나,
        // GameController::CleanupGameResources()에서 일괄 삭제됩니다.

        GameController::changeCurrentLevel(static_cast<int>(LEVEL_TITLE));
        return;
    }
}

Object* Level1::getPlayerCharacterObject() {
    return playerCharacter;
}