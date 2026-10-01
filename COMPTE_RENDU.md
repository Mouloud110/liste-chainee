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

## Exercice 3 — Trois erreurs classiques

Les modifications fautives ont été faites une par une dans des copies de travail,
puis le code correct a été recompilé. Les chemins absolus affichés par GCC ont été
retirés du tableau pour le rendre lisible.

| Cas | Premier message utile observé | Étape |
| --- | --- | --- |
| 1 — objet oublié | `main.c:7:(.text+0x34): undefined reference to 'liste_inserer'` | Édition de liens |
| 2 — inclusion oubliée | `main.c:5:5: error: unknown type name 'Maillon'` | Compilation |
| 3 — garde oubliée | `liste.h:3:16: error: redefinition of 'struct Maillon'` | Compilation |

Le premier cas se termine également par :

```text
collect2.exe: error: ld returned 1 exit status
```

| Question | Réponse |
| --- | --- |
| 1 | Le cas 1 vient de l'éditeur de liens : chaque source a pu être compilée, mais `ld` ne trouve pas les définitions des fonctions contenues dans `liste.o`. Les termes `undefined reference` et `ld returned 1 exit status` l'indiquent. |
| 2 | Le premier message, `unknown type name 'Maillon'`, est la cause utile. Les déclarations implicites et conversions qui suivent ne sont que des conséquences de l'en-tête absent. |
| 3 | Le problème devient réaliste dès que plusieurs en-têtes s'incluent : par exemple, `main.c` inclut deux modules qui incluent tous deux `liste.h`. Sans garde, le contenu de `liste.h` est alors défini deux fois dans la même unité de traduction. |

## Exercice 4 — Premier Makefile

`make clean && make` exécute les deux compilations puis l'édition de liens. Une
seconde commande `make`, sans modification, affiche :

```text
make: 'demo' is up to date.
```

| Question | Réponse |
| --- | --- |
| A | `make` compare la date de la cible avec celles de ses dépendances. Comme `demo`, `main.o` et `liste.o` existent et qu'aucune dépendance n'est plus récente, aucune commande n'est nécessaire. |
| B | Avec quatre espaces à la place de la tabulation, GNU Make 4.4.1 affiche `Makefile:2: *** missing separator.  Stop.` |

## Exercice 5 — Dépendance à l'en-tête

| Étape | Ce que `make` recompile |
| --- | --- |
| 2 — avec la dépendance | `main.c` et `liste.c`, puis l'édition de liens de `demo` |
| 4 — sans la dépendance de `main.o` | Seulement `liste.c`, puis l'édition de liens de `demo` |

| Question | Réponse |
| --- | --- |
| A | Avec la dépendance correcte, toucher `liste.h` invalide les deux objets, car les deux sources incluent l'en-tête. Dans la version fautive, `main.o` paraît encore à jour et n'est pas reconstruit. |
| B | L'exécutable peut mélanger un `main.o` compilé avec l'ancienne définition de `Maillon` et un `liste.o` compilé avec la nouvelle. Les deux unités de traduction n'ont alors plus la même vision de la structure : le programme a un comportement indéfini malgré une édition de liens réussie. |
