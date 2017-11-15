#include <cstdio>
#include "lcd.hpp"


int main(int argc, char *argv[]){
    if(argc < 2)
        return 1;
    LCD lcd;

    RGB rgb = {255,255,0};

    lcd.display(argv[1],argv[2],rgb);
    getchar();

}
