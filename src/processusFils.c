#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define SIZE_ALPHABET 27

int main(int argc, char **argv) {
    //printf("coucou je suis dans le fils \n");

    char alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ ";

    char cara;
    char message[200];
    char mot[100];
    int j = 0;
    int m = 0;
    int trouve = 0;

    while (read(atoi(argv[3]), &cara, sizeof(char)) != 0) {
        if (cara == '.' || cara == ',') {
            message[j] = cara;
        } else {
            for (int i = 0; i < SIZE_ALPHABET; i++) {
                if (cara == alphabet[i]) {
                    message[j] = alphabet[(i - atoi(argv[2]) + SIZE_ALPHABET) % SIZE_ALPHABET];
                }
            }
        }

        if (message[j] != ' ' && message[j] != '.') {
            mot[m] = message[j];

            m++;
        } else {
            mot[m] = '\0';
            m = -1;
            if (strcmp(mot, argv[1]) == 0) {
                trouve = 1;
                printf("----------(enfant) Le mot est trouvé !---------- \n");
            }
            m++;
        }
        j++;
    }

    message[j] = '\0';

    close(atoi(argv[3]));

    if (trouve) {
        printf("Message : %s\n", message);
        return atoi(argv[2]);
    } else {
        return 0;
    }

}