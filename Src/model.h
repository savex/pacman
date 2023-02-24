// model.h: interface for the Cmodel class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MODEL_H__D4E9D204_4AA8_4F78_AFA1_F878C685A6BD__INCLUDED_)
#define AFX_MODEL_H__D4E9D204_4AA8_4F78_AFA1_F878C685A6BD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <D3D8.h>
#include <D3dx8mesh.h>

#include <fstream>
#include <stdio.h>


struct tMaterial
{
	char name[32];
	int type;
	D3DMATERIAL8 material;
	LPDIRECT3DTEXTURE8 texture;

	int num;
	tMaterial *next,*prev;

	tMaterial()
		:texture(NULL),
		type(0),num(0),
		next(NULL),
		prev(NULL)
	{
		ZeroMemory(name,sizeof(name));
		ZeroMemory(&material,sizeof(material));

	}
	~tMaterial()
	{
		if (texture) texture->Release();
		prev->next=next;
		if(next!=0) next->prev=prev;
	}
	void Release()
	{
		if (--num>0) return;
		delete this;
	}
};

struct tModel
{
	char name[32];
	int type;

	DWORD verex_flag;
	LPD3DXMESH mesh;
	D3DXMATRIX transform_matrix;

	int nmaterial;
	tMaterial *material[16];

	tModel *next;
	tModel *child;

////////////
	tModel()
		:verex_flag(0),
		type(0),
		mesh(NULL),
		nmaterial(0),
		next(NULL),
		child(NULL)
	{
		D3DXMatrixIdentity(&transform_matrix);
		ZeroMemory(material,sizeof(material));
	}

	~tModel()
	{
		if (mesh!=0) mesh->Release();
		for(int x=0;x<16;x++)
		{
			if (material[x]!=0) material[x]->Release();
		}
	}
};

struct tModelList
{
	char name[32];

	tModel *model;
	int num;
	tModelList *next,*prev;

/////////////////
	tModelList()
		:model(NULL),
		num(0),
		next(NULL),
		prev(NULL)
	{
		ZeroMemory(name,sizeof(name));
	}
	~tModelList()
	{
		delete [] model;
		prev->next=next;
		if(next!=0) next->prev=prev;
	}
};

class Cmodel  
{
public:
	HRESULT Release(tModel *model);
	Cmodel();
	virtual ~Cmodel();

	tModel* Cmodel::OpenModel(char *name,LPDIRECT3DDEVICE8 m_pd3dDevice);
	tMaterial* OpenMaterial(char* dir,char *name,LPDIRECT3DDEVICE8 m_pd3dDevice);
	HRESULT DrawModel(tModel *model, D3DXMATRIX *matrix, LPDIRECT3DDEVICE8 pDevice);

protected:
	tModelList *ModelList;
	tMaterial *material;
};

#endif // !defined(AFX_MODEL_H__D4E9D204_4AA8_4F78_AFA1_F878C685A6BD__INCLUDED_)
