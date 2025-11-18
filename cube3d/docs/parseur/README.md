# Vue d’ensemble du parser

Ce document décrit le fonctionnement réel du code placé dans `src/` après la refonte actuelle. Il détaille l’ordre d’appel, les fichiers concernés et le rôle de chaque fonction principale.

## 1. Architecture des fichiers
```
src/
├── main.c                         // point d’entrée : vérifie les arguments et déclenche parse()
├── error.c                        // impression uniforme de "Error\n..."
├── utils/
│   └── string_utils.c             // ft_strlen, ft_isdigit, ft_isspace, ft_strdup
└── parser/
    ├── parser.c                   // orchestrateur : init/destroy state, parse_header, parse_map, validate_map
    ├── parse_header/
    │   └── parse_header.c         // logique détaillée de lecture des identifiants
    └── parse_map/
        ├── parse_map.c            // lecture de la carte, charset, joueur unique
        └── validate_map.c         // vérification des bords/espaces/débordements
```
`cube3d.h` expose toutes les structures (`t_parser_state`, `t_color`, `TEX_*`) et les prototypes partagés.

## 2. Flux global
1. `main(int ac, char **av)` (src/main.c)
   - Vérifie `ac == 2`.
   - Teste l’extension `.cub` via `is_cub_file`.
   - Appelle `parse(av[1])`.
2. `parse(const char *path)` (src/parser/parser.c)
   - Initialise un `t_parser_state` vide (`init_state`).
   - Ouvre le fichier, sinon `print_error("Impossible d'ouvrir le fichier")`.
   - Appelle successivement :
     - `parse_header(fd, &state, &first_map_line);`
     - `parse_map(fd, &state, first_map_line);`
     - `validate_map(&state);`
   - Détruit l’état (`destroy_state`) puis retourne `0` ou `1`.

## 3. Détails par module

### 3.1 src/main.c
- `static bool is_cub_file(const char *path)` vérifie que le nom se termine par `.cub` sans caractères suspects juste avant (pas de `.../.cub`).
- `main` ne connaît rien du parsing lui-même ; il délègue tout à `parse`.

### 3.2 src/parser/parser.c
- `init_state` met tous les chemins de textures à `NULL` et marque `floor/ceiling` comme non définis.
- `destroy_state` libère chaque `tex_path[i]`.
- `parse_map` stocke chaque ligne de carte (charset strict, conversion du joueur en `0`, rejet ligne vide, joueur unique).
- `validate_map` contrôle la fermeture : bords supérieurs/inférieurs en `1/espaces`, bords latéraux, débordements murés, espaces internes entourés de `1/espaces`.
- `parse` relie tous ces éléments et assure la fermeture du descripteur même en cas d’erreur.

### 3.3 src/parser/parse_header/parse_header.c
Contient l’ensemble des helpers nécessaires à la phase header :
- `trim_newline`, `line_is_empty`, `looks_like_map_line` détectent quand on bascule vers la carte.
- `parse_texture_path` stocke `NO/SO/WE/EA` (avec `trim_trailing_spaces`, contrôle `.xpm`, ouverture lecture, duplication).
- `parse_color_value` convertit `F` et `C` en `t_color` (contrôle strict de la syntaxe `R,G,B`).
- `handle_identifier` choisit le bon parseur selon le préfixe.
- `parse_header` boucle sur `get_next_line`, saute les lignes vides tant que la carte n’a pas commencé, stocke les identifiants, et arrête tout si une ligne de carte arrive avant que les six éléments soient présents.
  - Sortie réussie : `*first_map_line` pointe sur la première ligne de map (à gérer par `parse_map`).
  - Sortie en erreur : message explicite (`Identifiants manquants`, `Carte manquante`, etc.).

### 3.4 src/utils/string_utils.c
- Fournit les équivalents maison de fonctions de base pour éviter les dépendances externes.
- Utilisé par tous les modules (d’où la présence de leurs prototypes dans `cube3d.h`).

### 3.5 src/error.c
- `print_error(const char *msg)` affiche le préfixe `"Error\n"` puis le message optionnel.
- Garantit un format unique pour tous les échecs (aligné avec le sujet).

## 4. Enchaînement des appels
```
main
└── parse
    ├── init_state
    ├── parse_header
    │   ├── get_next_line (boucle)
    │   ├── trim_newline / line_is_empty
    │   ├── handle_identifier
    │   │    ├── parse_texture_path
    │   │    └── parse_color_value
    │   └── looks_like_map_line
    ├── parse_map
    │    └── handle_map_line (charset, joueur unique, stockage)
    ├── validate_map (bords, débordements, espaces internes)
    └── destroy_state
```
Cette hiérarchie reflète exactement ce qui se passe dans le code.

## 5. Prochaines étapes prévues
- Tester les cartes “bad” déjà fournies via le script `tools/run_bad_maps.sh` (attend un exit code ≠0).
- Étendre plus tard le validateur si besoin (ex : flood-fill complet).

Ce document doit être tenu à jour dès que la logique change ou qu’un nouveau module est ajouté.
