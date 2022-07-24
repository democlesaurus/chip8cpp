#include "chip8.hpp"
#include <cassert>
#include <cstdlib>
#include <fstream>
#include <iostream>

Chip8::Chip8() {
    //put our font into our emulator's memory;
    memcpy((memory + FONT_OFFSET),font_store, sizeof(std::uint8_t)*80);
    memset(display,false,sizeof(bool)*DISPLAY_HEIGHT*DISPLAY_WIDTH);
}
Instruction Chip8::fetch() {
    Instruction inst{};

    assert(pc < 4094);

    
    inst.code = memory[pc++];
    inst.code <<= 8;
    inst.code += memory[pc++];
    printf("Instruction at %x:\n",pc-2);
    printf(" -- %x %x %x %x\n",inst.A.kind, inst.A.vx, inst.A.vy, inst.A.n);
  
    return inst;
};

bool Chip8::decode_and_execute(Instruction& inst) {

    if (inst.Main.byte1 == 0x00) { 
        if (inst.Main.byte2 == 0xe0) { // CLEAR SCREEN
            memset(display,false,DISPLAY_HEIGHT*DISPLAY_WIDTH*sizeof(bool));
            return true;
        }
        else if (inst.Main.byte2 == 0xee) { // RETURN FROM SUBROUTINE
            pc = stack.top();
            stack.pop();
            return true;
        }
    };
    
    switch (inst.A.kind) {

        case 0x1: // JUMP nnn
            pc = inst.C.nnn;
            break;
        case 0x2: // CALL SUBROUTINE AT NNN
            stack.push(pc);
            pc = inst.C.nnn;
            break;
        case 0x3: // SKIP IF VX equals NN
            if (varRegister[inst.A.vx] == inst.B.nn) {
            pc += 2;
            };
            break;
        case 0x4: // SKIP IF VX does not equal NN
            if (varRegister[inst.A.vx] != inst.B.nn) {
            pc += 2;
            };
            break;
        case 0x5:
            if (inst.A.n == 0x0 && varRegister[inst.A.vx] == varRegister[inst.A.vy]) {
                pc += 2;
            };
            break;
        case 0x9:
            if (inst.A.n == 0x0 && varRegister[inst.A.vx] != varRegister[inst.A.vy]) {
                pc += 2;
            };
            break;
        case 0x6:
            varRegister[inst.A.vx] = inst.B.nn;
            break;
        case 0x7:
            varRegister[inst.A.vx] += inst.B.nn;
            break;
        case 0x8:
            switch (inst.A.n) {
                case 0x0:
                    varRegister[inst.A.vx] = varRegister[inst.A.vy];
                    break;
                case 0x1:
                    varRegister[inst.A.vx] |= varRegister[inst.A.vy];
                    break;
                case 0x2:
                    varRegister[inst.A.vx] &= varRegister[inst.A.vy];
                    break;
                case 0x3:
                    varRegister[inst.A.vx] ^= varRegister[inst.A.vy];
                    break;
                case 0x4:
                    {
                        std::uint16_t temp = varRegister[inst.A.vx];
                        varRegister[inst.A.vx] += varRegister[inst.A.vy];
                        if (varRegister[inst.A.vx] < temp) {
                            varRegister[0xF] = 1;
                        } else {
                            varRegister[0xF] = 0;
                        }
                    }
                    break;
                case 0x5:
                    if (varRegister[inst.A.vx] > varRegister[inst.A.vy]) {
                        varRegister[0xF] = 1;
                    } else {
                        varRegister[0xF] = 0;
                    }
                    varRegister[inst.A.vx] -= varRegister[inst.A.vy];
                    break;
                case 0x7:
                    if (varRegister[inst.A.vx] < varRegister[inst.A.vy]) {
                        varRegister[0xF] = 1;
                    } else {
                        varRegister[0xF] = 0;
                    }
                    varRegister[inst.A.vx] = varRegister[inst.A.vy] - varRegister[inst.A.vx];
                    break;
                case 0x6:
                    if (varRegister[inst.A.vx] & 0x1 == 0x1) { // if bit shifted out is 1
                        varRegister[0xF] = 1;
                    } else {
                        varRegister[0xF] = 0;
                    }
                    varRegister[inst.A.vx] >>= 1;
                    break;
                case 0xE:
                    if (varRegister[inst.A.vx] & 0x8 == 0x8) { // if bit shifted out is 1
                        varRegister[0xF] = 1;
                    } else {
                        varRegister[0xF] = 0;
                    }
                    varRegister[inst.A.vx] <<= 1;
                    break;
            };
            break;
        case 0xA: // Set Index Register value to NNN
            idxRegister = inst.C.nnn;
            break;
        case 0xB: // jump with offset XNN -> XNN + VX
            assert (inst.C.nnn + varRegister[inst.A.vx] > inst.C.nnn);
            pc = inst.C.nnn + varRegister[inst.A.vx];
            break;
        case 0xC:
            {
                std::uint16_t random = rand();
                varRegister[inst.A.vx] = random & inst.B.nn;
            }
            break;
        case 0xD: // Display
            {
                std::uint16_t x{varRegister[inst.A.vx] % DISPLAY_WIDTH};
                std::uint16_t y{varRegister[inst.A.vy] % DISPLAY_HEIGHT};
                varRegister[0xF] = 0;
                for (int i = 0; i < inst.A.n; i++) {
                    std::uint8_t spriteData = memory[idxRegister + i];
                    std:uint8_t mask = 1;

                    // hit the bottom of the screen
                    if ((y + i) >= DISPLAY_HEIGHT)
                        continue;

                    for (std::uint8_t j = 0; j < 8; j++) {
                        
                        bool isSpritePixelOn = (spriteData & 0x80);
                        spriteData <<= 1;

                        // // hit the edge so quit drawing this row or we don't have a pixel to flip
                        // if (x + j >= DISPLAY_WIDTH || !isSpritePixelOn) {
                        //     continue;
                        // }
                            

                        if(display[y + i][x + j] && isSpritePixelOn) { 
                            display[y+i][x+j] = false;
                            varRegister[0xF] = 1;
                        } else if (isSpritePixelOn){
                            display[y+i][x+j] = true;
                        };
                    }
                    flagDraw();
                };
            }
            break;
        
        case 0xE:
            {
                bool isKeyPressed = checkKeyStatus(varRegister[inst.A.vx]);

                if (inst.B.nn == 0x9E && isKeyPressed) {
                    pc += 2;
                } else if (inst.B.nn == 0xA1 && !isKeyPressed)
                {
                    pc += 2;
                };
            }
            break;
        case 0xF:
            switch (inst.B.nn) {
                case 0x07:
                    varRegister[inst.A.vx] = delayTimer;
                    break;
                case 0x15:
                    delayTimer = varRegister[inst.A.vx];
                    break;
                case 0x18:
                    soundTimer = varRegister[inst.A.vx];
                    break;
                case 0x1e:
                    idxRegister += varRegister[inst.A.vx];
                    break;
                case 0x0A:
                    if (!keyPressed) {
                        pc -= 2;
                    } else {
                        varRegister[inst.A.vx] = curKey;
                    }
                    break;
                case 0x29:
                    idxRegister = FONT_OFFSET + varRegister[inst.A.vx] * 5;
                    break; 
                case 0x33:
                    {
                        std::uint8_t value = varRegister[inst.A.vx];
                        memory[idxRegister + 0] = value / 100;
                        memory[idxRegister + 1] = (value % 100) / 10;
                        memory[idxRegister + 2] = value % 10;
                    }
                    break;
                case 0x55:
                    for (int i = 0; i <= inst.A.vx;i++) {
                        memory[idxRegister + i] = varRegister[i];
                    };
                    break;
                case 0x65:
                    for (int i = 0; i <= inst.A.vx;i++) {
                        varRegister[i] = memory[idxRegister + i];
                    }
                    break;
            }
            break;
    }
    
    return false;
};

