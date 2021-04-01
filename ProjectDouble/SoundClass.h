//////////////////////////////////////////////////////////////////////////////
// Filename: soundclass.h
///////////////////////////////////////////////////////////////////////////////
#ifndef _SOUNDCLASS_H_
#define _SOUNDCLASS_H_
//The following librariesand headers are required for DirectSound to compile properly.

/////////////
// LINKING //
/////////////
#pragma comment(lib, "dsound.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "winmm.lib")


//////////////
// INCLUDES //
//////////////
#include <windows.h>
#include <mmsystem.h>
#include <dsound.h>
#include <stdio.h>
#include <string>

///////////////////////////////////////////////////////////////////////////////
// Class name: SoundClass
///////////////////////////////////////////////////////////////////////////////
enum SoundState
{
	Play, Stop, Pause, Loop
};
class SoundClass
{
private:
	//The WaveHeaderType structure used here is for the.wav file format.
	
		struct WaveHeaderType
	{
		char chunkId[4];
		unsigned long chunkSize;
		char format[4];
		char subChunkId[4];
		unsigned long subChunkSize;
		unsigned short audioFormat;
		unsigned short numChannels;
		unsigned long sampleRate;
		unsigned long bytesPerSecond;
		unsigned short blockAlign;
		unsigned short bitsPerSample;
		char dataChunkId[4];
		unsigned long dataSize;
	};

public:

	static const int gSamplesPS = 48000;
	static const int gBitsPS = 16;
	static const int gChannels = 2;
	SoundClass();
	SoundClass(const SoundClass&) {};
	~SoundClass() {};
	//Initializeand Shutdown will handle everything needed for this tutorial.The Initialize function will initialize DirectSoundand load in the.wav audio fileand then play it once.Shutdown will release the.wav fileand shutdown DirectSound.

	bool Initialize(HWND);
	void Shutdown();

	bool PlayAmbientSoundFile();
	bool PlayFootStepsFile(SoundState state);
	bool PlayUnderWaterFile(SoundState state);

private:
	SoundState gCurrentSoundState = SoundState::Stop;
	SoundState gAmbientSoundState = SoundState::Loop;
	const char* gAmbientSound = ".//Media//Sounds//ambient.wav";
	const char* gFootSteps = ".//Media//Sounds//footsteps.wav";
	const char* gUnderWater = ".//Media//Sounds//underwater.wav";

	bool InitializeDirectSound(HWND);
	void ShutdownDirectSound();

	bool LoadSoundFile(char*, IDirectSoundBuffer8**);
	void ShutdownSoundFile(IDirectSoundBuffer8**);


private:
	IDirectSound8* m_DirectSound;
	IDirectSoundBuffer* m_primaryBuffer;
	//Note that I only have one secondary buffer as this tutorial only loads in one sound.

		IDirectSoundBuffer8* m_ambientBuffer;
		IDirectSoundBuffer8* m_footStepsBuffer;
		IDirectSoundBuffer8* m_underWaterBuffer;

};
extern SoundClass* SoundLoader;
#endif