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
    array<uint32_t, 4> reg = array<uint32_t, 4>();

    // Here the first program hard coded as an array
    array<uint32_t, 100000> progr = {
        // As minimal RISC-V assembler example
        //0x00200093, // addi x1 x0 2
        //0x00300113, // addi x2 x0 3
        //0x002081b3, // add x3 x1 x2
        0x00a00293,
        0x01400313,
        0x00730533,
        0x40a385b3,
        0x02b30633,
    };

    cout << "Hello RISC-V World!" << endl;

    
    //Simulation loop
    while(1) {
        a = 0;
    
        uint32_t instr = progr[pc >> 2];
        uint32_t opcode = instr & 0x7f;
        uint32_t rd = (instr >> 7) & 0x01f;
        uint32_t rs1 = (instr >> 15) & 0x01f;
        uint32_t rs2 = (instr >> 15) & 0x01f;
        uint32_t imm = (instr >> 20);
        uint32_t funct3 = ((instr >> 11) & 0b111);
        uint32_t funct7 = (instr >> 25);


        switch (opcode) {

            case 0x37: //LUI
                
                break;
            case 0x17: //AUIPC
                
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

                        break;
                    
                    case 0b001: //LH

                        break;

                    case 0b010: //LW

                        break;
                    
                    case 0b100: //LBU

                        break;
                    
                    case 0b101: //LHU

                        break;
                break;
                }
            
            //----------Store instructions----------
            case 0x23: // SB/SH/SW
                
                break;
            
            //----------Immediate arithmetic instructions----------
            case 0x13: // ADDI/SLTI/XORI/ORI/ANDI/SLLI/SRLI/SRAI
                switch (funct3)
                {
                case 0x0: //ADDI
                    reg[rd] = reg[rs1] + imm;
                    break;
                case 0x2: //SLTI
                    if(int32_t(reg[rs1]) < imm) //make sure it is sign extended
                        reg[rd] = 1;
                    else
                        reg[rd] = 0;
                    break;
                case 0x3: //SLTIU 
                    if (reg[rs1] < imm)  //same as above, but imm is treated as unsigned
                        reg[rd] = 1;
                    else
                        reg[rd] = 0;
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
                    reg[rd] = reg[rs1] << imm; // todo: be sure that this is correct
                    break;
                case 0x5: //SRLI/SRAI
                    switch (funct7)
                    {
                    case 0b0100000: //SRAI
                        if((reg[rs1] & 0x80000000) == 0x80000000){
                            reg[rd] = ((reg[rs1] & 0x7FFFFFFF ) >> imm) | 0x80000000;
                        }
                        else{
                            reg[rd] = (reg[rs1] & 0x7FFFFFFF ) >> imm;
                        }
                        break;
                
                    case 0b0000000: //SRLI
                        reg[rd] = reg[rs1] >> imm;

                        break;
                    }
                    break;
                
                //default:
                  //  cout << "Unexpected funct3 value "<<endl;
                    //break;
                }
                
                //break;

            //----------Arithmetic instructions----------
            case 0x33: // ADD/SUB/SLL/SLT/SLTU/XOR/SRL/SRA/OR/ANDI
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
        if (progr[pc] == 0)
            break;

        if ((pc >> 2) >= progr.size()) {
            break;
        }
        cin >> a;
        if (a == 1){
            for (size_t i(0); i < 31; ++i) {
            cout << "x"<< i<< ": "<<reg[i] << "\n";
            }
            cout << endl;
        }
        
    }
    cin >> a;
    
    cout << "Program exit" << endl;

    
    return NO_ERR;
}