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

    // Boucle de lecture de caractères du pipe
    while (read(atoi(argv[3]), &cara, sizeof(char)) != 0) {
        // Si le caractère est un point ou une virgule, il est copié dans le message déchiffré sans modification
        if (cara == '.' || cara == ',') {
            message[j] = cara;
        } else {
            // Sinon, on recherche l'index du caractère dans l'alphabet et on le remplace par le caractère correspondant
            // à l'index calculé à partir de la clé de déchiffrement passée en argument
            for (int i = 0; i < SIZE_ALPHABET; i++) {
                if (cara == alphabet[i]) {
                    message[j] = alphabet[(i - atoi(argv[2]) + SIZE_ALPHABET) % SIZE_ALPHABET];
                }
            }
        }

        // Si le caractère n'est pas un espace ou un point, on le copie dans le mot en cours de construction
        if (message[j] != ' ' && message[j] != '.') {
            mot[m] = message[j];
            m++;
        } else {
            // Sinon, on termine la construction du mot en cours et on teste s'il correspond au mot recherché
            mot[m] = '\0';
            m = -1;
            if (strcmp(mot, argv[1]) == 0) {
                trouve = 1;
            }
            m++;
        }
        j++;
    }

    // On termine la construction du message déchiffré
    message[j] = '\0';

    // Fermeture du pipe
    close(atoi(argv[3]));

    // Si le mot recherché a été trouvé, on affiche le message déchiffré et on renvoie la clé de déchiffrement comme code de retour
    if (trouve) {
        printf("Message : %s\n", message);
        return atoi(argv[2]);
    } else {
        // Sinon, on renvoie 0 comme code de retour
        return 0;
    }

}