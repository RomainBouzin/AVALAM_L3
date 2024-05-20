#include <stdio.h>
#include <stdlib.h>
#include "avalam.h"
#include "moteur.h"
#include <unistd.h>

#define PROFONDEUR_MAX 6
#define INFINI 10000

typedef struct {
    int origine;
    int destination;
} CoupImportant;

// Prototypes des fonctions supplémentaires
int evaluerPosition(T_Position position, octet maCouleur);
int minimax(T_Position position, int profondeur, int maximisant, int alpha, int beta, octet maCouleur);
void placementBonusMalus(T_Position *position);
void strategieDemineur(T_Position *position, T_ListeCoups *coups, octet maCouleur, CoupImportant *coupsImportants, int *nbCoupsImportants);
void choisirCoup(T_Position currentPosition, T_ListeCoups listeCoups);

// Fonction pour obtenir la liste des coups légaux
T_ListeCoups getListeCoups(T_Position position) {
    return getCoupsLegaux(position);
}

// Fonction de placement des bonus et malus
void placementBonusMalus(T_Position *position) {
    // Placement du malus jaune à côté du bonus rouge
    if (position->cols[14].nb == 0) {
        position->cols[13].couleur = JAU; // Malus jaune
    } else {
        position->cols[15].couleur = JAU; // Malus jaune
    }

    // Placement du bonus jaune à la position 28
    position->cols[28].couleur = JAU; // Bonus jaune

    // Placement du bonus rouge au centre
    if (position->cols[14].nb == 0) {
        position->cols[14].couleur = ROU; // Bonus rouge
    } else {
        position->cols[31].couleur = ROU; // Bonus rouge
    }

    // Placement du malus rouge à côté du bonus jaune
    if (position->cols[27].nb == 0) {
        position->cols[27].couleur = ROU; // Malus rouge
    } else {
        position->cols[29].couleur = ROU; // Malus rouge
    }
}

// Stratégie du démineur
void strategieDemineur(T_Position *position, T_ListeCoups *coups, octet maCouleur, CoupImportant *coupsImportants, int *nbCoupsImportants) {
    for (int i = 0; i < coups->nb; i++) {
        T_Coup coup = coups->coups[i];
        if (position->cols[coup.origine].nb == 1 && position->cols[coup.destination].nb == 1 && 
            position->cols[coup.origine].couleur != maCouleur && position->cols[coup.destination].couleur != maCouleur) {
            coupsImportants[*nbCoupsImportants].origine = coup.origine;
            coupsImportants[*nbCoupsImportants].destination = coup.destination;
            (*nbCoupsImportants)++;
        }
    }
}

// Fonction d'évaluation de la position
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

// Algorithme MinMax avec élagage alpha-beta
int minimax(T_Position position, int profondeur, int maximisant, int alpha, int beta, octet maCouleur) {
    if (profondeur == 0) {
        return evaluerPosition(position, maCouleur);
    }

    T_ListeCoups listeCoups = getListeCoups(position);
    if (listeCoups.nb == 0) {
        return evaluerPosition(position, maCouleur);
    }

    if (maximisant) {
        int maxEval = -INFINI;
        for (int i = 0; i < listeCoups.nb; i++) {
            T_Coup coup = listeCoups.coups[i];
            T_Position nouvellePosition = jouerCoup(position, coup.origine, coup.destination);
            if (nouvellePosition.cols[coup.destination].nb > 5) continue; // Vérification du nombre de jetons
            int eval = minimax(nouvellePosition, profondeur - 1, 0, alpha, beta, maCouleur);
            maxEval = (eval > maxEval) ? eval : maxEval;
            alpha = (alpha > eval) ? alpha : eval;
            if (beta <= alpha) {
                break;
            }
        }
        return maxEval;
    } else {
        int minEval = INFINI;
        for (int i = 0; i < listeCoups.nb; i++) {
            T_Coup coup = listeCoups.coups[i];
            T_Position nouvellePosition = jouerCoup(position, coup.origine, coup.destination);
            if (nouvellePosition.cols[coup.destination].nb > 5) continue; // Vérification du nombre de jetons
            int eval = minimax(nouvellePosition, profondeur - 1, 1, alpha, beta, maCouleur);
            minEval = (eval < minEval) ? eval : minEval;
            beta = (beta < eval) ? beta : eval;
            if (beta <= alpha) {
                break;
            }
        }
        return minEval;
    }
}

// Fonction principale pour déterminer le meilleur coup
void choisirCoup(T_Position currentPosition, T_ListeCoups listeCoups) {
    int meilleurScore = -INFINI;
    int meilleurCoup = 0;
    octet maCouleur = currentPosition.trait;

    CoupImportant coupsImportants[NBCASES];
    int nbCoupsImportants = 0;

    // Application de la stratégie du démineur
    strategieDemineur(&currentPosition, &listeCoups, maCouleur, coupsImportants, &nbCoupsImportants);

    // Si des coups importants sont trouvés, on les utilise pour le MinMax
    if (nbCoupsImportants > 0) {
        for (int i = 0; i < nbCoupsImportants; i++) {
            CoupImportant coup = coupsImportants[i];
            T_Position nouvellePosition = jouerCoup(currentPosition, coup.origine, coup.destination);
            if (nouvellePosition.cols[coup.destination].nb > 5) continue; // Vérification du nombre de jetons
            int score = minimax(nouvellePosition, PROFONDEUR_MAX - 1, 0, -INFINI, INFINI, maCouleur);

            if (score > meilleurScore) {
                meilleurScore = score;
                meilleurCoup = i;
            }
        }
    } else {
        // Sinon, on procède avec tous les coups disponibles
        for (int i = 0; i < listeCoups.nb; i++) {
            T_Coup coup = listeCoups.coups[i];
            T_Position nouvellePosition = jouerCoup(currentPosition, coup.origine, coup.destination);
            if (nouvellePosition.cols[coup.destination].nb > 5) continue; // Vérification du nombre de jetons
            int score = minimax(nouvellePosition, PROFONDEUR_MAX - 1, 0, -INFINI, INFINI, maCouleur);

            if (score > meilleurScore) {
                meilleurScore = score;
                meilleurCoup = i;
            }
        }
    }

    // Écrire le coup choisi
    ecrireIndexCoup(meilleurCoup);
}
