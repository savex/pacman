// level.cpp: implementation of the level class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "level.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


level::level()
{
	bLoaded=false;
	bFname=false;
	bReflect=true;
	bIncAngle=false;
	sprintf(filename,"noname");
	lvlMap.lenX=0;
	lvlMap.lenY=0;
	lvlMap.stPX=1;
	lvlMap.stPY=1;
	for(int x=0;x<64;x++)
		for(int y=0;y<64;y++)
		{
			lvlMap.Tiles[x][y].objType=0x0A;
			lvlMap.Tiles[x][y].tubeRot=0;
		}
	pSpr=NULL;
	Scale=0;
	Selected=0;
	Angle=0;
	highlight=-1;
	mSelX=mSelY=-1;

	for(x=0;x<MAXPS;x++) Part.pPS[x]=NULL;
	Part.iNumPS=0;

	for(int i=0;i<16;i++) tObj[i]=NULL;

	Monst=NULL;
}


level::~level()
{

}

//Загрузка обьектов
bool level::LoadObjects(Cmodel* pMdl,LPDIRECT3DDEVICE8 pDevice)
{
	char stTmp[128];
	for(int i=0;i<16;i++) {
		switch (i) {
		case 0:
			//Нулевой обьект - пустота
			break;
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
			sprintf(stTmp,".\\data\\t%1d",i);
			Obj[i].mdl=pMdl->OpenModel(stTmp,pDevice);
			Obj[i].type=i;
			if (!Obj[i].mdl) return false;
			break;
		case 10: //Вместо всех остальных обьектов пока точки
		case 12:
		case 13:
		case 14:
		case 15:
			sprintf(stTmp,".\\data\\point");
			Obj[i].mdl=pMdl->OpenModel(stTmp,pDevice);
			Obj[i].type=i;
			if (!Obj[i].mdl) return false;
			break;
		case 9: //7,8 - monsters
			Monst=new monster();
			if (!Monst->LoadMonsters(pMdl,pDevice)) return false;
			break;
		case 11:	//Particle systems as power points
			for(int j=0;j<MAXPS;j++) {
				Part.pPS[j]=new psystem();
				Part.pPS[j]->Init(pDevice,".\\data\\smoke.bmp");
				Part.pPS[j]->tAttr.bStop=true;
			}
/*			sprintf(stTmp,".\\data\\power");
			Obj[i].mdl=pMdl->OpenModel(stTmp,pDevice);
			Obj[i].type=i;
			if (!Obj[i].mdl) return false;
*/			break;
		}

		sprintf(stTmp,".\\data\\obj%04d.bmp",i);
		D3DXCreateTextureFromFile(pDevice,stTmp,&tObj[i]);
	}

	sprintf(stTmp,".\\data\\plane");
	Plane.mdl=pMdl->OpenModel(stTmp,pDevice);
	Plane.type=0;
	if (!Plane.mdl) return false;

	D3DXCreateSprite(pDevice,&pSpr);

	return true;
}

//Обновить уровень
void level::UpdateLevel(DWORD delta)
{
	for(int i=0;i<Part.iNumPS;i++) Part.pPS[i]->Update(delta);

	Monst->UpdateMonsters(delta,this);
	
	float hX,hY;
	hX=5.f;
	hY=5.f;

	vUL=D3DXVECTOR3(hX,hY,-5.0f);
	vUR=D3DXVECTOR3(-hX,hY,-5.0f);
	vDL=D3DXVECTOR3(hX,-hY,-5.0f);
	vDR=D3DXVECTOR3(-hX,-hY,-5.0f);

	D3DXVec3TransformCoord(&vUL,&vUL,&mScale);
	D3DXVec3TransformCoord(&vUR,&vUR,&mScale);
	D3DXVec3TransformCoord(&vDL,&vDL,&mScale);
	D3DXVec3TransformCoord(&vDR,&vDR,&mScale);
	
	D3DXPlaneFromPoints(&RefPlane,&vUL,&vUR,&vDL);
	D3DXMatrixReflect(&mReflect,&RefPlane);
}

//Главная функция вывода
void level::Draw(Cmodel* pMdl,D3DXVECTOR3 vEyePt,LPDIRECT3DDEVICE8 pDevice)
{
	DrawReflect(pMdl,vEyePt,pDevice);
	DrawLevel(pMdl,pDevice);
}

