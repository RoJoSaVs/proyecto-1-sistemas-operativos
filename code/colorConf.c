// Add color to text in console

#include <stdio.h>

void red () {
    printf("\033[0;31m");
}

void bold_red () {
    printf("\033[1;31m");
}

void yellow() {
    printf("\033[0;33m");
}

void green() {
    printf("\033[0;32m");
}

void bold_green() {
    printf("\033[1;32m");
}
void blue() {
    printf("\033[0;34m");
}

void bold_blue() {
    printf("\033[1;34m");
}

void purple(){
    printf("\033[1;35m");
}

void cyan(){
    printf("\033[1;36m");
}

void reset() {
    printf("\033[0m");
}

// Source links
// http://web.theurbanpenguin.com/adding-color-to-your-output-from-c/
// https://stackoverflow.com/questions/3219393/stdlib-and-colored-output-in-c


