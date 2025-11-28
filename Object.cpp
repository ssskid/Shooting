#include "Object.h"
#include "GameLevel.h"
#include <cmath>


Object* Object::PlayerCharacter = nullptr;
Object* Object::Character = nullptr;
Object* Object::Event = nullptr;
Object* Object::FarBackGround = nullptr;
Object* Object::BackGround = nullptr;
Object* Object::ForeGround = nullptr;
Object* Object::PlayerBullet = nullptr;
Object* Object::EnemyBullet = nullptr;
Graphics* Object::gfx = nullptr;

const float Object::idle_cyle[3] = { 0.19f, 0.5f, 0.7f };
const float Object::idle_amp[3] = { 5.25f, 3.5f, 10.0f };

void Object::addPrevious(Object* Prev) {
	previous = Prev;
}
void Object::addNext(Object* Next) {
	next = Next;
}
Object* Object::getNext() {
	return next;
}

void Object::reassignObj(OBJECT_TYPE O_type, ACTION_TYPE A_type){
	if (previous) previous->addNext(this->next);
	if (next) next->addPrevious(previous);
	E_OBJECT = O_type;
	E_AI = A_type;
	if (E_OBJECT == OBJECT_PLAYABLE_CHARACTER)previous = PlayerCharacter;
	else if (E_OBJECT == OBJECT_ENEMY || E_OBJECT == OBJECT_NEUTRAL) previous = Character;
	else if (E_OBJECT == OBJECT_EVENT || E_OBJECT == OBJECT_EFFECT) previous = Event;
	else if (E_OBJECT == OBJECT_PLAYER_BULLET) previous = PlayerBullet;
	else if (E_OBJECT == OBJECT_ENEMY_BULLET) previous = EnemyBullet;
	else if (E_OBJECT == OBJECT_FARBACKGROUND) previous = FarBackGround;
	else if (E_OBJECT == OBJECT_BACKGROUND) previous = BackGround;
	else previous = ForeGround;
	addNext(previous->getNext());
	previous->addNext(this);
	if (next) next->addPrevious(this);
}

void Object::Init(Graphics* graphics, Object* pPlayerCharacter, Object* pCharacter, Object* pEvent, Object* pPlayerBullet, Object* pEnemyBullet, Object* pFarBackGround, Object* pBackGround, Object* pForeGround) {
	gfx = graphics;
	PlayerCharacter = pPlayerCharacter;
	Character = pCharacter;
	Event = pEvent;
	PlayerBullet = pPlayerBullet;
	EnemyBullet = pEnemyBullet;
	FarBackGround = pFarBackGround;
	BackGround = pBackGround;
	ForeGround = pForeGround;
	return;
}

//for render and update
Object* Object::getPlayerCharacter() {
	return PlayerCharacter;
}
Object* Object::getCharacter() {
	return Character;
}
Object* Object::getEvent() {
	return Event;
}
Object* Object::getFarBackGround() {
	return FarBackGround;
}
Object* Object::getBackGround() {
	return BackGround;
}
Object* Object::getForeGround() {
	return ForeGround;
}
Object* Object::getPlayerBullet() {
	return PlayerBullet;
}
Object* Object::getEnemyBullet() {
	return EnemyBullet;
}

//shape
SHAPE_TYPE Object::getShapeType() {
	return E_SHAPE;
}

void Object::switchShapeType(SHAPE_TYPE s_type) {
	E_SHAPE = s_type;
}
void Object::switchShapeNum(int shapenum) {
	nShapeNum = shapenum;
}
void Object::changeOpaque(float opaque) {
	DrawOpaque = opaque;
}

//action
void Object::setBlip() {
	blips = true;
	bliptime = GameController::GetTimeTotal(); // 현재 시간으로 무적 시작 시간 기록
	OutputDebugString(L"Player: Blip started.\n");
}

void Object::setPosition(float x, float y) {
	Position_X = x, Position_Y = y;
}
void Object::setSpeed(float dx, float dy) {
	Speed_X = dx, Speed_Y = dy;
}
void Object::changeSpeed(float vx, float vy) {
	Speed_X += vx, Speed_Y += vy;
}
void Object::Destroy() {
	switch (getObjectType()) {
	case OBJECT_PLAYABLE_CHARACTER:
		GameController::setPlayerShot(); // 플레이어 사망 처리 (라이프 감소 등)
		// 즉시 ACTION_GOINGDOWN 상태로 변경하여 추락 시작
		reassignObj(OBJECT_EFFECT, ACTION_GOINGDOWN);
		// 추락 시에는 일반적으로 데미지를 입지 않으므로 isInvincible이나 blips를 설정할 수 있습니다.
		// setInvincible(true); // 추락 중에는 무적 (선택 사항)
		// setBlip(); // 또는 짧은 시간 깜빡임
		resetcreationtime(GameController::GetTimeTotal()); // 추락 시작 시간 기록
		// 플레이어 모양은 그대로 유지하거나, 파괴된 모양으로 변경할 수 있습니다.
		// switchShapeNum(2); // 예: 파괴된 모양
		OutputDebugString(L"Object: Player destroyed, changing to GOINGDOWN effect.\n");
		break; // IsDestroyed = true; 는 reassignObj 이후에 설정될 필요 없음 (EFFECT가 관리)
	case OBJECT_NEUTRAL:
		reassignObj(OBJECT_EFFECT, ACTION_GOINGDOWN);
		resetcreationtime(GameController::GetTimeTotal());
		break;
	case OBJECT_ENEMY:
		if (rand() % 2 == 1 || E_SHAPE == SHAPE_AIRSHIP) {
			reassignObj(OBJECT_EFFECT, ACTION_GOINGDOWN);
            resetcreationtime(GameController::GetTimeTotal());
		} else {
			reassignObj(OBJECT_EFFECT, ACTION_EFFECT);
            switchShapeType(SHAPE_EXPLOSION);
            switchShapeNum(0);
            setSpeed(0.0f, 0.0f);
            resetcreationtime(GameController::GetTimeTotal());
		}
		//point
		switch (getShapeType()) // E_SHAPE는 reassignObj 전에 확인해야 함
		{
		case SHAPE_AIRSHIP:
			GameController::addPlayerPoint(1000);
			break;
		case SHAPE_FOKKER:
			GameController::addPlayerPoint(10);
			break;
		default:
			break;
		}
		break;
	case OBJECT_PLAYER_BULLET:
	case OBJECT_ENEMY_BULLET:
		reassignObj(OBJECT_EFFECT, ACTION_EFFECT);
        switchShapeType(SHAPE_IMPACT);
        switchShapeNum(0);
        setSpeed(0.0f, 0.0f);
        resetcreationtime(GameController::GetTimeTotal());
		break;
	default:
		break;
	}
	// IsDestroyed 플래그는 reassignObj에서 OBJECT_EFFECT로 바뀌면 해당 EFFECT의 로직에 따라 관리되므로,
	// 여기서 직접 true로 설정하는 것은 주의해야 합니다.
	// 만약 Destroy() 호출 즉시 객체를 삭제 로직으로 넘기고 싶다면 Isbeingdeleted = true; 를 고려할 수 있으나,
	// GOINGDOWN 효과를 위해서는 객체가 살아있어야 합니다.
	// IsDestroyed = true; // 이 줄은 제거하거나, OBJECT_EFFECT의 Update 로직과 연동되도록 수정
}

