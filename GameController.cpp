#include "GameController.h"
#include "Object.h" // Object 클래스 정의 포함
#include "GameLevel.h" // GameLevel::Button_... 접근을 위해

// 정적 멤버 변수 초기화
GameLevel* GameController::currentLevel = nullptr;
Object* GameController::pendingPlayerObject = nullptr; // 추가
bool GameController::IsGameLoading = false;
Timer* GameController::hpTimer = nullptr;
int GameController::nPlayerLife = 0;
int GameController::nPlayerPoint = 0;
int GameController::nEnemyCount = 0;
float GameController::fPlayerShottime = 0.0;
bool GameController::bPlayerActive = false;
eLevel GameController::EGameLevel = LEVEL_TITLE;
HWND GameController::mainhWnd = nullptr; // 정적 멤버 초기화

CString GameController::Status;
CString GameController::LifeandPoint;
CString GameController::Stage;

long long GameController::GetTickDelta() {
	return hpTimer->GetTickDelta();
}

void GameController::setNumberofCurrentLevel(int level) {
	setCurrentLevel(static_cast <eLevel> (level));
	return;
}

void GameController::setCurrentLevel(eLevel level) {
	EGameLevel = level;
	return;
}

void GameController::SetPendingPlayerObject(Object* player) {
    // 이전 pendingPlayerObject가 있었다면? (일반적으로는 없을 것으로 예상)
    // if (pendingPlayerObject && pendingPlayerObject != player) { delete pendingPlayerObject; }
    pendingPlayerObject = player;
}

Object* GameController::GetPendingPlayerObjectAndClear() {
    Object* player = pendingPlayerObject;
    pendingPlayerObject = nullptr; // 전달 후 GameController는 포인터를 비움
    return player;
}

void GameController::changeCurrentLevel(int levelNum) {
	eLevel targetLevel = static_cast<eLevel>(levelNum);

	// 플레이어 객체 상태 변경은 Title에서 GameController로 객체를 넘기기 전에 수행
    // 또는 Level1::Load에서 GameController로부터 객체를 받은 후 수행

	switch (targetLevel)
	{
	case LEVEL_TITLE:
		SwitchLevel(new Title());
		setCurrentLevel(LEVEL_TITLE);
		break;
	case LEVEL_1:
		SwitchLevel(new Level1()); // Level1 생성, 내부 Load에서 pendingPlayerObject 사용
		setCurrentLevel(LEVEL_1);
		break;
	case LEVEL_2:
		SwitchLevel(new Level2());
		setCurrentLevel(LEVEL_2);
		break;
	default:
		break;
	}
}

eLevel GameController::getCurrentLevel() {
	return EGameLevel;
}

int GameController::getNumberofCurrentLevel() {
	return static_cast <int> (getCurrentLevel()) & 7;
}

GameLevel* GameController::GetCurrentLevelObject() {
	return currentLevel;
}

void GameController::LoadInitialLevel(GameLevel* lev) {
	if (hpTimer) hpTimer->Reset(); // 타이머 리셋
	currentLevel = lev; 
	IsGameLoading = true;
	if (currentLevel) {
		currentLevel->Load(true); // 로직과 리소스 모두 초기 로드
	}
	IsGameLoading = false;
}

void GameController::SwitchLevel(GameLevel* lev) {
    OutputDebugString(L"GameController::SwitchLevel called.\n");
    IsGameLoading = true;

    // 현재 레벨의 플레이어 객체가 있다면 GameController로 전달 (타이틀 화면 등에서 사용 가능하도록)
    if (currentLevel && currentLevel->getPlayerCharacterObject()) {
        Object* currentPlayerObject = currentLevel->getPlayerCharacterObject();
        // 플레이어가 OBJECT_EVENT로 전환된 상태(스테이지 클리어 후)가 아니라면 전달
        if (currentPlayerObject->getObjectType() == OBJECT_PLAYABLE_CHARACTER || currentPlayerObject->getObjectType() == OBJECT_EFFECT) {
            OutputDebugString(L"GameController::SwitchLevel - Setting pending player object.\n");
            SetPendingPlayerObject(currentPlayerObject); 
        }
    }

    if (currentLevel) {
        currentLevel->Unload();
        delete currentLevel;
        currentLevel = nullptr;
    }

    // 레벨 전환 시 배경을 제외한 모든 오브젝트 정리
    CleanupGameResources(); // 배경 제외하고 정리하도록 수정된 함수 호출

    // <<< 레벨 전환 직전 모든 버튼 상태 초기화 >>>
	GameLevel::clearButtons();
    OutputDebugString(L"GameController: All button states reset during SwitchLevel.\n");

    currentLevel = lev;
    if (currentLevel) {
        // Load()는 기본적으로 reinitializeLogic = true로 호출됨
        currentLevel->Load(); 
        IsGameLoading = false;
        OutputDebugString(L"GameController::SwitchLevel - New level loaded.\n");
    } else {
        OutputDebugString(L"GameController::SwitchLevel - New level is null.\n");
    }
}

