// TODO: Utiliser les optimisations à la compilation (-O3) et les tester
#include "../include/avalam.h"
#include "../include/moteur.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

/****************************************************************************************************************/
/*  Nom : rechercheCoup                                                                                         */
/*  Objectif : Rechercher l'index d'un coup avec une case d'origine et une case de destination données          */
/*  Paramètres en entrée :                                                                                      */
/*      - listeCoups : la liste des coups légaux                                                                */
/*      - origine : la case d'origine du coup                                                                   */
/*      - destination : la case de destination du coup                                                          */
/*  Paramètres en sortie :                                                                                      */
/*      - retourne l'index du coup trouvé, -1 sinon                                                             */
/****************************************************************************************************************/
int rechercheCoup(T_ListeCoups listeCoups, octet origine, octet destination) {
    printf("RC: o:%d d:%d\n", origine, destination);
    int moy = 0;
    int debut = 0;
    int size = 8 * NBCASES;
    while (debut <= size) {
        moy = (debut + size) / 2;
        if (listeCoups.coups[moy].origine == 0 && moy != 0) {
            size = moy - 1;
        } else if (listeCoups.coups[moy].origine == origine) {
            if (listeCoups.coups[moy].destination > destination) {
                while (listeCoups.coups[moy].origine == origine && listeCoups.coups[moy].destination > destination)
                    moy--;
            } else if (listeCoups.coups[moy].destination < destination) {
                while (listeCoups.coups[moy].origine == origine && listeCoups.coups[moy].destination < destination)
                    moy++;
            }
            if (listeCoups.coups[moy].origine == origine && listeCoups.coups[moy].destination == destination) {
                return moy;
            }
            return -1;
        } else if (listeCoups.coups[moy].origine < origine) {
            debut = moy + 1;
        } else {
            size = moy - 1;
        }
    }
    return -1;
}

/****************************************************************************************************************/
/*  Nom : placerBonus                                                                                           */
/*  Objectif : Placer les bonus/malus selon l'état actuel du plateau                                            */
/*  Paramètres en entrée :                                                                                      */
/*      - currentPosition : la position actuelle du plateau                                                     */
/*      - listeCoups : la liste des coups légaux                                                                */
/*  Paramètres en sortie :                                                                                      */
/*      - retourne l'index du coup à jouer pour placer le bonus/malus                                           */
/****************************************************************************************************************/
int placerBonus(T_Position currentPosition, T_ListeCoups listeCoups) {
    int coup;
    octet tempo;
    switch (currentPosition.numCoup) {
        case 3: // Malus rouge
            coup = rechercheCoup(listeCoups, 2, 2);
            return coup;

        case 2: // Malus jaune
            coup = rechercheCoup(listeCoups, 1, 1);
            return coup;

        case 1: // Bonus rouge
            octet bonusJaune = currentPosition.evolution.bonusJ;
            for (int i = 0; i < 2; i++) {
                if (19 == bonusJaune || 28 == bonusJaune)
                    tempo = bonusJaune;
            }
            if (tempo == 28) {
                coup = rechercheCoup(listeCoups, 22, 22);
            } else if (tempo == 19) {
                coup = rechercheCoup(listeCoups, 25, 25);
            } else {
                coup = rechercheCoup(listeCoups, 22, 22);
            }
            return coup;

        case 0: // Bonus jaune
            if (currentPosition.trait == JAU) {
                coup = rechercheCoup(listeCoups, 19, 19);
            }
            return coup;

        default:
            return -1;
    }
}