//Рисование отражения при помощи clipping planes
void level::DrawReflect(Cmodel* pMdl,D3DXVECTOR3 vEyePt,LPDIRECT3DDEVICE8 pDevice)
{
	D3DXMATRIX mWorld;
	D3DXVECTOR3 lvEye;

	if (bReflect) {
		pDevice->GetTransform(D3DTS_WORLD,&mWorld);
		pDevice->SetTransform(D3DTS_WORLD,&mReflect);
		pDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CW );
		
		pDevice->SetClipPlane(0,*D3DXPlaneFromPoints(&RefPlane,&vUR,&vUL,&vEyePt));
		pDevice->SetClipPlane(1,*D3DXPlaneFromPoints(&RefPlane,&vUL,&vDL,&vEyePt));
		pDevice->SetClipPlane(2,*D3DXPlaneFromPoints(&RefPlane,&vDL,&vDR,&vEyePt));
		pDevice->SetClipPlane(3,*D3DXPlaneFromPoints(&RefPlane,&vDR,&vUR,&vEyePt));
		pDevice->SetRenderState( D3DRS_CLIPPLANEENABLE,
			D3DCLIPPLANE0 | D3DCLIPPLANE1 | D3DCLIPPLANE2 | D3DCLIPPLANE3 );
		//Draw scene
		DrawLevel(pMdl,pDevice);
		
		pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
		pDevice->SetRenderState(D3DRS_CLIPPLANEENABLE, 0x00);
		pDevice->SetTransform(D3DTS_WORLD,&mWorld);
		pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,   TRUE);
		pDevice->SetRenderState(D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA);
		pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	}

	//Draw ground with corresponding scale matrix calculated in CalcMatrices
	pMdl->DrawModel(Plane.mdl,&mScale,pDevice);
}

//Вывод обьектов уровня на экран
void level::DrawLevel(Cmodel* pMdl,LPDIRECT3DDEVICE8 pDevice)
{
	int x,y,i;
	D3DXMATRIX mLocal,mWorld;
	//Draw level
	pDevice->GetTransform(D3DTS_WORLD,&mWorld);
	for(x=0;x<lvlMap.lenX;x++)
		for(y=0;y<lvlMap.lenY;y++)
		{
			switch (lvlMap.Tiles[x][y].objType) {
			case 1:
			case 2:
			case 3:
			case 4:
			case 5:
			case 6:
			case 10:
				D3DXMatrixMultiply(&mLocal,&lvlMap.Tiles[x][y].mOrg,&mWorld);
				pMdl->DrawModel(Obj[lvlMap.Tiles[x][y].objType].mdl,
								&mLocal,
								pDevice);
				break;
			case 7:
			case 8:
			case 9:
				break;
			case 11:
				// Give the particles a glowing effect
/*				pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);
				pDevice->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_ONE);
				pDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_ONE);
				D3DXMatrixMultiply(&mLocal,&lvlMap.Tiles[x][y].mOrg,&mWorld);
				for(i=0;i<Part.iNumPS;i++) Part.pPS[i]->Render(&mLocal,pDevice);
				pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);
*/				break;
			case 12:
			case 13:
			case 14:
			case 15:
				;
			}
		}
	//at last we draw psystem
	pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);
	pDevice->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_ONE);
	pDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_ONE);
	for(x=0;x<lvlMap.lenX;x++)
		for(y=0;y<lvlMap.lenY;y++) 
		if (lvlMap.Tiles[x][y].objType==0x0B) {
				// Give the particles a glowing effect
				D3DXMatrixMultiply(&mLocal,&lvlMap.Tiles[x][y].mOrg,&mWorld);
				for(i=0;i<Part.iNumPS;i++) Part.pPS[i]->Render(&mLocal,pDevice);
		}
	pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);

	//Draw monsters
	for(i=0;i<Monst->iTotal;i++)
	{
		D3DXMatrixMultiply(&mLocal,&Monst->monsters[i].mOrg,&mWorld);
		pMdl->DrawModel(Monst->monsters[i].curModel,&mLocal,pDevice);
	}

	D3DXMatrixMultiply(&mLocal,&pman.mOrg,&mWorld);
	pMdl->DrawModel(pman.mdl,&mLocal,pDevice);
	pDevice->SetTransform(D3DTS_WORLD,&mWorld);
}

