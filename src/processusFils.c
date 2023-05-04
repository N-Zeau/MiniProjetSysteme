#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define sizeAlphabet 27

int main(int argc, char **argv) {
    printf("coucou je suis dans le fils\n");

    char alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ ";

    char cara;
    int bool;

    char message[100];
    int j=0;
    while (read(atoi(argv[3]), &cara, sizeof(char)) != 0) {
        if (cara == '.' || cara == ',') {
            message[j] = cara;
            //printf("%c\n", cara);
        } else {
            for (int i = 0; i < sizeAlphabet; i++) {
                if (cara == alphabet[i]) {
                    message[j] = alphabet[(i - atoi(argv[2]) + sizeAlphabet) % sizeAlphabet];
                    //printf("%c", alphabet[(i - atoi(argv[2]) + sizeAlphabet) % sizeAlphabet]);
                }
            }
        }
        j++;
    }

    message[j] = '\0';
    printf("%s\n", message);

    close(atoi(argv[3]));

    return 0;
}