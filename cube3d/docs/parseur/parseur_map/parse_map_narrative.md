# Revue de Conception Narrative – parse_map.c

## 1. But du module
- Ce fichier prend le relais après `parse_header` pour lire la partie carte du `.cub`.
- Il empile chaque ligne dans un buffer dynamique avec des contrôles basiques (charset autorisé, joueur unique, pas de ligne vide).
- Il convertit le joueur (`N/S/E/W`) en `0` et mémorise sa position/direction, ce qui simplifie la suite (validate_map, flood-fill, collisions) en travaillant sur une carte uniforme (`0/1/ `).
- Au final, il produit `state->map_lines` + largeur/hauteur + position du joueur, prêt pour `validate_map`.
- C’est le “collecteur” de la carte : il transforme les lignes lues en structure exploitable.

## 2. Structures importantes
### t_map_buffer
- Conteneur dynamique pour les lignes : `lines` (char**), `size` (stockées), `capacity` (allouées).
- Sert à stocker sans connaître la hauteur à l’avance ; doublement de capacité pour limiter les réallocations.
- Créé/initialisé/détruit dans `parse_map` ; en cas de succès, les lignes passent à `state->map_lines`, sinon `free_map_buffer` libère tout.
```
t_map_buffer
 ├─ lines: char**   (pointeurs vers chaque ligne)
 ├─ size: current   (nb de lignes stockées)
 └─ capacity: alloc (taille du tableau alloué)
```

### t_parser_state (champs utilisés ici)
- Porte la carte finale (`map_lines`), dimensions (`map_width`, `map_height`) et infos joueur (`player_count`, `player_x/y`, `player_dir`).
- Mis à jour par `handle_map_line` (dimensions, joueur unique). Libération finale dans le destructeur global (`destroy_state`).
- Possédé par le parseur global : initialisé/détruit dans `parse`, rempli ici.

## 3. Fonctions principales
### 3.1 init_map_buffer(t_map_buffer *buf)
- **Objectif** : démarrer un buffer vide (NULL, size/capacity = 0) avant de pousser des lignes.
- **Stratégie** : affectations à zéro, pas d’allocation. “Je prépare un sac vide avant de ramasser les lignes”.
- **Ce que ça modifie** : `lines/size/capacity` de `buf`.
- **Exemple** : appelée en début de `parse_map` pour préparer les push.

### 3.2 free_map_buffer(t_map_buffer *buf)
- **Objectif** : libérer toutes les lignes et le tableau.
- **Stratégie** : free de chaque `lines[i]`, puis free du tableau. “On vide le sac et on le jette”.
- **Ce que ça modifie** : mémoire détenue par `buf->lines`.
- **Exemple** : en cas d’erreur dans `parse_map`, pour éviter les fuites.

### 3.3 push_line_resize(t_map_buffer *buf)
- **Objectif** : garantir qu’il y a de la place avant d’ajouter.
- **Stratégie** : si `size == capacity`, capacité 8 puis doublement, réalloc du tableau. Ce doublement amortit les réallocations et reste trivial.
- **Ce que ça modifie** : `lines` (pointeur tableau) et `capacity`.
- **Exemple** : appelé par `push_line` ; si 8 lignes sont déjà là, on passe à 16.

### 3.4 push_line(t_map_buffer *buf, const char *line, t_parser_state *st)
- **Objectif** : dupliquer et stocker une ligne validée, mettre à jour les dimensions.
- **Stratégie** : `push_line_resize`, `ft_strdup`, maj de `map_width`, incrémente `size` et `map_height`. “J’ajoute une photo au tas et j’agrandis la pochette si besoin”.
- **Ce que ça modifie** : `buf->lines`, `buf->size`, `buf->capacity` (indirect), `st->map_width`, `st->map_height`.
- **Exemple** : `push_line(&buf, "10N01", st)` stocke et ajuste la largeur si nécessaire.

### 3.5 handle_map_line(t_map_buffer *buf, t_parser_state *st, char *line)
- **Objectif** : contrôler une ligne de carte et l’ajouter.
- **Stratégie** : rejette ligne vide, convertit tab→espace, valide chaque caractère. `N/S/E/W` → enregistre joueur (erreur si doublon) puis remplace par `'0'`. Charset invalide → erreur. Sinon, `push_line`.
- **Ce que ça modifie** : `player_count/dir/x/y` et, via `push_line`, `map_width/map_height`. La ligne est modifiée en place (joueur → '0').
- **Exemple** : `"10N01"` → joueur enregistré (x=2, y=buf->size), `N` devient `0`, ligne poussée. `"10A01"` → erreur.

### 3.6 parse_map(int fd, t_parser_state *state, char *first_map_line)
- **Objectif** : lire toutes les lignes de carte, appliquer les contrôles grossiers, remplir `state` avec carte et joueur.
- **Stratégie** : init buffer, `handle_map_line` sur `first_map_line`, boucle `get_next_line` (trim `\n`, `handle_map_line`), puis vérifie qu’il y a exactement un joueur. En cas d’erreur ou de joueur manquant/multiple, libère le buffer.
- **Ce que ça modifie** : `state->map_lines` (pointe vers le buffer final), `map_width/height`, `player_*`. En succès, les lignes passent dans `state`; en échec, `free_map_buffer` nettoie tout.
- **Exemple** : carte 4 lignes → push première, lire/pousser 3 suivantes, à la fin `map_lines` a 4 lignes, largeur max, joueur unique. Sinon erreur `"Joueur absent ou multiple"` et libération du buffer.

## 4. Guide de lecture du code
1. Lire `t_map_buffer` (dans le header) pour comprendre le stockage dynamique.
2. Suivre `parse_map` pour voir l’orchestration (init buffer, handle first line, boucle GNL, check joueur).
3. Détail de `handle_map_line` pour les contrôles immédiats (charset, joueur unique, remplacements).
4. Plonger dans `push_line` et `push_line_resize` pour voir comment les lignes sont réellement stockées et comment les dimensions sont mises à jour.
5. Garder à l’esprit que `validate_map` viendra ensuite pour les validations structurelles (non implémenté ici).

## 5. Check-list de compréhension
- Je sais où et comment la première ligne de carte est poussée, et comment les suivantes sont lues via `get_next_line`.
- Je comprends pourquoi le joueur (`N/S/E/W`) est converti en `'0'` et où sa position/direction sont stockées.
- Je peux expliquer le rôle de `capacity` et pourquoi on double la taille du buffer quand il est plein.
- Je sais quels caractères sont autorisés dans une ligne de carte et ce qui déclenche une erreur immédiate.
- Je peux décrire ce que `parse_map` laisse dans `state` à la fin (carte brute + dimensions + joueur unique).

## Pièges / invariants importants
- `player_count` doit être exactement 1 en sortie de `parse_map` ; sinon c’est une erreur bloquante.
- Les lignes de carte doivent être rejetées si elles sont vides après le début ou contiennent un caractère hors `{0,1, ,N,S,E,W}` (tabs transformés en espaces).
- `handle_map_line` est le seul endroit qui modifie `player_*` et marque les `N/S/E/W` en `0` ; ailleurs on ne touche pas aux coordonnées/direction.
- En cas d’échec (charset, joueur multiple, alloc), le buffer est libéré ; en cas de succès, la propriété de `lines` est transférée à `state->map_lines`.
- `push_line_resize` doit être appelé avant chaque ajout ; si `size == capacity`, il agrandit sinon il ne fait rien.
