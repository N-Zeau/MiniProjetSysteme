#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <wait.h>

#define size 25

int existenceEtOuverture(char *nom) {
    if (access(nom, F_OK) != 0) {
        printf("Le fichier n'existe pas \n");
        exit(1);
    }

    int file = open(nom, O_RDONLY);
    if (file == -1) {
        printf("Erreur de l'ouverture du fichier \n");
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

void lectureMessage(int file, int *infos, int *ecriture) {
    char cara;
    lseek(file, 2 * sizeof(char) + 2 * sizeof(int) + infos[1], SEEK_SET);
    for (int i = 0; i < infos[0]; i++) {
        if (!read(file, &cara, sizeof(char))) {
            printf("Erreur lors de la lecture du fichier \n");
            exit(1);
        } else {
            for (int j = 0; j < size; j++)
                write(ecriture[j], &cara, sizeof(char));
        }
    }
    for (int i = 0; i < size; i++)
        close(ecriture[i]);
}

int *decryptage(char *nomFichier, char *mot) {
    int tab[size][2];
    int *tabRes = malloc(size * sizeof(int));
    char lecture[5];

    pid_t res;

    for (int i = 1; i <= size; ++i) {
        if (pipe(tab[i - 1]) == -1) {
            printf("Erreur lors de la création du pipe \n");
            exit(1);
        }

        res = fork();
        switch (res) {
            case -1:
                printf("Erreur lors de la création du processus enfant \n");
                exit(1);
                break;
            case 0:
                //printf("Le fils est créer. \n");
                sprintf(lecture, "%d", tab[i - 1][0]);

                char decal[5];
                sprintf(decal, "%d", i);
                close(tab[i - 1][1]);
                execl("./processusFils", "processusFils", mot, decal, lecture, NULL); //Declanche le programme fils
                break;
            default:
                //printf("Les fils est créer et est resté dans le père. \n");
                break;
        }
    }


    for (int i = 0; i < size; i++) {
        tabRes[i] = tab[i][1];
    }
    return tabRes;
}

int main(int argc, char **argv) {
    int file;

    if (argc != 3)
        return -1;

    file = existenceEtOuverture(argv[1]);

    verifCrypt(file);

    int *infos = afficheInformations(file);
    int *ecriture = decryptage(argv[1], argv[2]);

    lectureMessage(file, infos, ecriture);

    free(infos);
    close(file);

    int statut;
    int decalage;
    int trouve = 0;
    for (int i = 1; i <= size; i++) {
        wait(&statut);
        decalage = WEXITSTATUS(statut);
        if (decalage != 0) {
            printf("Le nom du fichier est de : %s \n", argv[1]);
            printf("Le decalage est de : %d \n", decalage);
            trouve = 1;
        }
    }

    return trouve;
}
