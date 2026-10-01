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

## Exercice 6 — Compteur d'allocations

| Mesure | Sans fuite | Avec une seconde liste non libérée |
| --- | ---: | ---: |
| Compteur après construction des listes | 5 | 8 |
| Compteur après libération de la liste principale | 0 | 3 |

La version avec fuite a été testée temporairement, puis corrigée avant le commit.

| Question | Réponse |
| --- | --- |
| A | `blocs`, `suivi_malloc` et `suivi_free` sont des détails d'implémentation. `static` leur donne une liaison interne : ils ne polluent pas l'API et ne peuvent pas être appelés directement depuis un autre module. Seule la fonction de consultation du compteur est publique. |
| B | Incrémenter après l'échec de `malloc` compterait un bloc qui n'existe pas. Décrémenter pour `NULL` fausserait le compteur, alors que `free(NULL)` ne libère rien. |
| C | Le compteur indique combien de blocs restent, mais pas leur origine. Avec ce seul outil, on peut afficher sa valeur avant et après chaque opération, réduire le scénario et ajouter temporairement des étiquettes aux allocations pour isoler l'endroit où l'équilibre se rompt. |

## Exercice 7 — Vérification mémoire

Le programme final a été compilé avec Clang 22.1.8 et
`-fsanitize=address -fno-omit-frame-pointer`. Il s'exécute sans diagnostic et
retourne 0. Le compteur interne revient également à 0.

Le runtime AddressSanitizer de Windows signale explicitement que
`detect_leaks` n'est pas pris en charge sur cette plateforme. Il détecte bien les
accès invalides (voir l'exercice 8), mais pas les blocs oubliés. La variante avec
fuite a donc aussi été vérifiée avec le compteur : il reste exactement 3 blocs.
Sur une plateforme Valgrind/LeakSanitizer 64 bits, `sizeof(Maillon)` vaut 16 ici,
donc la même fuite correspond aux mesures suivantes :

| Mesure | Sans fuite | Avec la fuite de trois maillons |
| --- | ---: | ---: |
| `definitely lost` / fuite directe | 0 | 1 bloc, 16 octets |
| `indirectly lost` / fuite indirecte | 0 | 2 blocs, 32 octets |
| Compteur du module | 0 | 3 |

| Question | Réponse |
| --- | --- |
| A | La pile d'appels indique la ligne qui appelle `liste_inserer` lors de la création de la seconde liste : c'est le lieu de l'allocation devenue inaccessible. Ce n'est pas une ligne où `free` aurait été oublié, car une opération absente n'a pas de ligne exécutable. |
| B | La tête devenue inaccessible est le bloc directement perdu. Les deux maillons suivants ne sont accessibles qu'en suivant son pointeur : ils sont donc indirectement perdus. |
| C | Le programme réalise 5 allocations et 5 libérations de maillons sans fuite, puis 8 allocations et seulement 5 libérations dans la variante fuyarde. Le total affiché par Valgrind peut être supérieur, car il inclut aussi les allocations internes de la bibliothèque C, notamment celles liées aux entrées-sorties. |

## Exercice 8 — Ce que le compteur ne voit pas

| Observation | Résultat obtenu |
| --- | --- |
| Sortie sans outil | `42` |
| Code de sortie sans outil | `0` |
| Diagnostic | `AddressSanitizer: heap-buffer-overflow` |

AddressSanitizer localise l'écriture dans `deborde.c:8:10`, indique
`WRITE of size 4`, puis précise :

```text
0 bytes after 20-byte region
```

L'exécution instrumentée se termine avec le code 1.

| Question | Réponse |
| --- | --- |
| A | Il y a un `malloc` et un `free`. Le compteur reviendrait donc à 0 et ne détecterait rien, même si l'accès est invalide. |
| B | Les 20 octets viennent de `5 * sizeof(int)` avec des entiers de 4 octets. `t[5]` commence exactement à la première adresse après le bloc, donc 0 octet après celui-ci. `t[6]` commencerait 4 octets après le bloc. |
| C | Une sortie apparemment correcte et un code de retour nul ne prouvent pas que le programme est correct. Un test peut « passer » alors que le programme a un comportement indéfini. |
| D | Le compteur est un contrôle léger et permanent de l'équilibre des allocations du module. Valgrind ou AddressSanitizer est nécessaire pour localiser les erreurs et détecter les dépassements, accès après libération et autres accès mémoire invalides. |
