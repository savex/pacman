// monster.cpp: implementation of the monster class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "monster.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

monster::monster()
{
	ZeroMemory(monsters,sizeof(MONSTER)*12);
	iTotal=0;
	PathLen=-1;
}

monster::~monster()
{

}

bool monster::LoadMonsters(Cmodel* pMdl,LPDIRECT3DDEVICE8 pDevice)
{
	char stTmp[128];
	for(int i=0;i<18;i++) {
			sprintf(stTmp,".\\data\\smon%df",i);
			anims.sMove[i]=pMdl->OpenModel(stTmp,pDevice);
			if (!anims.sMove[i]) return false;

			sprintf(stTmp,".\\data\\mmon%df",i);
			anims.mMove[i]=pMdl->OpenModel(stTmp,pDevice);
			if (!anims.mMove[i]) return false;

			sprintf(stTmp,".\\data\\smond%df",i);
			anims.sDead[i]=pMdl->OpenModel(stTmp,pDevice);
			if (!anims.sDead[i]) return false;
	}

	for(i=0;i<6;i++) {
			sprintf(stTmp,".\\data\\bmon%df",i);
			anims.bMove[i]=pMdl->OpenModel(stTmp,pDevice);
			if (!anims.bMove[i]) return false;

			sprintf(stTmp,".\\data\\bmond%df",i);
			anims.bDead[i]=pMdl->OpenModel(stTmp,pDevice);
			if (!anims.bDead[i]) return false;
	}

	sprintf(stTmp,".\\data\\mmond",i);
	anims.mDead=pMdl->OpenModel(stTmp,pDevice);
	if (!anims.mDead) return false;

	return true;
}