bool Object::CheckHostility(Object* A, Object* B) {
	bool CrashMap[11][11] = {
		{ false, true, true, false, false, false, true, false, false, false, false },
		{ true, false, true, false, false, true, false, false, false, false, false },
		{ true, true, false, false, false, true, true, false, false, false, false },
		{ false, false, false, false, false, false, false, false, false, false, false },
		{ false, false, false, false, false, false, false, false, false, false, false },
		{ false, true, true, false, false, false, false, false, false, false, false },
		{ true, false, true, false, false, false, false, false, false, false, false },
		{ false, false, false, false, false, false, false, false, false, false, false },
		{ false, false, false, false, false, false, false, false, false, false, false },
		{ false, false, false, false, false, false, false, false, false, false, false },
		{ false, false, false, false, false, false, false, false, false, false, false }
	};
	return CrashMap[static_cast <int> (A->getObjectType())][static_cast <int> (B->getObjectType())];
}

//AI
bool Object::Shootball(float timeTotal, float degree_360) {
	while (degree_360 >= 360.0f) degree_360 -= 360.0f;
	float degree = degree_360 * PI / 180.0f ;
	Object* Ball = nullptr;
	Ball = new Object(Position_X, Position_Y, BALLSPEED * sinf(degree), BALLSPEED * cosf(degree), 10, OBJECT_ENEMY_BULLET, ACTION_STILL, SHAPE_BALL, 0, 1.0F, 3.0f);
	if (Ball) {
		lastshoot = timeTotal;
		E_ACTION = LA_SHOOTBALL;
	}	
	return Ball ? true : false;
}

bool Object::Shootball(float timeTotal, float x, float y) {
	float dx = Position_X - 12.0f - x;
	float dy = Position_Y - 12.0f - y;
	float td = dy + dx;
	Object* Ball = nullptr;
	Ball = new Object(Position_X - 12.0f, Position_Y, -BALLSPEED * dx / td, -BALLSPEED * dy / td, 10, OBJECT_ENEMY_BULLET, ACTION_STILL, SHAPE_BALL, 0, 1.0F, 3.0f);
	if (Ball) {
		lastshoot = timeTotal;
		E_ACTION = LA_SHOOTBALL;
	}
	return Ball ? true : false;
}

bool Object::Shootfront(float timeTotal) {
	Object* Ball = nullptr;
	Ball = new Object(Position_X - 12.0f, Position_Y, BALLSPEED * -1, 0.0f, 10, OBJECT_ENEMY_BULLET, ACTION_STILL, SHAPE_BALL, 0, 1.0F, 3.0f);
	if (Ball) {
		lastshoot = timeTotal;
		E_ACTION = LA_SHOOTBALL;
	}
	return Ball ? true : false;
}
void Object::DropBomb(float timeTotal) {
	Object* Bomb = new Object(Position_X, Position_Y - 12.0f, 0.0f, 0.0f, 10, OBJECT_ENEMY, ACTION_BOMBER, SHAPE_BOMB, 0, 1.0F, 10.0f);
	E_ACTION = LA_SHOOTBALL;
	lastshoot = timeTotal;
}
bool Object::ShootSWIRL(float timeTotal) {
	bool A = true;
	A &= Shootball(timeTotal, 0.0f + fSwirlDegree);
	A &= Shootball(timeTotal, 45.0f + fSwirlDegree);
	A &= Shootball(timeTotal, 90.0f + fSwirlDegree);
	A &= Shootball(timeTotal, 135.0f + fSwirlDegree);
	A &= Shootball(timeTotal, 180.0f + fSwirlDegree);
	A &= Shootball(timeTotal, 225.0f + fSwirlDegree);
	A &= Shootball(timeTotal, 270.0f + fSwirlDegree);
	A &= Shootball(timeTotal, 315.0f + fSwirlDegree);
	if (A) {
		nShootCounter++;
		E_ACTION = LA_8_BALL;
		lastshoot = timeTotal;
	}
	return A ? true : false;
}

bool Object::ShootPB(float timeTotal) {
	Object* Bullet = nullptr;
	Bullet = new Object(Position_X + 15.0f, Position_Y, PBULLETSPEED, 0.0f, 10, OBJECT_PLAYER_BULLET, ACTION_STILL, SHAPE_BULLET, 0, 1.0F, 3.0f);
	if (Bullet) {
		lastshoot = timeTotal;
		E_ACTION = LA_SHOOTBALL;
	}
	return Bullet ? true : false;
}

bool Object::getDestroyed() {
	return IsDestroyed;
}

bool Object::askIsbeingdeleted() {
	return Isbeingdeleted;
}

bool Object::createSmoke(float timeTotal) {
	Object* Smoke = nullptr;
	Smoke = new Object(Position_X, Position_Y, 0.0f, -0.5f, 10, OBJECT_EFFECT, ACTION_EFFECT, SHAPE_SMOKE, 0, 0.25f);
	return Smoke ? true : false;
}

void Object::updatePosition(float timeDelta) {
	Position_X += Speed_X * timeDelta * 100, Position_Y += Speed_Y * timeDelta * 100;
}

