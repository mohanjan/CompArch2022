#include "ISAsupp.h"
#include <iostream>
#include <fstream>
#include <string>

//function to read byte files
void read_file(std::string fileloc, ){
    std::ifstream myfile;
    myfile.open(fileloc);
    
    if ( myfile.is_open() ){ // always check whether the file is open
        myfile >> mystring; // pipe file's content into stream
        }

}


