#include <iostream>
#include <string>

#include "md5.h"

int main(){
    std::string message;
    std::getline(std::cin, message);
    std::cout << md5(message) << std::endl;
    return 0;
}