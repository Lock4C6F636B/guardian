#pragma once
#include <iostream>
#include <cstdint>
#include "stb_image.h"
#include <vector>

enum Extension{ //enum to figure the type
	png,
	jpg,
	bmp
};



class Image { //class for Image
private:
    uint8_t* data;//positive int in range 0 to 8bits, basically pointer to start of data array
	size_t size = 0; //size of data (array)
	int height, width;
	int channels;

public:
//--------------------------------------------------------------------------------------------------------------
	Image(const char* filename); //default constructor, unfortunately cannot be a string because of stb library

	Image(const Image& other); //copy constructor


	~Image() { //need to clear up data manually
		delete[] data;
	};


//--------------------------------------------------------------------------------------------------------------
	Image crypt_it(const char* text);

	Image decrypt_it(std::vector<char>& buffer);

	Image decrypt_it(std::vector<char>& buffer, const size_t& text_length);


//--------------------------------------------------------------------------------------------------------------
	bool read(const char* filename); //load into the Image
	bool write(const char* filename); //write from the Image

	Extension getExtension(const char* filename); //to determine extension
};