/****************************************************************************************************************/
/*  Nom : zonesafe                                                                                              */
/*  Objectif : Vérifier que les bonus sont sécurisés pour déterminer s'il faut interrompre l'ouverture          */
/*  Paramètres en entrée :                                                                                      */
/*      - currentPosition : la position actuelle du plateau                                                     */
/*  Paramètres en sortie :                                                                                      */
/*      - retourne 1 si les bonus sont sécurisés, -1 sinon                                                      */
/****************************************************************************************************************/
int zonesafe(T_Position currentPosition) {
    octet bonus = (currentPosition.trait == JAU) ? currentPosition.evolution.bonusJ : currentPosition.evolution.bonusR;
    if (currentPosition.cols[bonus].couleur != currentPosition.trait)
        return -1;
    else if (bonus != 28 && bonus != 20 && bonus != 19 && bonus != 27)
        return -1;
    return 1;
}

/****************************************************************************************************************/
/*  Nom : ouverture                                                                                             */
/*  Objectif : Effectuer les premiers coups pour sécuriser les bonus ou récupérer ceux de l'adversaire          */
/*  Paramètres en entrée :                                                                                      */
/*      - currentPosition : la position actuelle du plateau                                                     */
/*      - listeCoupsSoi : la liste des coups légaux pour le joueur                                              */
/*  Paramètres en sortie :                                                                                      */
/*      - retourne l'index du coup à jouer pour l'ouverture                                                     */
/****************************************************************************************************************/
int ouverture(T_Position currentPosition, T_ListeCoups listeCoupsSoi) {
    if (zonesafe(currentPosition) == -1) {
        printf("Erreur zonesafe == -1 (?)\n");
        return -1;
    }

    if (currentPosition.trait == JAU) {
        if (currentPosition.evolution.bonusJ == 28) {
            switch (currentPosition.numCoup) {
                case 4:
                    return rechercheCoup(listeCoupsSoi, 21, 29);
                case 6:
                    return rechercheCoup(listeCoupsSoi, 29, 20);
                case 8:
                    return rechercheCoup(listeCoupsSoi, 20, 28);
                default:
                    break;
            }
        } else {
            switch (currentPosition.numCoup) {
                case 4:
                    return rechercheCoup(listeCoupsSoi, 26, 18);
                case 6:
                    if ((currentPosition.cols[19].nb == 1 && currentPosition.cols[27].nb == 1)) {
                        return rechercheCoup(listeCoupsSoi, 18, 27);
                    } else if (currentPosition.cols[18].couleur == ROU && currentPosition.cols[27].nb == 0 && currentPosition.cols[18].nb == 3) {
                        if (currentPosition.evolution.bonusR == 11) {
                            return rechercheCoup(listeCoupsSoi, 18, 25);
                        } else {
                            return rechercheCoup(listeCoupsSoi, 18, 11);
                        }
                    } else if (currentPosition.cols[19].nb == 2) {
                        return rechercheCoup(listeCoupsSoi, 18, 19);
                    } else if (currentPosition.cols[27].nb == 2 && currentPosition.cols[27].couleur == JAU) {
                        return rechercheCoup(listeCoupsSoi, 35, 40);
                    } else if (currentPosition.cols[27].nb == 2 && currentPosition.cols[27].couleur == ROU) {
                        return rechercheCoup(listeCoupsSoi, 18, 27);
                    } else if (currentPosition.cols[18].nb == 3 && currentPosition.cols[27].nb == 0) {
                        if (currentPosition.cols[11].nb != 0) {
                            return rechercheCoup(listeCoupsSoi, 18, 11);
                        } else {
                            return rechercheCoup(listeCoupsSoi, 18, 25);
                        }
                    } else {
                        return -1;
                    }
                case 8:
                    if (currentPosition.cols[19].nb >= 3 && currentPosition.cols[19].couleur == JAU)
                        return -1;
                    else {
                        return rechercheCoup(listeCoupsSoi, 27, 19);
                    }
                default:
                    break;
            }
        }
    } else if (currentPosition.trait == ROU) {
        if (currentPosition.evolution.bonusR == 22) {
            switch (currentPosition.numCoup) {
                case 5:
                    if ((currentPosition.cols[22].nb == 1 && currentPosition.cols[21].nb == 1)) {
                        return rechercheCoup(listeCoupsSoi, 22, 21);
                    } else if ((currentPosition.cols[21].nb == 0 && currentPosition.cols[29].nb == 2)) {
                        return rechercheCoup(listeCoupsSoi, 22, 29);
                    }
                    break;
                case 7:
                    if (currentPosition.evolution.bonusR == 29 && currentPosition.cols[29].nb == 3)
                        return rechercheCoup(listeCoupsSoi, 29, 28);
                    else if (currentPosition.evolution.bonusR == 21 && currentPosition.cols[21].nb >= 2 &&
                             currentPosition.cols[21].couleur == ROU && currentPosition.cols[29].couleur == ROU)
                        return rechercheCoup(listeCoupsSoi, 21, 29);
                    else if (currentPosition.evolution.bonusR == 21 && currentPosition.cols[21].nb >= 2 &&
                             currentPosition.cols[28].nb == 0) {
                        if (currentPosition.cols[29].nb == 2 && currentPosition.cols[29].couleur == JAU) {
                            return rechercheCoup(listeCoupsSoi, 36, 29);
                        } else {
                            return rechercheCoup(listeCoupsSoi, 12, 30);
                        }
                    } else {
                        return -1;
                    }
                case 9:
                    return rechercheCoup(listeCoupsSoi, 20, 28);
                default:
                    break;
            }
        } else {
            switch (currentPosition.numCoup) {
                case 5:
                    if ((currentPosition.cols[25].nb == 1 && currentPosition.cols[26].nb == 1)) {
                        return rechercheCoup(listeCoupsSoi, 25, 26);
                    } else if ((currentPosition.cols[26].nb == 0 && currentPosition.cols[18].nb == 2)) {
                        return rechercheCoup(listeCoupsSoi, 25, 18);
                    }
                    break;
                case 7:
                    if (currentPosition.evolution.bonusR == 18 && currentPosition.cols[18].nb == 3)
                        return rechercheCoup(listeCoupsSoi, 18, 19);
                    else if (currentPosition.evolution.bonusR == 26 && currentPosition.cols[26].nb >= 2 &&
                             currentPosition.cols[26].couleur == ROU && currentPosition.cols[18].couleur == ROU)
                        return rechercheCoup(listeCoupsSoi, 26, 18);
                    else if (currentPosition.evolution.bonusR == 26 && currentPosition.cols[26].nb >= 2 &&
                             currentPosition.cols[19].nb == 0) {
                        if (currentPosition.cols[18].nb == 2 && currentPosition.cols[18].couleur == JAU) {
                            return rechercheCoup(listeCoupsSoi, 11, 18);
                        } else {
                            return rechercheCoup(listeCoupsSoi, 35, 27);
                        }
                    } else {
                        return -1;
                    }
                case 9:
                    return rechercheCoup(listeCoupsSoi, 27, 19);
                default:
                    break;
            }
        }
    }
    return -1;
}

