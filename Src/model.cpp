// model.cpp: implementation of the Cmodel class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "model.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Cmodel::Cmodel()
{
	ModelList=new tModelList;
	material=new tMaterial;
}

Cmodel::~Cmodel()
{

}

tModel* Cmodel::OpenModel(char *name,LPDIRECT3DDEVICE8 m_pd3dDevice)
{
	HRESULT hr;
	int tmp[4];
	char *data,nam[32];

	tModelList *mlist;

	mlist=ModelList;

	while(1)
	{
		if(strcmp(name,mlist->name)==0)
		{
			mlist->num++;
			return mlist->model;
		}
		if(mlist->next==0)
		{
			break;
		}
		else mlist=mlist->next;
	}

	sprintf(nam,"%s.mdl",name);
	using namespace std;
	fstream in( nam, ios_base::in | ios_base::binary );
	if(!in.is_open())
	{
		MessageBox(NULL,"Model file not found.","ERROR",MB_ICONERROR|MB_OK);
		return 0;
	}
	mlist->next=new tModelList;
	mlist->next->prev=mlist;
	mlist=mlist->next;
	strcpy(mlist->name,name);
	
	in.read((char*)tmp,16);
	if (tmp[0]!=' LDM'){ in.close();return NULL;}
	data=new char[tmp[1]];
	in.seekg(-16,ios::cur);
	in.read((char*)data,tmp[1]);
	in.close();


	mlist->model=new tModel[((int*)data)[2]];
	int *md;
	mlist->model[0].type=((int*)data)[3];

	for (int obj=0;obj<((int*)data)[2];obj++)
	{
		md=(((int*)(data+obj*0x100))+8);

		//strcpy(hie[obj].dxObject->name,Objects[obj]->name);
		hr=D3DXCreateMeshFVF(md[5],md[2],D3DXMESH_WRITEONLY,
			md[0],m_pd3dDevice,&(mlist->model[obj].mesh));

		BYTE *buf;
		mlist->model[obj].mesh->LockVertexBuffer(D3DLOCK_DISCARD,&buf);
		memcpy(buf,data+md[1],md[2]*md[3]*4);
		mlist->model[obj].mesh->UnlockVertexBuffer();

		mlist->model[obj].mesh->LockIndexBuffer(D3DLOCK_DISCARD,&buf);
		for(int x=0;x<md[5]*3;x++) ((WORD*)buf)[x]=((int*)(data+md[4]))[x];
		//memcpy(buf,data+md[4],md[5]*3*2);
		mlist->model[obj].mesh->UnlockIndexBuffer();

		DWORD *dat;
		mlist->model[obj].mesh->LockAttributeBuffer(D3DLOCK_DISCARD,&dat);
		if(md[24]!=0) memcpy(dat,data+md[24],md[5]*4);
		else for(int x=0;x<md[5];x++) dat[x]=0;
		mlist->model[obj].mesh->UnlockAttributeBuffer();


		mlist->model[obj].nmaterial=md[25];
		for(int m=0;m<mlist->model[obj].nmaterial;m++)
		{
			mlist->model[obj].material[m]=OpenMaterial(".\\data\\",data+md[26]+32*m,m_pd3dDevice);
			if (mlist->model[obj].material[m]==NULL) {
				MessageBox(NULL,"Error opening material.","ERROR",MB_ICONERROR|MB_OK);
				return 0;
			}
		}

		if(md[6]!=(-1)) mlist->model[obj].next=mlist->model+md[6];
		if(md[7]!=(-1)) mlist->model[obj].child=mlist->model+md[7];

		strcpy(mlist->model[obj].name,(char*)&md[28]);
	}
	delete data;

	mlist->num=1;
	return mlist->model;
}

tMaterial* Cmodel::OpenMaterial(char* dir,char *name, LPDIRECT3DDEVICE8 m_pd3dDevice)
{
	char nam[64];
	int dat[0x40];

	tMaterial *mat=material;

	while(1)
	{
		if(strcmp(name,mat->name)==0) return mat;
		if(mat->next==0)
		{
			mat->next=new tMaterial;
			mat->next->prev=mat;
			mat=mat->next;
			break;
		}
		else mat=mat->next;
	}

	sprintf(nam,"%s%s.mat",dir,name);
	using namespace std;
	fstream in(nam, ios_base::in | ios_base::binary );
	if (!in.is_open()) return NULL;
	in.read((char*)dat,0x100);
	in.close();
	sprintf(mat->name,"%s%s",dir,name);
	if(((char*)(dat+4))[0]!=0)
	{
		char ftex[64];
		sprintf(ftex,"%s%s",dir,(char*)(dat+4));
		D3DXCreateTextureFromFileA(m_pd3dDevice,ftex,&mat->texture);
	}
	memcpy(&mat->material,dat+8,0x44);
	mat->type=dat[3];

	return mat;
}

HRESULT Cmodel::Release(tModel *model)
{
	tModelList *mlist;
	mlist=ModelList;

	while(mlist->model!=model)
	{
		if (mlist->next==0) return S_FALSE;
		mlist=mlist->next;
	}

	mlist->num--;
	if(mlist->num==0)
	{
		delete mlist;
	}

	return S_OK;
}

HRESULT Cmodel::DrawModel(tModel *model, D3DXMATRIX *matrix, LPDIRECT3DDEVICE8 pDevice)
{

	HRESULT hr;

	D3DXMATRIX matr2,oldMatr;
	D3DXMatrixMultiply(&matr2,matrix,&model->transform_matrix);
	pDevice->GetTransform( D3DTS_WORLD, &oldMatr); 
	hr=pDevice->SetTransform( D3DTS_WORLD, &matr2 );

	for (int x=0;x<model->nmaterial;x++)
	{
		switch (model->material[x]->type&0xFF)
		{
		case 0:
			hr=pDevice->SetMaterial(&model->material[x]->material);
			hr=pDevice->SetTexture(0,model->material[x]->texture);
			hr=model->mesh->DrawSubset(x);
			break;
		case 1:
			hr=pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,true);
			hr=pDevice->SetRenderState(D3DRS_SRCBLEND , D3DBLEND_ONE);
			hr=pDevice->SetRenderState(D3DRS_DESTBLEND ,D3DBLEND_ONE);
			hr=pDevice->SetMaterial(&model->material[x]->material);
			hr=pDevice->SetTexture(0,model->material[x]->texture);
			hr=model->mesh->DrawSubset(x);
			hr=pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,false);
			break;
		case 2:
			hr=pDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
			hr=pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,true);
			hr=pDevice->SetRenderState(D3DRS_SRCBLEND , D3DBLEND_ONE);
			hr=pDevice->SetRenderState(D3DRS_DESTBLEND ,D3DBLEND_ONE);
			hr=pDevice->SetMaterial(&model->material[x]->material);
			hr=pDevice->SetTexture(0,model->material[x]->texture);
			hr=model->mesh->DrawSubset(x);
			hr=pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,false);
			hr=pDevice->SetRenderState( D3DRS_ZWRITEENABLE, true );
			break;
		}
	}

	if (model->next!=0) DrawModel(model->next,matrix,pDevice);
	if (model->child!=0) DrawModel(model->child,&matr2,pDevice);

	pDevice->SetTransform( D3DTS_WORLD, &oldMatr); 

	return S_OK;
}
