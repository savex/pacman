// audio.cpp: implementation of the audio class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <windows.h>
#include <objbase.h>
#include <dxerr8.h>
#include "audio.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

audio::audio()
{
	// Reset pointers
	pBuffer = NULL;
	p3DBuffer = NULL;
	pMLoader = NULL;
	pMPerf = NULL;
	pMSeg = NULL;

	bMusicPlaying=false;
	MusDelta=0;
}

audio::~audio()
{
	CoUninitialize(); 
}

bool audio::Init(HWND hWnd)
{			  
    HRESULT hr;

    CoInitialize(NULL);

	// Создаем DirectSound и DirectMusic
	hr = CoCreateInstance(CLSID_DirectSound8,
									NULL, 
									CLSCTX_INPROC_SERVER,
									IID_IDirectSound8,
									(void**)&pDS); 
	if SUCCEEDED(hr)
		hr = pDS->Initialize(NULL);
	else DXTRACE_ERR("pDS",hr);
 	
	hr = pDS->SetCooperativeLevel(hWnd, DSSCL_PRIORITY);

    
	CoCreateInstance( CLSID_DirectMusicLoader, NULL, CLSCTX_INPROC, 
                      IID_IDirectMusicLoader8, (void**)&pMLoader );

    CoCreateInstance( CLSID_DirectMusicPerformance, NULL, CLSCTX_INPROC, 
                      IID_IDirectMusicPerformance8, (void**)&pMPerf );

    pMPerf->InitAudio( NULL, NULL, NULL, 
                       DMUS_APATH_DYNAMIC_STEREO, 64,
                       DMUS_AUDIOF_ALL, NULL );

    CHAR strPath[MAX_PATH]=".\\Data";

    // Tell DirectMusic where the default search path is
    WCHAR wstrSearchPath[MAX_PATH];
    MultiByteToWideChar( CP_ACP, 0, strPath, -1, 
                         wstrSearchPath, MAX_PATH );

	pMLoader->SetSearchDirectory( GUID_DirectMusicAllTypes, 
	                               wstrSearchPath, FALSE );

    WCHAR wstrFileName[MAX_PATH] = L"track1.wav";   
    if( FAILED( hr = pMLoader->LoadObjectFromFile( CLSID_DirectMusicSegment,
                                               IID_IDirectMusicSegment8,
                                               wstrFileName,
                                               (LPVOID*) &pMSeg ) ) )
    {
        return false;
    }

 	
	return true;

}


bool audio::PlayMusic()
{
   // Download the segment's instruments to the synthesizer
    pMSeg->Download(pMPerf);

    // Play segment on the default audio path
    pMPerf->PlaySegmentEx( pMSeg, NULL, NULL, 0, 
                                   0, NULL, NULL, NULL );

	bMusicPlaying=true;
	return true;
}

bool audio::IsPlaying()
{
	return (bMusicPlaying);
}

//Загрущить файлы и определить формат главного буфера
bool audio::LoadFiles()
{
	if (FAILED(wFile.Open(".\\Data\\file1.wav", NULL, WAVEFILE_READ)))
	{
		wFile.Close();
	}

	
	memset(&sbDesc, 0, sizeof(DSBUFFERDESC)); 
	sbDesc.dwSize = sizeof(DSBUFFERDESC); 
	sbDesc.dwFlags = 0; 
	
	// The wave format and size of the data chunk are stored in
	// CWaveFile after CWaveFile::Open has been called. 
	
	sbDesc.dwBufferBytes = wFile.GetSize(); 
	sbDesc.lpwfxFormat = wFile.m_pwfx; 
	
	// lpds is a valid IDirectSound8 pointer.
	
	if FAILED(pDS->CreateSoundBuffer(&sbDesc, &pBuffer, NULL))
	{
		return false;
	}

	LPVOID lpvAudio1;
	DWORD  dwBytes1;
	
	if FAILED(pBuffer->Lock(
        0,              // Offset of lock start.
        0,              // Size of lock; ignored in this case.
        &lpvAudio1,     // Address of lock start.
        &dwBytes1,      // Number of bytes locked.
        NULL,           // Wraparound start; not used.
        NULL,           // Wraparound size; not used.
        DSBLOCK_ENTIREBUFFER))  
	{
		return false;
	}
	
		
	DWORD dwBytesRead;
	wFile.Read((BYTE*)lpvAudio1, dwBytes1, &dwBytesRead);
	
	pBuffer->Unlock(lpvAudio1, dwBytes1, NULL, 0);
	wFile.Close();

	if (FAILED(wFile.Open(".\\Data\\file2.wav", NULL, WAVEFILE_READ)))
	{
		wFile.Close();
	}

	
	memset(&sbDesc, 0, sizeof(DSBUFFERDESC)); 
	sbDesc.dwSize = sizeof(DSBUFFERDESC); 
	sbDesc.dwFlags = 0; 
	
	// The wave format and size of the data chunk are stored in
	// CWaveFile after CWaveFile::Open has been called. 
	
	sbDesc.dwBufferBytes = wFile.GetSize(); 
	sbDesc.lpwfxFormat = wFile.m_pwfx; 
	
	// lpds is a valid IDirectSound8 pointer.
	
	if FAILED(pDS->CreateSoundBuffer(&sbDesc, &pBuffer2, NULL))
	{
		return false;
	}

	if FAILED(pBuffer2->Lock(
        0,              // Offset of lock start.
        0,              // Size of lock; ignored in this case.
        &lpvAudio1,     // Address of lock start.
        &dwBytes1,      // Number of bytes locked.
        NULL,           // Wraparound start; not used.
        NULL,           // Wraparound size; not used.
        DSBLOCK_ENTIREBUFFER))  
	{
		return false;
	}
	
		
	wFile.Read((BYTE*)lpvAudio1, dwBytes1, &dwBytesRead);
	
	pBuffer2->Unlock(lpvAudio1, dwBytes1, NULL, 0);
	wFile.Close();
	

	return true;
}

//Проиграть звук из определенного буфера
bool audio::PlaySound(int num)
{
	LPDWORD Stat=NULL;

	if (num==0) {
		pBuffer->Stop();
		pBuffer->SetCurrentPosition(0);
		pBuffer->Play(0,0,0);
	}
	else {
		pBuffer2->Stop();
		pBuffer2->SetCurrentPosition(0);
		pBuffer2->Play(0,0,0);
	}
	return true;
}

bool audio::StopSound()
{
	pBuffer->Stop();
	return true;
}

//Остановить музыку
bool audio::StopMusic()
{
    // Stop the music, and close down 
    pMPerf->Stop( NULL, NULL, 0, 0 );
	bMusicPlaying=false;
	MusDelta=0;
	return true;
}

void audio::Update(DWORD delta)
{
	MusDelta+=delta;
	if (bMusicPlaying && MusDelta>1000)
		if (pMPerf->IsPlaying(pMSeg,NULL)==S_FALSE) 
		{
			bMusicPlaying=false;
			MusDelta=0;
		}

}

//Выгрузить ВСЕ!
void audio::Release()
{
     pMPerf->CloseDown();

	 // Cleanup all interfaces
	if (pDS) pDS->Release();
	if (pMLoader) pMLoader->Release(); 
    if (pMPerf) pMPerf->Release();
    if (pMSeg) pMSeg->Release();
}