bool Chip8::checkKeyStatus(std::uint8_t key) {
    return curKey == key;
}

bool Chip8::loadROM(const char* filename) {

    	// Open the file as a stream of binary and move the file pointer to the end
	std::cout << "Attempting to open: " << filename << std::endl;
    std::ifstream file(filename, std::ios::binary | std::ios::ate);

	if (file.is_open())
	{
		// Get size of file and allocate a buffer to hold the contents
		std::streampos size = file.tellg();
        std::cout<< "Creating buffer of size: " << size << std::endl;
		char* buffer = new char[size]; 
        std::cout<< "Buffer created" << std::endl;

		// Go back to the beginning of the file and fill the buffer
		file.seekg(0, std::ios::beg);
		file.read(buffer, size);
		file.close();

		// Load the ROM contents into the Chip8's memory, starting at 0x200
		for (long i = 0; i < size; ++i)
		{
			memory[ROM_OFFSET + i] = buffer[i];
		}

		// Free the buffer
		delete[] buffer;
        return true;
	}
    // couldn't open file
    return false;
}

void Chip8::decrementCounters() {
    if (soundTimer > 0)
        soundTimer--;
    if (delayTimer > 0)
        delayTimer--;
}

void Chip8::registerKeyPress(std::uint8_t key) {
    
    prevKey = curKey;
    curKey = key;

    if (curKey != prevKey) {
        keyPressed = true;
        printf("log: Key Pressed (%i)\n",curKey);
    }
};