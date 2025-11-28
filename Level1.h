#pragma once
#include "GameLevel.h"

class Object;

class Level1 :
    public GameLevel
{
private:
    float effect[5], stagestart, ReentryTimer, GameoverTimer; // NewPCShowtime은 NewPC 객체 생성 시점으로 대체 가능
    bool flag[20] = { false }, Gameover = false, StageClear = false; // StageClear 추가
    float flag15ActivationTime = 0.0f; // flag[15]가 활성화된 시간 기록

    // Level1에서 플레이어 캐릭터를 직접 가리키는 포인터. GameController에서 받아옴.
    class Object* playerCharacter = nullptr;

    void init_game_logic_state(); // 게임 로직 상태 초기화 함수 (기존 init_stage에서 이름 변경 또는 역할 분담)

public:
    Level1();
    ~Level1(); // playerCharacter 해제 고려 (만약 Level1이 소유권을 가진다면)
    // Load 함수에 매개변수 추가
    void Load(bool reinitializeLogic = true) override;
    void Unload() override;
    void CLS() override;
    void GLRender() override;
    void GLUpdate(float timeTotal, float timeDelta) override;

    Object* getPlayerCharacterObject() override;
};