void Object::Update(float timeTotal, float timeDelta) {  //Action STILL and IDLE

	if (E_AI == ACTION_NULL) UpdateNull(timeTotal, timeDelta);
	else if (E_AI == ACTION_STILL)  UpdateSTILL(timeTotal, timeDelta);
	else if (E_AI == ACTION_IDLE)  UpdateIDLE(timeTotal, timeDelta);
	else if (E_AI == ACTION_FIGHTER)  UpdateFIGHTER(timeTotal, timeDelta);
	else if (E_AI == ACTION_TRAINED)  UpdateTRAINED(timeTotal, timeDelta);
	else if (E_AI == ACTION_VETERAN)  UpdateVETERAN(timeTotal, timeDelta);
	else if (E_AI == ACTION_KAMIKAZE)  UpdateKAMIKAZE(timeTotal, timeDelta);
	else if (E_AI == ACTION_SHOOTNBANZAI)  UpdateSHOOTNBANZAI(timeTotal, timeDelta);
	else if (E_AI == ACTION_MG)  UpdateMG(timeTotal, timeDelta);
	else if (E_AI == ACTION_BALLOON8)  UpdateBALLOON8(timeTotal, timeDelta);
	else if (E_AI == ACTION_SWIRL)  UpdateSWIRL(timeTotal, timeDelta);
	else if (E_AI == ACTION_BOSS)  UpdateBOSS(timeTotal, timeDelta);
	else if (E_AI == ACTION_BOMBER)  UpdateBOMBER(timeTotal, timeDelta);
	else if (E_AI == ACTION_PLAYER)  UpdatePLAYER(timeTotal, timeDelta);
	else if (E_AI == ACTION_BACKGROUND)  UpdateBACKGROUND(timeTotal, timeDelta);
	else if (E_AI == ACTION_WEATHER)  UpdateWEATHER(timeTotal, timeDelta);
	else if (E_AI == ACTION_INTRO)  UpdateINTRO(timeTotal, timeDelta);
	else if (E_AI == ACTION_EFFECT)  UpdateEFFECT(timeTotal, timeDelta);
	else if (E_AI == ACTION_FALLING)  UpdateFALLING(timeTotal, timeDelta);
	else  UpdateGOINGDOWN(timeTotal, timeDelta);
}
// crash check

bool Object::CheckCollision(Object* A, Object* B) {
	if (Object::CheckHostility(A, B)) {
		float ax = A->getXPos(), ay = A->getYPos(), bx = B->getXPos(), by = B->getYPos();
		int as = static_cast <int> (A->getShapeType()), bs = static_cast <int> (B->getShapeType());
		float library[5][4] = {
			{COLLISION_CAMEL_XH, COLLISION_CAMEL_XL, COLLISION_CAMEL_YH, COLLISION_CAMEL_YL},
			{COLLISION_FOKKER_XH, COLLISION_FOKKER_XL, COLLISION_FOKKER_YH, COLLISION_FOKKER_YL},
			{COLLISION_AIRSHIP_XH, COLLISION_AIRSHIP_XL, COLLISION_AIRSHIP_YH, COLLISION_AIRSHIP_YL},
			{COLLISION_PB_XH, COLLISION_PB_XL, COLLISION_PB_YH, COLLISION_PB_YL},
			{COLLISION_BB_XH, COLLISION_BB_XL, COLLISION_BB_YH, COLLISION_BB_YL},
		};
		
		if (bx + library[bs][1] <= ax + library[as][0] && ax + library[as][1] <= bx + library[bs][0] && by + library[bs][3] <= ay + library[as][2] && ay + library[as][3] <= by + library[bs][2]) {
			//wchar_t String[101];
			//swprintf_s(String, 100, L"COLLISION %d(%d) with %d(%d) @: X: %.0f // %.0f Y: %.0f // %.0f\n", static_cast <int> (A->getObjectType()), static_cast <int> (A->getShapeType()), static_cast <int> (B->getObjectType()), static_cast <int> (B->getShapeType()), A->getXPos(), B->getXPos(), A->getYPos(), B->getYPos());
			//GameController::Status.Append(String);
			return true;
		}
		else return false;
	}
	else return false;
}


