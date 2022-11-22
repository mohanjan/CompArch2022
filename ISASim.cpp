// read file function
// the big switch
// write file results

/**
 * RISC-V Instruction Set Simulator
 * <p>
 * A tiny first step to get the simulator started. Can execute just a single
 * RISC-V instruction.
 * 
 * To run this program: 
 *      mkdir build
 *      cd build
 *      cmake ..
 *      make
 *      ./isasim
 * specification found on p130
 * 
 */

#include <iostream>
#include <array>
#include <cstdint>
#include "ISAsupp.h"

#define NO_ERR 0;

using namespace std;

int main(void) {
    int a = 0;
    uint32_t pc(0);
    uint32_t i;
    array<uint32_t, 32> reg;
    // Here the first program hard coded as an array
    array<uint32_t, 10000> memo;
    array<uint32_t, 7> inst = {
        0x00a00513,
        0x01400593,
        0x00b50633,
        0x00200693,
        0x00269713,
        0x00175793,
        0x40c58533
    };

    for(i = 0; i<= 31;i++){
        reg[i] = 0;
    }

    i = 0;

    while(i < sizeof(memo)/sizeof(uint32_t)){
        memo[i] = 0;
        i++;
    }

    i = 0;
    
    while(i < sizeof(inst)/sizeof(uint32_t)){
        memo[i] = inst[i];
        i++;
    }
    i = 0;

    cout << "Hello RISC-V World!" << endl;

    while(1) {
        
    
        uint32_t instr = memo[pc >> 2];
        uint32_t opcode = instr & 0x7f;
        uint32_t rd = (instr >> 7) & 0x01f;
        uint32_t rs1 = (instr >> 15) & 0x01f;
        uint32_t rs2 = (instr >> 20) & 0x01f;
        uint32_t imm = (instr >> 20);
        uint32_t imm5 = (instr >> 20) & 0b11111;
        uint32_t immU = (instr >> 12) << 12;
        uint32_t immS = (instr >> 25) << 5 | ((instr >> 7) & 0b11111); //todo: add signage
        uint32_t funct3 = ((instr >> 12) & 0b111); 
        uint32_t funct7 = (instr >> 25);


        switch (opcode) {

            case 0x37: //LUI
                reg[rd] = immU;
            break;
            case 0x17: //AUIPC
                reg[rd] = immU + pc;
            break;
            
            //----------Jump & branch instructions----------
            case 0x6F: //JAL
                
            break;
            case 0x67: //JALR
                
            break;
            case 0x63: //BEQ/BNE/BLTU/BGEU (funct 3 value)
                
            break;

            //----------Load instructions----------
            case 0x3: // LB/LH/LW/LBU/LHU (funct 3 value)

            switch (funct3)
            {
                case 0b000: //LB
                    if (int32_t(( memo[(imm + reg[rs1])>>2] & 0xFF ) < 0))
                    {
                        reg[rd] = (memo[(imm + reg[rs1])>>2] & 0xFF); //todo: sign extend
                    }
                    else{
                        reg[rd] = memo[(imm + reg[rs1])>>2] & 0xFF;
                    }
                        
                    break;
                
                case 0b001: //LH
                    if (int32_t(( memo[(imm + reg[rs1]) >> 2] & 0xFFFF ) < 0))
                    {
                        reg[rd] = (memo[(imm + reg[rs1]) >> 2] & 0xFFFF ); //todo: sign extend
                    }
                    else{
                        reg[rd] = memo[(imm + reg[rs1]) >> 2] & 0xFFFF;
                    }
                        
                    break;

                case 0b010: //LW
                    reg[rd] = memo[(imm + reg[rs1]) >> 2];
                    break;
                
                case 0b100: //LBU
                    reg[rd] = memo[(imm + reg[rs1]) >> 2] & 0xFF;
                    break;
                
                case 0b101: //LHU
                    reg[rd] = memo[(imm + reg[rs1]) >> 2] & 0xFFFF;
                    break;
            
            }
            break;
            
            //----------Store instructions----------
            case 0x23: // SB/SH/SW
            switch (funct3)
            {
                case 0b000: 
                    memo[(rs1 + immS) >> 2] = rs2 & 0xff;
                    break;

                case 0b001:
                    memo[(rs1 + immS) >> 2] = rs2 & 0xffff;
                    break;

                case 0b010:
                    memo[(rs1 + immS) >> 2] = rs2;
                    break;
                }    
                break;
            
            //----------Immediate arithmetic instructions----------
            case 0b0010011: // ADDI/SLTI/XORI/ORI/ANDI/SLLI/SRLI/SRAI
                switch (funct3)
                {
                case 0x0: //ADDI
                    reg[rd] = reg[rs1] + imm;
                    break;
                case 0x2: //SLTI
                    if(int32_t(reg[rs1]) < imm) //make sure it is sign extended
                    {
                        reg[rd] = 1;
                    } 
                    else
                    {
                        reg[rd] = 0;
                    }
                    break;

                case 0x3: //SLTIU 
                    if (reg[rs1] < imm)  //same as above, but imm is treated as unsigned
                    {
                        reg[rd] = 1;
                    }
                    else
                    {
                        reg[rd] = 0;
                    }
                    break;

                case 0x4: //XORI
                    reg[rd] = reg[rs1] ^ imm;
                    break;

                case 0x6: //ORI
                    reg[rd] = reg[rs1] | imm;
                    break;

                case 0x7: //ANDI
                    reg[rd] = reg[rs1] & imm;
                    break;

                case 0x1: //SLLI
                    reg[rd] = (reg[rs1]) << imm5; // todo: be sure that this is correct
                    break;

                case 0x5: //SRLI/SRAI
                    switch (funct7)
                    {
                    case 0b0100000: //SRAI
                        if((reg[rs1] & 0x80000000) == 0x80000000){
                            reg[rd] = ((reg[rs1] & 0x7FFFFFFF ) >> imm5) | 0x80000000;
                        }
                        else
                        {
                            reg[rd] = (reg[rs1] & 0x7FFFFFFF ) >> imm5;
                        }
                        break;
                
                    case 0b0000000: //SRLI
                        reg[rd] = reg[rs1] >> imm;

                        break;
                    }
                    break;
                
                }
                
                break;

            //----------Arithmetic instructions----------
            case 0b0110011: // ADD/SUB/SLL/SLT/SLTU/XOR/SRL/SRA/OR/ANDI
                switch (funct3)
                {
                    case 0b000: //add / sub
                        switch (funct7)
                        {
                        case 0b0: 
                            reg[rd] = reg[rs1] + reg[rs2];
                            break;
                        
                        case 0b0100000:
                            reg[rd] = reg[rs1] - reg[rs2];
                            break;
                        }
                        break;
                    
                    case 0b001: //sll
                    reg[rd] = reg[rs1] << reg[rs2];
                    break;

                    case 0b010: //slt
                        if( int32_t(reg[rs1]) < int32_t(reg[rs2]) ){
                            reg[rd] = 1;
                        }
                        else{
                            reg[rd] = 0;
                        }
                    break;

                    case 0b011: //sltu
                        if(reg[rs1] < reg[rs2]){
                            reg[rd] = 1;
                        }
                        else{
                            reg[rd] = 0;
                        }
                    break;

                    case 0b100: //XOR
                        reg[rd] = reg[rs1] ^ reg[rs2];
                    break;
                    
                    case 0b101:
                        switch (funct7)
                        {
                        case 0b0000000: //srl
                            reg[rd] = reg[rs1]>>reg[rs2];
                            break;
                        
                        case 0b0100000: //sra
                            if((reg[rs1] & 0x80000000) == 0x80000000)
                            {
                                reg[rd] = ((reg[rs1] & 0x7FFFFFFF ) >> reg[rs2]) | 0x80000000;
                            }
                            else
                            {
                                reg[rd] = (reg[rs1] & 0x7FFFFFFF ) >> reg[rs2];
                            }
                            break;
                        break;                  

                        }
                    case 0b110://or
                        reg[rd] = reg[rs1] | reg[rs2];
                    break;
                    
                    case 0b111: //and
                        reg[rd] = reg[rs1] & reg[rs2];
                    break;
                    
                }
                break;

            default:
                cout << "Opcode " << opcode << " not yet implemented" << endl;
                break;
        }
        
        pc += 4; // One instruction is four bytes
        if (memo[pc>>2] == 0)
            break;

        //if ((pc >> 2) >= memo.size()) {
        //    break;
        //}

        
        
        
    }
   
    for (size_t i(0); i <= 31; ++i) {
        cout << "x"<< i<< ": "<< int32_t(reg[i]) << "\n";
    }
    cout << endl;
 
    cin >> a ;
    cout << "Program exit" << endl;
    
    
    return NO_ERR;
}

/*
void big_switch(uint32_t opcode){

    switch (opcode) {

            case 0x37: //LUI
                
                break;
            case 0x17: //AUIPC
                
                break;
            
            //----------Jump & branch instructions----------
            case 0b1100011:
            reg[rd] = B_type();
            break;

            //----------Load instructions----------
            case : // LB/LH/LW/LBU/LHU (funct 3 value)

            
            //----------Store instructions----------
            case : // SB/SH/SW
              
            //----------Immediate arithmetic instructions----------
            case 0x13: // ADDI/SLTI/XORI/ORI/ANDI/SLLI/SRLI/SRAI
                
                    
                

            //----------Arithmetic instructions----------
            case : // ADD/SUB/SLL/SLT/SLTU/XOR/SRL/SRA/OR/ANDI
                    
                


        }
}


void R_type(){
    
}

void I_type(){

}

void S_type(){
    
}

void B_type(){
    
}

void U_type(){
    
}

void J_type(){
    
}
*/