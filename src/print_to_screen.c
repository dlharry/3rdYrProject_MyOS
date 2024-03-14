#include "print_to_screen.h"
#include "printf.h"
#include "common.h"
#include "mm.h"
#include "fb.h"

void print_to_screen(char *str){
    drawString(output_loc_x, output_loc_y, str, 0x0f, 2);
}

char* my_sprintf(char *fmt, ...){
    char* s_o = "";
    va_list va;
    va_start(va,fmt);
    sprintf(s_o, fmt, va);
    va_end(va);
    return s_o;
}

void clear_screen(){
    drawRect(0,0,1920,1080,0,1);
    output_loc_x = 100;
    output_loc_y = 100;
}