/****************************************************************************************************************/
/*  Nom : evaluerScorePlateau                                                                                   */
/*  Objectif : Évaluer le score du plateau pour déterminer l'avantage du joueur                                 */
/*  Paramètres en entrée :                                                                                      */
/*      - currentPosition : la position actuelle du plateau                                                     */
/*  Paramètres en sortie :                                                                                      */
/*      - retourne le score du plateau (positif pour avantage joueur, négatif pour avantage adversaire)         */
/****************************************************************************************************************/
float evaluerScorePlateau(T_Position currentPosition) {
    float evaluation = 0;

    // Liste des paramètres
    int score_soi, score_adv, score5_soi, score5_adv;
    int score_soi_coeff = 1;
    int score_adv_coeff = 1;
    int score5_soi_coeff = 1000;
    int score5_adv_coeff = 1000;

    // On évalue le score
    T_Score score = evaluerScore(currentPosition);
    if (JAU == currentPosition.trait) {
        score_soi = (int)score.nbJ;
        score_adv = (int)score.nbR;
        score5_soi = (int)score.nbJ5;
        score5_adv = (int)score.nbR5;
    } else {
        score_soi = (int)score.nbR;
        score_adv = (int)score.nbJ;
        score5_soi = (int)score.nbR5;
        score5_adv = (int)score.nbJ5;
    }

    evaluation = score_soi * score_soi_coeff - score_adv * score_adv_coeff +
                 score5_soi * score5_soi_coeff - score5_adv * score5_adv_coeff;

    return evaluation;
}

