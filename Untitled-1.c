// 65536 locations for memory in the simulated LC-3
unit16_t memory[UINT16_MAX];

// registers are the CPUs workbench. its where calculations occur in the cpu
// the  LC-3 has 10 register, each of which are 16 bits

enum{
    R_R0 = 0,
    R_R1,
    R_R2,
    R_R3,
    R_R4,
    R_R5,
    R_R6,
    R_R7, //(R_R0-7 general purpose register)
    R_PC, // program counter
    R_COND, //condition flag (cond) register
    R_COUNT
};


unint16_t reg[R_COUNT] // the registers are stored as an array

//instructions sets  contain the opcodes that are commands
//given to the LC-3 computer as well as the parameters
//each instruction is 16 bits long with 4 bits for storing the
//opcode
//there are 16 opcodes in the LC-3 

enum{
    OP_BR = 0, /*branch */
    OP_ADD, 
    OP_LD,  /*LOAD*/
    OP_ST, /*STORE*/
    OP_JSR, /*jump register*/
    OP_AND, //bitwise and
    OP_LDR, //load register
    OP_STR, //store register
    OP_RTI, //unused !!
    OP_NOT, //bitwise not
    OP_LDI, //load indirect
    OP_STI, //store indirect
    OP_JMP,  //jump
    OP_RES, //reserved (unused!!)
    OP_LEA, //load effective address
    OP_TRAP //execute trap
};


//the R_COND register stores condition flags that
// provide information on the most recently executed calculation

// its good cus it alows programs to check out stuff like
// if (x>0){...}

// tocheck %% what happens is that positive, zero and negative are defined here??
enum{
    FL_POS = 1 << 0, //Positive
    FL_ZRO = 1 << 1, //zero
    FL_NEG = 1 << 2
};

