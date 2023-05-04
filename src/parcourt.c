#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <wait.h>

int main(int argc, char **argv) {

    if (argc != 3)
        return -1;

    char *nomRepertoire = argv[1];
    char *mot = argv[2];


    DIR *dir;
    struct dirent *entry;

    //Ouvre le repertoire
    dir = opendir(nomRepertoire);

    if (dir == NULL) {
        perror("Erreur lors de l'ouverture du repertoire");
        return 1;
    }

    pid_t res;
    char *fichier;
    int nbFichier = 0;

    //Itération sur tout les repertoires
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG) {
            res = fork();
            fichier = strdup(nomRepertoire);
            strcat(fichier, entry->d_name);
            switch (res) {
                case -1:
                    printf("(Parcourt) Erreur lors de la création du processus enfant \n");
                    break;
                case 0:
                    execl("./main", "main", fichier, mot, NULL); //Déclanche le programme main
                    break;
                default:
                    break;
            }
            nbFichier++;
            if (!fichier)
                free(fichier);
        }
    }
    closedir(dir);

    int statut;
    int trouve = 0;

    for (int i = 0; i < nbFichier; i++) {
        wait(&statut);
        if (WEXITSTATUS(statut) != 0) { //Test si le nom a été trouvé
            trouve = 1;
        }
    }

    if (!trouve)
        printf("--- Erreur le nom n'est pas trouvé --- \n");


    return 0;
}