// vsprite.cpp: implementation of the vsprite class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "vsprite.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

vsprite::vsprite()
{

	vert[0].x = -1.f;vert[0].y =  1.f;vert[0].z= 0.f;
	vert[1].x =  1.f;vert[1].y =  1.f;vert[1].z= 0.f;
	vert[2].x = -1.f;vert[2].y = -1.f;vert[2].z= 0.f;
	vert[3].x =  1.f;vert[3].y = -1.f;vert[3].z= 0.f;

	vert[0].Color= 0xffffffff;
	vert[1].Color= 0xffffffff;
	vert[2].Color= 0xffffffff;
	vert[3].Color= 0xffffffff;
	
	vert[0].tu = 0.f;
	vert[0].tv = 1.f;

	vert[1].tu = 1.f;
	vert[1].tv = 1.f;
	
	vert[2].tu = 0.f;
	vert[2].tv = 0.f;
	
	vert[3].tu = 1.f;
	vert[3].tv = 0.f;


}

vsprite::~vsprite()
{

}


bool vsprite::Init(IDirect3DDevice8 * pDevice,char* fn,DWORD height,DWORD width)
{
	for(int i=0;i<4;i++) {
		vert[i].x*=height;
		vert[i].y*=width;
	}

	D3DXCreateTextureFromFile(pDevice,".\\data\\smoke.jpg",&pTex);

	
	return true;
}


void vsprite::Update(DWORD delta)
{

}


void vsprite::Render(IDirect3DDevice8 * pDevice)
{
	pDevice->SetRenderState(D3DRS_LIGHTING,FALSE);

	// Give the particles a glowing effect
	pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);
	pDevice->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_ONE);
	pDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_ONE);

	pDevice->SetTexture(0,pTex);
	pDevice->SetVertexShader( D3DFVF_MYVERT );
	pDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,2, vert,sizeof(VERTEX));
}


void vsprite::Release()
{
	pTex->Release();
}
