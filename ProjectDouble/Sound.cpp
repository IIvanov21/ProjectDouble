#include "SoundClass.h"

SoundClass::SoundClass()
{
	m_DirectSound = 0;
	m_primaryBuffer = 0;
	m_ambientBuffer = 0;
	m_footStepsBuffer = 0;
	m_underWaterBuffer = 0;
}

bool SoundClass::Initialize(HWND hwnd)
{
	bool result;
	//Initialize the DirectSound API and  the primary buffer.
	//Now the LoadWaveFile function can be called which will load in the.wav audio file
	//and Innitialize the secondary buffer with the audio information from the.wav file.
	//After loading is complete then PlayWaveFile is called which then plays the.wav file once.

		// Initialize direct sound and the primary sound buffer.
		result = InitializeDirectSound(hwnd);
	if (!result)
	{
		return false;
	}

	// Load a wave audio file onto a secondary buffer.
	result = LoadSoundFile((char*)gAmbientSound, &m_ambientBuffer);
	if (!result)
	{
		return false;
	}
	result = LoadSoundFile((char*)gFootSteps, &m_footStepsBuffer);
	if (!result)
	{
		return false;
	}
	result = LoadSoundFile((char*)gUnderWater, &m_underWaterBuffer);
	if (!result)
	{
		return false;
	}
	// Play the wave file now that it has been loaded.
	PlayAmbientSoundFile();
	

	return true;
}

void SoundClass::Shutdown()
{
	// Release the secondary buffer.
	ShutdownSoundFile(&m_ambientBuffer);
	ShutdownSoundFile(&m_footStepsBuffer);
	ShutdownSoundFile(&m_underWaterBuffer);
	// Shutdown the Direct Sound API.
	ShutdownDirectSound();

	return;
}