//Очистка уровня
void level::ClearLvl()
{
	int x,y;

	bLoaded=false;
	sprintf(lvlMap.Name,"Unknown");
	for(x=0;x<lvlMap.lenX;x++)
		for(y=0;y<lvlMap.lenY;y++)
		{
			lvlMap.Tiles[x][y].objType=0;
			lvlMap.Tiles[x][y].tubeRot=0;
		}
	lvlMap.lenX=0;
	lvlMap.lenY=0;
	lvlMap.stPX=0;
	lvlMap.stPY=0;
	Monst->Clear();
}

//Загрузка уровня
bool level::LoadLevel(char *filename)
{
	using namespace std;
	fstream in;
	int x,y;
	unsigned char byte;

	//Загрузка уровня из файла
	char tmpName[128],desc[20],ldesc[20];

	sprintf(desc,"EXEPACMANLVL");
	sprintf(tmpName,".\\data\\%s.lvl",filename);
	in.open(tmpName,ios_base::binary|ios_base::in);
	if (!in.is_open()) return false;
	in.read(ldesc,12);
	ldesc[12]=0;
	if (strcmp(desc,ldesc)!=0) return false;
	
	//Все готово для загрузки уровня Очистим и будем загружать
	ClearLvl();
	in.read(lvlMap.Name,32);
	in.read((char*)&lvlMap.lenX,sizeof(unsigned char));
	in.read((char*)&lvlMap.lenY,sizeof(unsigned char));
	in.read((char*)&lvlMap.stPX,sizeof(unsigned char));
	in.read((char*)&lvlMap.stPY,sizeof(unsigned char));
	for(x=0;x<lvlMap.lenX;x++)
		for(y=0;y<lvlMap.lenY;y++)
		{
			in.read((char*)&byte,sizeof(unsigned char));
			lvlMap.Tiles[x][y].objType=(byte&0xF0)>>4;
			lvlMap.Tiles[x][y].tubeRot=byte&0x0F;
		}
	in.close();
	
	if (!CalcMatrices()) return false;
	bLoaded=true;
	return true;
}

void level::Release(Cmodel* pMdl)
{
	for(int i=0;i<16;i++) {
		pMdl->Release(Obj[i].mdl);
		if (tObj[i]) tObj[i]->Release();
	}
	
	for(i=0;i<MAXPS;i++) {
		if (Part.pPS[i]) Part.pPS[i]->Release();
		delete Part.pPS[i];
	}
	Part.iNumPS=0;

	pMdl->Release(Plane.mdl);
	if (pSpr) pSpr->Release();
	if (Monst) Monst->Release(pMdl);
	delete Monst;
}

bool level::NewLevel(unsigned char ucLX,unsigned char ucLY)
{

	sprintf(lvlMap.Name,"No name for this level!");
	lvlMap.lenX=ucLX;
	lvlMap.lenY=ucLY;
	lvlMap.stPX=1;
	lvlMap.stPY=2;

	float sclX=500/lvlMap.lenX/32.0f;
	float sclY=440/lvlMap.lenY/32.0f;
	Scale=(sclX<sclY)? sclX:sclY;	
	sX=94+(546-lvlMap.lenX*(32*Scale))/2;
	sY=(480-lvlMap.lenY*(32*Scale))/2;

	//границы уровня по умолчанию
	lvlMap.Tiles[0][0].objType=1;
	lvlMap.Tiles[0][0].tubeRot=1;
	lvlMap.Tiles[lvlMap.lenX-1][0].objType=1;
	lvlMap.Tiles[lvlMap.lenX-1][0].tubeRot=2;
	lvlMap.Tiles[lvlMap.lenX-1][lvlMap.lenY-1].objType=1;
	lvlMap.Tiles[lvlMap.lenX-1][lvlMap.lenY-1].tubeRot=3;
	lvlMap.Tiles[0][lvlMap.lenY-1].objType=1;
	lvlMap.Tiles[0][lvlMap.lenY-1].tubeRot=0;
	for(int i=1;i<lvlMap.lenX-1;i++) {
		lvlMap.Tiles[i][0].objType=2;
		lvlMap.Tiles[i][0].tubeRot=1;
		lvlMap.Tiles[i][lvlMap.lenY-1].objType=2;
		lvlMap.Tiles[i][lvlMap.lenY-1].tubeRot=1;
	}
	for(i=1;i<lvlMap.lenY-1;i++) {
		lvlMap.Tiles[0][i].objType=2;
		lvlMap.Tiles[0][i].tubeRot=0;
		lvlMap.Tiles[lvlMap.lenX-1][i].objType=2;
		lvlMap.Tiles[lvlMap.lenX-1][i].tubeRot=0;
	}
	if (!CalcMatrices()) return false;
	bLoaded=true;
	return true;
}

