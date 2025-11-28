#pragma once
#include "GameLevel.h"
//This is Menu Level which Performs Start (1)New Game, (2)Settings, (3)Staff, (4)Exit
//basic loading of resources and initialization will be performed on this level.

class Title :
    public GameLevel
{
private:
    int nMenuSelected = 0;
    // nLogoOpaque 제거 또는 주석 처리 (fLogoOpaque로 대체)
    int nMenuInput = -1;

    float logotime = 0.0f;          // 로고 애니메이션 관련 기준 시간 (로직 초기화 시 설정)
    float fLogoOpaque = 0.0f;       // 로고의 현재 투명도 (0.0 ~ 1.0)

    // 로고 애니메이션 제어 변수 추가
    float logoAppearDelay = 2.0f;   // 로고가 나타나기 시작하는 지연 시간 (초)
    float logoFadeInDuration = 3.0f; // 로고가 완전히 나타나는 데 걸리는 시간 (초)

    bool Settings = false;          // 유지
    class Object* pPC; // 타이틀 화면의 플레이어 캐릭터 표현용 Object

    void initializeLogicState(); // 로직 상태 초기화 함수

    static bool backgroundObjectsCreated; // 정적 멤버로 선언

public:
    Title();
    ~Title(); // pPC 해제를 위해 소멸자 선언
    void Load(bool reinitializeLogic = true) override;
    void Unload() override;
    void CLS() override;
    void GLUpdate(float timeTotal, float timeDelta) override;
    void GLRender() override;
    void GLKeyUp() override;
    void GLKeyDown() override;
    void GLKeyReturn() override;
    void GLKeyShoot() override;
    void GLUnKeyUp() override;
    void GLUnKeyDown() override;
    void GLUnKeyReturn() override;
    void GLUnKeyShoot() override;
    Object* getPlayerCharacterObject() override; // pPC를 반환하도록 Title.cpp에 구현
};

