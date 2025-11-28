#pragma once
#include <cmath>
#include <random>
#include <cstdio>
#include "Graphics.h"
#include "GameController.h"

#define BALLSPEED 4.0f
#define AIRSPEED 3.0f
#define BALLOONSPEED 0.8f
#define PI 3.1415926535f
#define CYCLETIME 0.8f
#define BGSPEED 0.4f
#define FBGSPEED 0.2f
#define FGSPEED 7.0f
#define EFTCYCLE 0.125f
#define WEATHERCYCLE 0.375f

#define PSPEEDF 2.0f
#define PSPEEDH 1.414213f
#define PBULLETSPEED 6.0f
#define SHOOTDELAYTIME 0.14f
#define BLIPTIME 2.0f       // 전체 무적 시간 (초 단위)
#define BLIPBLINKCYCLE 0.1f // 플레이어 깜빡임 주기 (초 단위, 0.1초마다 보였다 안보였다)

#define EFFECTTIME 0.125f
#define EFFECTCYCLE 1.0f

//box for collision check

#define COLLISION_CAMEL_XH 21.0f
#define COLLISION_CAMEL_XL -12.0f
#define COLLISION_CAMEL_YH 3.0f
#define COLLISION_CAMEL_YL -4.0f

#define COLLISION_BB_XH 3.0f
#define COLLISION_BB_XL -3.0f
#define COLLISION_BB_YH 3.0f
#define COLLISION_BB_YL -3.0f

#define COLLISION_PB_XH 2.5f
#define COLLISION_PB_XL -2.5f
#define COLLISION_PB_YH 3.0f
#define COLLISION_PB_YL -3.0f

#define COLLISION_FOKKER_XH 20.0f
#define COLLISION_FOKKER_XL -24.0f
#define COLLISION_FOKKER_YH 5.0f
#define COLLISION_FOKKER_YL -3.0f

#define COLLISION_AIRSHIP_XH 62.5f
#define COLLISION_AIRSHIP_XL -62.5f
#define COLLISION_AIRSHIP_YH 12.5f
#define COLLISION_AIRSHIP_YL -12.5f

class GameController;

enum OBJECT_TYPE
{
	OBJECT_PLAYABLE_CHARACTER,	// Playable character (can be destroyed)
	OBJECT_ENEMY,				// enemy class(destroyable, adds point)
	OBJECT_NEUTRAL,				// neutral(destroyable, doesn't gives point)
	OBJECT_EVENT,				// Use on event(menu, interlude, end etc...)
	OBJECT_EFFECT,				// Impact, explosion, destroyed debris etc
	OBJECT_PLAYER_BULLET,		// Player bullet
	OBJECT_ENEMY_BULLET,		// Enemy bullet
	OBJECT_FARBACKGROUND,		// Far Background
	OBJECT_BACKGROUND,			// Background
	OBJECT_FOREGROUND,			// Foreground
	OBJECT_NULL					// NULL
};

enum ACTION_TYPE  // defines Character AI
{
	ACTION_NULL,				// NULL
	ACTION_STILL,				// No action
	ACTION_IDLE,				// IDLE(occasionally switches position a little)
	ACTION_FIGHTER,				// shoots
	ACTION_TRAINED,				// shoot towards player
	ACTION_VETERAN,				// follows and shoots
	ACTION_KAMIKAZE,			// rushes to target
	ACTION_SHOOTNBANZAI,		// shoots and rushes
	ACTION_MG,					// shoots heavy machine gun(8)
	ACTION_BALLOON8,			// shoots 8 Direction
	ACTION_SWIRL,				// shoots SWIRL gun(8 Direction swirl)
	ACTION_BOSS,				// boss(SWIRL, HMG)
	ACTION_BOMBER,				// Drops Bomb
	ACTION_PLAYER,				// Moves
	ACTION_BACKGROUND,			// No action Background
	ACTION_WEATHER,				// switches shapes Background
	ACTION_INTRO,				// switches Moves(for intro)
	ACTION_EFFECT,				// effect(switch shape and goes away)
	ACTION_FALLING,				// smoking and falling
	ACTION_GOINGDOWN			// Smoke and fall
};
enum SHAPE_TYPE  // defines Object Shapes
{
	SHAPE_CAMEL,				// Soapwith Camel
	SHAPE_FOKKER,				// fokker dr.1
	SHAPE_AIRSHIP,				// airship
	SHAPE_BULLET,				// player bullet
	SHAPE_BALL,					// ball - enemy
	SHAPE_BOMB,					// bomb - enemy
	SHAPE_EXPLOSION,			// Explosion
	SHAPE_IMPACT,				// Bullet Impact
	SHAPE_SMOKE,				// Smoke
	SHAPE_CLOUD,				// Cloud
	SHAPE_SKY1,					// SKY1(clear)
	SHAPE_SKY2,					// SKY2(cloudy or smoke)
	SHAPE_SKY3,					// SKY3(stormy)
	SHAPE_SKY4,					// SKY4(burning)
	SHAPE_NULL					// NULL
};

