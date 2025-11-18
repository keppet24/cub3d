# Consignes du dépôt

## Communication
Merci de communiquer exclusivement en français dans ce dépôt, y compris dans les revues, issues et discussions techniques. Toute nouvelle consigne ou clarification doit être rédigée en français pour rester cohérente avec le reste de la documentation.

## Organisation du projet
Tout nouveau code doit suivre l’architecture en dossiers : `src/` contient les sources générales, `src/parser/` accueille la fonction `parse` ainsi que ses sous-modules (`src/parser/parse_header/`, `src/parser/parse_map/`, etc.), et `src/utils/` regroupe les helpers partagés. Ne laissez plus de logique “globale” à la racine ; seuls les fichiers d’entrée (`get_next_line.c`, `.cub` de test, etc.) peuvent y rester. `cube3d.h` demeure l’unique point d’entrée pour les structures et prototypes. Chaque fois que du code évolue, mettez immédiatement à jour la documentation associée (ex. `docs/parseur/**`, et plus tard `docs/exec/**`) afin que l’arborescence reste synchronisée avec l’implémentation. `design_parser.md` et `subject.md` servent de référence fonctionnelle — mettez-les à jour dès que le comportement évolue.

## Documentation
- **RÈGLE PERMANENTE** : À chaque fois que tu écris ou modifies du code, tu dois impérativement écrire ou mettre à jour la documentation associée. Lis et applique strictement les règles décrites dans `doc.md` (format visuel, hiérarchie des titres, mise à jour systématique) avant toute modification.
- La documentation doit suivre l’étalon de qualité fixé par `design_parser.md` et la charte `doc.md`. Respecte la hiérarchie (Niveau 0/1/2) et les organigrammes ASCII pour les flux.
- Les fichiers de doc concernés doivent vivre sous `docs/` (ex. `docs/parseur/**`, `docs/exec/**`) et être synchronisés avec chaque changement de code.

## Règle de structuration des fichiers
- Un fichier source (`.c`) ne doit pas contenir plus de 5 fonctions. Si une séparation est nécessaire, scinde la logique dans des fichiers supplémentaires au sein du même module et ajoute-les au Makefile.
- Les déclarations de structures doivent vivre dans des headers (`.h`), jamais dans des `.c`.

## Compilation, tests et commandes utiles
Utilisez le Makefile fourni : `make` produit `cub3d` en compilant tout `src/` (objets sous `obj/`), `make clean` supprime les `.o` et `make fclean` supprime l’exécutable. Lancez les scènes avec `./cub3d sample.cub` ou tout autre `.cub`. Toute nouvelle source doit être ajoutée dans la variable `SRCS` du Makefile afin que la structure reste cohérente.

## Style de code et conventions
Respect strict de la Norme 42 : une responsabilité par fonction, ≤25 lignes, fonctions en snake_case, structures `t_*`, macros en MAJUSCULES. Indentation 4 espaces, gardes dans chaque header, et helpers `static` pour la logique locale. Exécutez `norminette *.c cube3d.h` avant toute revue pour éviter les surprises.

## Stratégie de tests
Les tests de parser restent proches de `test.c`. Nommez les helpers `test_<feature>.c` et encapsulez le code exploratoire avec `#ifdef TEST` pour garder les builds propres. Couvrez l’ordre des tokens (NO, SO, EA, WE), la validation RGB et, plus tard, la fermeture de carte par flood-fill (cf. `subject.md`). Utilisez `valgrind ./cub3d test.cub` (ou équivalent) pour garantir l’absence de fuites et vérifier que `get_next_line` libère bien les descripteurs. Stockez les cartes de test dans un dossier `maps/` dédié.

## Commits et revues
En l’absence d’historique fourni, adoptez immédiatement un format proche de Conventional Commits (ex. `parser: rejette les textures manquantes`). Conservez un sujet ≤72 caractères et un corps court décrivant motivation et impacts. Les pull requests doivent lister les commandes/tests exécutés, référencer les tickets 42 concernés, et inclure captures ou extraits vidéo quand le rendu ou le parser change. Signalez les nouveaux assets ou réglages pour faciliter la reproduction locale.

## Conseils pour les cartes et assets
Les fichiers `.cub` doivent contenir les chemins de textures (NO/SO/EA/WE), les tuples RGB puis la grille de carte ; gardez des exemples représentatifs versionnés pour documenter les cas limites. Rangez les textures dans un dossier `assets/` (à créer si besoin) et utilisez toujours des chemins relatifs pour que les binaires fonctionnent depuis la racine ou un dossier de build.