void Object::crashcheck() {
	if (Object::IsPlayerAlive() && !Object::DoesPlayerBLIPS()) {
		float temp1, temp2, temp3;
		temp1 = unitSize + getPlayerSize();
		temp2 = Position_X - getPlayerX();
		temp3 = Position_Y - getPlayerY();
		if (getShapeType() == SHAPE_AIRSHIP) {
			if (Object::CheckCollision(this, getPlayerCharacter()->getNext())) {
				int nHPPb = getPlayerCharacter()->getNext()->getHP(), nHPT = this->getHP(), nInflictedDamage;
				nInflictedDamage = nHPPb < nHPT ? nHPPb : nHPT;
				getPlayerCharacter()->getNext()->Destroy(); // changed from getPlayerCharacter()->getNext()->Damage(nInflictedDamage);
				Damage(nInflictedDamage);
				this->Damage(nInflictedDamage);
			}
		}
		if (Object::CheckCollision(this, getPlayerCharacter()->getNext())) {  // changed from temp1 * temp1 >= temp2 * temp2 + temp3 * temp3

			int nPlayerHP = getPlayerCharacter()->getNext()->getHP(), nTHP = this->getHP(), nInflictedDamage;
			nInflictedDamage = nPlayerHP < nTHP ? nPlayerHP : nTHP;
			getPlayerCharacter()->getNext()->Destroy();	// changed from getPlayerCharacter()->getNext()->Damage(nInflictedDamage);
			if (this->getObjectType() != OBJECT_ENEMY_BULLET) this->Damage(nInflictedDamage);
			else this->Destroy();
		}
	}
}
void Object::shotcheck() {
	if (getObjectType() == OBJECT_ENEMY_BULLET || getObjectType() == OBJECT_PLAYER_BULLET || getObjectType() == OBJECT_PLAYABLE_CHARACTER) return;
	Object* Playershot, * Enemyshot;
	if (getObjectType() == OBJECT_NEUTRAL) {
		Enemyshot = getEnemyBullet()->getNext();
		while (Enemyshot && Enemyshot->IsitActualObject()) {
			float temp1, temp2, temp3;
			temp1 = unitSize + Enemyshot->getSize();
			temp2 = Position_X - Enemyshot->getXPos();
			temp3 = Position_Y - Enemyshot->getYPos();
			if (getObjectType() == SHAPE_AIRSHIP) {
				if (Object::CheckCollision(this, Enemyshot)) {  // changed from temp2 <= 62.5f && temp2 >= -62.5f && temp3 <= 12.5f && temp2 >= -12.5f
					int nHPPb = Enemyshot->getHP(), nHPT = this->getHP(), nInflictedDamage;
					nInflictedDamage = nHPPb < nHPT ? nHPPb : nHPT;
					Damage(nInflictedDamage);
					Enemyshot->Destroy();
				}
			}
			else {
				if (Object::CheckCollision(this, Enemyshot)) { // changed from temp1 * temp1 >= temp2 * temp2 + temp3 * temp3

					//wchar_t String[101];
					//swprintf_s(String, 100, L"Neutral Crashed on Enemy shot @: X: %.0f // %.0f Y: %.0f // %.0f\n", getXPos(), Enemyshot->getXPos(), getYPos(), Enemyshot->getYPos());
					//GameController::Status.Append(String);

					int nHPPb = Enemyshot->getHP(), nHPT = this->getHP(), nInflictedDamage;
					nInflictedDamage = nHPPb < nHPT ? nHPPb : nHPT;
					Damage(nInflictedDamage);
					Enemyshot->Destroy();
				}
			}
			Enemyshot = Enemyshot->getNext();
		}
		Playershot = Enemyshot->getNext();
	}
	else Playershot = getPlayerBullet()->getNext();
	while (Playershot && Playershot->IsitActualObject()) {
		float temp1, temp2, temp3;
		temp1 = getSize() + Playershot->getSize();
		temp2 = getXPos() - Playershot->getXPos();
		temp3 = getYPos() - Playershot->getYPos();
		if (getShapeType() == SHAPE_AIRSHIP) {
			if (Object::CheckCollision(this, Playershot)) { // changed from temp2 <= 62.5f && temp2 >= -62.5f && temp3 <= 12.5f && temp2 >= -12.5f
				int nHPPb = Playershot->getHP(), nHPT = this->getHP(), nInflictedDamage;
				nInflictedDamage = nHPPb < nHPT ? nHPPb : nHPT;
				GameController::addPlayerPoint(nInflictedDamage);  //add Point
				Damage(nInflictedDamage);
				Playershot->Destroy();
			}
		}
		else {
			if (Object::CheckCollision(this, Playershot)) { // changed from temp1 * temp1 >= temp2 * temp2 + temp3 * temp3

				//wchar_t String[101];
				//swprintf_s(String, 100, L"Crashed on Player shot @: X: %.0f // %.0f Y: %.0f // %.0f\n", getXPos(), Playershot->getXPos(), getYPos(), Playershot->getYPos());
				//GameController::Status.Append(String);

				int nHPPb = Playershot->getHP(), nHPT = this->getHP(), nInflictedDamage;
				nInflictedDamage = nHPPb < nHPT ? nHPPb : nHPT;
				GameController::addPlayerPoint(nInflictedDamage);  //add Point
				Damage(nInflictedDamage);
				Playershot->Destroy();
			}
		}
		Playershot = Playershot->getNext();
	}
}
bool Object::IsPlayerAlive() {
	if (PlayerCharacter->getNext() && PlayerCharacter->next->getHP() > 0.0f) return true;
	else return false;
}

