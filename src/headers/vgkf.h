#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include "onnpu.h"

#ifdef _WIN32
#pragma comment(lib, "winmm.lib")
#endif
#pragma warning(suppress : 5208)

class vgkf
{
public:

	typedef struct {
		int16_t* data;
		uint32_t samples;
	} sound;

	typedef struct
	{
		char                       RIFF[4]; //RIFF in ascii
		uint32_t                   ChunkSize;
		char                       VGKF[4]; //VGK in ascci
	}RIFF_header;

	typedef struct
	{
		char                       fmt[4]; // fmt in ascii
		uint32_t                   Subchunk1Size;
		uint16_t                   AudioFormat;  // wav audio specification
		uint16_t                   NumOfChan;
		uint32_t                   SamplesPerSec;
		uint32_t                   bytesPerSec;
		uint16_t                   blockAlign;
		uint16_t                   bitsPerSample;


	}fmt_chunk;

	typedef struct
	{
		char                       Subchunk2ID[4]; //wave in ascii
		uint32_t                   Subchunk2Size;

		sound s;
	}audio_chunk;

	typedef struct
	{
		char                       Subchunk3ID[4]; //note in ascii
		uint32_t                   Subchunk3Size;
		std::vector<onnpu>         Notes;


	}note_chunk;

	RIFF_header riff_h;
	fmt_chunk fmt_c;
	audio_chunk audio_c;
	note_chunk note_c;


	vgkf(const char* audio_filename,std::vector<onnpu> Notes);

	bool write(const char* filename);

	vgkf(const char* filename);

	void play();

	~vgkf();


};
