# Plan de parsing (approche pas à pas)

Je décris ici exactement les fonctions que j’imagine écrire, dans l’ordre où elles s’exécutent depuis `main`. Le but est que, même sans diagramme, on comprenne pourquoi chaque étape existe et ce qu’elle fait concrètement.

## Vue rapide des appels
```
┌──────────────┐
│     main     │
└──────┬───────┘
       │
       ├─────────────┐
       │             │
       ▼             ▼
┌────────────┐   ┌──────────┐
│is_cub_file │   │  parse   │
└────────────┘   └────┬─────┘
                      │
      ┌───────────────┼───────────────────────────┐
      ▼               ▼                           ▼
┌───────────┐   ┌────────────┐             ┌───────────────┐
│parse_head │   │ parse_map  │             │validate_map   │
│    er     │   └──────┬─────┘             └───────────────┘
└─────┬─────┘          │
      │                ▼
┌─────▼─────────┐ ┌──────────────┐
│skip_if_empty  │ │handle_map_line│
├───────────────┤ └──────────────┘
│handle_identif │
│     ier       │
└─────┬─────────┘
      │
      ▼
┌───────────┐
│parse_color│
└───────────┘
```


## Fiches fonctions détaillées (structurées comme le graphe)

### Racine : `main`
```
╔════════════════════════════╗
║  main(int ac, char **av)   ║
╚════════════════════════════╝
Objectif : contrôler les arguments et déléguer le parsing.
Étapes :
1. Vérifier `ac == 2`, sinon annoncer l’erreur et `return 1`.
2. Lancer `is_cub_file(av[1])`; si faux → `return 1`.
3. Appeler `parse(av[1])` et relayer son code de retour (`0` succès, `1` erreur).
```

#### Sous-fonction : `bool is_cub_file(const *p)`
```
╔════════════════════════════╗
║ bool is_cub_file(const *p) ║
╚════════════════════════════╝
Objectif : confirmer que le nom se termine par `.cub`.
Étapes :
1. Parcourir la chaîne jusqu’au bout.
2. Vérifier que les quatre derniers caractères valent `.cub`.
Retour : `true` si le suffixe est correct, sinon `false`.
```

#### Sous-fonction : `int parse(const *path)`
```
╔════════════════════════════╗
║   int parse(const *path)   ║
╚════════════════════════════╝
Objectif : ouvrir, lire, valider et remplir `t_scene`.
Étapes :
1. `open(path)` → si échec, afficher l’erreur et `return 1`.
2. Initialiser `t_parser_state` (compteurs, `map_started`, tableau de lignes).
3. Appeler `parse_header(fd, &state, &first_map_line)` ; si `false`, nettoyer et `return 1`.
4. Appeler `parse_map(fd, &state, first_map_line)` ; si `false`, nettoyer et `return 1`.
5. Appeler `validate_map(&state)` ; si `false`, nettoyer et `return 1`.
6. Construire la scène finale (copier chemins, couleurs, map) puis `return 0`.
Nettoyage : `destroy_parser_state` est appelé sur chaque sortie anticipée.
```

##### Sous-fonction de `parse` : `bool parse_header(int fd, …, char **first_map_line)`
```
╔════════════════════════════╗
║ bool parse_header(int fd, …)║
╚════════════════════════════╝
Objectif : consommer uniquement la zone des identifiants avant la carte.
Étapes :
1. Lire avec `get_next_line`.
2. Utiliser `skip_if_empty` pour avaler les lignes vides tant qu’on est encore dans la zone en-tête.
3. Appeler `handle_identifier` pour chaque ligne non vide ; cette fonction incrémente les compteurs et stocke le contenu.
4. Dès qu’une ligne ressemble à une map (caractères 0/1/N/S/E/W), arrêter la lecture :
   - si tous les compteurs ne valent pas 1 → afficher “Error\nIdentifiants manquants” et retourner `false`;
   - sinon, dupliquer cette ligne dans `*first_map_line` et retourner `true`.
5. Si la fin du fichier est atteinte sans jamais trouver la map, retourner `false`.
```