void monster::UpdateMonsters(DWORD delta,level* pLvl)
{
	for(int i=0;i<iTotal;i++) {
		//animation for monsters
		if (monsters[i].aniPr.bAnim) {
			if (monsters[i].bDead) {
				if (monsters[i].aniPr.frDelta>=monsters[i].aniPr.dDead) {
					monsters[i].aniPr.frDelta-=monsters[i].aniPr.dDead;	
					if (monsters[i].aniPr.curFrame!=monsters[i].aniPr.frDead-1) 
						monsters[i].aniPr.curFrame++;
					else monsters[i].aniPr.curFrame=0;
				}
				monsters[i].aniPr.frDelta+=delta;
			}
			else {
				if (monsters[i].aniPr.frDelta>=monsters[i].aniPr.dMove) {
					monsters[i].aniPr.frDelta-=monsters[i].aniPr.dMove;	
					if (monsters[i].aniPr.curFrame!=monsters[i].aniPr.frMove-1)
						monsters[i].aniPr.curFrame++;
					else monsters[i].aniPr.curFrame=0;
				}
				monsters[i].aniPr.frDelta+=delta;
			}
		}

		int tmp;

		//moving monsters according to their type
		switch (monsters[i].Type) {
		case SMALL:
			if (monsters[i].bDead)
				monsters[i].curModel=anims.sDead[monsters[i].aniPr.curFrame];
			else monsters[i].curModel=anims.sMove[monsters[i].aniPr.curFrame];
			
			if (monsters[i].bStand) {
				monsters[i].iDir=GetRandomDir(monsters[i].iDir,monsters[i].pX,monsters[i].pY,pLvl);
			}
			break;
		case MEDIUM:
			if (!monsters[i].bDead)
				monsters[i].curModel=anims.mMove[monsters[i].aniPr.curFrame];
			else monsters[i].curModel=anims.mDead;

			if (monsters[i].nDir<0) {
				if (monsters[i].tX==pLvl->pman.x && monsters[i].tY>pLvl->pman.y) 
					monsters[i].nDir=0;
				if (monsters[i].tX==pLvl->pman.x && monsters[i].tY<pLvl->pman.y) 
					monsters[i].nDir=2;
				if (monsters[i].tX<pLvl->pman.x && monsters[i].tY==pLvl->pman.y) 
					monsters[i].nDir=1;
				if (monsters[i].tX>pLvl->pman.x && monsters[i].tY==pLvl->pman.y) 
					monsters[i].nDir=3;
			}

			if (monsters[i].bStand) {
				tmp=LookFor(monsters[i].pX,monsters[i].pY,pLvl);
				if (tmp!=-1) {
					monsters[i].iDir=tmp;
					monsters[i].tX=pLvl->pman.x;
					monsters[i].tY=pLvl->pman.y;
					monsters[i].bSight=true;
				}
				else if (!monsters[i].bSight) {
						monsters[i].iDir=GetRandomDir(monsters[i].iDir,monsters[i].pX,monsters[i].pY,pLvl);
				}
				else {
					if (monsters[i].tX==monsters[i].pX && monsters[i].tY==monsters[i].pY) {
						monsters[i].iDir=monsters[i].nDir;
						monsters[i].nDir=-1;
						monsters[i].bSight=false;
					}
				}
			}
			break;
		case BIG:
			if (monsters[i].bDead)
				monsters[i].curModel=anims.bDead[monsters[i].aniPr.curFrame];
			else monsters[i].curModel=anims.bMove[monsters[i].aniPr.curFrame];

			if (monsters[i].bStand) {
				if (PathLen!=-1) {
					if (pLvl->pman.x==monsters[i].tX &&
						pLvl->pman.y==monsters[i].tY)
						monsters[i].iDir=Path[PathLen--];
					else {
						FindPath(monsters[i].pX,monsters[i].pY,pLvl);
						if (PathLen==-1) 
							break;
						monsters[iTotal].tX=pLvl->pman.x;
						monsters[iTotal].tY=pLvl->pman.y;
						monsters[i].iDir=Path[PathLen--];
					}
				}
				else {
					monsters[i].iDir=-1;
					FindPath(monsters[i].pX,monsters[i].pY,pLvl);
					if (PathLen==-1) 
						break;
					monsters[iTotal].tX=pLvl->pman.x;
					monsters[iTotal].tY=pLvl->pman.y;
					monsters[i].iDir=Path[PathLen--];
				}
				
			}
			break;
		}

		//передвижение монстров в выбранном направлении
		if (monsters[i].bStand && monsters[i].iDir>=0) {
			//start moving
			if (CanMove(monsters[i].iDir,i,pLvl)) {			//проверка препятствия
				monsters[i].bStand=false;
				monsters[i].mTime=monsters[i].Speed;
			}
		}
		else if (!monsters[i].bStand && monsters[i].iDir>=0) {
			//continue moving
			float fD=0;
			//Если прошло больше времени чем осталось в mTime,
			//то значит нужно передвинуть персонаж на последний отрезок;
			if (delta>monsters[i].mTime) {
				fD=(float)(10.f/monsters[i].Speed)*monsters[i].mTime;
				monsters[i].mTime=0;
				monsters[i].bStand=true;
			}
			else {
				fD=(float)(10.f/monsters[i].Speed)*delta;
				monsters[i].mTime-=delta;
			}
			
			switch (monsters[i].iDir) {
			case 0:
				monsters[i].vPos.y+=fD;
				if (monsters[i].bStand==true) monsters[i].pY--;
				break;
			case 1:
				monsters[i].vPos.x-=fD;
				if (monsters[i].bStand==true) monsters[i].pX++;
				break;
			case 2:
				monsters[i].vPos.y-=fD;
				if (monsters[i].bStand==true) monsters[i].pY++;
				break;
			case 3:
				monsters[i].vPos.x+=fD;
				if (monsters[i].bStand==true) monsters[i].pX--;
				break;
			}

		}
				
	
		//calc positions and direction matrices
		D3DXMATRIX	mTmp;
		int r;
		r=monsters[i].iDir*90;
		D3DXMatrixTranslation(&mTmp,monsters[i].vPos.x,monsters[i].vPos.y,monsters[i].vPos.z);
		D3DXMatrixRotationZ(&monsters[i].mOrg,D3DXToRadian(r));
		D3DXMatrixMultiply(&monsters[i].mOrg,&monsters[i].mOrg,&mTmp);
	}
}

void monster::DrawMonsters(Cmodel* pMdl,LPDIRECT3DDEVICE8 pDevice)
{
	//delete this func later
}

void monster::Release(Cmodel* pMdl)
{
	for(int i=0;i<18;i++) {
		pMdl->Release(anims.sMove[i]);
		pMdl->Release(anims.mMove[i]);
		pMdl->Release(anims.sDead[i]);
	}
	for(i=0;i<6;i++) {
		pMdl->Release(anims.bMove[i]);
		pMdl->Release(anims.bDead[i]);
	}
	pMdl->Release(anims.mDead);
}

void monster::Clear()
{
	ZeroMemory(monsters,sizeof(MONSTER)*12);
	iTotal=0;
}

