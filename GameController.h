#pragma once
#include "GameLevel.h"
#include "Timer.h"
#include <atlstr.h>
#include "Title.h"
#include "Level1.h"
#include "Level2.h"

enum eLevel {
	LEVEL_TITLE,
	LEVEL_1,
	LEVEL_2,
	LEVEL_3,
	LEVEL_ENDING = 8,
	LEVEL_STAFF = 16,
	LEVEL_SETTING = 32
};

class Object; // Object 클래스 전방 선언

class GameController {
private:
	static class GameLevel * currentLevel;
	static class Timer* hpTimer;
	static int nPlayerLife;
	static int nPlayerPoint;
	static float fPlayerShottime;
	static bool bPlayerActive;
	static eLevel EGameLevel;
	static int nEnemyCount;
	static Object* pendingPlayerObject; // 다음 레벨로 전달될 플레이어 객체
public:
	GameController() {
		IsGameLoading = true;
		currentLevel = nullptr;
		EGameLevel = LEVEL_TITLE;
		nEnemyCount = 0;
	}
	static bool IsGameLoading;
	static float GetTimeTotal();
	static Timer* GetTimerClass();
	static long long GetTickDelta();
	static CString Status, LifeandPoint, Stage;

	static int getPlayerLife();
	static void setPlayerLife(int set);
	static void cutPlayerLife();
	static void addPlayerLife();

	static int getPlayerPoint();
	static void setPlayerPoint(int set);
	static void addPlayerPoint(int point);
	static int getEnemycount();

	static void setPlayerStarted();
	static void setPlayerShot();
	static bool getPlayerAlive();
	static float getPlayerShottime();
	static bool IsThisGameOver();

	static void setNumberofCurrentLevel(int level);
	static void setCurrentLevel(eLevel level);
	static void changeCurrentLevel(int levelNum); // int levelNum 대신 eLevel을 받는 것이 더 타입 안전합니다.
	static eLevel getCurrentLevel();
	static int getNumberofCurrentLevel();
	static class GameLevel* GetCurrentLevelObject();
	static void LoadInitialLevel(class GameLevel* lev);
	static void SwitchLevel(class GameLevel* lev);
	static void Render();
	static void Update();
	static void Init();

	static void SetPendingPlayerObject(Object* player);
	static Object* GetPendingPlayerObjectAndClear(); // 객체를 가져오고 컨트롤러의 포인터는 비움

	static void KeyUp();
	static void KeyDown();
	static void KeyLeft();
	static void KeyRight();
	static void KeyReturn();
	static void KeyShoot();

	static void UnKeyUp();
	static void UnKeyDown();
	static void UnKeyLeft();
	static void UnKeyRight();
	static void UnKeyReturn();
	static void UnKeyShoot();

	static HWND mainhWnd; // wWinMain에서 초기화
	static void RequestExit(); // 게임 종료 요청
	static void CleanupGameResources(); // 게임 리소스 정리 (Graphics 제외)
};