bool level::SaveLevel()
{
	using namespace std;
	fstream out;
	int x,y;
	unsigned char byte;
	if (!bFname) {
		//Запрос на имя файла

	}

	//Запись уровня с упаковкой информации о клетке в один байт
	//т.е. objType		tubeRot
	//		|				|
	//		|--  ------------
	//		|  ||  |
	//		00000000
	/*	
		EXEPACMANLVL
		lvlMap.Name
		lvlMap.lenX
		lvlMap.lenX
		lvlMap.stPX
		lvlMap.stPY
		lvlMap.Tiles[lenX][lenY]
	*/

	char tmpName[128],desc[20];

	sprintf(tmpName,".\\data\\%s.lvl",filename);
	sprintf(desc,"EXEPACMANLVL");
	out.open(tmpName,ios_base::out|ios::binary);
	if (!out.is_open()) return false;

	out.write(desc,12);
	out.write(lvlMap.Name,32);
	out.write((char*)&lvlMap.lenX,sizeof(unsigned char));
	out.write((char*)&lvlMap.lenY,sizeof(unsigned char));
	out.write((char*)&lvlMap.stPX,sizeof(unsigned char));
	out.write((char*)&lvlMap.stPY,sizeof(unsigned char));
	for(x=0;x<lvlMap.lenX;x++)
		for(y=0;y<lvlMap.lenY;y++)
		{
			byte=lvlMap.Tiles[x][y].objType<<4;
			byte=byte|lvlMap.Tiles[x][y].tubeRot;
			out.write((char*)&byte,sizeof(unsigned char));
		}
	out.close();
	return true;
}

bool level::CalcMatrices()
{
	int x,y;	

	if (lvlMap.lenX==0 || lvlMap.lenY==0) return false;

	float sclX=500/lvlMap.lenX/32.0f;
	float sclY=440/lvlMap.lenY/32.0f;
	Scale=(sclX<sclY)? sclX:sclY;	

	//Вычисление матрицы положения и вращения
	float sX,sY;				//верхняя левая точка отсчета карты уровня
	float pX,pY;
	int r;

	if (Monst) Monst->Clear();	//Очистим список монстров
	
	sX=(float)(lvlMap.lenX/2.0)*TLSIZE;
	sY=(float)(lvlMap.lenY/2.0)*TLSIZE;

	pman.x=lvlMap.stPX;
	pman.y=lvlMap.stPY;
	
	for(x=0;x<lvlMap.lenX;x++)
		for(y=0;y<lvlMap.lenY;y++)
		{
			pX=sX-HTLSIZE-x*TLSIZE;
			pY=sY-HTLSIZE-y*TLSIZE;
			r=lvlMap.Tiles[x][y].tubeRot*90;

			D3DXMatrixTranslation(&lvlMap.Tiles[x][y].mPos,pX,pY,0.0f);
			D3DXMatrixRotationZ(&lvlMap.Tiles[x][y].mRot,D3DXToRadian(r));
			D3DXMatrixMultiply(&lvlMap.Tiles[x][y].mOrg,
							   &lvlMap.Tiles[x][y].mRot,
							   &lvlMap.Tiles[x][y].mPos);
			if (lvlMap.Tiles[x][y].objType==0x07)
				Monst->AddMonster(Monst->SMALL,x,y,this);
			if (lvlMap.Tiles[x][y].objType==0x08)
				Monst->AddMonster(Monst->MEDIUM,x,y,this);
			if (lvlMap.Tiles[x][y].objType==0x09)
				Monst->AddMonster(Monst->BIG,x,y,this);
			if (lvlMap.Tiles[x][y].objType==0x0B) {
				Part.pPS[Part.iNumPS]->tAttr.bStop=false;
				Part.iNumPS++;
			}
		}

	if (Monst) Monst->FillMap(this);

	//Вычисление матрицы масштабирования для обьекта plane
	D3DXMATRIX mTmp;
	D3DXMatrixTranslation(&mTmp,0,0,0);
	D3DXMatrixScaling(&mScale,lvlMap.lenX*0.95f,lvlMap.lenY*0.95f,1.0f);
	D3DXMatrixMultiply(&mScale,&mScale,&mTmp);  
	return true;
}
//рисование карты уровня в редакторе
void level::DrawMap(LPDIRECT3DDEVICE8 pDevice)
{
	int x,y;
	float rtc=(32*Scale)/2.0f;
	DWORD col;

	for(x=0;x<lvlMap.lenX;x++)
		for(y=0;y<lvlMap.lenY;y++)
		{
			if ((x==mSelX) && (y==mSelY)) col=0xFFFFFFFF;
				else col=0xD0D0D0D0;

			if ((lvlMap.Tiles[x][y].objType>0) && (lvlMap.Tiles[x][y].objType<7))
				pSpr->Draw(tObj[lvlMap.Tiles[x][y].objType],
						NULL,
						&D3DXVECTOR2(Scale,Scale),
						&D3DXVECTOR2(rtc,rtc),
						D3DXToRadian(0-lvlMap.Tiles[x][y].tubeRot*90),
						&D3DXVECTOR2(sX+x*(32*Scale),sY+y*(32*Scale)),
						col);
			else {
				pSpr->Draw(tObj[lvlMap.Tiles[x][y].objType],
						NULL,
						&D3DXVECTOR2(Scale,Scale),
						NULL,
						0,
						&D3DXVECTOR2(sX+x*(32*Scale),sY+y*(32*Scale)),
						col);
			}
	}
}

