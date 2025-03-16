#include "image.h" //needs to be defined before stb_lib
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_write.h"

//--------------------------------------------------------------------------------------------------------------
Image::Image(const char* filename){ //constructor
	read(filename); //hope stb lib constructs
	size = width * height * channels; //determine size of data (array)

} 


Image::Image(const Image& other) { //copy constructor
	size = other.size; //copy attributes
	height = other.height;
	width = other.width;
	channels = other.channels;

	data = new uint8_t[size];

	for (size_t i = 0; i < size; ++i) { //copy data
		data[i] = other.data[i];
	}

}


//--------------------------------------------------------------------------------------------------------------
Image Image::crypt_it(const char* text) {
	size_t length = strlen(text); //length of text in bytes
	if ((length*8) >= size) { //if text is bigger than size of image
		std::cerr << "message is too damn long" << std::endl;
		return *this;
	}

	int data_itr = 0;//ensure that last bit is always placed in next index of data

	for (int itr = 0; itr < length; itr++) {
		uint8_t character = (uint8_t)text[itr]; //take char of text as number
		
		for (int i = 0; i < 8; i++) { //to manipulate all bits of each character
			int last_bit = character & 1; //extract last bit by masking

			data[data_itr] &= 0xFE; //clean last bit of pixel
			data[data_itr] |=  last_bit; //add last bit from text to pixel
			character = character >> 1; //moves bits of char by 1 to right
			data_itr++; //ensure that last bit is always placed in next index of data
		}

	}

	return *this;
}


//--------------------------------------------------------------------------------------------------------------
Image Image::decrypt_it(std::vector<char>& buffer) {
	int bit_position = 0;
	uint8_t current_char = 0;

	for (int itr = 0; itr < size; itr++) { //loop through each pixel of image
		int last_bit = data[itr] & 1; //extract last bit of pixel by masking

		current_char |= (last_bit << bit_position); //append extracted bit to the current character
		
		bit_position++; //increment bit count

		//if a character is fully decoded (all 8 bits), add it to the buffer
		if (bit_position == 8) {
			buffer.push_back(current_char); //add decoded character to the buffer
			current_char = 0;               //clear current character and bit_position
			bit_position = 0;
		}
	}

	return *this;
}


//--------------------------------------------------------------------------------------------------------------
Image Image::decrypt_it(std::vector<char>& buffer, const size_t& text_length) {
	int bit_position = 0;
	uint8_t current_char = 0;

	for (int itr = 0; itr < (text_length * 8); itr++) { //loop through each pixel of image
		int last_bit = data[itr] & 1; //extract last bit of pixel by masking

		current_char |= (last_bit << bit_position); //append extracted bit to the current character

		bit_position++; //increment bit count

		//if a character is fully decoded (all 8 bits), add it to the buffer
		if (bit_position == 8) {
			buffer.push_back(current_char); // add decoded character to the buffer
			current_char = 0;               //clear current character and bit_position
			bit_position = 0;
		}
	}


	return *this;
}


//--------------------------------------------------------------------------------------------------------------
bool Image::read(const char* filename) {
	data = stbi_load(filename, &width, &height, &channels, 0); //fills the data array

	//determine if loading was success
	if (data == NULL) {
		return 1;
	}
	else return 0;
}


//--------------------------------------------------------------------------------------------------------------
bool Image::write(const char* filename){ //arguments is name of file that is to be written in
	Extension image_type = getExtension(filename); //determine extenssion

	int foo;
	switch (image_type) {
		case png:
			foo = stbi_write_png(filename, width, height, channels, data, width * channels);
			break;
		case jpg:
			foo = stbi_write_jpg(filename, width, height, channels, data, 100);
			break;
		case bmp:
			foo = stbi_write_bmp(filename, width, height, channels, data);
			break;

	}

	return 0;
}


//--------------------------------------------------------------------------------------------------------------
Extension Image::getExtension(const char* filename) {
	const char* ext = strrchr(filename, '.'); //strrchr returns pointer to last occurance of ., which should follow


	//if extension is found return type, if there is error with finding extension return png by default
	if (ext != nullptr) {
		if (strcmp(ext, ".png") == 0) { //strcmp compares extension to string
			return png;
		}
		else if (strcmp(ext, ".jpg") == 0) {
			return jpg;
		}
		else if (strcmp(ext, ".bmp") == 0) {
			return bmp;
		}
	}

    return png;
};
