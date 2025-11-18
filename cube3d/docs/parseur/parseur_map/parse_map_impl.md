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

### validate_map (à implémenter après parse_map)
```
┌──────────────────────────┐
│ validate_map(map)        │
└───────────┬──────────────┘
            ▼
┌──────────────────────────┐
│ padding rectangulaire    │
└──────────┬───────────────┘
           ▼
┌──────────────────────────┐
│ localiser joueur unique  │
└──────────┬───────────────┘
           ▼
┌──────────────────────────┐
│ flood/voisinage :        │
│  0 touche bord/espaces ? │
└──────┬─────────┬─────────┘
       │oui      │non
       ▼         ▼
   ┌────────┐ ┌────────┐
   │erreur  │ │ OK     │
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
- Jusqu’à un seul joueur : converti en `0`, coordonnées + dir stockées ; erreur si doublon ou joueur absent.
- Ligne vide après début de map rejetée.
- `validate_map` reste à écrire (enceinte fermée).

## Prochaine étape
- Ajouter `validate_map` (padding rectangulaire + flood/voisinage).
- Documenter `validate_map` avec organigrammes et l’insérer sous `docs/parseur/parseur_map/`.
