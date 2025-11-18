# Parser Map – Implémentation (version initiale)

Conforme à `doc.md` : organigrammes ASCII, hiérarchie Nivel 0/1/2, et documentation à jour après codage.

## Vue rapide des appels (Niveau 0)
```
┌────────────────────┐
│ parse_map(fd, st, l)│
└───────────┬────────┘
            ▼
┌────────────────────┐
│ init_map_buffer    │
└───────────┬────────┘
            ▼
┌────────────────────┐
│ handle_map_line(l) │ // first_map_line
└───────────┬────────┘
            ▼
┌────────────────────┐
│ boucle get_next_line│
└──────┬─────┬───────┘
       │EOF  │ligne
       ▼     ▼
   ┌────────┐┌────────────────────┐
   │return  ││ handle_map_line()  │
   └────────┘└─────────┬──────────┘
                        ▼
                free(line) si ok
```

## Sous-graphes par fonction (Niveau 1)

### parse_map (boucle principale)
```
┌──────────────────────────┐
│ parse_map(fd, st, line)  │
└───────────┬──────────────┘
            ▼
┌──────────────────────────┐
│ init_map_buffer          │
└──────────┬───────────────┘
           ▼
┌──────────────────────────┐
│ handle_map_line(first)   │
└──────┬─────────┬─────────┘
       │erreur   │ok
       ▼         ▼
   ┌────────┐ ┌────────────────┐
   │return  │ │ boucle GNL     │
   └────────┘ └──────┬─────────┘
                     ▼
             ligne vide/espaces ? ─▶ fin_de_carte=true, free
                     │non
                     ▼
             fin_de_carte ? ─────▶ erreur (contenu après fin)
                     │non
                     ▼
             handle_map_line(line)
                     │
             erreur ?│
                     ▼
             return false
                     │
                     ▼
             free(line) et continue
```

### handle_map_line
```
┌──────────────────────────┐
│ handle_map_line(buf, st,¶│
│                line)     │
└───────────┬──────────────┘
            ▼
┌──────────────────────────┐
│ ligne vide ?             │
└──────┬─────────┬─────────┘
       │oui      │non
       ▼         ▼
   ┌────────┐ ┌──────────────────────────┐
   │erreur  │ │ charset {0,1,' ',N,S,E,W}│
   └────────┘ └──────┬─────────┬─────────┘
                     │ko       │ok
                     ▼         ▼
                ┌────────┐ ┌────────────────┐
                │erreur  │ │ joueur unique ?│
                └────────┘ └──────┬─────────┬┘
                                   │ko       │ok
                                   ▼         ▼
                              ┌────────┐ ┌───────────────┐
                              │erreur  │ │ push_line     │
                              └────────┘ └───────────────┘
```

### validate_map (vérification de fermeture)
```
┌──────────────────────────┐
│ validate_map(state)      │
└───────────┬──────────────┘
            ▼
┌──────────────────────────┐
│ map présente ? joueur=1 ?│
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

#### scan_row (lignes internes)
```
┌──────────────────────────┐
│ scan_row(state, y)       │
└───────────┬──────────────┘
            ▼
┌──────────────────────────┐
│ trim espaces tête/fin    │
│ → first/last doivent = 1 │
└──────────┬───────────────┘
           ▼
┌──────────────────────────┐
│ len_top / len_bot        │
└──────────┬───────────────┘
           ▼
┌──────────────────────────┐
│ boucle x depuis first    │
└──────┬─────────┬─────────┘
       │overflow │espace
       ▼         ▼
┌──────────────┐ ┌──────────────────────┐
│ si dépasse   │ │ voisins ∈ {1,' '} ? │
│ top/bot → '1'│ └──────┬───────────────┘
└──────────────┘        ▼
                        erreur ?
                        │oui  │non
                        ▼     ▼
                    ┌────────┐ ┌────────┐
                    │erreur  │ │continue│
                    └────────┘ └────────┘
```

#### init_map_buffer / push_line (Niveau 2)
```
┌──────────────────────────┐
│ init_map_buffer          │
└──────────┬──────────────┘
           ▼
  lines=NULL, size=0, cap=0

┌──────────────────────────┐
│ push_line(buf, line, st) │
└──────────┬──────────────┘
           ▼
┌──────────────────────────┐
│ push_line_resize(buf)    │
└──────┬─────────┬─────────┘
       │false    │true
       ▼         ▼
  ┌────────┐ ┌──────────────────────────┐
  │erreur  │ │ dup(line) → buf->lines[] │
  └────────┘ └──────────┬───────────────┘
                        ▼
┌──────────────────────────┐
│ maj map_width/map_height │
└──────────────────────────┘
```

## Hypothèses / contrôles actuels
- Charset strict : `0`, `1`, espaces, `N/S/E/W` (tabs convertis en espaces).
- Jusqu’à un seul joueur : converti en `0`, coordonnées + dir stockées ; erreur si doublon ou joueur absent (re-check dans `validate_map`).
- Ligne vide après début de carte marque la fin de la carte ; toute ligne non vide après cet arrêt déclenche une erreur.
- `validate_map` vérifie :
  - lignes bord haut/bas uniquement `1` ou espace ;
  - lignes internes : premier et dernier non-espace = `1` ;
  - débordement par rapport aux longueurs haut/bas → caractères obligatoirement `1` ;
  - espaces internes entourés uniquement de `1` ou d’espaces (voisins N/E/S/O).

## Prochaine étape
- Étendre plus tard aux vérifications supplémentaires si nécessaire (ex: flood-fill pour cas extrêmes).