void GameController::Render()
{
	if (IsGameLoading) return;
	else {
		
		currentLevel->CLS();
		Object* temp = (Object::getFarBackGround())->getNext();
		while (temp && temp->IsitActualObject()) {
			temp->Render();
			temp = temp->getNext();
		}
		temp = (Object::getBackGround())->getNext();
		while (temp && temp->IsitActualObject()) {
			temp->Render();
			temp = temp->getNext();
		}
		temp = (Object::getEvent())->getNext();
		while (temp && temp->IsitActualObject()) {
			temp->Render();
			temp = temp->getNext();
		}
		temp = (Object::getPlayerBullet())->getNext();
		while (temp && temp->IsitActualObject()) {
			temp->Render();
			temp = temp->getNext();
		}
		temp = (Object::getEnemyBullet())->getNext();
		while (temp && temp->IsitActualObject()) {
			temp->Render();
			temp = temp->getNext();
		}
		temp = (Object::getCharacter())->getNext();
		while (temp && temp->IsitActualObject()) {
			temp->Render();
			temp = temp->getNext();
		}
		temp = (Object::getPlayerCharacter())->getNext();
		while (temp && temp->IsitActualObject()) {
			temp->Render();
			temp = temp->getNext();
		}
		temp = (Object::getForeGround())->getNext();
		while (temp && temp->IsitActualObject()) {
			temp->Render();
			temp = temp->getNext();
		}
		currentLevel->GLRender();
		return;
	}
}

