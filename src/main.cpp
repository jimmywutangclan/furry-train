#include <iostream>
#include "SDLGraphicsProgram.hpp"


int main(int argc, char** argv){

	std::cout << "Non-euclidian engine booting up" << std::endl;
    SDLGraphicsProgram program(1280, 720);
    program.Run();
	return 0;
}