//Добавление монстра
void monster::AddMonster(int Type,int pX,int pY,level* pLvl)
{
	monsters[iTotal].Type=Type;
	monsters[iTotal].aniPr.curFrame=0;
	monsters[iTotal].aniPr.frDelta=0;
	monsters[iTotal].pX=pX;
	monsters[iTotal].pY=pY;
	monsters[iTotal].bDead=false;
	monsters[iTotal].Speed=600;
	switch (Type) {
	case SMALL: //слабый идет куда глаза глядят 
				//и на перекрестках случайно выбираем направление
		monsters[iTotal].curModel=anims.sMove[0];
		monsters[iTotal].iDir=GetRandomDir(-1,pX,pY,pLvl);
		monsters[iTotal].aniPr.frMove=18;
		monsters[iTotal].aniPr.frDead=18;
		break;
	case MEDIUM://средний смотрит по сторонам и ищет пакмана,
				//если не находит то идет в случайном направлении
		monsters[iTotal].curModel=anims.mMove[0];
		monsters[iTotal].iDir=LookFor(pX,pY,pLvl);
		if (monsters[iTotal].iDir==-1)
			monsters[iTotal].iDir=GetRandomDir(-1,pX,pY,pLvl);
		monsters[iTotal].aniPr.frMove=18;
		monsters[iTotal].aniPr.frDead=1;
		break;
	case BIG:
		monsters[iTotal].curModel=anims.bMove[0];
		//find path to pacman
		monsters[iTotal].aniPr.frMove=6;
		monsters[iTotal].aniPr.frDead=6;
		break;
	}
	monsters[iTotal].aniPr.dMove=(DWORD)monsters[iTotal].Speed/monsters[iTotal].aniPr.frMove;
	monsters[iTotal].aniPr.dDead=(DWORD)monsters[iTotal].Speed/monsters[iTotal].aniPr.frDead;
	if (monsters[iTotal].iDir!=-1) monsters[iTotal].aniPr.bAnim=true;
	monsters[iTotal].bStand=true;
	monsters[iTotal].bSight=false;

	lvProp.stX=(float)(pLvl->lvlMap.lenX/2.0)*TLSIZE;
	lvProp.stY=(float)(pLvl->lvlMap.lenY/2.0)*TLSIZE;

	monsters[iTotal].vPos.x=lvProp.stX-monsters[iTotal].pX*TLSIZE-HTSIZE,
	monsters[iTotal].vPos.y=lvProp.stY-monsters[iTotal].pY*TLSIZE-HTSIZE,
	monsters[iTotal].vPos.z=0.f;

	iTotal++;
}

//Выбор случайного направления
int monster::GetRandomDir(int iDir,int x,int y,level* pLvl)
{
	int tmp[4];
	int inDir=(iDir+2)%4;
	char cDirs=0;

	if (pLvl->lvlMap.Tiles[x][y-1].objType<1 || pLvl->lvlMap.Tiles[x][y-1].objType>6) {
		tmp[cDirs]=0;
		if (tmp[cDirs]!=inDir || iDir==-1) cDirs++;
	}
	if (pLvl->lvlMap.Tiles[x+1][y].objType<1 || pLvl->lvlMap.Tiles[x+1][y].objType>6) {
		tmp[cDirs]=1;
		if (tmp[cDirs]!=inDir || iDir==-1) cDirs++;
	}
	if (pLvl->lvlMap.Tiles[x][y+1].objType<1 || pLvl->lvlMap.Tiles[x][y+1].objType>6) {
		tmp[cDirs]=2;
		if (tmp[cDirs]!=inDir || iDir==-1) cDirs++;
	}
	if (pLvl->lvlMap.Tiles[x-1][y].objType<1 || pLvl->lvlMap.Tiles[x-1][y].objType>6) {
		tmp[cDirs]=3;
		if (tmp[cDirs]!=inDir || iDir==-1) cDirs++;
	}
	if (cDirs==0) return -1;
	else return tmp[rand()%cDirs];
}

//просмотр в четырех направлениях в поисках пакмана
int monster::LookFor(int x,int y,level* pLvl)
{
	int lX,lY,i;
	for(i=0;i<4;i++) {
		lX=x;lY=y;
		while (pLvl->lvlMap.Tiles[lX][lY].objType<1 || pLvl->lvlMap.Tiles[lX][lY].objType>6) {
			if (pLvl->pman.x==lX && pLvl->pman.y==lY) return i;
			switch (i) {
			case 0:
				lY--;
				break;
			case 1:
				lX++;
				break;
			case 2:
				lY++;
				break;
			case 3:
				lX--;
				break;
			} //switch
		}//while
	}//for
	return -1; //pacman not in monster sight
}