void GameController::Update() {
    if (IsGameLoading) return;

    hpTimer->Update();
    float tt = hpTimer->GetTimeTotal(), td = hpTimer->GetTimeDelta();
    currentLevel->GLUpdate(tt, td);

    nEnemyCount = 0;

    // 각 Object 리스트를 순회하며 업데이트 및 삭제
    auto updateAndDeleteList = [&](Object* headNode) {
        if (!headNode) return;
        Object* current = headNode->getNext();
        Object* next = nullptr;
        while (current && current->IsitActualObject()) { // IsitActualObject()로 실제 객체인지 확인
            next = current->getNext(); // 삭제 전에 다음 포인터 저장
            current->Update(tt, td);
            if (current->getObjectType() == OBJECT_ENEMY) {
                // 주의: OBJECT_EFFECT로 전환된 적도 카운트될 수 있으므로,
                //       실제로 살아있는 적만 카운트하려면 추가 조건 필요
                if (current->getHP() > 0) { // 예: 살아있는 적만 카운트
                    nEnemyCount++;
                }
            }
            if (current->askIsbeingdeleted()) {
                // Level1의 playerCharacter가 이 current와 동일하고, GameController가 소유권을 가져가지 않았다면 Level1에서 delete.
                // 그 외의 경우는 여기서 delete.
                // playerCharacter는 Level1::GLUpdate에서 직접 관리/삭제되므로 여기서 중복 delete 방지 필요.
                bool isPlayerManagedByLevel = false;
                if (currentLevel && currentLevel->getPlayerCharacterObject() == current) {
                    isPlayerManagedByLevel = true;
                }

                if (!isPlayerManagedByLevel) {
                    OutputDebugString(L"GameController::Update - Deleting object.\n");
                    delete current;
                } else {
                    OutputDebugString(L"GameController::Update - Object deletion skipped (managed by Level).\n");
                }
            }
            current = next;
        }
    };

    updateAndDeleteList(Object::getFarBackGround());
    updateAndDeleteList(Object::getBackGround());
    updateAndDeleteList(Object::getCharacter());
    updateAndDeleteList(Object::getEvent()); // EFFECT 객체도 여기서 업데이트 및 삭제
    updateAndDeleteList(Object::getPlayerBullet());
    updateAndDeleteList(Object::getEnemyBullet());
    updateAndDeleteList(Object::getForeGround());
    // PlayerCharacter 리스트는 Level1::playerCharacter가 직접 관리하므로,
    // 여기서의 updateAndDeleteList(Object::getPlayerCharacter()); 호출은 주의해야 합니다.
    // Level1::GLUpdate에서 playerCharacter를 직접 new/delete/reassign 하므로,
    // GameController::Update에서 중복 업데이트/삭제를 피해야 합니다.
    // 만약 Level1::playerCharacter가 Object::PlayerCharacter 리스트에 항상 존재한다면,
    // 아래처럼 처리할 수 있습니다.
    if (Object::getPlayerCharacter() && Object::getPlayerCharacter()->getNext() && Object::getPlayerCharacter()->getNext()->IsitActualObject()){
        // Level1의 playerCharacter 포인터가 가리키는 객체가 이 리스트의 첫번째 실제 객체와 동일한지 확인 필요
        // 또는 Level1::playerCharacter를 직접 업데이트
        Object* currentPlayerInList = Object::getPlayerCharacter()->getNext(); // 실제 플레이어 객체
        
        // currentLevel->getPlayerCharacterObject()는 Level1에서 관리하는 playerCharacter 포인터.
        // 이 포인터가 가리키는 객체가 실제로 PlayerCharacter 리스트에 있는 첫번째 객체와 동일할 때만 업데이트.
        // 또한, Level1::GLUpdate에서 이미 playerCharacter->Update()를 호출했을 수 있으므로 중복 호출 방지 필요.
        // 현재 Level1::GLUpdate에서는 playerCharacter->Update()를 명시적으로 호출하지 않음.
        // 따라서 여기서 업데이트하는 것이 맞을 수 있으나, Level1의 playerCharacter 포인터와 리스트의 객체가 일치하는지 확인.
        if (currentLevel && currentPlayerInList == currentLevel->getPlayerCharacterObject()) {
             currentPlayerInList->Update(tt,td); // Level1이 관리하는 플레이어 객체 업데이트
             if(currentPlayerInList->askIsbeingdeleted()){
                // Level1에서 playerCharacter = nullptr 처리를 해주어야 함 (Level1::GLUpdate에서 이미 처리).
                OutputDebugString(L"GameController::Update - Player object is being deleted (handled by Level logic).\n");
                // delete currentPlayerInList; // Level1에서 관리하므로 여기서 delete하지 않음
             }
        } else if (currentPlayerInList != nullptr && (currentLevel == nullptr || currentPlayerInList != currentLevel->getPlayerCharacterObject())) {
            // PlayerCharacter 리스트에 있지만 Level에서 관리하지 않는 다른 플레이어블 캐릭터 객체 (이런 경우가 있는지 확인 필요)
            // 일반적으로는 없을 것으로 예상됨. 만약 있다면 여기서 업데이트 및 삭제 처리.
            currentPlayerInList->Update(tt, td);
            if (currentPlayerInList->askIsbeingdeleted()) {
                OutputDebugString(L"GameController::Update - Deleting unmanaged PlayerCharacter object from list.\n");
                delete currentPlayerInList;
            }
        }
    }


    wchar_t String[101];
    IsThisGameOver() ? swprintf_s(String, 100, L"       POINTS : %9d", getPlayerPoint()) : swprintf_s(String, 100, L" LIFE: %d  POINTS: %9d", getPlayerLife(), getPlayerPoint());
    GameController::LifeandPoint = String;
}

void GameController::Init()
{
	IsGameLoading = true;
	currentLevel = nullptr;
	hpTimer = new Timer();
	// mainhWnd = hWnd; // Shooting3.cpp의 hWnd를 GameController가 알 수 있도록 설정
}

