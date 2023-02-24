// pic2spr.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fstream.h>


char	filename[128];
int		digits;

struct SPR_INFO
{
	char	head[14];
	int		rate;
	int		frames;
	bool	loop;
};
struct FRAME
{
	long	StartPos;
	long	Len;
};
struct MEM
{
	unsigned char*	data;
	long	len;
};

SPR_INFO	desc;
FRAME*		pTab;
MEM*		pFiles;

char		str[80];
bool		isN=false;
bool		isR=false;
bool		isF=false;
bool		isL=false;
bool		isName=false;

void getNumber(char* str,char* param)
{

	int pos=strcspn(param,":")+1;
	int len=strlen(param)-pos;
	strncpy(str,param+pos,len+1);
}

int main(int argc, char* argv[])
{
	//Название программы
	printf("pic2spr program v0.1\n\n");
	//Хелп если нехватает параметров
	if (argc<3) {
		printf("Usage: pic2spr <filename> -f:XXX [-n:X] [-r:XX] ");
		printf("[-l:yes/no]\n\n");
		printf("-n:X		number of numeric symbols in filename, e.g. name0000. Default:4\n");
		printf("-r:XX		framerate to play this anim. Default:20\n");
		printf("-f:XXX		number of frames in the anim.\n");
		printf("-l:yes/no	anim loopped or not. Default:yes\n\n");
		return 0;
	}

	//разбор параметров
	for (int i=1;i<argc;i++) {
		for (int j=0;j<3;j++) str[j]=argv[i][j];
		str[3]=0;
		
		//параметр -n:X
		if (!strcmp(str,"-n:")) {
			getNumber(str,argv[i]);
			digits=atoi(str);
			if (digits==0 || digits>9) {
				printf("Number of digits invalid: %s\n\n",argv[i]);
				return 1;
			}
			else isN=true;
			continue;
		}
		//параметр -r:XX
		if (!strcmp(str,"-r:")) {
			getNumber(str,argv[i]);
			desc.rate=atoi(str);
			if (desc.rate==0 || desc.rate>30) {
				printf("Framerate invalid: %s\n\n",argv[i]);
				return 1;
			}
			else isR=true;
			continue;
		}
		//параметр -f:XXX
		if (!strcmp(str,"-f:")) {
			getNumber(str,argv[i]);
			desc.frames=atoi(str);
			if (desc.frames==0 || desc.frames>999) {
				printf("Number of total frames invalid: %s\n\n",argv[i]);
				return 1;
			}
			else isF=true;
			continue;
		}
		//параметр -l:yes/no
		if (!strcmp(str,"-l:")) {
			getNumber(str,argv[i]);
			strlwr(str);
			if (!strcmp(str,"yes")) desc.loop=true;
			else if (!strcmp(str,"no")) desc.loop=false;
			else {
				printf("Loop flag invalid: %s\n\n",argv[i]);
				return 1;
			}
			isL=true;
			continue;
		}

		if (str[0]!='-') {
			strcpy(str,argv[i]);
			if (strstr(str,"-") || strstr(str,":") || strstr(str,".")) {
				printf("Filename or parameter is invalid: %s\n\n",argv[i]);
				return 1;
			}
			else {
				isName=true;
				strcpy(filename,str);
			}
			continue;
		}

	}

	if (!isF) {
		printf("Missing framerate parameter, e.g. -f:XXX\n\n");
		return 1;
	}
	if (!isName) {
		printf("Missing filename...\n\n");
		return 1;
	}

	if (!isN) digits=4;
	if (!isR) desc.rate=20;
	if (!isL) desc.loop=true;

	strcpy(desc.head,"SHOOTERSPRITE");

	//Перепись в формат SPR
	/*	0-12	SHOOTERSPRITE
		13+		SPR_INFO
				Sizes table
				data
	*/

	//Загрузка всех кадров в память
	pTab=new FRAME[desc.frames];
	char	jpg[]=".jpg";
	char	tga[]=".tga";
	char	bmp[]=".bmp";
	char	ext[]=".xxx";

	char	fn[256];
	unsigned int j,k;
	ifstream in;

	{	
		char sdig[20]="000000000000";
		char tmp[20];
		
		pFiles=new MEM[desc.frames];
		
		strcat(sdig,itoa(desc.frames-1,tmp,10));
		for(j=(strlen(sdig)-digits),k=0;j<strlen(sdig);j++,k++) 
			sdig[k]=sdig[j];
		sdig[k]=0;
		strcpy(ext,jpg);
		sprintf(fn,"%s%s%s",filename,sdig,ext);
		in.open(fn,ios::binary|ios::nocreate,filebuf::sh_read);
		if (!in.is_open()) {
			strcpy(ext,tga);
			sprintf(fn,"%s%s%s",filename,sdig,ext);
			in.open(fn,ios::binary|ios::nocreate,filebuf::sh_read);
		}
		if (!in.is_open()) {
			strcpy(ext,bmp);
			sprintf(fn,"%s%s%s",filename,sdig,ext);
			in.open(fn,ios::binary|ios::nocreate,filebuf::sh_read);
		}
		if (!in.is_open()) {
			printf("Failed to open files...");
			return 1;
		}
		in.close();
		sdig[0]=0;
	}
	
	for (i=0;i<desc.frames;i++) {
		char sdig[20]="000000000000";
		char tmp[20];

		strcat(sdig,itoa(i,tmp,10));
		for(j=(strlen(sdig)-digits),k=0;j<strlen(sdig);j++,k++) 
			sdig[k]=sdig[j];
		sdig[k]=0;
		sprintf(fn,"%s%s%s",filename,sdig,ext);

		in.open(fn,ios::binary|ios::nocreate,filebuf::sh_read);
		if (!in.is_open()) {
			printf("\rFailed to open file: %s",fn);
			return 1;
		}
		printf("\rLoading frame: %3d",i+1);
		in.seekg(0,ios::end);
		pFiles[i].len=in.tellg();
		pFiles[i].data=new unsigned char[pFiles[i].len];
		in.seekg(0,ios::beg);
		in.read(pFiles[i].data,pFiles[i].len);
		in.close();
		
		if (i!=0) pTab[i].StartPos=pTab[i-1].StartPos+pFiles[i-1].len;
		else pTab[i].StartPos=sizeof(SPR_INFO)+sizeof(FRAME)*desc.frames;
		pTab[i].Len=pFiles[i].len;
	}

	printf("\nLoading OK!\n");
	//Запись всех кадров на диск
	ofstream out;
	sprintf(fn,"%s.spr",filename);
	out.open(fn,ios::binary,filebuf::sh_none);
	if (!out.is_open()) {
		printf("\rFailed to open output file...");
		return 1;
	}
	printf("\nSaving to file: %s\n",fn);

	out.write((char *)&desc,sizeof(SPR_INFO));
	for (i=0;i<desc.frames;i++) {
		out.write((char *)&pTab[i].StartPos,sizeof(long));
		out.write((char *)&pTab[i].Len,sizeof(long));
	}
	for (i=0;i<desc.frames;i++) {
		out.write(pFiles[i].data,pFiles[i].len);
		printf("\rSaving frame %3d...",i+1);
	}
	out.close();
	printf("\nSaving OK!\n");


	return 0;
}

