#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

int existenceEtOuverture(char *nom) {
    if (access(nom, F_OK) != 0) {
        printf("Le fichier n'existe pas \n");
        fflush(stdout);
        exit(1);
    }

    int file = open(nom, O_RDONLY);
    if (file == -1) {
        printf("Erruer de l'ouverture du fichier");
        exit(1);
    }

    return file;

}

void verifCrypt(int file) {
    char cr[2];
    if (read(file, &cr, 2 * sizeof(char))) {
        if (cr[0] != 'C' || cr[1] != 'R') {
            printf("Le fichier n'est pas crypté \n");
            close(file);
            exit(1);
        }
    }
}

int *afficheInformations(int file) {
    int *tab = malloc(2 * sizeof(int));
    int infos[2];

    if (read(file, &infos, 2 * sizeof(int))) {
        if (!(infos[0] >= 0 && infos[0] < lseek(file, 0, SEEK_END))) {
            printf("Taille du fichier incorrecte \n");
            exit(1);
        } else {
            tab[0] = infos[0];
        }

        if (!(infos[1] > 0 && infos[1] < lseek(file, 0, SEEK_END) && infos[1] < infos[0])) {
            printf("Nombre d'octets avant le message hors en-tête incorrecte \n");
            exit(1);
        } else {
            tab[1] = infos[1];
        }
    }
    return tab;
}

void lectureMessage(int file, int *infos) {
    char cara;
    lseek(file, 2 * sizeof(char) + 2 * sizeof(int) + infos[1], SEEK_SET);
    for (int i = 0; i < infos[0]; i++) {
        if (!read(file, &cara, sizeof(char))) {
            printf("Erreur de la lecture du fichier");
            exit(1);
        } else {
            printf("%c", cara);
        }
    }
    printf(" \n");
}


int main(int argc, char **argv) {
    int file;
    if (argc == 2){
        file = existenceEtOuverture(argv[1]);
    } else {
        exit(1);
    }

    verifCrypt(file);
    int *infos = afficheInformations(file);
    lectureMessage(file, infos);
    free(infos);

    close(file);


    return 0;
}




