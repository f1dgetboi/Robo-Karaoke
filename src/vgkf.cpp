#include "vgkf.h"
#include <thread>     // for sleep
#include <cstring>    // for memset
#include <iostream>
#include <fstream>

vgkf::vgkf(const char* audio_filename, std::vector<onnpu> Notes) {
    FILE* file = fopen(audio_filename, "rb");

    if (!file) {
        std::cerr << "Failed to open file\n";
        return;
    }

    fread(&riff_h.RIFF, 1, 4, file);
    fread(&riff_h.ChunkSize, 4, 1, file);
    fread(riff_h.VGKF, 1, 4, file);
    fread(&fmt_c.fmt, 1, 4, file);
    fread(&fmt_c.Subchunk1Size, 4, 1, file);
    fread(&fmt_c.AudioFormat, 2, 1, file);
    fread(&fmt_c.NumOfChan, 2, 1, file);
    fread(&fmt_c.SamplesPerSec, 4, 1, file);
    fread(&fmt_c.bytesPerSec, 4, 1, file);
    fread(&fmt_c.blockAlign, 2, 1, file);
    fread(&fmt_c.bitsPerSample, 2, 1, file);
    fread(&audio_c.Subchunk2ID, 1, 4, file);
    fread(&audio_c.Subchunk2Size, 4, 1, file);

    audio_c.s.data = new int16_t[audio_c.Subchunk2Size / sizeof(int16_t)];

    if (!audio_c.s.data) {
        fclose(file);
        return;
    }

    fread(audio_c.s.data, 1, audio_c.Subchunk2Size, file);
    fclose(file);

    audio_c.s.samples = fmt_c.bitsPerSample == 16
        ? audio_c.Subchunk2Size / sizeof(int16_t)
        : audio_c.Subchunk2Size / sizeof(int32_t);

    note_c.Notes = Notes;

    note_c.Subchunk3Size = 0;
    for (const onnpu& note : note_c.Notes) {
        note_c.Subchunk3Size += sizeof(note.start_time)
                              + sizeof(note.duration)
                              + sizeof(note.string_size)
                              + note.text.size()
                              + sizeof(note.note);
    }
}

bool vgkf::write(const char* filename) {
    std::ofstream fout(filename, std::ios::binary);
    if (!fout) {
        std::cerr << "Error opening file for writing\n";
        return false;
    }

    fout.write("RIFF", 4);
    fout.write(reinterpret_cast<const char*>(&riff_h.ChunkSize), 4);
    fout.write("VGKF", 4);
    fout.write("fmt",  4);
    fout.write(reinterpret_cast<const char*>(&fmt_c.Subchunk1Size), 4);
    fout.write(reinterpret_cast<const char*>(&fmt_c.AudioFormat), 2);
    fout.write(reinterpret_cast<const char*>(&fmt_c.NumOfChan), 2);
    fout.write(reinterpret_cast<const char*>(&fmt_c.SamplesPerSec), 4);
    fout.write(reinterpret_cast<const char*>(&fmt_c.bytesPerSec), 4);
    fout.write(reinterpret_cast<const char*>(&fmt_c.blockAlign), 2);
    fout.write(reinterpret_cast<const char*>(&fmt_c.bitsPerSample), 2);
    fout.write("wave", 4);
    fout.write(reinterpret_cast<const char*>(&audio_c.Subchunk2Size), 4);
    fout.write(reinterpret_cast<char*>(audio_c.s.data), audio_c.Subchunk2Size);
    fout.write("note", 4);
    fout.write(reinterpret_cast<char*>(&note_c.Subchunk3Size), 4);

    for (const onnpu& note : note_c.Notes) {
        fout.write(reinterpret_cast<const char*>(&note.start_time), sizeof(note.start_time));
        fout.write(reinterpret_cast<const char*>(&note.duration), sizeof(note.duration));
        fout.write(reinterpret_cast<const char*>(&note.string_size), sizeof(note.string_size));
        fout.write(note.text.c_str(), note.text.size());
        fout.write(reinterpret_cast<const char*>(&note.note), sizeof(note.note));
    }

    fout.close();
    return true;
}

vgkf::vgkf(const char* filename) {
    FILE* file = fopen(filename, "rb");

    if (!file) {
        std::cerr << "Failed to open file\n";
        return;
    }

    fread(&riff_h.RIFF, 1, 4, file);
    fread(&riff_h.ChunkSize, 4, 1, file);
    fread(riff_h.VGKF, 1, 4, file);
    fread(&fmt_c.fmt, 1, 4, file);
    fread(&fmt_c.Subchunk1Size, 4, 1, file);
    fread(&fmt_c.AudioFormat, 2, 1, file);
    fread(&fmt_c.NumOfChan, 2, 1, file);
    fread(&fmt_c.SamplesPerSec, 4, 1, file);
    fread(&fmt_c.bytesPerSec, 4, 1, file);
    fread(&fmt_c.blockAlign, 2, 1, file);
    fread(&fmt_c.bitsPerSample, 2, 1, file);
    fread(&audio_c.Subchunk2ID, 1, 4, file);
    fread(&audio_c.Subchunk2Size, 4, 1, file);

    audio_c.s.data = new int16_t[audio_c.Subchunk2Size / sizeof(int16_t)];
    std::memset(audio_c.s.data, 0, audio_c.Subchunk2Size);

    fread(audio_c.s.data, 1, audio_c.Subchunk2Size, file);

    audio_c.s.samples = fmt_c.bitsPerSample == 16
        ? audio_c.Subchunk2Size / sizeof(int16_t)
        : audio_c.Subchunk2Size / sizeof(int32_t);

    fread(note_c.Subchunk3ID, 1, 4, file);
    fread(&note_c.Subchunk3Size, 4, 1, file);

    uint32_t chunk3_read_memory = 0;

    while (chunk3_read_memory < note_c.Subchunk3Size) {
        float start_time = 0;
        float duration = 0;
        uint8_t string_size = 0;
        char* text = nullptr;
        uint8_t note = 0;

        fread(&start_time, sizeof(start_time), 1, file);
        fread(&duration, sizeof(duration), 1, file);
        fread(&string_size, sizeof(string_size), 1, file);

        text = new char[string_size];
        fread(text, string_size - 1, 1, file);
        fread(&note, sizeof(note), 1, file);

        note_c.Notes.emplace_back(start_time, duration, std::string(text), note);

        chunk3_read_memory += sizeof(start_time) + sizeof(duration) + sizeof(string_size) + string_size + sizeof(note);
        delete[] text;
    }

    fclose(file);
}

void vgkf::play() {
    // Stubbed out — no macOS equivalent for waveOutOpen
    std::cout << "Audio playback not implemented on macOS. Consider using PortAudio or OpenAL.\n";
}

vgkf::~vgkf() {
    delete[] audio_c.s.data;
}


//std::vector<onnpu> Notes;
//Notes.push_back(onnpu(5.2f, 10.0f, "test note", 2));

//vgkf test("testfiles/orchestra.wav",Notes);
//vgkf test("testfile.vgkf");
//test.play();
//test.write("testfiles/testfile.vgkf");