OBJECT_TYPE Object::getObjectType() {
	return E_OBJECT;
}
void Object::UpdateNull(float timeTotal, float timeDelta) {
	return;
}
void Object::UpdateSTILL(float timeTotal, float timeDelta) {
	Position_X += Speed_X * timeDelta * 100;
	Position_Y += Speed_Y * timeDelta * 100;
	if (E_OBJECT == OBJECT_ENEMY || E_OBJECT == OBJECT_NEUTRAL || E_OBJECT == OBJECT_ENEMY_BULLET) crashcheck(), shotcheck();
	if (Position_X <= (float)(-SCREEN_X) || Position_X >= (float)(SCREEN_X << 1) || Position_Y <= (float)(-SCREEN_Y) || Position_Y >= (float)(SCREEN_Y << 1))Isbeingdeleted = true;
	return;
}
void Object::UpdateIDLE(float timeTotal, float timeDelta) {
	Position_X += Speed_X * timeDelta * 100;
	Position_Y += Speed_Y * timeDelta * 100 - prev_idlepos_y;
	float timegap = (timeTotal - creationtime) * 2 * PI;
	while (timegap >= PI + PI) timegap -= PI + PI;
	Position_Y += (prev_idlepos_y = 7 * sin(timegap));
	if (E_OBJECT == OBJECT_ENEMY || E_OBJECT == OBJECT_NEUTRAL || E_OBJECT == OBJECT_ENEMY_BULLET) crashcheck(), shotcheck();
	if (Position_X <= (float)(-SCREEN_X) || Position_X >= (float)(SCREEN_X << 1) || Position_Y <= (float)(-SCREEN_Y) || Position_Y >= (float)(SCREEN_Y << 1))Isbeingdeleted = true;
	return;
}
void Object::UpdateFIGHTER(float timeTotal, float timeDelta) {
	Position_X += Speed_X * timeDelta * 100;
	Position_Y += Speed_Y * timeDelta * 100;
	if (E_ACTION == LA_NONE && timeTotal > creationtime + 0.9f && Position_X < 500.0f) {
		Shootfront(timeTotal);
		E_ACTION = LA_SHOOTBALL;
	}
	if (E_OBJECT == OBJECT_ENEMY || E_OBJECT == OBJECT_NEUTRAL) crashcheck(), shotcheck();
	if (Position_X <= (float)(-SCREEN_X) || Position_X >= (float)(SCREEN_X << 1) || Position_Y <= (float)(-SCREEN_Y) || Position_Y >= (float)(SCREEN_Y << 1))Isbeingdeleted = true;
	return;
}
void Object::UpdateTRAINED(float timeTotal, float timeDelta) {
	if (E_ACTION == LA_NONE && timeTotal > creationtime + 0.7f && timeTotal > lastshoot + 0.3f && Position_X < 550.0f) {
		Shootball(timeTotal, getPlayerX(), getPlayerY());
		lastshoot = timeTotal;
		if (nShootCounter++ > 1) E_ACTION = LA_SHOOTBALL;
	}
	else if (E_ACTION == LA_SHOOTBALL) {
		if (getPlayerY() < Position_Y)accelerationY = -1.0f;
		else accelerationY = 1.0f;
		Speed_Y = sqrt(AIRSPEED * AIRSPEED - Speed_X * Speed_X);
		Speed_Y *= accelerationY;
		E_ACTION = LA_AVOID;
	}
	Position_X += Speed_X * timeDelta * 100;
	Position_Y += Speed_Y * timeDelta * 100;
	if (E_OBJECT == OBJECT_ENEMY || E_OBJECT == OBJECT_NEUTRAL) crashcheck(), shotcheck();
	if (Position_X <= (float)(-SCREEN_X) || Position_X >= (float)(SCREEN_X << 1) || Position_Y <= (float)(-SCREEN_Y) || Position_Y >= (float)(SCREEN_Y << 1))Isbeingdeleted = true;
	return;
}
void Object::UpdateVETERAN(float timeTotal, float timeDelta) {
	if (E_ACTION == LA_NONE && (Position_Y - getPlayerY()) * (Position_Y - getPlayerY()) < 10000.0f) {
		if (getPlayerY() == Position_Y) accelerationY = 0.0f;
		else if (getPlayerY() < Position_Y) accelerationY = 0.5f;
		else accelerationY = -0.5f;
	}
	if (E_ACTION == LA_NONE && timeTotal > creationtime + 0.7f && timeTotal > lastshoot + 0.2f && Position_X < 600) {
		Shootball(timeTotal, getPlayerX(), getPlayerY());
		lastshoot = timeTotal;
		if(nShootCounter++ > 2) E_ACTION = LA_SHOOTBALL;
	}
	else if (E_ACTION == LA_SHOOTBALL) {
		if (getPlayerY() < Position_Y)accelerationY = -1.0f;
		else accelerationY = 1.0f;
		Speed_Y = sqrt(AIRSPEED * AIRSPEED - Speed_X * Speed_X);
		Speed_Y *= accelerationY;
		E_ACTION = LA_AVOID;
	}
	Position_X += Speed_X * timeDelta * 100;
	Position_Y += Speed_Y * timeDelta * 100;
	if (E_OBJECT == OBJECT_ENEMY || E_OBJECT == OBJECT_NEUTRAL) crashcheck(), shotcheck();
	if (Position_X <= (float)(-SCREEN_X) || Position_X >= (float)(SCREEN_X << 1) || Position_Y <= (float)(-SCREEN_Y) || Position_Y >= (float)(SCREEN_Y << 1))Isbeingdeleted = true;
	return;
}
void Object::UpdateKAMIKAZE(float timeTotal, float timeDelta) {
	if (E_ACTION == LA_NONE && (Position_Y - getPlayerY()) * (Position_Y - getPlayerY()) < 10000.0f) {
		if (getPlayerY() == Position_Y) accelerationY = 0.0f;
		else if (getPlayerY() < Position_Y) accelerationY = 0.1f;
		else accelerationY = -0.1f;
	}
	float temp1 = Speed_Y + accelerationY * (float)timeDelta * 100;
	if (temp1 * temp1 + Speed_X * Speed_X >= AIRSPEED * AIRSPEED) {
		Speed_Y = sqrt(AIRSPEED * AIRSPEED - Speed_X * Speed_X);
		accelerationY = 0.0f;
	}
	else Speed_Y = temp1;
	Position_X += Speed_X * timeDelta * 100;
	Position_Y += Speed_Y * timeDelta * 100;
	if (E_OBJECT == OBJECT_ENEMY || E_OBJECT == OBJECT_NEUTRAL) crashcheck(), shotcheck();
	if (Position_X <= (float)(-SCREEN_X) || Position_X >= (float)(SCREEN_X << 1) || Position_Y <= (float)(-SCREEN_Y) || Position_Y >= (float)(SCREEN_Y << 1))Isbeingdeleted = true;
	return;
}
void Object::UpdateSHOOTNBANZAI(float timeTotal, float timeDelta) {
	if (getPlayerY() == Position_Y) accelerationY = 0.0f;
	else if (getPlayerY() < Position_Y) accelerationY = 0.1f;
	else accelerationY = -0.1f;
	if (E_ACTION == LA_NONE && Position_X < 600) {
		Shootball(timeTotal, getPlayerX(), getPlayerY());
		lastshoot = timeTotal;
		if (nShootCounter++ > 2) E_ACTION = LA_SHOOTBALL;
	}
	Position_X += Speed_X * timeDelta * 100;
	Position_Y += Speed_Y * timeDelta * 100 + accelerationY * timeDelta * 100;
	if (E_OBJECT == OBJECT_ENEMY || E_OBJECT == OBJECT_NEUTRAL) crashcheck(), shotcheck();
	if (Position_X <= (float)(-SCREEN_X) || Position_X >= (float)(SCREEN_X << 1) || Position_Y <= (float)(-SCREEN_Y) || Position_Y >= (float)(SCREEN_Y << 1))Isbeingdeleted = true;
	return;
}
void Object::UpdateMG(float timeTotal, float timeDelta) {
	if(timeTotal - lastshoot > 0.33 && Position_X < 500 && Position_X > getPlayerX() + 100){
		Shootball(timeTotal, getPlayerX(), getPlayerY());
		lastshoot = timeTotal;
	}
	else if (timeTotal - lastshoot > 0.33 && Position_X < getPlayerX() + 100) {
		Shootfront(timeTotal);
		lastshoot = timeTotal;
	}
	Position_X += Speed_X * timeDelta * 100;
	Position_Y += Speed_Y * timeDelta * 100;
	if (E_OBJECT == OBJECT_ENEMY || E_OBJECT == OBJECT_NEUTRAL) crashcheck(), shotcheck();
	if (Position_X <= (float)(-SCREEN_X) || Position_X >= (float)(SCREEN_X << 1) || Position_Y <= (float)(-SCREEN_Y) || Position_Y >= (float)(SCREEN_Y << 1))Isbeingdeleted = true;
	return;
}
void Object::UpdateBALLOON8(float timeTotal, float timeDelta) {
	Position_X += Speed_X * timeDelta * 100;
	if (Position_X < 480.0f) Speed_X = 0.0f;
	Position_Y += Speed_Y * timeDelta * 100 - prev_idlepos_y;
	float timegap = (timeTotal - creationtime) * 2 * PI;
	while (timegap >= PI + PI) timegap -= PI + PI;
	Position_Y += ( prev_idlepos_y = 7 * (float) sin( timegap ));
	if (timeTotal - lastshoot > 0.25 && Position_X < 550 && nShootCounter < 10) {
		ShootSWIRL(timeTotal);
		lastshoot = timeTotal;
		nShootCounter++;
	}
	if (nShootCounter >= 10 && timeTotal - lastshoot > 1.25) nShootCounter = 0;
	if (E_OBJECT == OBJECT_ENEMY || E_OBJECT == OBJECT_NEUTRAL) crashcheck(), shotcheck();
	if (Position_X <= (float)(-SCREEN_X) || Position_X >= (float)(SCREEN_X << 1) || Position_Y <= (float)(-SCREEN_Y) || Position_Y >= (float)(SCREEN_Y << 1))Isbeingdeleted = true;
	return;
}
void Object::UpdateSWIRL(float timeTotal, float timeDelta) {
	Position_X += Speed_X * timeDelta * 100;
	if (Position_X < 480.0f) Speed_X = 0.0f;
	Position_Y += Speed_Y * timeDelta * 100 - prev_idlepos_y;
	float timegap = (timeTotal - creationtime) * 2 * PI;
	while (timegap >= PI + PI) timegap -= PI + PI;
	Position_Y += (prev_idlepos_y = 7 * (float)sin(timegap));
	if (nShootCounter > 30) {
		IsCoolDown = true;
		if (timeTotal - lastshoot > 2.5f) nShootCounter = 0;
	}
	else if (timeTotal - lastshoot > 0.2 && Position_X < 600) {
		ShootSWIRL(timeTotal);
		fSwirlDegree += 4.5f;
		lastshoot = timeTotal;
	}
	if (E_OBJECT == OBJECT_ENEMY || E_OBJECT == OBJECT_NEUTRAL) crashcheck(), shotcheck();
	if (Position_X <= (float)(-SCREEN_X) || Position_X >= (float)(SCREEN_X << 1) || Position_Y <= (float)(-SCREEN_Y) || Position_Y >= (float)(SCREEN_Y << 1))Isbeingdeleted = true;
	return;
}
void Object::UpdateBOSS(float timeTotal, float timeDelta) {
	if (Position_X >= 500) Speed_Y = -0.66f;
	else if (Position_X <= 450) Speed_Y = 0.66f;
	if (Position_Y <= 60) Speed_Y = 0.76f;
	else if (Position_Y >= 300) Speed_Y = -0.76f;
	Position_X += Speed_X * timeDelta * 100;
	Position_Y += Speed_Y * timeDelta * 100 - prev_idlepos_y;
	float timegap = (timeTotal - creationtime) * 2 * PI;
	while (timegap >= PI + PI) timegap -= PI + PI;
	Position_Y += (prev_idlepos_y = 7 * (float)sin(timegap));
	if (timeTotal - lastshoot > 0.15 && Position_X < 550) {
		ShootSWIRL(timeTotal);
		fSwirlDegree += 3.5f;
		lastshoot = timeTotal;
	}
	if (E_OBJECT == OBJECT_ENEMY || E_OBJECT == OBJECT_NEUTRAL) crashcheck(), shotcheck();
	return;
}
void Object::UpdateBOMBER(float timeTotal, float timeDelta) {
	Position_X += Speed_X * timeDelta * 100;
	Position_Y += Speed_Y * timeDelta * 100 - prev_idlepos_y;
	float timegap = (timeTotal - creationtime) * 2 * PI;
	while (timegap >= PI + PI) timegap -= PI + PI;
	Position_Y += (prev_idlepos_y = 7 * (float)sin(timegap));
	if (timeTotal - lastshoot > 0.25) {
		DropBomb(timeTotal);
		lastshoot = timeTotal;
	}
	if (E_OBJECT == OBJECT_ENEMY || E_OBJECT == OBJECT_NEUTRAL) crashcheck(), shotcheck();
	if (Position_X <= (float)(-SCREEN_X) || Position_X >= (float)(SCREEN_X << 1) || Position_Y <= (float)(-SCREEN_Y) || Position_Y >= (float)(SCREEN_Y << 1))Isbeingdeleted = true;
	return;
}
void Object::UpdateBACKGROUND(float timeTotal, float timeDelta) {
	//MessageBox(NULL, L"Note!", L"Background called", MB_OK);
	Position_X += Speed_X * timeDelta * 100;
	Position_Y += Speed_Y * timeDelta * 100;
	if (Position_X <= -(SCREEN_X / 2)) Position_X += SCREEN_X + SCREEN_X;
	if (Position_X <= (float)(-SCREEN_X) || Position_X >= (float)(SCREEN_X << 1) || Position_Y <= (float)(-SCREEN_Y) || Position_Y >= (float)(SCREEN_Y << 1))Isbeingdeleted = true;
	return;
}
void Object::UpdateWEATHER(float timeTotal, float timeDelta) {
	Position_X += Speed_X * timeDelta * 100;
	Position_Y += Speed_Y * timeDelta * 100;
	if (Position_X <= (float)(-SCREEN_X) || Position_X >= (float)(SCREEN_X << 1) || Position_Y <= (float)(-SCREEN_Y) || Position_Y >= (float)(SCREEN_Y << 1))Isbeingdeleted = true;
	return;
}
void Object::UpdateINTRO(float timeTotal, float timeDelta) {
	Position_X += Speed_X * timeDelta * 100 - prev_idlepos_x;
	Position_Y += Speed_Y * timeDelta * 100 - prev_idlepos_y;
	float timegap = (timeTotal - creationtime) * 2 * PI;
	while (timegap >= PI + PI) timegap -= PI + PI;
	Position_X += (prev_idlepos_x = 3 * (float)cos(timegap));
	Position_Y += (prev_idlepos_y = 7 * (float)sin(timegap));
	//Position_X += getidle_x(timeTotal);
	//Position_Y += getidle_y(timeTotal);
}
void Object::UpdateEFFECT(float timeTotal, float timeDelta) {
	float duration = timeTotal - creationtime;
	if (E_SHAPE == SHAPE_EXPLOSION) Speed_X = 0.5f;
	int slide = (int) (duration / EFFECTTIME);
	if (duration >= EFFECTCYCLE || slide >= SPRITE_IMGMAXPERCHARACTER) Isbeingdeleted = IsAnimationFinished = true, nShapeNum = 0;
	else nShapeNum = slide;
	if (nShapeNum == 6) DrawOpaque *= 0.75f;
	else if (nShapeNum == 7) DrawOpaque *= 0.66f;
	else if (nShapeNum == 8) DrawOpaque *= 0.5f;
	Position_X += Speed_X * timeDelta * 100;
	Position_Y += Speed_Y * timeDelta * 100;
	if (Position_X <= (float)(-SCREEN_X) || Position_X >= (float)(SCREEN_X << 1) || Position_Y <= (float)(-SCREEN_Y) || Position_Y >= (float)(SCREEN_Y << 1))Isbeingdeleted = true;
}
void Object::UpdateFALLING(float timeTotal, float timeDelta) {
	accelerationY = 0.2f;
	Speed_Y += accelerationY * timeDelta * 100;
	Position_X += Speed_X * timeDelta * 100;
	Position_Y += Speed_Y * timeDelta * 100;
	if (Position_X <= (float)(-SCREEN_X) || Position_X >= (float)(SCREEN_X << 1) || Position_Y <= (float)(-SCREEN_Y) || Position_Y >= (float)(SCREEN_Y << 1))Isbeingdeleted = true;
}
void Object::UpdateGOINGDOWN(float timeTotal, float timeDelta) {
    // 추락 시작 시 상승 중이었다면 속도를 즉시 아래로 향하게 조정
    if (Speed_Y > 0.0f && E_ACTION != LA_GOINGDOWN_INITIATED) { // LA_GOINGDOWN_INITIATED 같은 플래그로 최초 한 번만 실행
        Speed_Y = 0.0f; // 또는 작은 음수값 (예: -0.1f)으로 설정하여 바로 하강 시작
        OutputDebugString(L"Object: GOINGDOWN initiated while moving up, Speed_Y reset.\n");
        E_ACTION = LA_GOINGDOWN_INITIATED; // 상태 플래그 업데이트 (LAST_ACTION enum에 추가 필요)
    } else if (E_ACTION != LA_GOINGDOWN_INITIATED) {
        // GOINGDOWN이 처음 호출된 것이 아니라면 이 플래그를 설정 (reassignObj에서 ACTION_GOINGDOWN으로 바로 올 때)
        E_ACTION = LA_GOINGDOWN_INITIATED;
    }


    accelerationY = 0.0075f; // 아래로 향하는 가속도
    if (E_SHAPE == SHAPE_AIRSHIP) { // 에어쉽은 등속 하강
        Speed_Y = 0.6f;
    } else {
        Speed_Y += accelerationY * timeDelta * 100; // 가속도 적용
    }

    if (Speed_Y >= 1.7f) Speed_Y = 1.7f; // 최대 하강 속도 제한

    Position_X += Speed_X * timeDelta * 100;
    Position_Y += Speed_Y * timeDelta * 100;

    if (timeTotal - lastshoot >= 0.0625) {
    if (getShapeType() == SHAPE_CAMEL) {
			new Object(Position_X - 30.0f, Position_Y - 10.0f, -0.5f, -0.5f, 10, OBJECT_EFFECT, ACTION_EFFECT, SHAPE_SMOKE, 0, 0.8f);
		}
		else if (getShapeType() == SHAPE_AIRSHIP) {
			createSmoke(timeTotal);
			new Object(Position_X + 20.0f, Position_Y - 10.0f, 0.0f, -0.5f, 10, OBJECT_EFFECT, ACTION_EFFECT, SHAPE_SMOKE, 0, 0.8f);
			new Object(Position_X - 30.0f, Position_Y - 10.0f, 0.0f, -0.5f, 10, OBJECT_EFFECT, ACTION_EFFECT, SHAPE_SMOKE, 0, 0.8f);
			new Object(Position_X - 50.0f, Position_Y - 30.0f, 0.0f, -0.5f, 10, OBJECT_EFFECT, ACTION_EFFECT, SHAPE_SMOKE, 0, 0.8f);
			new Object(Position_X + 50.0f, Position_Y - 30.0f, 0.0f, -0.5f, 10, OBJECT_EFFECT, ACTION_EFFECT, SHAPE_SMOKE, 0, 0.8f);
		}
		else createSmoke(timeTotal);
        lastshoot = timeTotal;
    }
    if (Position_X <= (float)(-SCREEN_X) || Position_X >= (float)(SCREEN_X << 1) || Position_Y <= (float)(-SCREEN_Y) || Position_Y >= (float)(SCREEN_Y << 1))Isbeingdeleted = true;
}

