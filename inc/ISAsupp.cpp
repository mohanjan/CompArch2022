#include "ISAsupp.h"
#include <iostream>
#include <fstream>
#include <array>
#include <string>
#include <cstdint>

//function to read byte files
void read_bin(std::string fileloc, uint32_t * mem_p){
    std::streampos size;
    int byte_length;
    char * memblock;

    std::ifstream file ("addlarge.bin",std::ios::binary | std::ios::ate);

    size = file.tellg();
    byte_length = (size);
    
    if (file.is_open())
    {
        file.seekg (0, std::ios::beg);
        memblock = new char [1];
        file.read (memblock, 1);

        for(int i = 0;i < byte_length/4;i++){
            for(int j = 0;j<4;j++){
                file.seekg(j+(i*4), std::ios::beg);
                file.read(memblock,size);
                mem_p[i] |= uint32_t(memblock) << j+(i*4);
            }
        }
        file.close();

        delete[] memblock;
    }
    else std::cout << "Unable to open file";    
}



/*
int main () {
    int a;
    
        streampos begin,end;
        ifstream myfile ("addlarge.bin", ios::binary);
        //begin = myfile.tellg();
        cout<< myfile
        
        //myfile.seekg (0, ios::end);
        //end = myfile.tellg();
        myfile.close();
        cout << "size is: " << (end-begin) << " bytes.\n";
        cin >> a;
    
  return 0;
}
int main () {
    int a;
    streampos size;
    int byte_length;
    char * memblock;
    
    ifstream file ("addlarge.bin",ios::binary | ios::ate);

    size = file.tellg();
    byte_length = (size);

    cout << byte_length;
    uint32_t instructions[byte_length/4];
    cout << "size of instructions array:" <<sizeof(instructions)/4 <<"\n";
    
    for(int i = 0; i < byte_length/4; i++){
        instructions[i] = 0;
    }
     

    if (file.is_open())
    {
        file.seekg (0, ios::beg);
        memblock = new char [1];
        file.read (memblock, 1);

        for(int i = 0;i < byte_length/4;i++){
            for(int j = 0;j<4;j++){
                file.seekg(j+(i*4), ios::beg);
                file.read(memblock,size);
                instructions[i] |= uint32_t(memblock) & (0xff << j+(i*4));
                
                

            }
            cout << hex << instructions[i]<<"\n";

        }

       // memblock = new char [size];
        //file.seekg (0, ios::beg);
        
        //file.read (memblock, size);
        file.close();

        cout << "the entire file content is in memory";

        delete[] memblock;
    }
    else cout << "Unable to open file";
    cin>> a;
    return 0;
    }*/
