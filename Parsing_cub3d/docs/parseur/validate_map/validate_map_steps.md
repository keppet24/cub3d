# Résumé logique par fonction – validate_map.c

Format “petit 1…” conforme à `doc.md`.

## validate_map
Petit 1 : vérifier que `map_lines` existe, que `map_height > 0` et que `player_count == 1`.  
Petit 2 : parcourir chaque ligne `y`.  
Petit 3 : si `y` est la première ou la dernière ligne, chaque caractère doit être `'1'` ou espace (sinon erreur).  
Petit 4 : pour les autres lignes, appliquer `scan_row`.  
Petit 5 : si tout passe, retourner true, sinon afficher l’erreur et retourner false.  
Scénario : une carte sans joueur unique ou sans ligne provoque une erreur immédiate ; une bordure contenant un `0` échoue au pas 3.

## scan_row
Petit 1 : trouver le premier et le dernier caractère non-espace ; ils doivent être `'1'`.  
Petit 2 : boucler sur les colonnes à partir du premier non-espace.  
Petit 3 : pour chaque espace interne (hors espaces de tête), vérifier que les voisins N/E/S/O sont `1` ou espace (via `space_neighbors_ok`) et refuser si la colonne dépasse la ligne du haut ou du bas.  
Petit 4 : pour chaque `'0'`, vérifier qu’il ne touche pas un débordement vertical (lignes du haut/bas suffisamment longues) et qu’il n’est pas collé aux bords gauche/droite via `zero_neighbors_closed`.  
Petit 5 : si une règle casse, afficher l’erreur correspondante et retourner false.  
Scénario : un espace touchant un caractère hors {1,' '} ou placé au-delà de la longueur d’une ligne voisine déclenche “Espace adjacent a vide” ou “Carte non fermee verticalement”; un `0` placé sur un bord ou au-delà de la longueur d’une ligne voisine déclenche “Carte non fermee verticalement”.

## get_content_start
Petit 1 : avancer `start` en sautant les espaces de tête.  
Petit 2 : reculer `end` en sautant les espaces de fin.  
Petit 3 : si la ligne est vide (`start == end`) ou si `line[start] != '1'` ou `line[end-1] != '1'`, retourner erreur “Carte non fermee sur les bords”.  
Petit 4 : sinon, retourner vrai avec `*start` positionné sur le premier non-espace.  
Scénario : `"  1 0010  "` passe (`start=2`), `"   0  "` échoue.

## safe_char
Petit 1 : calculer la longueur de la ligne `y`.  
Petit 2 : si `x` dépasse cette longueur, retourner `' '`.  
Petit 3 : sinon, retourner le caractère `line[y][x]`.  
Scénario : sur une ligne courte, `safe_char(st, y, len+3)` renvoie `' '`.

## space_neighbors_ok
Petit 1 : lire `up` et `down` via `safe_char` aux positions `y-1, x` et `y+1, x`.  
Petit 2 : lire `left` (si `x == 0`, forcer `' '`, sinon `safe_char(y, x-1)`).  
Petit 3 : lire `right` via `safe_char(y, x+1)`.  
Petit 4 : si l’un des voisins n’est pas dans `{ '1', ' ' }`, retourner false.  
Petit 5 : sinon, retourner true.  
Scénario : un espace adjacent à un caractère `2` déclenche false.

## zero_neighbors_closed
Petit 1 : récupérer les longueurs des lignes `y-1`, `y`, `y+1`.  
Petit 2 : si `x` dépasse `len_up` ou `len_down`, retourner false.  
Petit 3 : si `x == 0` ou si `x + 1 >= len_curr`, retourner false (bord gauche/droite).  
Petit 4 : sinon, retourner true.  
Scénario : un `0` sur la première colonne ou sous une ligne plus courte déclenche “Carte non fermee verticalement”.
