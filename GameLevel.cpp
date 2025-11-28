#include "GameLevel.h"
#include "GameController.h"

Graphics* GameLevel::gfx;
bool GameLevel::Button_Up = false;
bool GameLevel::Button_Down = false;
bool GameLevel::Button_Left = false;
bool GameLevel::Button_Right = false;
bool GameLevel::Button_Return = false;
bool GameLevel::Button_Shoot = false;

void GameLevel::Init(Graphics* graphics) {
	gfx = graphics;
	return;
}
bool GameLevel::isButton_Up() {
	if (GameLevel::Button_Up) {
		if (!GameLevel::Button_Down) return true;
		else  return false;
	}
	else return false;
}
bool GameLevel::isButton_Down(){
	if (GameLevel::Button_Down) {
		if (!GameLevel::Button_Up) return true;
		else  return false;
	}
	else return false;
}
bool GameLevel::isButton_Left() {
	if (GameLevel::Button_Left) {
		if (!GameLevel::Button_Right) return true;
		else  return false;
	}
	else return false;
}
bool GameLevel::isButton_Right() {
	if (GameLevel::Button_Right) {
		if (!GameLevel::Button_Left) return true;
		else  return false;
	}
	else return false;
}
bool GameLevel::isButton_Return() {
	return GameLevel::Button_Return;
}
bool GameLevel::isButton_Shoot() {
	return GameLevel::Button_Shoot;
}

void GameLevel::GLKeyUp() {
	GameLevel::Button_Up = true;
}

void GameLevel::GLKeyDown() {
	GameLevel::Button_Down = true;
}

void GameLevel::GLKeyLeft() {
	GameLevel::Button_Left = true;
}

void GameLevel::GLKeyRight() {
	GameLevel::Button_Right = true;
}

void GameLevel::GLKeyReturn() {
	GameLevel::Button_Return = true;
}

void GameLevel::GLKeyShoot() {
	GameLevel::Button_Shoot = true;
}

void GameLevel::GLUnKeyUp() {
	GameLevel::Button_Up = false;
}
void GameLevel::GLUnKeyDown() {
	GameLevel::Button_Down = false;
}
void GameLevel::GLUnKeyLeft() {
	GameLevel::Button_Left = false;
}
void GameLevel::GLUnKeyRight() {
	GameLevel::Button_Right = false;
}
void GameLevel::GLUnKeyReturn() {
	GameLevel::Button_Return = false;
}
void GameLevel::GLUnKeyShoot() {
	GameLevel::Button_Shoot = false;
}

void GameLevel::clearButtons() {
	Button_Up = false;
	Button_Down = false;
	Button_Left = false;
	Button_Right = false;
	Button_Return = false;
	Button_Shoot = false;
}