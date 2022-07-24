#ifndef CHIP8_HPP
#define CHIP8_HPP

#include <stack>
#include <cstdint>
#include <string>
#include <cstring>

#define DISPLAY_HEIGHT 64
#define DISPLAY_WIDTH 128
#define MEM_SIZE 4096
#define FONT_OFFSET 0x050
#define ROM_OFFSET 0x200

union Instruction {

    uint16_t code;
    struct {
        std::uint16_t byte2: 8;
        std::uint16_t byte1: 8;
    } Main; 
    struct {
        std::uint16_t n: 4;
        std::uint16_t vy: 4;
        std::uint16_t vx: 4;
        std::uint16_t kind: 4;
    } A;

    struct {
        std::uint16_t nn: 8;
        std::uint16_t vx: 4;
        std::uint16_t kind: 4;
    } B;

    struct {
        std::uint16_t nnn: 12;
        std::uint16_t kind: 4;
    } C;
};

static std::uint8_t font_store[] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

class Chip8 {
    public:
        Chip8();
        ~Chip8() = default;

        bool loadROM(const char*);
        bool checkKeyStatus(std::uint8_t);
        bool* grabDisplay() { return (bool*)display;};
        bool shouldDraw() { return drawFlag;};
        void clearDrawFlag() { drawFlag = false;};
        void flagDraw() { drawFlag = true;};
        void registerKeyPress(std::uint8_t);
        void clearKeyPress() { keyPressed = false;};
        Instruction fetch();
        bool decode_and_execute(Instruction&);
        void decrementCounters();
    private:

        std::uint8_t varRegister[16]{0};
        std::uint8_t delayTimer{255};
        std::uint8_t soundTimer{255};
        std::uint16_t pc{ROM_OFFSET};
        std::uint16_t idxRegister{0};
        std::uint8_t memory[MEM_SIZE]{0};
        bool display[DISPLAY_HEIGHT][DISPLAY_WIDTH]{false};
        std::stack<std::uint16_t> stack;
        bool drawFlag{true};
        std::uint8_t curKey{0},prevKey{0};
        bool keyPressed{false};
};
#endif CHIP8_HPP