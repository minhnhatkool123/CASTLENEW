﻿#include <d3dx9.h>
#include <algorithm>


#include "Utils.h"
#include "Textures.h"
#include "Game.h"
#include "GameObject.h"
#include "Sprites.h"

CGameObject::CGameObject()
{
	x = y = 0;
	vx = vy = 0;
	nx = 1;
}

void CGameObject::Update(DWORD dt, vector<LPGAMEOBJECT> *coObjects,bool clk)
{
	this->dt = dt;
	dx = vx * dt;
	dy = vy * dt;
}

bool CGameObject::AABBCheck(float l_a, float t_a, float r_a, float b_a, float l_b, float t_b, float r_b, float b_b)
{
	return (l_a < r_b && r_a > l_b && t_a < b_b && b_a > t_b);
	//return !(l_a > r_b || r_a<l_b || t_a>b_b || b_a < t_b);
}


/*
	Extension of original SweptAABB to deal with two moving objects
*/
LPCOLLISIONEVENT CGameObject::SweptAABBEx(LPGAMEOBJECT coO)
{
	//float sl, st, sr, sb;		// static object bbox
	//float ml, mt, mr, mb;		// moving object bbox
	//float t, nx, ny;

	//coO->GetBoundingBox(sl, st, sr, sb);

	//// deal with moving object: m speed = original m speed - collide object speed

	///*Chú ý: Vận tốc của vật trong chương này là khoảng cách vật di chuyển
	//trong 1 frame: có được bằng cách nhân vận tốc tính trên giây với
	//	khoảng thời gian giữa các frame(delta time)*/
	//float svx, svy;
	//coO->GetSpeed(svx, svy);

	//float sdx = svx * dt;
	//float sdy = svy * dt;

	//float dx = this->dx - sdx;
	//float dy = this->dy - sdy;

	//GetBoundingBox(ml, mt, mr, mb);

	//CGame::SweptAABB(
	//	ml, mt, mr, mb,
	//	dx, dy,
	//	sl, st, sr, sb,
	//	t, nx, ny
	//);

	//CCollisionEvent * e = new CCollisionEvent(t, nx, ny, coO);
	//return e;
	float sl, st, sr, sb;		// static object bbox
	float ml, mt, mr, mb;		// moving object bbox
	float t, nx, ny;

	coO->GetBoundingBox(sl, st, sr, sb);

	// deal with moving object: m speed = original m speed - collide object speed
	float svx, svy;
	coO->GetSpeed(svx, svy);

	float sdx = svx * dt;
	float sdy = svy * dt;

	// (rdx, rdy) is RELATIVE movement distance/velocity 
	float rdx = this->dx - sdx;
	float rdy = this->dy - sdy;

	GetBoundingBox(ml, mt, mr, mb);

	CGame::SweptAABB(
		ml, mt, mr, mb,
		rdx, rdy,
		sl, st, sr, sb,
		t, nx, ny
	);

	CCollisionEvent * e = new CCollisionEvent(t, nx, ny, rdx, rdy, coO);
	return e;
}

/*
	Calculate potential collisions with the list of colliable objects

	coObjects: the list of colliable objects
	coEvents: list of potential collisions
*/
void CGameObject::CalcPotentialCollisions(
	vector<LPGAMEOBJECT> *coObjects,
	vector<LPCOLLISIONEVENT> &coEvents)
{
	for (UINT i = 0; i < coObjects->size(); i++)
	{
		LPCOLLISIONEVENT e = SweptAABBEx(coObjects->at(i));

		if (e->t > 0 && e->t <= 1.0f)
			coEvents.push_back(e);
		else
			delete e;
	}

	std::sort(coEvents.begin(), coEvents.end(), CCollisionEvent::compare);
}

void CGameObject::FilterCollision(
	vector<LPCOLLISIONEVENT> &coEvents,
	vector<LPCOLLISIONEVENT> &coEventsResult,
	float &min_tx, float &min_ty,
	float &nx, float &ny)
{
	min_tx = 1.0f;
	min_ty = 1.0f;
	int min_ix = -1;
	int min_iy = -1;

	nx = 0.0f;
	ny = 0.0f;

	coEventsResult.clear();

	for (UINT i = 0; i < coEvents.size(); i++)
	{
		LPCOLLISIONEVENT c = coEvents[i];

		if (c->t < min_tx && c->nx != 0) {
			min_tx = c->t; nx = c->nx; min_ix = i;
		}

		if (c->t < min_ty  && c->ny != 0) {
			min_ty = c->t; ny = c->ny; min_iy = i;
		}
	}

	if (min_ix >= 0)
		coEventsResult.push_back(coEvents[min_ix]);

	if (min_iy >= 0)
		coEventsResult.push_back(coEvents[min_iy]);
}


void CGameObject::RenderBoundingBox()
{
	D3DXVECTOR3 p(x, y, 0);
	RECT rect;

	LPDIRECT3DTEXTURE9 bbox = CTextures::GetInstance()->Get(ID_TEX_BBOX);

	float l, t, r, b;

	GetBoundingBox(l, t, r, b);
	rect.left = 0;
	rect.top = 0;
	rect.right = (int)r - (int)l;
	rect.bottom = (int)b - (int)t;

	CGame::GetInstance()->Draw(nx,l, t, bbox, rect.left, rect.top, rect.right, rect.bottom,100);
}

//void CGameObject::AddAnimation(int aniId)
//{
//	LPANIMATION ani = CAnimations::GetInstance()->Get(aniId);
//	animations.push_back(ani);
//}


CGameObject::~CGameObject()
{

}