#include "image.h"
#include <limits>  // For std::numeric_limits


int main(int argc, char* argv[]) {
    std::string str_text;
    std::vector<char> buffer; //here the decrypted text will be stored

    std::cout << "name of file [extension included][for example: image.png]" << std::endl;
    std::string filename_str;
    std::cin >> filename_str;

    Image image(filename_str.c_str()); //i know what a fine variable name


    std::cout << "what would thou want to do? [input number]" << std::endl
              << "1.Crypt text to image\n"
              << "2.Decrypt text from image\n"
              << "3.Change format\n"
              << "4.Exit"
              << std::endl;

    int input; //input for the choice above
    std::cin >> input;
    std::cout<< input;

    switch(input){
        //--------------------------------------------------------------------------------------------------------------
    case 1:
        std::cout << "input your text, milord: ";
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); //clear buffer before using getline()
        std::getline(std::cin, str_text);
        std::cout << str_text;

        image.crypt_it(str_text.c_str()); //crypt text to image
        image.write(filename_str.c_str()); //save the changed image
        break;

        //--------------------------------------------------------------------------------------------------------------
    case 2:
        std::cout << "Thou must indicate desired number of characters to be decrypted\n"
                  << "1. Unlimited [Warning, be the image too large the text might get lost in terminal]\n"
                  << "2. Limited"
                  << std::endl;

        std::cin >> input; //input for the choice above

        if (input == 1) { //decrypt entire image
            image.decrypt_it(buffer);
        }
        else if (input == 2) { //decrypt only part of image specified by size_t length
            std::cout << "specify number of characters to decrypt: ";
            size_t length;
            std::cin >> length;
            image.decrypt_it(buffer, length);
        }

        for (auto symbol : buffer) { //write out decrypted text
            std::cout << symbol;
        }

        break;

        //--------------------------------------------------------------------------------------------------------------
    case 3:
        image.write(filename_str.c_str());
        break;
        //--------------------------------------------------------------------------------------------------------------
    case 4:
        break;
        //--------------------------------------------------------------------------------------------------------------
    default:
        break;
        //--------------------------------------------------------------------------------------------------------------
    }



    std::cout << "\nPress any key and hit enter to end...";
    std::cin.clear(); // Clear any error flags
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear the input buffer
    std::cin.get(); // Wait for a single character input

    return 0;
}
