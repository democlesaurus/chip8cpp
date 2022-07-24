#include <SDL2/SDL.h>
#include <iostream>
#include <string>

#include "chip8.hpp"
#include "framework.hpp"
#define WINDOW_HEIGHT 400
#define WINDOW_WIDTH 800

int main(int argc, char * argv[]){


	std::string filename;
	Chip8 c8{};
	std::cout << "Welcome to Chip8 Emulator" << std::endl;
	std::cout << "please input path of rom to load:" << std::endl;
	
	while(1) {
		std::getline(std::cin,filename);
		// only break if we got a valid filename
		// NOTE: infinite loop if we don't get a valid file
		if (c8.loadROM(filename.c_str())) {
			break;
		}

		//we didn't successfully load a rom
		std::cout<< std::endl << "Incorrect Filepath: could not open " << filename << std::endl;
		std::cout<< "please enter valid path to rom:" << std::endl;
	}

	std::cout << "successfully loaded rom: " << filename << std::endl;

    // Creating the object by passing Height and Width value.
    Framework fw(WINDOW_HEIGHT, WINDOW_WIDTH);
	
    SDL_Event event;    // Event variable
	auto renderer = fw.grabRenderer();

    // Below while loop checks if the window has terminated using close in the
    //  corner.
    while(!(event.type == SDL_QUIT)){
        SDL_Delay(20);  // setting some Delay
        SDL_PollEvent(&event);  // Catching the poll event.

		const Uint8* keyboard_state = SDL_GetKeyboardState(NULL);

		// Go ahead and flag appropriate keys if pressed
		// CHIP8   -> SDL_Scancode
		// 1,2,3,c -> 1,2,3,4
		// 4,5,6,d -> q,w,e,r
		// 7,8,9,e -> a,s,d,f
		// a,0,b,f -> z,x,c,v
		if (keyboard_state[SDL_SCANCODE_X])
			c8.registerKeyPress(0);
		if (keyboard_state[SDL_SCANCODE_1]) 
			c8.registerKeyPress(1);
		if (keyboard_state[SDL_SCANCODE_2])
			c8.registerKeyPress(2);
		if (keyboard_state[SDL_SCANCODE_3])
			c8.registerKeyPress(3);
		if (keyboard_state[SDL_SCANCODE_Q])
			c8.registerKeyPress(4);
		if (keyboard_state[SDL_SCANCODE_W])
			c8.registerKeyPress(5);
		if (keyboard_state[SDL_SCANCODE_E])
			c8.registerKeyPress(6);
		if (keyboard_state[SDL_SCANCODE_A])
			c8.registerKeyPress(7);
		if (keyboard_state[SDL_SCANCODE_S])
			c8.registerKeyPress(8);
		if (keyboard_state[SDL_SCANCODE_D])
			c8.registerKeyPress(9);
		if (keyboard_state[SDL_SCANCODE_Z])
			c8.registerKeyPress(0xA);
		if (keyboard_state[SDL_SCANCODE_C])
			c8.registerKeyPress(0xB);
		if (keyboard_state[SDL_SCANCODE_4])
			c8.registerKeyPress(0xC);
		if (keyboard_state[SDL_SCANCODE_R])
			c8.registerKeyPress(0xD);
		if (keyboard_state[SDL_SCANCODE_F])
			c8.registerKeyPress(0xE);
		if (keyboard_state[SDL_SCANCODE_V]) {
			c8.registerKeyPress(0xF);
		}

		// fetch and run an instruction from memory
		Instruction inst = c8.fetch();
		c8.decode_and_execute(inst);
		
		// only render an update if our display has been changed.
		if (c8.shouldDraw()) {
			std::uint16_t delta_x{WINDOW_WIDTH / DISPLAY_WIDTH};
			std::uint16_t delta_y{WINDOW_HEIGHT / DISPLAY_HEIGHT};
			auto display = c8.grabDisplay();

			SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);      // setting draw color
        	SDL_RenderClear(renderer);  
			
			
			SDL_SetRenderDrawColor(renderer,255,255,255,255);

			for (int x = 0; x < DISPLAY_WIDTH; x++) {
				for (int y = 0; y < DISPLAY_HEIGHT; y++) {
					if (display[y*DISPLAY_WIDTH + x]) {
						SDL_Rect rect{};
						rect.x = x*delta_x;
						rect.y = y*delta_y;
						rect.w = delta_x;
						rect.h = delta_y;
						SDL_RenderFillRect(renderer,&rect);
					}
				}
			}
			
			SDL_RenderPresent(renderer);
				
			c8.clearDrawFlag(); // now that we have drawn, make sure to clear flag
		}

		c8.clearKeyPress();
		c8.decrementCounters();
	}
}