void GameController::CleanupGameResources() {
  OutputDebugString(L"GameController::CleanupGameResources called for selective object cleanup.\n");

  // pendingPlayerObject는 레벨 전환 시 새 레벨로 전달되거나, 게임 종료 시 wWinMain에서 Object 헤드 노드들과 함께 정리될 수 있습니다.
  // SwitchLevel 내에서 CleanupGameResources가 호출될 때 pendingPlayerObject를 여기서 삭제하면
  // 새 레벨이 이를 사용할 수 없게 됩니다. 따라서 pendingPlayerObject는 여기서 직접 삭제하지 않거나,
  // 게임 전체 종료 시에만 삭제하도록 wWinMain의 최종 정리 로직에 맡기는 것이 좋습니다.
  // 여기서는 SwitchLevel 중의 정리이므로 pendingPlayerObject를 건드리지 않습니다.
  // if (pendingPlayerObject) {
  //     OutputDebugString(L"GameController: Cleaning up pendingPlayerObject.\n");
  //     delete pendingPlayerObject;
  //     pendingPlayerObject = nullptr;
  // }

  Object* pHead = nullptr;
  Object* pCurrent = nullptr;
  Object* pNext = nullptr;

  // 특정 리스트의 객체를 삭제하는 람다 함수
  auto deleteObjectsFromList = [&](Object* headNode, const wchar_t* listName) {
    if (!headNode) return;
    pCurrent = headNode->getNext();
    while (pCurrent && pCurrent->IsitActualObject()) {
      pNext = pCurrent->getNext();
      // Object 소멸자에서 리스트 연결을 해제합니다.
      wchar_t logMsg[256];
      swprintf_s(logMsg, L"GameController: Deleting a %s object (Type: %d).", listName, static_cast<int>(pCurrent->getObjectType()));
      OutputDebugString(logMsg);
      delete pCurrent;
      pCurrent = pNext;
    }
    };

  // 1. 배경 오브젝트 (FarBackGround, BackGround)는 유지합니다.
  OutputDebugString(L"GameController: FarBackGround and BackGround objects are preserved.\n");

  // 2. 캐릭터(적, 중립) 오브젝트를 정리합니다.
  OutputDebugString(L"GameController: Cleaning up Character objects.\n");
  deleteObjectsFromList(Object::getCharacter(), L"Character");

  // 3. 플레이어 캐릭터 오브젝트를 정리합니다. (Level에서 관리하지 않는 경우)
  OutputDebugString(L"GameController: Cleaning up unmanaged PlayerCharacter objects.\n");
  pHead = Object::getPlayerCharacter();
  if (pHead) {
    pCurrent = pHead->getNext();
    while (pCurrent && pCurrent->IsitActualObject()) {
      pNext = pCurrent->getNext();
      bool isManagedByLevel = (currentLevel && currentLevel->getPlayerCharacterObject() == pCurrent);
      bool isPendingObject = (pendingPlayerObject == pCurrent);

      if (!isManagedByLevel && !isPendingObject) {
        OutputDebugString(L"GameController: Deleting an unmanaged/non-pending PlayerCharacter object from list.\n");
        delete pCurrent;
      }
      else {
        OutputDebugString(L"GameController: Preserving a PlayerCharacter object (managed by level or pending).\n");
      }
      pCurrent = pNext;
    }
  }

  // 4. 이벤트 오브젝트 (Event 및 Effect 리스트)는 유지합니다.
  OutputDebugString(L"GameController: Event/Effect objects (in Event list) are preserved.\n");

  // 5. 적 총알 오브젝트를 정리합니다.
  OutputDebugString(L"GameController: Cleaning up EnemyBullet objects.\n");
  deleteObjectsFromList(Object::getEnemyBullet(), L"EnemyBullet");

  // 6. 플레이어 총알 오브젝트를 정리합니다.
  OutputDebugString(L"GameController: Cleaning up PlayerBullet objects.\n");
  deleteObjectsFromList(Object::getPlayerBullet(), L"PlayerBullet");

  // 7. 전경 오브젝트(ForeGround)는 유지합니다.
  OutputDebugString(L"GameController: ForeGround objects are preserved.\n");

  // Timer 객체는 게임 전체 종료 시 wWinMain에서 정리하거나,
  // GameController::Init()에서 생성되고 게임 세션 동안 유지되므로
  // 레벨 전환 시의 CleanupGameResources에서는 삭제하지 않는 것이 일반적입니다.
  // 만약 hpTimer를 여기서 삭제하면 SwitchLevel 후 새 레벨에서 타이머가 없어 문제가 발생합니다.
  // 따라서 hpTimer 삭제 로직은 게임 완전 종료 시점으로 옮기는 것을 권장합니다.
  // (현재 코드에서는 wWinMain의 GameController::CleanupGameResources 호출 시 삭제됨)
  // 이 함수가 SwitchLevel에서 호출될 때는 hpTimer를 삭제하지 않도록 주의해야 합니다.
  // 지금은 요청에 따라 오브젝트 리스트만 수정합니다. hpTimer 관련 로직은 그대로 둡니다.
  if (hpTimer && EGameLevel == LEVEL_TITLE && !currentLevel) { // 예시: 게임 완전 종료 시점에만 타이머 삭제 (더 견고한 조건 필요)
    // OutputDebugString(L"GameController: Timer cleanup (conditionally).\n");
    // delete hpTimer;
    // hpTimer = nullptr;
  }
  OutputDebugString(L"GameController::CleanupGameResources (selective object cleanup) finished.\n");
}

