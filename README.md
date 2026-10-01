# Liste chaînée

Projet du TP « Sortir du fichier unique » : une liste simplement chaînée en C,
découpée en un module réutilisable et un programme de démonstration.

## Compiler et exécuter

```sh
make clean
make
./demo
```

Sous Windows, l'exécutable peut être lancé avec `./demo.exe`.

La compilation utilise C11, les informations de débogage et les avertissements
`-Wall -Wextra -Werror`.

## Contenu

- `liste.h` : interface publique et type `Maillon` ;
- `liste.c` : implémentation de la liste ;
- `main.c` : démonstration et cas de test ;
- `Makefile` : compilation séparée et nettoyage ;
- `deborde.c` : exemple pédagogique de dépassement de tampon ;
- `COMPTE_RENDU.md` : observations et réponses aux exercices.