//Заполнение локальной карты уровня стенками и монстрами
//-1 --> стена
//-2 --> монстр
void monster::FillMap(level* pLvl)
{
	int x,y;

	for(x=0;x<pLvl->lvlMap.lenX;x++)
		for(y=0;y<pLvl->lvlMap.lenY;y++) 
			if (pLvl->lvlMap.Tiles[x][y].objType>0 && pLvl->lvlMap.Tiles[x][y].objType<7)
				Map[x][y]=-1;
			else Map[x][y]=0;
	for (x=0;x<iTotal;x++) 
		if (pLvl->pman.x!=monsters[x].pX && pLvl->pman.y!=monsters[x].pY) 
			Map[monsters[x].pX][monsters[x].pY]=-2;
}

void monster::PutNumbers(char N,int x,int y)
{
	if (Map[x][y-1]==0) Map[x][y-1]=N;
	if (Map[x][y+1]==0) Map[x][y+1]=N;
	if (Map[x-1][y]==0) Map[x-1][y]=N;
	if (Map[x+1][y]==0) Map[x+1][y]=N;

//	if (Map[x][y-1]==-2) Map[x][y-1]=N+10;
//	if (Map[x][y+1]==-2) Map[x][y+1]=N+10;
//	if (Map[x-1][y]==-2) Map[x-1][y]=N+10;
//	if (Map[x+1][y]==-2) Map[x+1][y]=N+10;

}
//нахождение пути к Пакману из (x,y)
//волновой метод
void monster::FindPath(int x,int y,level* pLvl)
{
	char Step,tSteps;
	int lx,ly;
	bool bPass=false;

	FillMap(pLvl);
	ZeroMemory(Path,MAXPATH);
	//Первый проход, расставляем точки
	Map[x][y]=1;
	PutNumbers(2,x,y);
	for(Step=2;Step<128;Step++) 
	{
		for(lx=0;lx<pLvl->lvlMap.lenX;lx++)
			for(ly=0;ly<pLvl->lvlMap.lenY;ly++) {
				if (Map[pLvl->pman.x][pLvl->pman.y]>0) {
					bPass=true;
					tSteps=Step;
					break;
				}
				if (Map[lx][ly]==Step) PutNumbers(Step+1,lx,ly);
			}
		if (bPass) break;
	}

	PathLen=-1;
	if (!bPass) return;
	lx=pLvl->pman.x;
	ly=pLvl->pman.y;
	Step=tSteps+1;
	tSteps=0;

	//Второй проход, записываем путь
	while (Step>1) {
		if (Map[lx][ly-1]<Step && Map[lx][ly-1]>0) {
			Path[tSteps]=2;
			Step=Map[lx][ly-1];
			ly--;
			tSteps++;
			continue;
		}
		if (Map[lx][ly+1]<Step && Map[lx][ly+1]>0) {
			Path[tSteps]=0;
			Step=Map[lx][ly+1];
			ly++;
			tSteps++;
			continue;
		}
		if (Map[lx-1][ly]<Step && Map[lx-1][ly]>0) {
			Path[tSteps]=1;
			Step=Map[lx-1][ly];
			lx--;
			tSteps++;
			continue;
		}
		if (Map[lx+1][ly]<Step && Map[lx+1][ly]>0) {
			Path[tSteps]=3;
			Step=Map[lx+1][ly];
			lx++;
			tSteps++;
			continue;
		}
	}
	PathLen=--tSteps;
}

bool monster::CanMove(int iDir,int mon,level* pLvl)
{
	switch (iDir) {
	case 0:
		if ((monsters[mon].pY-1<0) ||
			(pLvl->lvlMap.Tiles[monsters[mon].pX][monsters[mon].pY-1].objType>0) &&
			(pLvl->lvlMap.Tiles[monsters[mon].pX][monsters[mon].pY-1].objType<7)) return false;
		else return true;
		break;
	case 1:
		if ((monsters[mon].pX+1>pLvl->lvlMap.lenX-1) ||
			(pLvl->lvlMap.Tiles[monsters[mon].pX+1][monsters[mon].pY].objType>0) &&
			(pLvl->lvlMap.Tiles[monsters[mon].pX+1][monsters[mon].pY].objType<7)) return false;
		else return true;
		break;
	case 2:
		if ((monsters[mon].pY+1>pLvl->lvlMap.lenY-1) ||
			(pLvl->lvlMap.Tiles[monsters[mon].pX][monsters[mon].pY+1].objType>0) &&
			(pLvl->lvlMap.Tiles[monsters[mon].pX][monsters[mon].pY+1].objType<7)) return false;
		else return true;
		break;
	case 3:
		if ((monsters[mon].pX-1<0) ||
			(pLvl->lvlMap.Tiles[monsters[mon].pX-1][monsters[mon].pY].objType>0) &&
			(pLvl->lvlMap.Tiles[monsters[mon].pX-1][monsters[mon].pY].objType<7)) return false;
		else return true;
		break;
	default: return false;
	}
}
