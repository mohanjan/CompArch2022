#include <iostream>
#include <fstream>
#include <array>
#include <string>
#include <cstdint>

void read_bin(std::string fileloc, uint8_t mem[]);
void write_bin(uint8_t mem[]);
uint32_t get_inst(uint8_t mem[], int32_t pc);
uint32_t get_mem(uint8_t mem[], int32_t address, int32_t size);
void set_mem(uint8_t mem[], int32_t address, int32_t size, uint32_t regval);
void binarydump(uint32_t reg[]);
#define NO_ERR 0;

using namespace std;

int main(void) {
    int a = 0;
    int32_t pc(0);
    uint32_t i;
    uint32_t reg[32];
    uint8_t memo[1000000];
    bool jflag;

    for(i = 0; i<= 31;i++){
        reg[i] = 0;
    }
    i = 0;
    
    while(i < sizeof(memo)/sizeof(uint32_t)){
        memo[i] = 0;
        i++;
    }

    std::string fileloc;
    cin>> fileloc;
   
    std::cout << fileloc << endl;

    read_bin(fileloc, memo);

    write_bin(memo);

    cout << "Hello RISC-V World!" << endl;

    while(1) {
        reg[0] = 0;
        jflag = false;
    
        uint32_t instr = memo[pc] + (memo[pc+1] << 8) + (memo[pc+2] << 16)+ (memo[pc+3] <<24);
        uint32_t opcode = instr & 0x7f;
        uint32_t rd = (instr >> 7) & 0x01f;
        uint32_t rs1 = (instr >> 15) & 0x01f;
        uint32_t rs2 = (instr >> 20) & 0x01f;
        uint32_t funct3 = ((instr >> 12) & 0b111);
        uint32_t funct7 = (instr >> 25);

        uint32_t imm = (instr >> 20);
        if(imm & 0x800 == 0x800){
            imm = imm | 0x800000000;
        }

        int32_t immS = ((instr >> 25) & 0x7F) << 5 | ((instr >> 7) & 0b11111);
        if ((immS & 0x1000) == 0x1000){
            immS = immS | 0xFFFFF000;
        }

        int32_t immB = (((instr >> 8) & 0b1111) | (((instr >> 25) & 0b111111) << 4) | ((instr >> 31) << 11) | (((instr & 0x80) >> 7) << 10) ) << 1;
        if ((immB & 0x1000) == 0x1000){
            immB = immB | 0xFFFFF000;
        }

        //                  --10:1--                    --11--                      --19:12--                          --20--
        int32_t immJ = (((instr >> 21) & 0x3FF) << 1) | (((instr >> 20) & 0b1) << 10) | (((instr >> 12) & 0xF) << 11) | (instr & 0x80000000)>>12;
        if ((immJ & 0x80000) == 0x80000){
            immJ = immJ | 0xFFF00000;
        }
        
        switch (opcode) {

            case 0x37: //LUI
                reg[rd] = (instr >> 12) << 12;
            break;
            case 0x17: //AUIPC
                reg[rd] = ((instr >> 12) << 12) + pc;
            break;
            
            //----------Jump & branch instructions----------
            case 0x6F: //JAL
                reg[rd] = pc+4;
                pc += immJ;
                jflag = true;
                break;
            case 0x67: //JALR
                reg[rd] = pc+4;
                pc = (imm + reg[rs1]) & 0xFFFFFFFE;
                jflag = true;
                break;

            case 0x63: //BEQ/BNE/BLTU/BGEU (funct 3 value)
                switch (funct3)
                {
                case 0b000: //BEQ
                    if(reg[rs1] == reg[rs2]){
                            pc += immB;
                            jflag = true;                        
                    }
                    break;

                case 0b001: //BNE
                    if(reg[rs1] != reg[rs2]){
                            pc += immB;
                            jflag = true; 
                    }
                    break;

                case 0b100: //BLT
                    if(int32_t(reg[rs1]) < int32_t(reg[rs2])){
                        pc += immB;
                        jflag = true; 
                    }
                    
                    break;

                case 0b101: //BGE
                    if(int32_t(reg[rs1]) >= int32_t(reg[rs2])){
                        pc += immB;
                        jflag = true; 
                    }
                    
                    break;

                case 0b110: //BLTU
                    if(reg[rs1] < reg[rs2]){
                        pc += immB;
                        jflag = true; 
                    }
                    
                    break;  

                case 0b111: //BGEU
                    if(reg[rs1] > reg[rs2]){
                        pc += immB;
                        jflag = true; 
                    }
                    
                    break;
                }
            
                break;
            //----------Load instructions----------
            case 0x3: // LB/LH/LW/LBU/LHU (funct 3 value)

            switch (funct3)
            {
                case 0b000: //LB
                    if (( get_mem(memo, (imm + reg[rs1]), 1) & 0x80 ) == 0x80)
                    {
                        reg[rd] = (get_mem(memo, (imm + reg[rs1]), 1) &0xFF) | 0xFFFFFF00;
                    }
                    else{
                        reg[rd] = get_mem(memo, (imm + reg[rs1]), 1) & 0xFF;
                    }
                        
                    break;
                
                case 0b001: //LH
                  
                    if ((get_mem(memo, imm + reg[rs1], 2) & 0x8000 ) == 0x8000)
                    {
                        reg[rd] = ((get_mem(memo, (imm + reg[rs1]), 2) & 0xFFFF) | 0xFFFF0000 );
                    }
                    else{
                        reg[rd] = get_mem(memo, (imm + reg[rs1]), 2) & 0xFFFF;
                    }
                        
                    break;

                case 0b010: //LW 
                    reg[rd] = get_mem(memo, (imm + reg[rs1]), 4);
                    break;
                
                case 0b100: //LBU
                    reg[rd] = get_mem(memo, (imm + reg[rs1]), 1) & 0xFF;
                    break;
                
                case 0b101: //LHU
                    reg[rd] = get_mem(memo, (imm + reg[rs1]), 2) & 0xFFFF;
                    break;
            
            }
                break;
            
            //----------Store instructions----------
            case 0x23: // SB/SH/SW
            switch (funct3)
            {
                case 0b000: 
                    set_mem(memo, (reg[rs1] + immS), 1, reg[rs2]);
                    break;

                case 0b001:
                    set_mem(memo, (reg[rs1] + immS), 2, reg[rs2]);             
                    break;

                case 0b010:
                    set_mem(memo, (reg[rs1] + immS), 4, reg[rs2]);
                    break;
                }    
                break;
            
            //----------Immediate arithmetic instructions----------
            case 0b0010011: // ADDI/SLTI/XORI/ORI/ANDI/SLLI/SRLI/SRAI
                switch (funct3)
                {
                case 0x0: //ADDI
                    if((imm & 0x800) == 0x800)
                    {
                        reg[rd] = reg[rs1] + (imm | 0xfffff000) ;
                    }
                    else
                    {
                        reg[rd] = reg[rs1] + imm;
                    }
                    break;

                case 0x2: //SLTI
                    if((imm & 0x800) == 0x800){
                        imm |= 0xFFFFF000; 
                    }
                    if(int32_t(reg[rs1]) < int32_t(imm))
                    {
                        reg[rd] = 1;
                    }
                    else
                    {
                        reg[rd] = 0;
                    }
                    break;

                case 0x3: //SLTIU 
                    if (reg[rs1] < imm) 
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
                    reg[rd] = (reg[rs1]) << (imm & 0x1f);
                    break;

                case 0x5: //SRLI/SRAI
                    switch (funct7)
                    {
                    case 0b0100000: //SRAI
                        if((reg[rs1] & 0x80000000) == 0x80000000){
                            reg[rd] = (int32_t(reg[rs1]) >> (imm & 0x1f));
                        }
                        else
                        {
                            reg[rd] = (reg[rs1] & 0x7FFFFFFF ) >> (imm & 0x1f);
                        }
                        break;
                
                    case 0b0000000: //SRLI
                        reg[rd] = reg[rs1] >> (imm & 0x1f);

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
                            reg[rd] = reg[rs1] >> (reg[rs2] & 0b11111);
                            break;
                        
                        case 0b0100000: //sra
                            if((reg[rs1] & 0x80000000) == 0x80000000)
                            {
                                reg[rd] = (int32_t(reg[rs1]) >> (reg[rs2] & 0b11111));
                            }
                            else
                            {
                                reg[rd] = (reg[rs1] & 0x7FFFFFFF ) >> (reg[rs2] & 0b11111);
                            }
                            break;
                        break;                  

                        }
                    break;

                    case 0b110://or
                        reg[rd] = reg[rs1] | reg[rs2];
                    break;
                    
                    case 0b111: //and
                        reg[rd] = reg[rs1] & reg[rs2];
                    break;
                    
                }
                break;

            default:
                cout << "Opcode " << hex << opcode << " not yet implemented" << endl;
                break;
        }
        
        if(jflag == false){
            pc += 4;
        }
        
        if (get_inst(memo,pc) == 0 || get_inst(memo,pc) == 0x73){
            break;
        }
            

       
    }
    binarydump(reg);
    for (int i = 0; i <= 31; i++) {
        cout << dec <<"x"<< i<< ": "<< hex << int32_t(reg[i]) << "\t\t" << dec << int32_t(reg[i])<<"\n";
    }
    
    cin >> a;
    cout << "Program exit" << endl;
    
    
    return NO_ERR;
}