/****************************************************************************************************************/
/*  Nom : evaluerScoreCoup                                                                                      */
/*  Objectif : Évaluer le score d'un coup pour déterminer son impact sur la partie                              */
/*  Paramètres en entrée :                                                                                      */
/*      - currentPosition : la position actuelle du plateau                                                     */
/*      - origine : la case d'origine du coup                                                                   */
/*      - destination : la case de destination du coup                                                          */
/*  Paramètres en sortie :                                                                                      */
/*      - retourne le score du coup (positif pour avantage joueur, négatif pour avantage adversaire)            */
/****************************************************************************************************************/
float evaluerScoreCoup(T_Position currentPosition, int origine, int destination) {
    float evaluation = 0;
    octet traitPerso = currentPosition.trait;
    T_Voisins voisinOrigine = getVoisins(origine);
    T_Voisins voisinDestination = getVoisins(destination);
    int nb_evaluations = 0; // Nombre de fois où l'évaluation a été modifiée, pour le debug

    // Si la tour que l'on déplace est de la couleur adverse
    if (currentPosition.cols[origine].couleur != traitPerso) {
        // Tour adverse sur soi -100
        if (currentPosition.cols[destination].couleur == traitPerso) {
            nb_evaluations++;
            evaluation -= 98;
        }

        // Tour 5 adverse -100
        if ((currentPosition.cols[origine].nb + currentPosition.cols[destination].nb) == 5) {
            nb_evaluations++;
            evaluation -= 100;
        }

        // Isoler pion adverse -100
        if (voisinDestination.nb - 1 == 0) {
            nb_evaluations++;
            evaluation -= 99;
        }

        // Tour adverse à moins de voisins à la destination
        if (voisinOrigine.nb > voisinDestination.nb - 1) {
            nb_evaluations++;
            evaluation -= 50;
        }

        // Averse sur adverse 68
        if (currentPosition.cols[destination].couleur != traitPerso) {
            nb_evaluations++;
            evaluation += 50;
        }

        // Un bonus pour l'adversaire est en jeu
        if (origine == currentPosition.evolution.bonusJ || origine == currentPosition.evolution.bonusR ||
            destination == currentPosition.evolution.bonusJ || destination == currentPosition.evolution.bonusR) {
            nb_evaluations++;
            evaluation -= 70;
        }

        // Un malus contre l'adversaire est en jeu
        if (origine == currentPosition.evolution.malusJ || origine == currentPosition.evolution.malusR ||
            destination == currentPosition.evolution.malusJ || destination == currentPosition.evolution.malusR) {
            nb_evaluations++;
            evaluation = 0;
        }
    } else {
        // Si la tour que l'on déplace est de notre couleur
        // Soi sur adverse 58
        if (currentPosition.cols[destination].couleur != traitPerso) {
            nb_evaluations++;
            evaluation += 50;
        } else {
            // Soi sur soi
            nb_evaluations++;
            evaluation -= 30;
        }

        // Tour alliée à moins de voisins à la destination
        if (voisinOrigine.nb > voisinDestination.nb - 1) {
            nb_evaluations++;
            evaluation += 50;
        }

        // Tour de 5 sur son propre pion 90
        if ((currentPosition.cols[origine].nb + currentPosition.cols[destination].nb) == 5 &&
            currentPosition.cols[destination].couleur == traitPerso) {
            nb_evaluations++;
            evaluation += 99;
        }

        // Tour de 4 si aucune tour de 1 à côté de destination
        if ((currentPosition.cols[origine].nb + currentPosition.cols[destination].nb) == 4 &&
            currentPosition.cols[origine].couleur == traitPerso) {
            int i = 0;
            for (i = 0; i < voisinDestination.nb; i++) {
                if (currentPosition.cols[voisinDestination.cases[i]].nb == 1 && voisinDestination.cases[i] != origine) {
                    break;
                }
            }
            if (currentPosition.cols[voisinDestination.cases[i]].nb != 1) {
                nb_evaluations++;
                evaluation += 80;
            }
        }

        // Isoler tour alliée
        if (voisinDestination.nb - 1 == 0 && currentPosition.cols[voisinDestination.cases[0]].couleur != traitPerso) {
            nb_evaluations++;
            evaluation += 110;
        }

        // Tour 5 sur pion adverse 100
        if ((currentPosition.cols[origine].nb + currentPosition.cols[destination].nb) == 5 &&
            currentPosition.cols[destination].couleur != traitPerso) {
            nb_evaluations++;
            evaluation += 100;
        }

        // Un bonus en notre faveur est en jeu
        if (origine == currentPosition.evolution.bonusJ || origine == currentPosition.evolution.bonusR ||
            destination == currentPosition.evolution.bonusJ || destination == currentPosition.evolution.bonusR) {
            nb_evaluations++;
            evaluation += 95;
        }

        // Un malus contre nous est en jeu
        if (origine == currentPosition.evolution.malusJ || origine == currentPosition.evolution.malusR ||
            destination == currentPosition.evolution.malusJ || destination == currentPosition.evolution.malusR) {
            nb_evaluations++;
            evaluation = 0;
        }
    }

    // Tour de 3 si tour de 2 à côté de destination -90
    if ((currentPosition.cols[origine].nb + currentPosition.cols[destination].nb == 3)) {
        for (int i = 0; i < voisinDestination.nb; i++) {
            if (currentPosition.cols[voisinDestination.cases[i]].nb == 2 && voisinDestination.cases[i] != origine &&
                currentPosition.cols[voisinDestination.cases[i]].couleur != currentPosition.trait) {
                nb_evaluations++;
                evaluation -= 105;
                break;
            }
        }
    }

    // Tour de 2 si tour de 3 à côté de destination -90
    if ((currentPosition.cols[origine].nb + currentPosition.cols[destination].nb == 2)) {
        for (int i = 0; i < voisinDestination.nb; i++) {
            if (currentPosition.cols[voisinDestination.cases[i]].nb == 3 && voisinDestination.cases[i] != origine &&
                currentPosition.cols[voisinDestination.cases[i]].couleur != currentPosition.trait) {
                nb_evaluations++;
                evaluation -= 105;
                break;
            }
        }
    }

    // Tour de 4 si tour de 1 à côté de destination -90
    if ((currentPosition.cols[origine].nb + currentPosition.cols[destination].nb == 4)) {
        for (int i = 0; i < voisinDestination.nb; i++) {
            if (currentPosition.cols[voisinDestination.cases[i]].nb == 1 && voisinDestination.cases[i] != origine &&
                currentPosition.cols[voisinDestination.cases[i]].couleur != currentPosition.trait) {
                nb_evaluations++;
                evaluation -= 105;
                break;
            }
        }
    }

    return evaluation;
}

