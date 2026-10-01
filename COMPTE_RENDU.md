# Compte rendu — TP « Sortir du fichier unique »

Les commandes et messages consignés ci-dessous ont été vérifiés avec GCC 16.2.0
(MSYS2 UCRT64) sous Windows.

## Exercice 0 — Dépôt et fichiers générés

Les fichiers objets et l'exécutable ne sont pas versionnés : ils sont générés à
partir des sources, dépendent du compilateur et de la plateforme, et peuvent être
recréés à tout moment. Les ignorer évite aussi d'alourdir le dépôt et de produire
des conflits inutiles.

## Exercice 1 — Découpage en trois fichiers

| Question | Réponse |
| --- | --- |
| A | `Maillon` doit être dans `liste.h` car `main.c` doit connaître le type pour déclarer et manipuler un pointeur de liste. L'en-tête partage cette définition entre les unités de traduction. |
| B | `liste.c` inclut son propre en-tête afin que le compilateur vérifie que les définitions correspondent exactement aux déclarations publiques. |

## Exercice 2 — Compilation séparée

Commandes utilisées :

```sh
gcc -Wall -Wextra -Werror -std=c11 -g -c main.c -o main.o
gcc -Wall -Wextra -Werror -std=c11 -g -c liste.c -o liste.o
gcc -Wall -Wextra -Werror -std=c11 -g -o demo main.o liste.o
```

Sortie obtenue :

```text
liste     : 50 -> 40 -> 30 -> 20 -> 10 -> NULL
longueur  : 5
contient 30 : oui
liberee
```

| Question | Réponse |
| --- | --- |
| A | Deux objets sont produits : `main.o` et `liste.o`. Il n'existe pas de `liste.h.o`, car un en-tête est inclus dans les fichiers source par le préprocesseur ; ce n'est pas une unité de traduction compilée séparément. |
| B | La commande directe recompile les deux sources à chaque fois. La compilation en deux étapes permet de ne recompiler que les objets dont les sources ou dépendances ont changé, puis de relier rapidement l'ensemble. |

