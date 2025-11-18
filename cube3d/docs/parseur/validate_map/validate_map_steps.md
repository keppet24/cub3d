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
Petit 3 : pour chaque espace interne, vérifier que les voisins N/E/S/O sont `1`, espace ou `0` (via `space_neighbors_ok`).  
Petit 4 : pour chaque `'0'`, vérifier qu’il ne touche pas un débordement (hauteur/largeur suffisante autour de lui) via `zero_neighbors_closed`.  
Petit 5 : si une règle casse, afficher l’erreur correspondante et retourner false.  
Scénario : un espace touchant un caractère hors {1,0,' '} déclenche “Espace adjacent a vide”; un `0` placé sur un bord ou au-delà de la longueur d’une ligne voisine déclenche “Carte non fermee verticalement”.

## Helpers
Petit 1 : `get_content_start` saute les espaces de tête/fin et exige `1` sur premier/dernier non-espace.  
Petit 2 : `safe_char` retourne `' '` si `x` dépasse la longueur de la ligne demandée.  
Petit 3 : `space_neighbors_ok` utilise `safe_char` pour valider que les voisins d’un espace sont `1` ou espace.  
Petit 4 : `zero_neighbors_closed` vérifie que les voisins d’un `0` existent (pas de débordement/gauche-droite/bas-haut trop courts).