void GameController::RequestExit() {
    OutputDebugString(L"GameController::RequestExit called.\n");
    // 여기에 게임 상태 저장 등의 로직 추가 가능
    if (mainhWnd) {
        OutputDebugString(L"Destroying main window to initiate clean shutdown.\n");
        DestroyWindow(mainhWnd); // WM_DESTROY 메시지를 유도하여 정상 종료 절차를 따르도록 함
    } else {
        OutputDebugString(L"mainhWnd is null. Calling PostQuitMessage directly.\n");
        CleanupGameResources(); // Graphics 제외한 리소스 정리 시도
        PostQuitMessage(0); // 최후의 수단
    }
}

float GameController::GetTimeTotal() {
	return hpTimer->Timer::GetTimeTotal();
}

void GameController::KeyUp() {
	if (!IsGameLoading && currentLevel) currentLevel->GLKeyUp();
}
void GameController::KeyDown() {
	if (!IsGameLoading && currentLevel) currentLevel->GLKeyDown();
}
void GameController::KeyLeft() {
	if (!IsGameLoading && currentLevel) currentLevel->GLKeyLeft();
}
void GameController::KeyRight() {
	if (!IsGameLoading && currentLevel) currentLevel->GLKeyRight();
}
void GameController::KeyReturn() {
	if (!IsGameLoading && currentLevel) currentLevel->GLKeyReturn();
}
void GameController::KeyShoot() {
	if (!IsGameLoading && currentLevel) currentLevel->GLKeyShoot();
}

void GameController::UnKeyUp() {
	if (!IsGameLoading && currentLevel) currentLevel->GLUnKeyUp();
}
void GameController::UnKeyDown() {
	if (!IsGameLoading && currentLevel) currentLevel->GLUnKeyDown();
}
void GameController::UnKeyLeft() {
	if (!IsGameLoading && currentLevel) currentLevel->GLUnKeyLeft();
}
void GameController::UnKeyRight() {
	if (!IsGameLoading && currentLevel) currentLevel->GLUnKeyRight();
}
void GameController::UnKeyReturn() {
	if (!IsGameLoading && currentLevel) currentLevel->GLUnKeyReturn();
}
void GameController::UnKeyShoot() {
	if (!IsGameLoading && currentLevel) currentLevel->GLUnKeyShoot();
}

Timer* GameController::GetTimerClass() {
	return hpTimer;
}


int GameController::getPlayerLife() {
	return nPlayerLife;
}
void GameController::setPlayerLife(int set) {
	nPlayerLife = set;
	return;
}
void GameController::cutPlayerLife() {
	nPlayerLife--;
	return;
}

void GameController::addPlayerLife() {
	nPlayerLife++;
	return;
}

int GameController::getPlayerPoint() {
	return nPlayerPoint;
}
void GameController::setPlayerPoint(int set) {
	nPlayerPoint = set;
	return;
}
void GameController::addPlayerPoint(int point) {
	nPlayerPoint += point;
	return;
}
int GameController::getEnemycount() {
	return nEnemyCount;
}

void GameController::setPlayerStarted() {
	bPlayerActive = true;
}

void GameController::setPlayerShot() {
	fPlayerShottime = GetTimeTotal();
	cutPlayerLife();
	bPlayerActive = false;
}

bool GameController::getPlayerAlive() {
	return bPlayerActive;
}

float GameController::getPlayerShottime() {
	return fPlayerShottime;
}

bool GameController::IsThisGameOver() {
	if (nPlayerLife < 0) return true;
	else return false;
}