enum LAST_ACTION // LAST_ACTION taken
{
	LA_NONE,					// NO ACTION
	LA_SHOOTBALL,				// SHOOTBALL
	LA_8_BALL,					// SHOOTBALL to 8 direction
	LA_MG,						// SHOOT RAPID FIRE
	LA_BOMBSAWAY,				// DROPPED A BOMB
	LA_CHANGEDIRECTION,			// KAMIKAZE DIRECTION CHANGE
	LA_SMOKE,					// SMOKE
	LA_AVOID,					// Evading
	LA_BOSSMOVE1,				// BOSS movement 1
	LA_BOSSMOVE2,				// BOSS movement 2
	LA_BOSSMOVE3,				// BOSS movement 3
	LA_GOINGDOWN_INITIATED		// 추가
};

class Object
{
protected:
	static Graphics* gfx;
	Object* previous,* next;
	float Position_X, Position_Y, DrawOpaque, unitSize = 15.0f;
	int nHealth, nMaxHealth, nShapeNum, nPrevShapeNum = 0;
	OBJECT_TYPE E_OBJECT;
	ACTION_TYPE E_AI;
	SHAPE_TYPE E_SHAPE;
	LAST_ACTION E_ACTION = LA_NONE;
	bool IsDestroyed = false, IsAnimationFinished = false, Isbeingdeleted = false, IsCoolDown = false;
	int nShootCounter = 0; 
	float lastshoot = 0, now = 0, creationtime = 0, Speed_X, Speed_Y;
	// linear pointer
	static Object* PlayerCharacter;
	static Object* Character;
	static Object* Event;
	static Object* FarBackGround;
	static Object* BackGround;
	static Object* ForeGround;
	static Object* PlayerBullet;
	static Object* EnemyBullet;
	//idle value
	static const float idle_cyle[3];
	static const float idle_amp[3];
	float accelerationY = 0.0f, fSwirlDegree = 0.0f;
	float prev_idlepos_x = 0.0f, prev_idlepos_y = 0.0f, idlepos_x = 0.0f, idlepos_y = 0.0f, movement = 0.0f;
	//Player character
	bool blips = false;
	float bliptime = 0.0f;
	bool isInvincible = false; // 영구적인 무적 상태 플래그 추가
public:
	Object()
		:previous(nullptr), next(nullptr), nHealth(0), Position_X(0.0f), Position_Y(0.0f), Speed_X(0.0f), Speed_Y(0.0f), E_OBJECT(OBJECT_NULL), E_AI(ACTION_NULL), E_SHAPE(SHAPE_NULL), nShapeNum(0), IsDestroyed(false), creationtime(0), movement(0.0), Isbeingdeleted(false), isInvincible(false) {
		//for Header object
		fSwirlDegree = DrawOpaque = 0.0f, unitSize = 15.0f;
		E_ACTION = LA_NONE;
		IsDestroyed = IsAnimationFinished = IsCoolDown = false;
		now = lastshoot = 0.0;
		nPrevShapeNum = nShootCounter = nMaxHealth = 0;
		prev_idlepos_x = prev_idlepos_y = idlepos_x = idlepos_y = bliptime = 0.0f;
	}
	Object(float x, float y, float s_x, float s_y, int hp, OBJECT_TYPE type, ACTION_TYPE ai, SHAPE_TYPE shape, int shapenum = 0, float fOpaque = 1.0f, float size = 15.0f)
		: Isbeingdeleted(false), isInvincible(false) {
		Position_X = x, Position_Y = y, Speed_X = s_x, Speed_Y = s_y, nMaxHealth = nHealth = hp;
		E_OBJECT = type;
		E_AI = ai;
		E_SHAPE = shape;
		nPrevShapeNum = nShapeNum = shapenum; // from 0 to 7
		DrawOpaque = fOpaque;
		//Base
		E_ACTION = LA_NONE;
		IsDestroyed = IsAnimationFinished = false;
		fSwirlDegree = prev_idlepos_x = prev_idlepos_y = idlepos_x = idlepos_y = movement = lastshoot = now = bliptime = 0.0f;
		nShootCounter = 0;
		unitSize = size;
		creationtime = GameController::GetTimeTotal();
		if (E_OBJECT == OBJECT_PLAYABLE_CHARACTER) previous = PlayerCharacter;
		else if (E_OBJECT == OBJECT_ENEMY || E_OBJECT == OBJECT_NEUTRAL) previous = Character;
		else if (E_OBJECT == OBJECT_EVENT || E_OBJECT == OBJECT_EFFECT) previous = Event;
		else if (E_OBJECT == OBJECT_PLAYER_BULLET) previous = PlayerBullet;
		else if (E_OBJECT == OBJECT_ENEMY_BULLET) previous = EnemyBullet;
		else if (E_OBJECT == OBJECT_FARBACKGROUND) previous = FarBackGround;
		else if (E_OBJECT == OBJECT_BACKGROUND) previous = BackGround;
		else previous = ForeGround;
		next = previous->getNext();
		previous->addNext(this);
		if (next) next->addPrevious(this);
	};
	~Object() {
		if (previous) previous->addNext(this->next);
		if (next) next->addPrevious(previous);
	};
	//for registration
	void addPrevious(Object* Prev);
	void addNext(Object* Next);
	Object* getNext();
	void reassignObj(OBJECT_TYPE O_type, ACTION_TYPE A_type);
	static void Init(Graphics* graphics, Object* pPlayerCharacter, Object* pCharacter, Object* pEvent, Object* pPlayerBullet, Object* pEnemyBullet, Object* pFarBackGround, Object* pBackGround, Object* pForeGround);
	bool askIsbeingdeleted();
	//for render and update
	static Object* getPlayerCharacter();
	static Object* getCharacter();
	static Object* getEvent();
	static Object* getFarBackGround();
	static Object* getBackGround();
	static Object* getForeGround();
	static Object* getPlayerBullet();
	static Object* getEnemyBullet();
	//shape
	SHAPE_TYPE getShapeType();
	void switchShapeType(SHAPE_TYPE s_type);
	void switchShapeNum(int shapenum);
	void changeOpaque(float opaque);
	//AI
	OBJECT_TYPE getObjectType();
	static bool IsPlayerAlive();
	static float getPlayerX();
	static float getPlayerY();
	static float getPlayerSize();
	static int getPlayerHP();
	static bool DoesPlayerBLIPS();
	void setBlip();
	void setInvincible(bool invincible); // 무적 상태 설정/해제 함수
	bool getInvincible() const;          // 무적 상태 확인 함수
	void setPosition(float x, float y);
	void setSpeed(float dx, float dy);
	void changeSpeed(float vx, float vy);
	void Destroy();
	void Damage(int power); // get damage by power. Destroyes when health become 0;
	static bool CheckHostility(Object* A, Object* B);

