# Design validate_map – Contrôles de fermeture

Document au format “design” (pas la doc implémentation) : diagramme principal, fiches synthétiques, règles exactes tirées des consignes (scan top→bottom, left→right). Cette fiche décrit ce que la future fonction `validate_map` fera et dans quel ordre.

## Vue rapide des appels (Niveau 0)
```
┌──────────────────────────┐
│  validate_map(state)     │
└───────────┬──────────────┘
            ▼
┌──────────────────────────┐
│  check_borders           │  // lignes 0 et dernière
└───────────┬──────────────┘
            ▼
┌──────────────────────────┐
│  check_cols_edges        │  // premier/dernier char des lignes intermédiaires
└───────────┬──────────────┘
            ▼
┌──────────────────────────┐
│  scan_inner_spaces       │  // espaces internes adjacents
└───────────┬──────────────┘
            ▼
┌──────────────────────────┐
│  scan_overflow_neighbors │  // dépassements de longueur vs lignes voisines
└───────────┬──────────────┘
            ▼
┌──────────────────────────┐
│  validate_player_exists  │  // déjà garanti par parse_map, double check
└──────────────────────────┘
```

## Fiches synthétiques (Niveau 1)

### check_borders
```
┌──────────────────────────┐
│ check_borders(map)       │
└───────────┬──────────────┘
            ▼
┌──────────────────────────┐
│ ligne 0 : chars ∈ {'1',' '}│
└───────────┬──────────────┘
            ▼
┌──────────────────────────┐
│ dernière ligne idem      │
└──────────────────────────┘
```
Objectif : refuser tout autre caractère que `1` ou espace sur les bords haut/bas.

### check_cols_edges
```
┌──────────────────────────┐
│ check_cols_edges(map)    │
└───────────┬──────────────┘
            ▼
┌──────────────────────────┐
│ pour chaque ligne (1..n-2)│
└──────┬─────────┬─────────┘
       │vide     │non vide
       ▼         ▼
   ┌────────┐ ┌─────────────────────┐
   │erreur  │ │ premier/dernier non│
   └────────┘ │ espace doivent être│
              │ '1'                │
              └────────────────────┘
```
Objectif : garantir des murs latéraux pour toutes les lignes intermédiaires.

### scan_inner_spaces
```
┌──────────────────────────┐
│ scan_inner_spaces(map)   │
└───────────┬──────────────┘
            ▼
┌──────────────────────────┐
│ pour chaque ' ' interne  │
└──────┬─────────┬─────────┘
       │voisin≠1/ │voisin OK
       │' '       │
       ▼          ▼
   ┌────────┐ ┌────────┐
   │erreur  │ │continue│
   └────────┘ └────────┘
```
Voisins considérés : haut, bas, gauche, droite (diagonales ignorées).

### scan_overflow_neighbors
```
┌──────────────────────────┐
│ scan_overflow_neighbors  │
└───────────┬──────────────┘
            ▼
┌──────────────────────────┐
│ pour chaque case (r,c)   │
└──────┬─────────┬─────────┘
       │c >= len(top/bot) │
       ▼         ▼
   ┌────────────────┐ ┌───────────┐
   │ si len courante│ │ sinon     │
   │ dépasse -> char│ │ continuer │
   │ doit être '1'  │ └───────────┘
   └────────────────┘
```
Objectif : si une ligne est plus longue que celle du dessus/dessous, le débordement doit être fermé par des `1`.

### validate_player_exists
```
┌──────────────────────────┐
│ validate_player_exists   │
└───────────┬──────────────┘
            ▼
┌──────────────────────────┐
│ state->player_count == 1?│
└──────┬─────────┬─────────┘
       │non      │oui
       ▼         ▼
   ┌────────┐ ┌────────┐
   │erreur  │ │success │
   └────────┘ └────────┘
```
Double-vérification en sortie.

## Règles exactes à coder (référentiel)
- Ignorer les espaces de tête lors des scans.
- Ligne 0 et dernière ligne : uniquement `'1'` ou `' '`.
- Toutes les autres lignes : premier et dernier caractère non-espace doivent être `'1'`.
- Espaces internes : leurs voisins (haut, bas, gauche, droite) doivent être dans `{ '1', ' ' }`.
- Si `strlen(curr) > strlen(top)` et `col >= strlen(top)` alors le caractère courant doit être `'1'`.
- Même règle avec la ligne du bas (`row_bottom`).
- Ces règles satisfont les contraintes de carte fermée même pour des lignes de longueurs différentes.