void Object::Render() {
	if (E_OBJECT == OBJECT_PLAYABLE_CHARACTER) PlayerRender();
	else if (E_OBJECT == OBJECT_NULL) return;
	else AllRender();
	return;
}

void Object::AllRender() {
	gfx->OnPaintRenderTarget(Position_X, Position_Y, static_cast<int>(E_SHAPE), nShapeNum, DrawOpaque);
}

bool Object::IsOutofBorder() {
	if (Position_X > (SCREEN_X << 1) || Position_X < (SCREEN_X * -1) || Position_Y >(SCREEN_Y << 1) || Position_Y < (SCREEN_Y * -1)) return true;
	else return false;
}

float Object::getidle_x(float timeTotal) { //re-program
	float time[3];
	float sum = 0.0f;
	time[0] = idle_cyle[0] * (timeTotal - creationtime);
	time[1] = idle_cyle[1] * (timeTotal - creationtime);
	time[2] = idle_cyle[2] * (timeTotal - creationtime);
	while (time[0] > PI)time[0] -= PI;
	while (time[1] > PI)time[1] -= PI;
	while (time[2] > PI)time[2] -= PI;
	for (int i = 0; i < 3; i++) {
		sum += idle_amp[i] * sin(time[i] * PI / 180.0f);
	}
	sum -= prev_idlepos_x;
	//if (sum >= 10.0f)sum = 10.0f;
	//else if (sum <= -10.0f)sum = -10.0f;
	return prev_idlepos_x = sum;
}
float Object::getidle_y(float timeTotal) {//re-program
	/*float sum = 0.0f;
	for (int i = 0; i < 3; i++) {
		sum += idle_amp[i] * cosf(idle_cyle[i] * (float)timeTotal);
	}*/
	float time[3];
	float sum = 0.0f;
	time[0] = idle_cyle[0] * (timeTotal - creationtime);
	time[1] = idle_cyle[1] * (timeTotal - creationtime);
	time[2] = idle_cyle[2] * (timeTotal - creationtime);
	while (time[0] > PI)time[0] -= PI;
	while (time[1] > PI)time[1] -= PI;
	while (time[2] > PI)time[2] -= PI;
	for (int i = 0; i < 3; i++) {
		sum += idle_amp[i] * cos(time[i] * PI / 180.0f);
	}
	sum -= prev_idlepos_y;
	//if (sum >= 10.0f)sum = 10.0f;
	//else if (sum <= -10.0f)sum = -10.0f;
	return prev_idlepos_y = sum;
}


