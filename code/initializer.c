#include <stdio.h>
#include "colorConf.c"



int main(){
    red();
    printf("1. Message in red\n");
    
    bold_red();
    printf("2. Message in bold_red\n");
    
    yellow();
    printf("3. Message in yellow\n");
    
    green();
    printf("4. Message in green\n");
    
    bold_green();
    printf("5. Message in bold_green\n");
    
    blue();
    printf("6. Message in blue\n");
    
    bold_blue();
    printf("7. Message in bold_blue\n");

    reset();
    printf("8. Message in reset\n");

    cyan();
    printf("9. Message in cyan\n");

    purple();
    printf("10. Message in purple\n");
	return 0;
}