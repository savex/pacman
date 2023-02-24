// player.cpp: implementation of the player class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "player.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/*
	������� ������ ��� �� �����.
	���� ����� ����� � ������ 'level'
	��� �������� ������ � ��������
	� ��������� � ���������� � �.�.
*/
player::player()
{
	anim.bAnim=false;
	pacman.Stand=true;
}

player::~player()
{

}

//�������� ��������
bool player::LoadObjects(Cmodel* pMdl,LPDIRECT3DDEVICE8 pDevice)
{
	char stTmp[128];
	for(int i=0;i<18;i++) {
			sprintf(stTmp,".\\data\\pm%df",i);
			pacman.mdl[i]=pMdl->OpenModel(stTmp,pDevice);
			if (!pacman.mdl[i]) return false;
	}
	return true;
}

//������������� ������� �������
bool player::Init(level* pLvl)
{
	pacman.pX=pLvl->lvlMap.stPX;
	pacman.pY=pLvl->lvlMap.stPY;
	pacman.iDir=2;
	pacman.Speed=500;
	pacman.mTime=0;
	anim.curFrame=0;
	anim.frDelta=0;

	lvProp.stX=(float)(pLvl->lvlMap.lenX/2.0)*TLSIZE;
	lvProp.stY=(float)(pLvl->lvlMap.lenY/2.0)*TLSIZE;

	pacman.vPos.x=lvProp.stX-pacman.pX*TLSIZE-HTLSIZE;
	pacman.vPos.y=lvProp.stY-pacman.pY*TLSIZE-HTLSIZE;
	pacman.vPos.z=0.f;

	return true;
}

//���������� �� �������
void player::Update(DWORD delta,level* pLvl,input* pInput)
{
	if (anim.bAnim) {
		if (anim.frDelta>=(DWORD)pacman.Speed/18) {
			anim.frDelta-=(DWORD)pacman.Speed/18;	
			if (anim.curFrame!=17) anim.curFrame++;
			else anim.curFrame=0;
		}
		anim.frDelta+=delta;
	}

	//���������� �����

	
	//���������� ��������
	if (!pacman.Stand) UpdateMove(delta,pLvl);

	//��������� ������
	//���� ������� ������ �� ����� ������� ������� ������ ��������
	if (pInput->Keys.Up && pacman.Stand) StartMove(0,pLvl);
	if (pInput->Keys.Down && pacman.Stand) StartMove(2,pLvl);
	if (pInput->Keys.Left && pacman.Stand) StartMove(3,pLvl);
	if (pInput->Keys.Right && pacman.Stand) StartMove(1,pLvl);

	D3DXMATRIX	mTmp;
	int r;
	r=pacman.iDir*90;
	D3DXMatrixTranslation(&mTmp,pacman.vPos.x,pacman.vPos.y,pacman.vPos.z);
	D3DXMatrixRotationZ(&pacman.mOrg,D3DXToRadian(r));
	D3DXMatrixMultiply(&pLvl->pman.mOrg,&pacman.mOrg,&mTmp);

	pLvl->pman.mdl=pacman.mdl[anim.curFrame];
	pLvl->pman.x=pacman.pX;
	pLvl->pman.y=pacman.pY;
}

//������� ������
void player::Release(Cmodel* pMdl)
{
	for(int i=0;i<18;i++) pMdl->Release(pacman.mdl[i]);
}

//�������� �������� ������� � ��������� �������
void player::StartMove(int iDir,level* pLvl)
{
	if (CanMove(iDir,pLvl)) {			//�������� �����������
		pacman.iDir=iDir;
		pacman.Stand=false;
		pacman.mTime=pacman.Speed;
		switch (iDir) {
		case 0:							//�����
			pacman.tX=pacman.pX;
			pacman.tY=--pacman.pY;
			break;
		case 1:							//������
			pacman.tX=++pacman.pX;
			pacman.tY=pacman.pY;
			break;
		case 2:							//����
			pacman.tX=pacman.pX;
			pacman.tY=++pacman.pY;
			break;
		case 3:							//�����
			pacman.tX=--pacman.pX;
			pacman.tY=pacman.pY;
			break;
		}
		if (pLvl->lvlMap.Tiles[pacman.tX][pacman.tY].objType>9 && 
			pLvl->lvlMap.Tiles[pacman.tX][pacman.tY].objType<16) anim.bAnim=true;
	}
	else pacman.Stand=true;
}

//���������� ��������
void player::UpdateMove(DWORD delta,level* pLvl)
{
	float fD=0;
	//���� ������ ������ ������� ��� �������� � mTime,
	//�� ������ ����� ����������� �������� �� ��������� �������;
	if (delta>pacman.mTime) {
		fD=(float)(10.f/pacman.Speed)*pacman.mTime;
		pacman.mTime=0;
		pacman.pX=pacman.tX;
		pacman.pY=pacman.tY;
		pacman.Stand=true;
		//��������, ����� ��������� ������� � �� ��� ����� ������� �������
		if (pLvl->lvlMap.Tiles[pacman.pX][pacman.pY].objType==0x0A ||
			pLvl->lvlMap.Tiles[pacman.pX][pacman.pY].objType==0x0B) {
			lvProp.objPicked=pLvl->lvlMap.Tiles[pacman.pX][pacman.pY].objType;
			pLvl->lvlMap.Tiles[pacman.pX][pacman.pY].objType=0;
		}
		StopAnim();
	}
	else {
		fD=(float)(10.f/pacman.Speed)*delta;
		pacman.mTime-=delta;
	}
	
	switch (pacman.iDir) {
	case 0:
		pacman.vPos.y+=fD;
		break;
	case 1:
		pacman.vPos.x-=fD;
		break;
	case 2:
		pacman.vPos.y-=fD;
		break;
	case 3:
		pacman.vPos.x+=fD;
		break;
	}
}

//��������� ��������� ������ �� ����������� ��������
bool player::CanMove(int iDir,level* pLvl)
{
	switch (iDir) {
	case 0:
		if ((pacman.pY-1<0) ||
			(pLvl->lvlMap.Tiles[pacman.pX][pacman.pY-1].objType>0) &&
			(pLvl->lvlMap.Tiles[pacman.pX][pacman.pY-1].objType<7)) return false;
		else return true;
		break;
	case 1:
		if ((pacman.pX+1>pLvl->lvlMap.lenX-1) ||
			(pLvl->lvlMap.Tiles[pacman.pX+1][pacman.pY].objType>0) &&
			(pLvl->lvlMap.Tiles[pacman.pX+1][pacman.pY].objType<7)) return false;
		else return true;
		break;
	case 2:
		if ((pacman.pY+1>pLvl->lvlMap.lenY-1) ||
			(pLvl->lvlMap.Tiles[pacman.pX][pacman.pY+1].objType>0) &&
			(pLvl->lvlMap.Tiles[pacman.pX][pacman.pY+1].objType<7)) return false;
		else return true;
		break;
	case 3:
		if ((pacman.pX-1<0) ||
			(pLvl->lvlMap.Tiles[pacman.pX-1][pacman.pY].objType>0) &&
			(pLvl->lvlMap.Tiles[pacman.pX-1][pacman.pY].objType<7)) return false;
		else return true;
		break;
	default: return false;
	}
}

//Stops pacman anim
void player::StopAnim()
{
	anim.bAnim=false;
	anim.curFrame=0;
	anim.frDelta=0;
}

//Starts pacman anim
void player::StartAnim()
{
	anim.bAnim=true;
}