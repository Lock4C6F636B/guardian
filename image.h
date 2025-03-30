#pragma once
#include <fstream>
#include <iostream>
#include <crc.hpp>
#include <vector>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>


enum Extension{ //enum to figure the type
	png,
	jpg,
	bmp
};



class Image { //class for Image
private:
    uint8_t* data;//positive int in range 0 to 8bits, basically pointer to start of data array
	size_t size = 0; //size of data (array)
    uint32_t height, width;
    uint8_t channels, filter;

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
    bool read_chunks(const std::string& filename);

    bool IHDR_chunk(std::ifstream &file);


    uint32_t crc32(const unsigned char* data, size_t length);
    uint32_t calculate_png_crc(const char* chunk_type, const unsigned char* chunk_data, size_t data_length);
};