float Object::getPlayerX() {
	if (PlayerCharacter && PlayerCharacter->next) return PlayerCharacter->next->getXPos();
	else return -1;
}
float Object::getPlayerY() {
	if (PlayerCharacter && PlayerCharacter->next) return PlayerCharacter->next->getYPos();
	else return -1;
}

float Object::getPlayerSize() {
	if (PlayerCharacter && PlayerCharacter->next) return PlayerCharacter->next->getSize();
	else return -1;
}

int Object::getPlayerHP() {
	if (PlayerCharacter && PlayerCharacter->next) return PlayerCharacter->next->getHP();
	else return -1;
}

bool Object::DoesPlayerBLIPS() {
	if (PlayerCharacter && PlayerCharacter->next) return PlayerCharacter->getNext()->blips;
	else return false;
}

float Object::getXPos() {
	return Position_X;
}
float Object::getYPos() {
	return Position_Y;
}

float Object::getSize() {
	return unitSize;
}

int Object::getHP() {
	return nHealth;
}

void Object::setHP(int hp) {
	nHealth = hp;
}

int Object::getMaxHP() {
	return nMaxHealth;
}

bool Object::IsitActualObject() {
	if (E_OBJECT == OBJECT_NULL) return false;
	else return true;
}

void Object::resetcreationtime(float timeTotal) {
	creationtime = timeTotal;
	return;
}

