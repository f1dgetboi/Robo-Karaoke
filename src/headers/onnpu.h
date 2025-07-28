#pragma once
#include <string>
#include <iostream>

class onnpu
{
public:

	float start_time; //start time in seconds
	float duration;   //duration in seconds
	uint8_t string_size; //to know how much data to read for the string
	std::string text; //text that is associated with the note
	uint8_t note;  //note

	onnpu(const float& start_time, const float& duration, const std::string& text, const uint8_t& note)
		: text(text), note(note), start_time(start_time), duration(duration) 
	{
		string_size = sizeof(text);
	}

	friend std::ostream& operator<< (std::ostream& out, const onnpu& onnpu)
	{
		// Since operator<< is a friend of the Point class, we can access Point's members directly.
		out << "onnpu(" << onnpu.start_time << ", " << onnpu.duration << ", " << onnpu.text << ", " << onnpu.note << ')'; // actual output done here

		return out; // return std::ostream so we can chain calls to operator<<
	}
};

