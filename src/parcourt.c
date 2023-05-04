#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <wait.h>

int main(int argc, char **argv) {
    // Vérifie s'il y a bien 3 arguments
    if (argc != 3)
        return -1;

    // Récupération du nom du répertoire et du mot à chercher
    char *nomRepertoire = argv[1];
    char *mot = argv[2];

    // Ouverture du répertoire
    DIR *dir;
    struct dirent *entry;
    dir = opendir(nomRepertoire);
    if (dir == NULL) {
        perror("Erreur lors de l'ouverture du repertoire");
        return 1;
    }

    pid_t res;

    char fichier[1024];
    int nbFichier = 0;

    // Itération sur tous les fichiers réguliers du répertoire
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG) {
            res = fork();
            sprintf(fichier, "%s/%s", nomRepertoire, entry->d_name);
            switch (res) {
                case -1:
                    printf("(Parcourt) Erreur lors de la création du processus enfant \n");
                    break;
                case 0:
                    // Lancement du programme main sur le fichier courant
                    execl("./main", "main", fichier, mot, NULL);
                    break;
                default:
                    break;
            }
            nbFichier++;
        }
    }
    closedir(dir);

    int statut;
    int trouve = 0;

    // Attente de la fin de tous les processus enfants
    for (int i = 0; i < nbFichier; i++) {
        wait(&statut);
        if (WEXITSTATUS(statut) != 0) {
            // Le mot a été trouvé dans un fichier, on arrête la recherche
            trouve = 1;
        }
    }

    if (!trouve)
        printf("--- Erreur le nom n'est pas trouvé --- \n");

    return 0;
}