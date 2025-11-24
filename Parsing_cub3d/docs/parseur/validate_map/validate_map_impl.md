# Validator Map – Implémentation

Conforme à `doc.md` : organigrammes ASCII, hiérarchie par niveaux, reflète le code de `src/parser/parse_map/validate_map.c`.

## Vue rapide des appels (Niveau 0)
```
┌──────────────────────────┐
│ validate_map(state)      │
└───────────┬──────────────┘
            ▼
┌──────────────────────────┐
│ checks généraux (map,    │
│ joueur unique)           │
└───────────┬──────────────┘
            ▼
┌──────────────────────────┐
│ boucle lignes            │
└──────┬─────────┬─────────┘
       │bord     │interne
       ▼         ▼
┌──────────────┐ ┌──────────────────────┐
│ check_bord   │ │ scan_row             │
└──────┬───────┘ └──────────┬───────────┘
       │ko                 │ko
       ▼                   ▼
   ┌────────┐          ┌────────┐
   │erreur  │          │erreur  │
   └────────┘          └────────┘
```

## Sous-graphes par fonction (Niveau 1)

### validate_map
```
┌──────────────────────────┐
│ validate_map(state)      │
└───────────┬──────────────┘
            ▼
┌──────────────────────────┐
│ map_lines existe ?       │
│ map_height > 0 ?         │
│ player_count == 1 ?      │
└───────────┬──────────────┘
            ▼
┌──────────────────────────┐
│ boucle y = 0..height-1  │
└──────┬─────────┬─────────┘
       │bord     │interne
       ▼         ▼
┌──────────────┐ ┌────────────────┐
│ check_bord   │ │ scan_row(y)    │
└──────────────┘ └──────┬─────────┘
                        ▼
                  erreur ? → stop
```

### check_border_line (lignes 0 et dernière)
```
┌──────────────────────────┐
│ check_border_line(line)  │
└───────────┬──────────────┘
            ▼
┌──────────────────────────┐
│ chaque char ∈ {1, ' '} ? │
└──────┬─────────┬─────────┘
       │non      │oui
       ▼         ▼
   ┌────────┐ ┌────────┐
   │erreur  │ │ OK     │
   └────────┘ └────────┘
```

### scan_row (lignes internes)
```
┌──────────────────────────┐
│ scan_row(state, y)       │
└───────────┬──────────────┘
            ▼
┌──────────────────────────┐
│ get_content_start        │
│ (trim tête/fin)          │
└──────────┬───────────────┘
           ▼
┌──────────────────────────┐
│ first/last non-espace=1?│
└──────────┬───────────────┘
           ▼
┌──────────────────────────┐
│ boucle x depuis start    │
└──────┬─────────┬─────────┘
       │espace   │'0'
       ▼         ▼
┌─────────────────────────────────┐ ┌──────────────────────────┐
│ voisins ∈ {1,' '} ? et pas de   │ │ bords/debordement ?      │
│ debordement vertical ?          │ └──────┬───────────────────┘
└──────┬──────────────────┬───────┘        │
       │oui               │non             ▼
       ▼                  ▼             continue
   continue            erreur                │
       │                                   ┌──┘
       └──────┐                            ▼
              ▼                        erreur
```

#### Helpers (Niveau 2)
- `get_content_start` : saute les espaces de tête et de fin, exige que les premiers/derniers non-espaces soient `'1'`.
- `safe_char` : retourne `' '` si la colonne dépasse la longueur de la ligne sollicitée.
- `space_neighbors_ok` : vérifie que les voisins N/E/S/O d’un espace sont uniquement `1` ou espace (utilise `safe_char`).

## Règles codées
- Carte présente et joueur unique requis (sinon erreur).
- Ligne 0 et dernière : uniquement `1` ou espaces.
- Lignes intermédiaires : premier et dernier non-espace doivent être `1`.
- Espaces internes (après les espaces de tête) : voisins N/E/S/O limités à `1` ou espace, et aucun débordement vertical n’est toléré (sinon mur attendu).
- Cases `'0'` ne doivent pas toucher un débordement : la ligne du haut ou du bas doit être suffisamment longue, et la case ne peut pas être en bord immédiat gauche/droite.