void level::DrawObject(LPDIRECT3DDEVICE8 pDevice,char obj)
{
	DWORD col;

	if (obj==Selected) col=0xFFFFFFFF;
	else if (obj==highlight) col=0xD0D0D0D0;
	else col=0xA0A0A0A0;

	if ((obj>=0) && (obj<8))
		pSpr->Draw(tObj[obj],
						NULL,
						&D3DXVECTOR2(1.0f,1.0f),
						NULL,
						0,
						&D3DXVECTOR2(20.f,25.f+obj*(32+5)),
						col);
	else if ((obj>7) && (obj<16))
		pSpr->Draw(tObj[obj],
						NULL,
						&D3DXVECTOR2(1.0f,1.0f),
						NULL,
						0,
						&D3DXVECTOR2(20.f+32+10,25.f+(obj-8)*(32+5)),
						col);

}

void level::SelObj(char obj)
{
	if ((obj>=0) && (obj<=15)) {
		Selected=obj;
		Angle=0;
	}
}

void level::IncAngle()
{
	++Angle%=4;
	SetMapObj();
}

char level::ChkSel(int x,int y)
{
	for(int i=0;i<8;i++)
		if ((y>=35+i*(32+5)) && (y<=35+32+i*(32+5)) &&
			(x>=40) && (x<=40+32)) return i;
	
	for(i=8;i<16;i++)
		if ((y>=35+(i-8)*(32+5)) && (y<=35+32+(i-8)*(32+5)) &&
			(x>=40+32+10) && (x<=40+2*32+10)) return i;
		
	return -1;
}

bool level::ChkMap(int mapX,int mapY)
{
	int x,y;

	for(x=0;x<lvlMap.lenX;x++)
		for(y=0;y<lvlMap.lenY;y++)
		{
			if ((mapX>=sX+15+x*(32*Scale)) && (mapX<=sX+15+(32*Scale)+x*(32*Scale)) &&
				(mapY>=sY+10+y*(32*Scale)) && (mapY<=sY+10+(32*Scale)+y*(32*Scale))) {
				mSelX=x;
				mSelY=y;
				return true;
			}
		}
	mSelX=mSelY=-1;
	return false;
}

void level::SetHL(char obj)
{
	highlight=obj;
}

void level::SetMapObj()
{
	if ((mSelX!=-1) && (mSelY!=-1) && (Selected!=-1)) {
		lvlMap.Tiles[mSelX][mSelY].objType=Selected;
		lvlMap.Tiles[mSelX][mSelY].tubeRot=Angle;
	}
}

int level::GetMaxLen()
{
	return ((lvlMap.lenX>lvlMap.lenY) ? lvlMap.lenX : lvlMap.lenY);
}