int int main(int argc, const char* argv[]) {
    {Load Arguments, 12}
    {Setup, 12}
    //set the program counter to the starting position
    //0x3000 is the default starting position

    enum{PC_START = 0x3000 };
    reg[R_PC] = PC_START;

    int running =1;
    while(running){
        //fetch 
        uint16_t instr = mem_read(reg[R_PC]++);
        uint16_t op = instr >>12

        switch (op)
        {
        case OP_ADD:
            {   
                //destination register (DR)
                uint16_t r0 =(instr >> 9) & 0x7;
                // first operand (SR)
                uint16_t r1 =(instr >> 6) & 0x7;
                // whether we are in immediate mode
                uint16_t imm_flag =(instr >> 5) &0x1;

                if (imm_flag){
                    uint16_t imm5 = sign_extend(instr & 0x1F, 5);
                    reg[r0] = reg[r1] + imm5;
                }
                else 
                {
                    uint16_t r2 = instr & 0x7;
                    reg[r0] = reg[r1] +reg[r2];
                }

                update_flag(r0);

            }
            break;
        case OP_AND:
            {   
                //destination register (DR)
                uint16_t r0 =(instr >> 9) & 0x7;
                // first operand (SR)
                uint16_t r1 =(instr >> 6) & 0x7;
                // whether we are in immediate mode
                uint16_t imm_flag =(instr >> 5) &0x1;

                if (imm_flag){
                    uint16_t imm5 = sign_extend(instr & 0x1F, 5);
                    reg[r0] = reg[r1] & imm5;
            

                }
                else 
                {
                    uint16_t r2 = instr & 0x7;
                    reg[r0] = reg[r1] & reg[r2];
                }
                update_flag(r0);
                }
            break;


        case OP_NOT: 
            {
                uint16_t r0 = (instr >> 9) & 0x7;

                uint16_t r1 = (instr >> 6) & 0x7;

                reg[r0]  ~= reg[r1];
                update_flag(r0);
            }
            break;
        case OP_JMP:
            {
                uint16_t r1 = (instr >> 6) & 0x7;
                reg[R_PC] = reg[r1]
            }
            break;
        case OP_JSR:
            {
                //whethere we are in immediate mode or not (base register mode)
                uint16_t long_flag = (instr >> 11) & 1;
                // set the program counter offset
                uint16_t long_pc_offset = sign_extend(instr & 0x07ff, 11);
                // set up the base register 
                uint16_t r1 = (instr >>6) & 0x7;
                //save the incremented PC to register 7
                
                reg[R_R7] = reg[R_PC];

                if(long_flag){
                    reg[R_PC] += long_pc_offset;
                }
                else{
                    reg[R_PC] =reg[r1];
                }
                break;
            }
            break;
        case OP_LD:
            {
                //destination register 
                uint16_t r0 = (instr >> 9) &0x7; 
                //program counter offset 9
                uint16_t pc_offset = sign_extend(instr & 0x01ff, 9);
                ref[r0] = mem_read(reg[R_PC] + pc_offset);
                update_flag(r0);
            }
            break;
        case OP_LDI:
            {
                //destination register 
                uint16_t r0 = (instr >> 9) & 0x7; 
                //program counter offset 9
                uint16_t pc_offset = sign_extend(instr & 0x01ff, 9);
                // add the pc_offset to the current PC, look at that memory location to get the 
                //final address
                reg[r0] = mem_read(mem_read(reg[R_PC] + pc_offset));
                update_flag(r0);

            }
            break;
        case OP_LDR:
        {
            //destination register 
            uint16_t r0 = (instr >> 9) & 0x7; 
            uint16_t r1 = (instr >> 6) & 0x7;
            uint16_t offset = sign_extend(instr & 0x3f, 6);
            reg[r0] = mem_read(reg[r1] + offset);
            update_flag(r0);
        }
            break;
        case OP_LEA:
            {
               //destination register 
                uint16_t r0 = (instr >> 9) & 0x7;  
                uint16_t pc_offset = sign_extend(instr & 0x01ff,9);
                reg[r0] = reg[R_PC] +pc_offset;
                update_flag(r0);
            }
            break;
        case OP_ST:
            {
                uint16_t r0 = (instr >> 9) & 0x7;  
                uint16_t pc_offset = sign_extend(instr & 0x01ff,9);
                mem_write(reg[R_PC] + pc_offset, reg[r0]);
            }
            break;
        case OP_STI:
            {
                uint16_t r0 = (instr >> 9) & 0x7;  
                uint16_t pc_offset = sign_extend(instr & 0x01ff,9);
                mem_write(mem_read(reg[R_PC] + pc_offset), reg[r0]);
                
            }
            break;
        case OP_STR:
            {
                uint16_t r0 = (instr >> 9) & 0x7;
                uint16_t r1 = (instr >> 6) & 0x7;
                uint16_t offset = sign_extend(instr & 0x3f,6);
                mem_write(reg[r1] + offset, reg[r0]); 
            }
            break;
        case OP_TRAP:
            {
                enum {
                    TRAP_GETEC = 0x20, /*get char from keyboard not echoed onto terminal*/
                    TRAP_OUT = 0x21, /*output a char*/
                    TRAP_PUTS = 0x22, /*output a word string*/
                    TRAP_IN = 0x23, /*get a char from keyboard, echoed onto terminal*/
                    TRAP_PUTSP = 0x24, /*outputs a byte string*/
                    TRAP_HALT = 0x25 /*halt the program*/
                }
            }
            break;
        case OP_RES:
        case OP_RTI:
    
        default:
            abort();
            break;
        }
    }
    {shutdown, 12}
}
// sign extend 
uint16_t sign_extend(uint16_t x, int bit_count){
    if ((x>>(bit_count-1)) & 1){
        x|= (0xFFFF <<bit_count);
    }
    return x;
}

//updating flag
uint16_t update_flag (uint16_t r){
    if (reg[r] == 0)
    {
        reg[R_COND] = FL_ZRO;
    }
    else if (reg[r] >> 15) //1 in the leftmost bit indicates negative
    {
        reg[R_COND] = FL_NEG;
    }
}
