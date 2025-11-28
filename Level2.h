#pragma once
#include "GameLevel.h"

class Object; // 전방 선언

class Level2 :
    public GameLevel
{
protected:
    int sequence;
    float player_x_speed, player_y_speed;
    Object* playerCharacter = nullptr; // Level2의 플레이어 캐릭터 포인터 (필요시)

    void initializeLogicState(); 

public:
    Level2();
    ~Level2(); // playerCharacter 해제 로직 필요시 추가
    void Load(bool reinitializeLogic = true) override;
    void Unload() override;
    void CLS() override;
    void GLRender() override;
    void GLUpdate(float timeTotal, float timeDelta) override;

    // <<< 추가: playerCharacter 포인터 반환 (Level2에 playerCharacter가 있다면) >>>
    Object* getPlayerCharacterObject() override;
};

