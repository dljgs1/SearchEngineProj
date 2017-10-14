//level 1 
#ifndef MYMACRO
#define MYMACRO

#define MEM_WRITE(fout,addr,size) fout##.write((char*)##addr##,##size##)

#define MEM_READ(fin,addr,size) fin##.read((char*)##addr##,##size##)

#define STR_MAX_LEN 128


#endif