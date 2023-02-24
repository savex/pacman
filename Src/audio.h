// audio.h: interface for the audio class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AUDIO_H__5482C1DE_8D3C_42A7_B10C_89DB07D0709F__INCLUDED_)
#define AFX_AUDIO_H__5482C1DE_8D3C_42A7_B10C_89DB07D0709F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <dmusicc.h>
#include <dmusici.h>
#include "wavFile.h"

class audio  
{
// Attribute
protected:
	LPDIRECTSOUND8 pDS;

	LPDIRECTSOUNDBUFFER			pBuffer;			// Sound buffer
	LPDIRECTSOUNDBUFFER			pBuffer2;			// Sound buffer
	LPDIRECTSOUND3DBUFFER		p3DBuffer;			// 3D buffer
	DSBUFFERDESC				sbDesc;				//формат буфера

	IDirectMusicLoader8*		pMLoader;
	IDirectMusicPerformance8*	pMPerf;
	IDirectMusicSegment8*		pMSeg;
	IDirectMusicSegmentState8*	pMSegState;

	CWaveFile wFile;								//класс для загрузки файлов

	bool bMusicPlaying;
	DWORD MusDelta;
public:
	audio();
	virtual ~audio();

	bool Init(HWND hWnd);
	bool LoadFiles();
	bool PlayMusic();
	bool IsPlaying();
	bool StopMusic();
	bool PlaySound(int num);
	bool StopSound();

	void Update(DWORD delta);

	void Release();
};

#endif // !defined(AFX_AUDIO_H__5482C1DE_8D3C_42A7_B10C_89DB07D0709F__INCLUDED_)
