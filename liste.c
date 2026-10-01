#include <stdio.h>
#include <stdlib.h>
#include "liste.h"

/* Compteur d'allocations interne au module. */
static int blocs = 0;

static void *suivi_malloc(size_t taille)
{
    void *p = malloc(taille);
    if (p != NULL) blocs++;
    return p;
}

static void suivi_free(void *p)
{
    if (p != NULL) {
        blocs--;
        free(p);
    }
}

int liste_blocs_en_circulation(void)
{
    return blocs;
}

Maillon *liste_inserer(Maillon *tete, int valeur)
{
    Maillon *m = suivi_malloc(sizeof(Maillon));
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

bool liste_maximum(const Maillon *tete, int *resultat)
{
    if (tete == NULL) return false;

    int max = tete->valeur;
    for (const Maillon *m = tete->suivant; m != NULL; m = m->suivant)
        if (m->valeur > max) max = m->valeur;

    *resultat = max;
    return true;
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
        suivi_free(m);
        m = suiv;
    }
}
