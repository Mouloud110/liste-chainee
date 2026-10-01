#ifndef LISTE_H
#define LISTE_H

#include <stdbool.h>

typedef struct Maillon {
    int valeur;
    struct Maillon *suivant;
} Maillon;

Maillon *liste_inserer(Maillon *tete, int valeur);
int      liste_longueur(const Maillon *tete);
bool     liste_contient(const Maillon *tete, int valeur);
bool     liste_maximum(const Maillon *tete, int *resultat);
void     liste_afficher(const Maillon *tete);
void     liste_liberer(Maillon *tete);
int      liste_blocs_en_circulation(void);

#endif