/****************************************************************************************************************/
/*  Nom : evaluerScoreGen                                                                                       */
/*  Objectif : Évaluer le score général d'un coup en comparant les scores du plateau avant et après le coup      */
/*  Paramètres en entrée :                                                                                      */
/*      - currentPosition : la position actuelle du plateau                                                     */
/*      - nextPosition : la position du plateau après le coup                                                   */
/*      - origine : la case d'origine du coup                                                                   */
/*      - destination : la case de destination du coup                                                          */
/*  Paramètres en sortie :                                                                                      */
/*      - retourne le score général du coup (positif pour avantage joueur, négatif pour avantage adversaire)    */
/****************************************************************************************************************/
float evaluerScoreGen(T_Position currentPosition, T_Position nextPosition, int origine, int destination) {
    float currentPositionScore = evaluerScorePlateau(currentPosition);
    float nextPositionScore = evaluerScorePlateau(nextPosition);
    float scorePlateau = fabs(currentPositionScore - nextPositionScore);

    if (currentPositionScore > nextPositionScore) {
        scorePlateau = -scorePlateau;
    }

    return scorePlateau + evaluerScoreCoup(currentPosition, origine, destination);
}

/****************************************************************************************************************/
/*  Nom : choisirCoup                                                                                           */
/*  Objectif : Choisir le meilleur coup à jouer en utilisant l'évaluation des coups et des ouvertures           */
/*  Paramètres en entrée :                                                                                      */
/*      - currentPosition : la position actuelle du plateau                                                     */
/*      - listeCoups : la liste des coups légaux possibles                                                      */
/****************************************************************************************************************/
void choisirCoup(T_Position currentPosition, T_ListeCoups listeCoups) {
    printf("\033[0;31m\n"); // Couleur rouge pour plus de lisibilité des messages de débogage
    int result = -1;
    float max_temp = -INFINITY;
    float score_temp;
    T_Position tempPlateau;

    // Gestion des bonus/malus
    if (currentPosition.numCoup < 4) {
        result = placerBonus(currentPosition, listeCoups);

        if (result != -1) {
            printf("On place le bonus/malus, coup %d (o:%d, d:%d)\n", result, listeCoups.coups[result].origine, listeCoups.coups[result].destination);
            ecrireIndexCoup(result);
            printf("\033[0m\n");
            return;
        }
        printf("IMPOSSIBLE DE PLACER LE BONUS/MALUS: -1 (numCoup: %d)\n", currentPosition.numCoup);
    }

    // Gestion des ouvertures
    if (currentPosition.numCoup < 10) { // Valeur de fin des ouvertures à déterminer
        result = ouverture(currentPosition, listeCoups);
        if (result != -1) {
            printf("On place une ouverture, coup %d (o:%d, d:%d)\n", result, listeCoups.coups[result].origine, listeCoups.coups[result].destination);
            ecrireIndexCoup(result);
            printf("\033[0m\n");
            return;
        }
        printf("IMPOSSIBLE DE PLACER L'OUVERTURE: -1 (numCoup: %d)\n", currentPosition.numCoup);
    }

    // Evaluation des coups pour choisir le meilleur
    for (int i = 0; i < listeCoups.nb; i++) {
        tempPlateau = jouerCoup(currentPosition, listeCoups.coups[i].origine, listeCoups.coups[i].destination);

        // Inverse le trait pour comparer les mêmes résultats
        tempPlateau.trait = (tempPlateau.trait == JAU) ? ROU : JAU;

        printf("o:%d d:%d n:%d", listeCoups.coups[i].origine, listeCoups.coups[i].destination, i);
        score_temp = evaluerScoreGen(currentPosition, tempPlateau, listeCoups.coups[i].origine, listeCoups.coups[i].destination);
        printf(" | SCG: %.0f", score_temp);
        if (score_temp > max_temp) {
            result = i;
            max_temp = score_temp;
            printf(" (CF)");
        }
        printf("\n");
    }

    if (result == -1) {
        printf("IMPOSSIBLE DE PLACER LE COUP: -1 (numCoup: %d)\n", currentPosition.numCoup);
    } else {
        printf("On place le coup %d (o:%d, d:%d)\n", result, listeCoups.coups[result].origine, listeCoups.coups[result].destination);
        ecrireIndexCoup(result);
    }

    printf("\033[0m\n"); // Revenir à la couleur par défaut pour les messages de gestion
}
