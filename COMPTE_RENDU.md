# Compte rendu — TP « Sortir du fichier unique »

Les commandes et messages consignés ci-dessous ont été vérifiés avec GCC 16.2.0
(MSYS2 UCRT64) sous Windows. Les mesures Valgrind ont été réalisées avec GCC
15.2.0 et Valgrind 3.25.1 sous Alpine Linux (WSL 1).

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
puis le code correct a été recompilé. Le tableau reproduit le premier diagnostic
utile exactement ; seul le préfixe de chemin absolu ajouté par l'éditeur de liens
a été retiré.

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
gcc -Wall -Wextra -Werror -std=c11 -g -c main.c -o main.o
gcc -Wall -Wextra -Werror -std=c11 -g -c liste.c -o liste.o
gcc -Wall -Wextra -Werror -std=c11 -g -o demo main.o liste.o
```

La seconde exécution affiche :

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

Le programme a été compilé avec `-g`, puis exécuté sous Valgrind 3.25.1. Les
deux dernières lignes utiles du rapport sans fuite sont :

```text
All heap blocks were freed -- no leaks are possible
ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)
```

La variante volontairement fuyarde produit cette pile d'appels :

```text
malloc (vg_replace_malloc.c:446)
suivi_malloc (liste.c:10)
liste_inserer (liste.c:30)
main (main.c:10)
```

| Mesure | Sans fuite | Avec la fuite de trois maillons |
| --- | ---: | ---: |
| `definitely lost` | 0 octet | 16 octets dans 1 bloc |
| `indirectly lost` | 0 octet | 32 octets dans 2 blocs |
| `total heap usage` | 5 allocations, 5 libérations, 80 octets | 8 allocations, 5 libérations, 128 octets |
| Compteur du module | 0 | 3 |

Le runtime AddressSanitizer de Windows ne prend pas en charge `detect_leaks`,
mais il a été utilisé en complément pour les accès invalides de l'exercice 8.

| Question | Réponse |
| --- | --- |
| A | La pile d'appels indique la ligne qui appelle `liste_inserer` lors de la création de la seconde liste : c'est le lieu de l'allocation devenue inaccessible. Ce n'est pas une ligne où `free` aurait été oublié, car une opération absente n'a pas de ligne exécutable. |
| B | La tête devenue inaccessible est le bloc directement perdu. Les deux maillons suivants ne sont accessibles qu'en suivant son pointeur : ils sont donc indirectement perdus. |
| C | Ici, Valgrind indique 5 allocations/5 libérations sans fuite et 8/5 avec fuite : la libc musl d'Alpine n'a pas fait d'allocation supplémentaire pour ces sorties. Avec une libc qui alloue un tampon pour `printf`, le total peut être supérieur au nombre de maillons (par exemple 6/6 puis 9/6), car Valgrind compte aussi les allocations internes de la bibliothèque C. |

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

## Exercice 9 — Ajouter `liste_maximum`

Sortie finale du programme :

```text
blocs apres construction : 5
liste     : 50 -> 40 -> 30 -> 20 -> 10 -> NULL
longueur  : 5
contient 30 : oui
maximum   : 50
liste vide : aucun maximum (resultat inchange : 12345)
liberee
blocs apres liberation   : 0
```

Le test avec `NULL` confirme à la fois le retour `false` et l'absence de
modification de `*resultat`.

Après cet ajout, une nouvelle compilation stricte et une nouvelle exécution sous
Valgrind donnent encore `All heap blocks were freed` et `ERROR SUMMARY: 0 errors`.
Une exécution AddressSanitizer séparée se termine également sans diagnostic.

| Question | Réponse |
| --- | --- |
| A | Trois fichiers ont été modifiés : `liste.h`, `liste.c` et `main.c`. Comme le Makefile fait dépendre chaque objet de `liste.h`, `make` recompile `main.o` et `liste.o`, puis relie `demo`. |
| B | `-1` peut être une valeur légitime de la liste, voire son maximum. Le booléen sépare sans ambiguïté l'absence de résultat de la valeur entière obtenue. |
