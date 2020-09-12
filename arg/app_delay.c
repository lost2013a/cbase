#include <stdio.h>
#include <string.h>
#include <unistd.h>


#define RENAME(A,B,C) void A##_init()

RENAME(c,a,a)
{
    printf("2\n");
}


int main(int argc, char *argv[]) 
{	
    
   c_init();
   printf("1\n");

}

