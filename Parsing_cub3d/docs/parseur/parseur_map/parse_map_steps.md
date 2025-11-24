# Résumé logique par fonction – parse_map.c

Format : mini-étapes “petit 1, petit 2…” et un court scénario d’exécution.

## init_map_buffer
Petit 1 : mettre `lines = NULL`.  
Petit 2 : mettre `size = 0`.  
Petit 3 : mettre `capacity = 0`.  
Scénario : au début de `parse_map`, on appelle `init_map_buffer(&buf)` pour partir d’un buffer vide prêt à recevoir des lignes.

## free_map_buffer
Petit 1 : si pas de `lines`, sortir.  
Petit 2 : parcourir chaque `lines[i]` et `free`.  
Petit 3 : `free` du tableau `lines`.  
Scénario : en cas d’erreur pendant `parse_map`, on appelle `free_map_buffer(&buf)` pour nettoyer toutes les lignes déjà poussées.

## push_line_resize
Petit 1 : si `size < capacity`, rien à faire → true.  
Petit 2 : sinon, calculer `newcap` (8 si 0, sinon double).  
Petit 3 : allouer un nouveau tableau `tmp` de `newcap` pointeurs.  
Petit 4 : copier les pointeurs existants dans `tmp`.  
Petit 5 : libérer l’ancien tableau, basculer `lines = tmp`, mettre `capacity = newcap`.  
Scénario : avant chaque insertion, `push_line` appelle `push_line_resize`; si le buffer est plein, on l’agrandit pour éviter une réalloc à chaque push.

## push_line
Petit 1 : `push_line_resize(buf)` pour garantir la place.  
Petit 2 : dupliquer `line` avec `ft_strdup` dans `lines[size]`.  
Petit 3 : mettre à jour `map_width` si la ligne est plus longue.  
Petit 4 : incrémenter `size` et synchroniser `map_height`.  
Petit 5 : retourner true si tout va bien.  
Scénario : lors d’une ligne valide, `push_line` l’ajoute au buffer et ajuste largeur/hauteur max.

## handle_map_line
Petit 1 : rejeter une ligne vide (après début).  
Petit 2 : convertir les tabs en espaces.  
Petit 3 : pour chaque char : si `N/S/E/W`, vérifier joueur unique, enregistrer dir/coords, remplacer par `'0'`; sinon accepter `0/1/espace` ou erreur.  
Petit 4 : appeler `push_line` pour stocker la ligne nettoyée.  
Petit 5 : retourner true/false selon succès.  
Scénario : sur `"10N01"`, on trouve `N`, on enregistre le joueur (x=2, y=taille actuelle), on remplace par `0` et on stocke la ligne. Sur `"10A01"`, on retourne une erreur immédiatement.

## parse_map
Petit 1 : `init_map_buffer` puis traiter `first_map_line` via `handle_map_line`.  
Petit 2 : boucle `get_next_line`, `trim_newline`, puis `handle_map_line` pour chaque ligne suivante.  
Petit 3 : si une ligne est vide (ou ne contient que des espaces), marquer la fin de carte ; toute ligne non vide après cette fin déclenche une erreur.  
Petit 4 : en cas d’échec à tout moment, libérer le buffer et retourner false.  
Petit 5 : après la boucle, vérifier que `player_count == 1`. Sinon, erreur et free.  
Petit 6 : en succès, transférer `buf.lines` dans `state->map_lines` et retourner true.  
Scénario : avec 4 lignes de carte puis une ligne vide finale, `parse_map` pousse les 4 lignes, ignore la ligne vide de fin, détecte le joueur unique, et laisse dans `state` un tableau prêt pour `validate_map`. Une ligne non vide après une ligne vide déclencherait une erreur.

## validate_map
Petit 1 : vérifier que la carte existe et que `player_count == 1`.  
Petit 2 : lignes 0 et dernière : chaque caractère doit être `'1'` ou espace.  
Petit 3 : lignes internes : trouver le premier/dernier non-espace, ils doivent être `'1'`.  
Petit 4 : si la ligne est plus longue que la ligne du haut ou du bas, toute colonne au-delà de la longueur voisine doit être `'1'`.  
Petit 5 : pour chaque espace (après les espaces de tête), les voisins N/E/S/O doivent être `'1'` ou espace.  
Scénario : une ligne qui déborde de 2 colonnes par rapport à la ligne du haut doit avoir `...11` en fin ; un espace collé à un `0` déclenche une erreur.
