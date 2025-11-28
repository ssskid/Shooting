#pragma once
#include "Graphics.h"

class Object; // Object 클래스 전방 선언 (getPlayerCharacterObject 반환 타입 때문)

class GameLevel
{
protected:
	static Graphics* gfx;
	static bool Button_Up;
	static bool Button_Down;
	static bool Button_Left;
	static bool Button_Right;
	static bool Button_Return;
	static bool Button_Shoot;

public:
	static bool isButton_Up();
	static bool isButton_Down();
	static bool isButton_Left();
	static bool isButton_Right();
	static bool isButton_Return();
	static bool isButton_Shoot();

	static void Init(Graphics* graphics);
	virtual void Load(bool reinitializeLogic = true) = 0; // 매개변수 추가
	virtual void Unload() = 0;
	virtual void CLS() = 0;
	virtual void GLRender() = 0;
	virtual void GLUpdate(float timeTotal, float timeDelta) = 0;
	
	virtual void GLKeyUp();
	virtual void GLKeyDown();
	virtual void GLKeyLeft();
	virtual void GLKeyRight();
	virtual void GLKeyReturn();
	virtual void GLKeyShoot();

	virtual void GLUnKeyUp();
	virtual void GLUnKeyDown();
	virtual void GLUnKeyLeft();
	virtual void GLUnKeyRight();
	virtual void GLUnKeyReturn();
	virtual void GLUnKeyShoot();

	static void clearButtons();

	// <<< 추가: 현재 레벨의 플레이어 캐릭터 객체를 반환하는 가상 함수 >>>
	virtual Object* getPlayerCharacterObject() { return nullptr; } 
};