bool SoundClass::InitializeDirectSound(HWND hwnd)
{
	HRESULT result;
	DSBUFFERDESC bufferDesc;
	WAVEFORMATEX waveFormat;


	// Initialize the direct sound interface pointer for the default sound device.
	result = DirectSoundCreate8(NULL, &m_DirectSound, NULL);
	if (FAILED(result))
	{
		return false;
	}

	// Set the cooperative level to priority so the format of the primary sound buffer can be modified.
	result = m_DirectSound->SetCooperativeLevel(hwnd, DSSCL_PRIORITY);
	if (FAILED(result))
	{
		return false;
	}
	//We have to setup the description of how we want to access the primary buffer.The dwFlags are the important part of this structure.
	//In this case we just want to setup a primary buffer description with the capability of adjusting its volume.
	//There are other capabilities you can grab but we are keeping it simple for now.
	// Setup the primary buffer description.
	bufferDesc.dwSize = sizeof(DSBUFFERDESC);
	bufferDesc.dwFlags = DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRLVOLUME;
	bufferDesc.dwBufferBytes = 0;
	bufferDesc.dwReserved = 0;
	bufferDesc.lpwfxFormat = NULL;
	bufferDesc.guid3DAlgorithm = GUID_NULL;

	// Get control of the primary sound buffer on the default sound device.
	result = m_DirectSound->CreateSoundBuffer(&bufferDesc, &m_primaryBuffer, NULL);
	if (FAILED(result))
	{
		return false;
	}

	// Setup the format of the primary sound bufffer.
	waveFormat.wFormatTag = WAVE_FORMAT_PCM;
	waveFormat.nSamplesPerSec = gSamplesPS;			
	waveFormat.wBitsPerSample = gBitsPS;			
	waveFormat.nChannels = gChannels;				
	waveFormat.nBlockAlign = (waveFormat.wBitsPerSample / 8) * waveFormat.nChannels;
	waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
	waveFormat.cbSize = 0;

	// Set the primary buffer to be the wave format specified.
	result = m_primaryBuffer->SetFormat(&waveFormat);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

void SoundClass::ShutdownDirectSound()
{
	// Release the primary sound buffer pointer.
	if (m_primaryBuffer)
	{
		m_primaryBuffer->Release();
		m_primaryBuffer = 0;
	}

	// Release the direct sound interface pointer.
	if (m_DirectSound)
	{
		m_DirectSound->Release();
		m_DirectSound = 0;
	}

	return;
}

bool SoundClass::LoadSoundFile(char* fileName, IDirectSoundBuffer8** secondaryBuffer)
{
	int error;
	FILE* filePtr;
	unsigned int count;
	WaveHeaderType waveFileHeader;
	WAVEFORMATEX waveFormat;
	DSBUFFERDESC bufferDesc;
	HRESULT result;
	IDirectSoundBuffer* tempBuffer;
	unsigned char* waveData;
	unsigned char* bufferPtr;
	unsigned long bufferSize;

	//First step open the file and read in the header file which contains all the information about the audio files to be able to create a secondary buffer
	//All the checks below are performed to check if the file isn't corrupted
	//Open the sound file in binary
	error = fopen_s(&filePtr, fileName, "rb");
	//Read the file header in
	count = fread(&waveFileHeader, sizeof(waveFileHeader), 1, filePtr);
	if (count != 1)return false;
	//Check tha the chunk Id is ther RIFF format
	if ((waveFileHeader.chunkId[0] != 'R') || (waveFileHeader.chunkId[1] != 'I') ||
		(waveFileHeader.chunkId[2] != 'F') || (waveFileHeader.chunkId[3] != 'F'))return false;
	//Check if file format is WAVE format
	if ((waveFileHeader.format[0] != 'W') || (waveFileHeader.format[1] != 'A') ||
		(waveFileHeader.format[2] != 'V') || (waveFileHeader.format[3] != 'E'))return false;
	//Check if the sub chunk ID is the fmt format
	if((waveFileHeader.subChunkId[0]!='f')||(waveFileHeader.subChunkId[1]!='m')||
		(waveFileHeader.subChunkId[2] != 't') || (waveFileHeader.subChunkId[3] != ' '))return false;
	//Check if the audio format is WAVE_FORMAT_PCM
	if (waveFileHeader.audioFormat != WAVE_FORMAT_PCM)return false;
	//Chec that the wave file was record in stero format
	if (waveFileHeader.numChannels < gChannels)return false;
	//Check that the wave file was recorded at a sample rate of 48KHz
	if (waveFileHeader.sampleRate < gSamplesPS)return false;
	//Check if the data chunk header
	if ((waveFileHeader.dataChunkId[0] != 'd') || (waveFileHeader.dataChunkId[1] != 'a') ||
		(waveFileHeader.dataChunkId[2] != 't') || (waveFileHeader.dataChunkId[3] != 'a'))return false;
	//After verification is done for the file we can setup the secondary buffer that we will load audio data onto
	//Set wave format and buffer description
	//Wave description
	waveFormat.wFormatTag = WAVE_FORMAT_PCM;
	waveFormat.nSamplesPerSec = gSamplesPS;
	waveFormat.wBitsPerSample = gBitsPS;
	waveFormat.nChannels = gChannels;
	waveFormat.nBlockAlign = (waveFormat.wBitsPerSample / 8) * waveFormat.nChannels;
	waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;

	//Set the buffer description
	bufferDesc.dwSize = sizeof(DSBUFFERDESC);
	bufferDesc.dwFlags = DSBCAPS_CTRLVOLUME;
	bufferDesc.dwBufferBytes = waveFileHeader.dataSize;
	bufferDesc.dwReserved = 0;
	bufferDesc.lpwfxFormat = &waveFormat;
	bufferDesc.guid3DAlgorithm = GUID_NULL;
	// Create a temporary sound buffer with the specific buffer settings.
	//This is performed in case secondary buffer is not created successfully
	result = m_DirectSound->CreateSoundBuffer(&bufferDesc, &tempBuffer, NULL);
	if (FAILED(result))
	{
		return false;
	}

	// Test the buffer format against the direct sound 8 interface and create the secondary buffer.
	result = tempBuffer->QueryInterface(IID_IDirectSoundBuffer8, (void**)&*secondaryBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Release the temporary buffer.
	tempBuffer->Release();
	tempBuffer = 0;

	//After creating the secondary buffer successfully we are ready to load the wave file data to the memory buffer for checks if it needs modifications
	// Move to the beginning of the wave data which starts at the end of the data chunk header.
	fseek(filePtr, sizeof(WaveHeaderType), SEEK_SET);

	// Create a temporary buffer to hold the wave file data.
	waveData = new unsigned char[waveFileHeader.dataSize];
	if (!waveData)
	{
		return false;
	}

	// Read in the wave file data into the newly created buffer.
	count = fread(waveData, 1, waveFileHeader.dataSize, filePtr);
	if (count != waveFileHeader.dataSize)
	{
		return false;
	}

	// Close the file once done reading.
	error = fclose(filePtr);
	if (error != 0)
	{
		return false;
	}

	// Lock the secondary buffer to write wave data into it.
	result = (*secondaryBuffer)->Lock(0, waveFileHeader.dataSize, (void**)&bufferPtr, (DWORD*)&bufferSize, NULL, 0, 0);
	if (FAILED(result))
	{
		return false;
	}

	// Copy the wave data into the buffer.
	memcpy(bufferPtr, waveData, waveFileHeader.dataSize);

	// Unlock the secondary buffer after the data has been written to it.
	result = (*secondaryBuffer)->Unlock((void*)bufferPtr, bufferSize, NULL, 0);
	if (FAILED(result))
	{
		return false;
	}

	// Release the wave data since it was copied into the secondary buffer.
	delete[] waveData;
	waveData = 0;

	return true;
}

//Release second buffer
void SoundClass::ShutdownSoundFile(IDirectSoundBuffer8** secondaryBuffer)
{
	// Release the secondary sound buffer.
	if (*secondaryBuffer)
	{
		(*secondaryBuffer)->Release();
		*secondaryBuffer = 0;
	}

	return;
}
bool SoundClass::PlayAmbientSoundFile()
{
	HRESULT result;
	if (gAmbientSoundState == SoundState::Loop)
	{
		// Set position at the beginning of the sound buffer.
		result = m_ambientBuffer->SetCurrentPosition(0);
		if (FAILED(result))
		{
			return false;
		}

		// Set volume of the buffer to 100%.
		result = m_ambientBuffer->SetVolume(DSBVOLUME_MAX);
		if (FAILED(result))
		{
			return false;
		}

		// Play the contents of the secondary sound buffer.
		result = m_ambientBuffer->Play(0, 0, DSBPLAY_LOOPING);
		if (FAILED(result))
		{
			return false;
		}
	}
	return true;
}

bool SoundClass::PlayFootStepsFile(SoundState state)
{
	HRESULT result;
	gCurrentSoundState = state;
	if (gCurrentSoundState==SoundState::Stop) 
	{
		 result=m_footStepsBuffer->Stop();
		if (FAILED(result))
		{
			return false;
		}
	}
	else if (gCurrentSoundState == SoundState::Play)
	{
		 result = m_footStepsBuffer->Play(0,0, DSBPLAY_LOOPING);
		if (FAILED(result))
		{
			return false;
		}
		// Set volume of the buffer to 100%.
		result = m_footStepsBuffer->SetVolume(DSBVOLUME_MAX);
		if (FAILED(result))
		{
			return false;
		}
	}
	
	
	return true;
}

bool SoundClass::PlayUnderWaterFile(SoundState state)
{
	HRESULT result;
	gCurrentSoundState = state;
	if (state == SoundState::Stop)
	{
		result = m_underWaterBuffer->Stop();
		if (FAILED(result))
		{
			return false;
		}
	}
	else if (state == SoundState::Play)
	{
		result = m_underWaterBuffer->Play(0, 0, DSBPLAY_LOOPING);
		if (FAILED(result))
		{
			return false;
		}
		// Set volume of the buffer to 100%.
		result = m_underWaterBuffer->SetVolume(DSBVOLUME_MAX);
		if (FAILED(result))
		{
			return false;
		}
	}


	return true;
}
