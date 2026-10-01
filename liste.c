#include <stdio.h>
#include <stdlib.h>
#include "liste.h"

Maillon *liste_inserer(Maillon *tete, int valeur)
{
    Maillon *m = malloc(sizeof(Maillon));
    if (m == NULL) { perror("malloc"); exit(EXIT_FAILURE); }
    m->valeur = valeur;
    m->suivant = tete;  /* 1. il pointe l'ancienne tete */
    return m;           /* 2. il devient la nouvelle tete */
}

int liste_longueur(const Maillon *tete)
{
    int n = 0;
    for (const Maillon *m = tete; m != NULL; m = m->suivant) n++;
    return n;
}

bool liste_contient(const Maillon *tete, int valeur)
{
    for (const Maillon *m = tete; m != NULL; m = m->suivant)
        if (m->valeur == valeur) return true;
    return false;
}

void liste_afficher(const Maillon *tete)
{
    for (const Maillon *m = tete; m != NULL; m = m->suivant)
        printf("%d -> ", m->valeur);
    printf("NULL\n");
}

void liste_liberer(Maillon *tete)
{
    Maillon *m = tete;
    while (m != NULL) {
        Maillon *suiv = m->suivant;  /* sauvegarder AVANT de liberer */
        free(m);
        m = suiv;
    }
}