###### Sous-fonctions de `parse_header`
```
╔════════════════════════════╗
║ bool handle_identifier(…)  ║
╚════════════════════════════╝
Objectif : traiter une ligne NO/SO/WE/EA/F/C.
Étapes :
1. Découper la ligne en `token` + `payload`.
2. Pour les textures : vérifier que le compteur associé vaut 0, dupliquer le chemin.
3. Pour F/C : appeler `parse_color(payload)` puis stocker les trois entiers.
4. Si la ligne ressemble déjà à de la map (chiffres, espaces suivis de 0/1), renvoyer `false` pour que `read_lines` bascule en mode carte.
Retour : `true` si l’identifiant a été consommé, `false` pour signaler que la ligne doit être traitée comme une ligne de map.
```

```
╔════════════════════════════╗
║ bool skip_if_empty(char *) ║
╚════════════════════════════╝
Objectif : détecter les lignes vides ou composées uniquement d’espaces.
Étapes :
1. Parcourir la chaîne ; si aucun caractère autre que l’espace n’est trouvé, retourner `true`.
2. Dans ce cas, libérer la ligne et permettre à `parse_header` de continuer sa boucle.
3. Sinon, retourner `false` pour que la ligne soit traitée par `handle_identifier`.
```

```
╔════════════════════════════╗
║ bool handle_map_line(…)    ║
╚════════════════════════════╝
Objectif : stocker une ligne de map telle quelle et vérifier les caractères.
Étapes :
1. Ajouter la ligne au tableau dynamique de `state`.
2. Vérifier que chaque caractère est dans `{ '0','1',' ','N','S','E','W' }`.
3. Compter les joueurs ; si >1, afficher l’erreur et retourner `false`.
4. Gérer les lignes vides : autorisées avant la carte, interdites après la première ligne “solide”.
```

```
╔════════════════════════════╗
║ bool parse_color(…)        ║
╚════════════════════════════╝
Objectif : convertir `R,G,B` en trois entiers.
Étapes :
1. Séparer la chaîne sur les virgules.
2. Vérifier qu’on obtient exactement trois nombres dans `[0,255]`.
3. Stocker les valeurs dans la structure couleur visée.
Retour : `true` si la couleur est valide, sinon `false`.
```

##### Autres sous-fonctions de `parse`
```
╔════════════════════════════╗
║ bool parse_map(int fd, …)  ║
╚════════════════════════════╝
Objectif : lire la carte à partir de la première ligne détectée.
Étapes :
1. Si `first_map_line` n’est pas NULL, l’envoyer dans `handle_map_line`.
2. Lire le reste du fichier ligne par ligne et appeler `handle_map_line` à chaque fois.
3. Refuser toute ligne vide après que la map a réellement commencé (le parser déclenche l’erreur correspondante).
4. Retourner `false` dès que `handle_map_line` échoue (caractère invalide, joueur multiple, etc.).
5. Retourner `true` lorsque la fin du fichier est atteinte sans incident.
```

```
╔════════════════════════════╗
║ bool validate_map(…)       ║
╚════════════════════════════╝
Objectif : s’assurer que la map est complète et fermée.
Étapes :
1. Vérifier que chaque compteur d’identifiant vaut exactement 1.
2. Créer un rectangle (padding d’espaces) pour faciliter les vérifications.
3. Localiser la case joueur (N/S/E/W) et mémoriser position + direction.
4. Parcourir la grille : toute case ‘0’ touchant l’extérieur ou un espace → erreur.
5. Implémenter cette vérification via un flood fill simple ou un contrôle de voisinage.
Retour : `true` si tout est ok, `false` sinon.
```

```
╔════════════════════════════╗
║ void destroy_parser_state  ║
╚════════════════════════════╝
Objectif : libérer toutes les allocations partielles.
Étapes :
1. Free sur chaque `tex_path`.
2. Free sur chaque ligne de map puis sur le tableau.
3. Réinitialiser les compteurs si on veut réutiliser la structure (optionnel).
```

Cette description correspond exactement à la façon dont je veux coder : pas de structures compliquées, juste des compteurs simples, des fonctions très explicites et des messages d’erreur clairs. Chaque étape est racontée du point de vue de l’exécution, ce qui facilite la relecture et la mise en œuvre.***