uint32_t get_inst(uint8_t mem[], int32_t pc){
    return mem[pc] +(mem[pc+1]<<8)+(mem[pc+2]<<16)+(mem[pc+3]<<16);
}

void binarydump(uint32_t reg[]){
    unsigned char buffer[4*32];
    FILE *write_ptr;

    ofstream wf("output.bin", ios::out | ios::binary);

    for(int i = 0; i<32;i++){
        wf.write((char * ) &reg[i], 4);
    }
    wf.close();
}

uint32_t get_mem(uint8_t mem[], int32_t address, int32_t size){
    if(size == 1){
        return mem[address];
    }
    else if (size == 2)
    {
        return mem[address] + (mem[address+1] << 8);
    }
    else if (size == 4)
    {
        return mem[address] + (mem[address+1] << 8) + (mem[address+2] << 16) + (mem[address+3] << 24);
    }
}

void set_mem(uint8_t mem[], int32_t address, int32_t size, uint32_t regval){
    if(size == 1){
        mem[address] = regval & 0xFF;
    }
    else if (size == 2)
    {
        mem[address] = regval & 0xFF;
        mem[address+1] = (regval & 0xFF00) >>8;
        
    }
    else if (size == 4)
    {
        mem[address] = regval & 0xFF;
        mem[address+1] = (regval & 0xFF00) >> 8;
        mem[address+2] = (regval & 0xFF0000) >> 16;
        mem[address+3] = (regval & 0xFF000000) >> 24;
    }
}

void read_bin(std::string fileloc, uint8_t mem[]){
    std::streampos size;
    uint32_t byte_length;
    char * memblock;

    std::ifstream file (fileloc,std::ios::binary | std::ios::ate);

    size = file.tellg();
    byte_length = (size);
    
    if (file.is_open())
    {
        file.seekg (0, std::ios::beg);
        memblock = new char [size];
        file.read (memblock, size);
       
        for(int i = 0;i < byte_length;i++){
            
                file.seekg(i, std::ios::beg);
                file.read(memblock,1);
                mem[i] = uint32_t(memblock[i] & 0xff);
            
        }

        file.close();

    }
    else std::cout << "Unable to open file";    

}

void write_bin(uint8_t mem[]){
    int i = 0;
    uint32_t pinst;
    while(!(mem[i]==0 && mem[i+1]==0 && mem[i+2]==0 && mem[i+3]==0)){
        pinst = mem[i] +(mem[i+1]<<8)+(mem[i+2]<<16)+(mem[i+3]<<16);
        std::cout<< hex << pinst <<"\n";
        i+=4;
    }

}
