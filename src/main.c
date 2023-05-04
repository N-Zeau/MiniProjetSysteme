#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <wait.h>

#define size 25

// Fonction qui vérifie si le fichier existe et peut être ouvert en lecture seule
int existenceEtOuverture(char *nom) {
    if (access(nom, F_OK) != 0) { // Si le fichier n'existe pas
        printf("Le fichier n'existe pas \n");
        exit(1);
    }

    int file = open(nom, O_RDONLY); // Ouvre le fichier en lecture seule
    if (file == -1) { // Si l'ouverture du fichier échoue
        printf("Erreur de l'ouverture du fichier \n");
        exit(1);
    }

    return file; // Retourne le descripteur de fichier
}

// Fonction qui vérifie si le fichier est crypté
void verifCrypt(int file) {
    char cr[2];
    if (read(file, &cr, 2 * sizeof(char))) { // Lit les deux premiers caractères du fichier
        if (cr[0] != 'C' || cr[1] != 'R') { // Si ces deux caractères ne correspondent pas à "CR"
            printf("Le fichier n'est pas crypté \n");
            close(file);
            exit(1);
        }
    }
}

// Fonction qui lit et vérifie les informations de l'en-tête du fichier
int *afficheInformations(int file) {
    int *tab = malloc(2 * sizeof(int)); // Alloue de la mémoire pour un tableau de deux entiers
    int infos[2];

    if (read(file, &infos, 2 * sizeof(int))) { // Lit les deux entiers de l'en-tête
        if (!(infos[0] >= 0 && infos[0] < lseek(file, 0, SEEK_END))) { // Si la taille du fichier n'est pas valide
            printf("Taille du fichier incorrecte \n");
            exit(1);
        } else {
            tab[0] = infos[0]; // Stocke la taille du message dans le tableau
        }

        if (!(infos[1] > 0 && infos[1] < lseek(file, 0, SEEK_END) &&
              infos[1] < infos[0])) { // Si le nombre d'octets avant le message hors en-tête est invalide
            printf("Nombre d'octets avant le message hors en-tête incorrecte \n");
            exit(1);
        } else {
            tab[1] = infos[1]; // Stocke le nombre d'octets avant le message dans le tableau
        }
    }
    return tab; // Retourne le tableau d'informations
}

// Fonction qui lit le message du fichier et le déchiffre en utilisant des processus fils
void lectureMessage(int file, int *infos, int *ecriture) {
    char cara;
    lseek(file, 2 * sizeof(char) + 2 * sizeof(int) + infos[1],
          SEEK_SET); // Se positionne au début du message dans le fichier
    for (int i = 0; i < infos[0]; i++) { // Pour chaque caractère dans le message
        if (!read(file, &cara, sizeof(char))) { // Si la lecture du fichier échoue
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
    // Allocation dynamique de mémoire pour le tableau de résultats
    int *tabRes = malloc(size * sizeof(int));
    // Déclaration d'un tableau de caractères pour la lecture des résultats
    char lecture[5];

    // Déclaration d'un processus
    pid_t res;

    // Boucle pour créer les pipes et les processus fils correspondants
    for (int i = 1; i <= size; ++i) {
        // Création d'un nouveau pipe pour chaque processus fils
        if (pipe(tab[i - 1]) == -1) {
            printf("Erreur lors de la création du pipe \n");
            exit(1);
        }

        // Création d'un nouveau processus fils
        res = fork();
        switch (res) {
            case -1:
                printf("Erreur lors de la création du processus enfant \n");
                exit(1);
                break;
            case 0:
                // Déclaration de la variable lecture en tant que chaîne de caractères contenant le descripteur de lecture du pipe
                sprintf(lecture, "%d", tab[i - 1][0]);

                // Déclaration de la variable decal en tant que chaîne de caractères contenant la valeur du décalage
                char decal[5];
                sprintf(decal, "%d", i);

                // Fermeture du descripteur d'écriture du pipe
                close(tab[i - 1][1]);

                // Exécution du programme processusFils avec les arguments mot, decal et lecture
                execl("./processusFils", "processusFils", mot, decal, lecture, NULL);

                // Sortie en cas d'erreur lors de l'exécution du programme
                break;
            default:
                // Attente de la fin de l'exécution du processus fils et retour dans le processus parent
                break;
        }
    }

    // Boucle pour lire les résultats de la décryption depuis les pipes
    for (int i = 0; i < size; i++) {
        // Lecture du résultat depuis le descripteur de lecture du pipe correspondant
        tabRes[i] = tab[i][1];
    }

    // Retourne le tableau de résultats
    return tabRes;

}

int main(int argc, char **argv) {
    int file;

    // Vérifie le nombre d'arguments
    if (argc != 3)
        return -1;

    // Ouvre le fichier et vérifie qu'il existe
    file = existenceEtOuverture(argv[1]);

    // Vérifie que le fichier est bien crypté
    verifCrypt(file);

    // Récupère les informations de l'en-tête du fichier
    int *infos = afficheInformations(file);

    // Décrypte le message en utilisant plusieurs processus fils
    int *ecriture = decryptage(argv[1], argv[2]);

    // Lit le message décrypté dans le fichier et l'affiche à l'écran
    lectureMessage(file, infos, ecriture);

    // Libère la mémoire allouée pour les informations de l'en-tête
    free(infos);

    // Ferme le fichier
    close(file);

    // Attends que tous les processus fils soient terminés et récupère leur statut
    int statut;
    int decalage;
    int trouve = 0;
    for (int i = 1; i <= size; i++) {
        wait(&statut);

        // Récupère le décalage retourné par le processus fils
        decalage = WEXITSTATUS(statut);

        // Si le décalage est différent de 0, affiche le nom du fichier et le décalage
        if (decalage != 0) {
            printf("Le nom du fichier est de : %s \n", argv[1]);
            printf("Le decalage est de : %d \n", decalage);
            trouve = 1;
        }
    }

    // Retourne 0 si aucun décalage n'a été trouvé, 1 sinon
    return trouve;

}
