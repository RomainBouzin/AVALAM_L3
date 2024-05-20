#include <stdio.h>
#include <stdlib.h>
#include "avalam.h"
#include "moteur.h"

#define PROFONDEUR_MAX 4
#define INFINI 10000

// Prototypes des fonctions supplémentaires
int evaluerPosition(T_Position position, octet maCouleur);
int minimax(T_Position position, int profondeur, int maximisant, int alpha, int beta, octet maCouleur);
void choisirCoup(T_Position currentPosition, T_ListeCoups listeCoups);

// Cette fonction est déjà déclarée dans avalam.h
// T_Position jouerCoup(T_Position p, octet origine, octet destination);

// Fonction pour obtenir la liste des coups légaux
T_ListeCoups getListeCoups(T_Position position) {
    T_ListeCoups listeCoups = getCoupsLegaux(position);
    return listeCoups;
}

void choisirCoup(T_Position currentPosition, T_ListeCoups listeCoups) {
    int meilleurScore = -INFINI;
    int meilleurCoup = 0;
    octet maCouleur = currentPosition.trait;

    for (int i = 0; i < listeCoups.nb; i++) {
        T_Coup coup = listeCoups.coups[i];
        T_Position nouvellePosition = jouerCoup(currentPosition, coup.origine, coup.destination);
        int score = minimax(nouvellePosition, PROFONDEUR_MAX - 1, 0, -INFINI, INFINI, maCouleur);

        if (score > meilleurScore) {
            meilleurScore = score;
            meilleurCoup = i;
        }
    }

    ecrireIndexCoup(meilleurCoup);
}

int minimax(T_Position position, int profondeur, int maximisant, int alpha, int beta, octet maCouleur) {
    if (profondeur == 0) {
        return evaluerPosition(position, maCouleur);
    }

    T_ListeCoups listeCoups = getListeCoups(position);

    if (listeCoups.nb == 0) {
        return evaluerPosition(position, maCouleur);
    }

    if (maximisant) {
        int meilleurScore = -INFINI;
        for (int i = 0; i < listeCoups.nb; i++) {
            T_Coup coup = listeCoups.coups[i];
            T_Position nouvellePosition = jouerCoup(position, coup.origine, coup.destination);
            int score = minimax(nouvellePosition, profondeur - 1, 0, alpha, beta, maCouleur);
            if (score > meilleurScore) {
                meilleurScore = score;
            }
            alpha = (alpha > score) ? alpha : score;
            if (beta <= alpha) {
                break;
            }
        }
        return meilleurScore;
    } else {
        int meilleurScore = INFINI;
        for (int i = 0; i < listeCoups.nb; i++) {
            T_Coup coup = listeCoups.coups[i];
            T_Position nouvellePosition = jouerCoup(position, coup.origine, coup.destination);
            int score = minimax(nouvellePosition, profondeur - 1, 1, alpha, beta, maCouleur);
            if (score < meilleurScore) {
                meilleurScore = score;
            }
            beta = (beta < score) ? beta : score;
            if (beta <= alpha) {
                break;
            }
        }
        return meilleurScore;
    }
}

int evaluerPosition(T_Position position, octet maCouleur) {
    int score = 0;
    for (int i = 0; i < NBCASES; i++) {
        if (position.cols[i].nb > 0) {
            if (position.cols[i].couleur == maCouleur) {
                score += position.cols[i].nb;
            } else {
                score -= position.cols[i].nb;
            }
        }
    }
    return score;
}