	//shoot
	bool Shootball(float timeTotal, float degree_360); //shoot BB toward requested direction
	bool Shootball(float timeTotal, float x, float y); //shoot BB toward enemy
	bool Shootfront(float timeTotal); // shoot BB toward -x direction
	bool ShootSWIRL(float timeTotal); // shoot BB toward 8 direction, change degree by nSwirlDegree
	void DropBomb(float timeTotal);

	//shoot player character
	bool ShootPB(float timeTotal);

	bool getDestroyed(); //returns 
	bool createSmoke(float timeTotal);
	void updatePosition(float timeDelta);
	virtual void Update(float timeTotal, float timeDelta);
	void Render();
	void PlayerRender();
	void AllRender();
	bool IsOutofBorder();
	int getHP();
	void setHP(int hp);
	int getMaxHP();
	float getXPos();
	float getYPos();
	float getSize();

	//collision check
	static bool CheckCollision(Object* A, Object* B);
	void crashcheck();
	void shotcheck();
	void resetcreationtime(float timeTotal = 0.0);
	float getCreationTime() const;

	float getidle_x(float timeTotal);
	float getidle_y(float timeTotal);

	bool IsitActualObject();

	//action - to be connected via method pointer
	void UpdateNull(float timeTotal, float timeDelta);
	void UpdateSTILL(float timeTotal, float timeDelta);
	void UpdateIDLE(float timeTotal, float timeDelta);
	void UpdateFIGHTER(float timeTotal, float timeDelta);
	void UpdateTRAINED(float timeTotal, float timeDelta);
	void UpdateVETERAN(float timeTotal, float timeDelta);
	void UpdateKAMIKAZE(float timeTotal, float timeDelta);
	void UpdateSHOOTNBANZAI(float timeTotal, float timeDelta);
	void UpdateMG(float timeTotal, float timeDelta);
	void UpdateBALLOON8(float timeTotal, float timeDelta);
	void UpdateSWIRL(float timeTotal, float timeDelta);
	void UpdateBOSS(float timeTotal, float timeDelta);
	void UpdateBOMBER(float timeTotal, float timeDelta);
	void UpdatePLAYER(float timeTotal, float timeDelta);
	void UpdateBACKGROUND(float timeTotal, float timeDelta);
	void UpdateWEATHER(float timeTotal, float timeDelta);
	void UpdateINTRO(float timeTotal, float timeDelta);
	void UpdateEFFECT(float timeTotal, float timeDelta);
	void UpdateFALLING(float timeTotal, float timeDelta);
	void UpdateGOINGDOWN(float timeTotal, float timeDelta);
};

