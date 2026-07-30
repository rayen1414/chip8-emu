class chip8
{
private:
    unsigned short opcode;
    unsigned char memory[4096];
    unsigned char V[16];
    unsigned short I;
    unsigned short pc;
    unsigned short stack[16];   // Stores return addresses for subroutines
    unsigned short sp;          // Stack Pointer
public:
    chip8(/* args */);

};


