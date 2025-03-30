#include "image.h" //needs to be defined before stb_lib
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_write.h"

//--------------------------------------------------------------------------------------------------------------
bool Image::read_chunks(const std::string& filename){
    // Open the file in binary mode
    std::ifstream file(filename, std::ios::binary);

    if (!file) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return false;
    }

    char signature[8];
    file.read(signature, 8);

    // Check if valid PNG signature
    const char PNG_SIGNATURE[8] = {'\x89', 'P', 'N', 'G', '\r', '\n', '\x1A', '\n'}; // \x89 is actually valid char syntax indicating ascii value 137... this exact sequence is always present
    bool isValidPNG = (memcmp(signature, PNG_SIGNATURE, 8) == 0);

    //flag user, but proceed
    if (!isValidPNG) {
        std::cerr<<"these are not the data you were looking for"<<std::endl;
    }


    if(!IHDR_chunk(file)){ //read IHDR chunk
        return false;
    }


}


bool Image::IHDR_chunk(std::ifstream &file){
    //store chunks here before processing
    char chunk_bytes[4]; //note reuse for entire IHDR chunk

    //read chunk length
    file.read(chunk_bytes, 4);

    //read chunk type
    file.read(chunk_bytes, 4);
    if (strcmp(chunk_bytes, "IHDR") != 0) {
        std::cerr<<"this is not IHDR chunk"<<std::endl;
        return false;
    }


    //4 bytes of width
    file.read(chunk_bytes, 4);
    //lowest dimension_bytes byte is moved 24 positons up... for example 00000000 00000000 00000000 FF is moved to FF 00000000 00000000 00000000 + add same action for second byte and so on
    width = (chunk_bytes[0] << 24) | (chunk_bytes[1] << 16) | (chunk_bytes[2] << 8) | chunk_bytes[3]; //set width

    //4 bytes of height
    file.read(chunk_bytes, 4);
    height = (chunk_bytes[0] << 24) | (chunk_bytes[1] << 16) | (chunk_bytes[2] << 8) | chunk_bytes[3]; //set width


    //1 byte of color precision, 8-bit is required
    file.read(chunk_bytes, 1);
    if (chunk_bytes[0] != 8) { //check if read bit depth has 8 bit value... better not allocate variable
        std::cerr << "Warning: Only 8-bit PNGs depth is supported" << std::endl;
        // Either return false here or continue with limited support
    }


    //1 byte of channels
    file.read(chunk_bytes, 1);
    uint8_t color_type = chunk_bytes[0];
    switch(color_type){ //byte does not represent number of channels directly
    case 0:
        channels = 1;
        break;
    case 2:
        channels = 3;
        break;
    case 3:
        channels = 1;
        break;
    case 4:
        channels = 2;
        break;
    case 6:
        channels = 4;
        break;
    }


    // 1 byte of compression method
    file.read(chunk_bytes, 1);
    uint8_t compression_method = chunk_bytes[0];
    if (compression_method != 0) {
        std::cerr << "Unsupported compression method: " << (int)compression_method << std::endl;
        return false; // Only method 0 (DEFLATE) is valid in PNG spec
    }


    // 1 byte of filter method
    file.read(chunk_bytes, 1);
    filter = chunk_bytes[0];
    if (filter != 0) {
        std::cerr << "Unsupported filter method: " << (int)filter<< std::endl;
        return false; // Only method 0 is valid in PNG spec
    }

    // 1 byte of interlace method
    file.read(chunk_bytes, 1);
    if (chunk_bytes[0] > 0) {
        std::cerr << "Unsupported interlace method, we support only none"<< std::endl;
        return false; // Only 0 (none) and 1 (Adam7) exist
    }


    //crc
    uint32_t expected_crc;
    file.read(reinterpret_cast<char*>(&expected_crc), 4);
    expected_crc = (expected_crc >> 24) | ((expected_crc >> 8) & 0xFF00) |
                   ((expected_crc << 8) & 0xFF0000) | (expected_crc << 24); // Convert from big-endian

    // Calculate CRC
    uint32_t calculated_crc = calculate_png_crc("IHDR", ihdr_data, ihdr_length);

    if (calculated_crc != expected_crc) {
        std::cerr << "CRC check failed for IHDR chunk" << std::endl;
        // Handle error
    }

}



//--------------------------------------------------------------------------------------------------------------
// CRC-32 implementation for PNG files
uint32_t Image::crc32(const unsigned char* data, size_t length) {
    static uint32_t crc_table[256];
    static bool table_computed = false;

    // Generate CRC table on first call
    if (!table_computed) {
        for (uint32_t i = 0; i < 256; i++) {
            uint32_t c = i;
            for (int j = 0; j < 8; j++) {
                c = (c & 1) ? (0xEDB88320 ^ (c >> 1)) : (c >> 1);
            }
            crc_table[i] = c;
        }
        table_computed = true;
    }

    uint32_t crc = 0xFFFFFFFF;

    for (size_t i = 0; i < length; i++) {
        crc = crc_table[(crc ^ data[i]) & 0xFF] ^ (crc >> 8);
    }

    return crc ^ 0xFFFFFFFF;
}

// Helper to calculate CRC for PNG chunks
uint32_t Image::calculate_png_crc(const char* chunk_type, const unsigned char* chunk_data, size_t data_length) {
    // Allocate temporary buffer for type + data
    std::vector<unsigned char> buffer(4 + data_length);

    // Copy chunk type (4 bytes)
    memcpy(buffer.data(), chunk_type, 4);

    // Copy chunk data (if any)
    if (data_length > 0) {
        memcpy(buffer.data() + 4, chunk_data, data_length);
    }

    // Calculate CRC
    return crc32(buffer.data(), buffer.size());
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



