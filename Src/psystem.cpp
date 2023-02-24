// psystem.cpp: implementation of the psystem class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "psystem.h"

inline DWORD FtoDW( FLOAT f ) { return *((DWORD*)&f); }

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

psystem::psystem()
{
	tAttr.fSpeed=0.2f;
	tAttr.wTotal=0;
	tAttr.bStop=false;

	tAttr.wRate=1000/13;	//10-11 частиц в кадре
	tAttr.fLife=800;		//0.8 сек

	dwRateDelta=0;
}

psystem::~psystem()
{

}


bool psystem::Init(IDirect3DDevice8 * pDevice,char* fn)
{
	//Создадим текстуру
	D3DXCreateTextureFromFile(pDevice,fn,&pTex);
	ZeroMemory(&tPS, sizeof(_PARTICLE) * NUM_PARTICLES);

	//Создадим вершинный буфер
	pDevice->CreateVertexBuffer( NUM_PARTICLES * sizeof(VERTEX), 
								D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, 
								D3DFVF_MYVERT, D3DPOOL_DEFAULT, 
								&pVB );

	//Заполним вершинный буфер стандартными значениями
	VERTEX* pV;
    if( FAILED(pVB->Lock(0, 0, (BYTE**)&pV, NULL)))
		return false;

	for (int i=0;i<NUM_PARTICLES;i++) {
		pV[i].x=0.f;
		pV[i].y=0.f; 
		pV[i].z=0.f;
		pV[i].Color=0;
	}

	pVB->Unlock();
	return true;
}

void psystem::Update(DWORD delta)
{
	//Генерация частиц, их удаление и дабавление в буфер вывода
	dwRateDelta+=delta;

	if (!tAttr.bStop) {
		for (int i=0;i<(int)(dwRateDelta/tAttr.wRate);i++) {
			SpawnParticle();
			dwRateDelta-=tAttr.wRate;
		}
	}
	
	for(int p=0; p<NUM_PARTICLES; p++)
	{
		if(!tPS[p].alive) continue;
		
		tPS[p].pos   += tPS[p].vel * tAttr.fSpeed;
		tPS[p].vel.y -= 0.002f * tAttr.fSpeed * delta;
		tPS[p].life  -=  (float)delta/tAttr.fLife;
		
		if( tPS[p].life <= 0 ) tPS[p].alive = false;
	}

	VERTEX* pV;
	WORD vi=0;
    if( FAILED(pVB->Lock(0, 0, (BYTE**)&pV, NULL)))
		return;

	for (int i=0;i<NUM_PARTICLES;i++) {
		if (tPS[i].alive==true) {
			pV[vi].x=tPS[i].pos.x;
			pV[vi].y=tPS[i].pos.y;
			pV[vi].z=tPS[i].pos.z;
			pV[vi].Color=D3DXCOLOR(tPS[i].life,
									tPS[i].life*0.2f,
									tPS[i].life*0.2f,
									tPS[i].life);

			vi++;
		}
	}
	tAttr.wTotal=vi;

	pVB->Unlock();
}

void psystem::Render(D3DXMATRIX *matr,IDirect3DDevice8 * pDevice)
{

	// Turn on the particle texture and save the old one
	IDirect3DBaseTexture8* pOldTex;
	pDevice->GetTexture(0, &pOldTex);
	pDevice->SetTexture(0, pTex);

	D3DXMATRIX matr2,oldMatr,mID;
	D3DXMatrixIdentity(&mID);
	D3DXMatrixMultiply(&matr2,matr,&mID);
	pDevice->GetTransform( D3DTS_WORLD, &oldMatr); 
	pDevice->SetTransform( D3DTS_WORLD, matr); 


	// Вывод всех активных частиц за один раз
	if (tAttr.wTotal>0) 
	{
		// Turn off lighting
//		pDevice->SetRenderState(D3DRS_LIGHTING,FALSE);
		
		pDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
		pDevice->SetRenderState( D3DRS_ZENABLE, FALSE );
	    pDevice->SetRenderState( D3DRS_SHADEMODE, D3DSHADE_FLAT );
		pDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
		pDevice->SetRenderState( D3DRS_POINTSPRITEENABLE, TRUE );
		pDevice->SetRenderState( D3DRS_POINTSCALEENABLE,  TRUE );
		pDevice->SetRenderState( D3DRS_POINTSIZE,     FtoDW(2.00f) );
		pDevice->SetRenderState( D3DRS_POINTSIZE_MIN, FtoDW(0.00f) );
		pDevice->SetRenderState( D3DRS_POINTSCALE_A,  FtoDW(0.00f) );
		pDevice->SetRenderState( D3DRS_POINTSCALE_B,  FtoDW(0.00f) );
		pDevice->SetRenderState( D3DRS_POINTSCALE_C,  FtoDW(1.00f) );

		pDevice->SetStreamSource(0,pVB,sizeof(VERTEX));
		pDevice->SetVertexShader(D3DFVF_MYVERT);

		pDevice->DrawPrimitive(D3DPT_POINTLIST,0, tAttr.wTotal);
	}

	pDevice->SetTransform( D3DTS_WORLD, &oldMatr); 
	pDevice->SetTexture(0, pOldTex);
	pDevice->SetRenderState( D3DRS_POINTSPRITEENABLE, FALSE );
	pDevice->SetRenderState( D3DRS_POINTSCALEENABLE,  FALSE );
	pDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
	pDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
    pDevice->SetRenderState( D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
	pDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
	pDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
}

void psystem::Release()
{
	SAFE_RELEASE(pTex);
	SAFE_RELEASE(pVB);
}

//Создание новых частиц на месте умерших
void psystem::SpawnParticle()
{
	for(int p=0; p<NUM_PARTICLES; p++)
	{
		if(tPS[p].alive) continue;
		
		tPS[p].alive = true;
		tPS[p].life = 1;
		tPS[p].pos = D3DXVECTOR3(0.f,0.f,0.f);
		tPS[p].vel = D3DXVECTOR3((randf-0.5f)*0.9f,
								 (randf-0.5f)*0.9f,
								 (randf-0.5f)*0.9f);

		break;
	}
}