float Object::getCreationTime() const {
	return creationtime;
}

void Object::setInvincible(bool invincible) {
	isInvincible = invincible;
	if (isInvincible) {
		blips = true; // 무적 상태일 때는 항상 깜빡이도록 설정 (선택적)
		bliptime = GameController::GetTimeTotal(); // 깜빡임 시작 시간도 업데이트
		OutputDebugString(L"Object: Invincibility set.\n");
	}
	else {
		blips = false; // 무적 해제 시 깜빡임도 중단 (setBlip으로 별도 관리 시 이 줄은 필요 없을 수 있음)
		OutputDebugString(L"Object: Invincibility removed.\n");
	}
}

bool Object::getInvincible() const {
	return isInvincible;
}

void Object::Damage(int power) {
	if (isInvincible || blips) { // 영구 무적 또는 시간제한 무적(깜빡임) 상태이면 데미지 안 받음
		OutputDebugString(L"Object: Damage attempt ignored due to invincibility/blips.\n");
		return;
	}
	this->nHealth -= power;
	if (nHealth <= 0) { 
		this->Destroy();
	}
}

void Object::PlayerRender() {
	bool actuallyDraw = true;

	if (blips || isInvincible) { // 일반 blips 또는 영구 무적 상태일 때 깜빡임
		// 영구 무적일 때는 계속 깜빡이거나, 다른 시각적 효과를 줄 수 있음
		// 여기서는 blips와 동일한 로직으로 깜빡임 처리

		int blinkCycleCounter = static_cast<int>((GameController::GetTimeTotal() - bliptime) / BLIPBLINKCYCLE);
		if (blinkCycleCounter % 2 != 0) {
			actuallyDraw = false;
		}

		// 만약 isInvincible일 때 항상 보이게 하려면:
		// if (blips && !isInvincible) { /* 기존 깜빡임 로직 */ }
		// else if (isInvincible) { actuallyDraw = true; /* 또는 다른 효과 */ }
	}
	if (actuallyDraw) {
		gfx->OnPaintRenderTarget(Position_X, Position_Y, static_cast<int>(E_SHAPE), nShapeNum, DrawOpaque);
	}
}

void Object::UpdatePLAYER(float timeTotal, float timeDelta) {
	if (GameLevel::isButton_Up()) {
		nShapeNum = 2;
		if (GameLevel::isButton_Left()) Speed_X = -PSPEEDH, Speed_Y = -PSPEEDH;
		else if (GameLevel::isButton_Right()) Speed_X = PSPEEDH, Speed_Y = -PSPEEDH;
		else Speed_X = 0.0f, Speed_Y = -PSPEEDF;
	}
	else if (GameLevel::isButton_Down()) {
		nShapeNum = 1;
		if (GameLevel::isButton_Left()) Speed_X = -PSPEEDH, Speed_Y = PSPEEDH;
		else if (GameLevel::isButton_Right()) Speed_X = PSPEEDH, Speed_Y = PSPEEDH;
		else Speed_X = 0.0f, Speed_Y = PSPEEDF;
	}
	else {
		nShapeNum = 0;
		if (GameLevel::isButton_Left()) Speed_X = -PSPEEDF, Speed_Y = 0.0f;
		else if (GameLevel::isButton_Right()) Speed_X = PSPEEDF, Speed_Y = 0.0f;
		else Speed_X = 0.0f, Speed_Y = 0.0f;
	}
	Position_X += Speed_X * timeDelta * 100, Position_Y += Speed_Y * timeDelta * 100;
	//Position_X += Speed_X / 1000, Position_Y += Speed_Y * timeDelta * 1000;
	if (Position_X > (float)(SCREEN_X - 35)) Position_X = (float)(SCREEN_X - 35);	// limit X
	else if (Position_X < (float)(35)) Position_X = (float)35;
	if (Position_Y > (float)(SCREEN_Y - 35)) Position_Y = (float)(SCREEN_Y - 35);	// limit Y
	else if (Position_Y < (float)(35)) Position_Y = (float)35;

	if (GameLevel::isButton_Shoot() && lastshoot + SHOOTDELAYTIME <= timeTotal) ShootPB(timeTotal), lastshoot = timeTotal;
	
	// 일반 blips (시간 제한 무적) 처리
	if (blips && !isInvincible && (bliptime + BLIPTIME <= timeTotal)) {
		blips = false; 
		OutputDebugString(L"Player: Blip time ended (timed invincibility).\n");
	}
	// isInvincible은 setInvincible(false)로 명시적으로 해제될 때까지 유지됨
	return;
}
