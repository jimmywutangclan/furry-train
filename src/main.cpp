#include <iostream>
#include "SDLGraphicsProgram.hpp"

// The main application loop
void loop(){
}

int main(int argc, char** argv){

	std::cout << "Non-euclidian engine booting up" << std::endl;
    SDLGraphicsProgram program(1280, 720);
    program.SetLoopCallback(loop);
	return 0;
}
