# Design parseur_map – Étape suivante

Ce document suit la charte `doc.md` : organigrammes ASCII, hiérarchie de titres reflétant les couches d’appel.

## Vue rapide des appels (Niveau 0)
```
┌────────────────────┐
│ parse_map(fd, st, l)│   // st = parser_state, l = first_map_line
└─────────┬──────────┘
          ▼
┌────────────────────┐
│ init_map_buffer    │
└─────────┬──────────┘
          ▼
┌────────────────────┐
│ push first_map_line│
└─────────┬──────────┘
          ▼
┌────────────────────┐
│ boucle get_next_line│
└──────┬─────┬───────┘
       │EOF  │ligne
       ▼     ▼
   ┌────────┐┌────────────────────┐
   │fin ok  ││ handle_map_line()  │
   └────────┘└─────────┬──────────┘
                        ▼
                ┌───────────────┐
                │ push + checks │
                └───────┬───────┘
                        ▼
                ┌───────────────┐
                │ continuer     │
                └───────────────┘
```
Idée clé : on accumule toutes les lignes de carte, on contrôle immédiatement les fautes grossières, puis `validate_map` vérifiera l’enceinte.

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
│ push(first_map_line)     │
└──────────┬───────────────┘
           ▼
┌──────────────────────────┐
│ boucle get_next_line     │
└──────┬─────────┬─────────┘
       │EOF      │ligne
       ▼         ▼
   ┌────────┐ ┌────────────────────┐
   │ return │ │ handle_map_line()  │
   └────────┘ └─────────┬──────────┘
                        ▼
           ┌──────────────────────────┐
           │ erreur ? → return false  │
           └───────────┬──────────────┘
                       ▼
                 free(line) et continue
```

### handle_map_line
```
┌──────────────────────────┐
│ handle_map_line(...)     │
└───────────┬──────────────┘
            ▼
┌──────────────────────────┐
│ ligne vide après début ? │
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

#### init_map_buffer / push_line (Niveau 2, utilitaires)
```
┌──────────────────────────┐
│ init_map_buffer          │
└──────────┬──────────────┘
           ▼
  size=0, capacity=K, alloc char**

┌──────────────────────────┐
│ push_line(buffer, line)  │
└──────────┬──────────────┘
           ▼
  size==capacity ? → realloc
  dup(line) → buffer[size++]
```

## Règles et contraintes (pour implémenter parse_map)
- Autoriser les espaces en tête de ligne de carte (font partie de la carte).
- Refuser toute ligne vide une fois la carte commencée.
- Autoriser exactement un joueur (N/S/E/W), convertir la case en 0 pour le flood-fill ultérieur.
- Les contrôles fins d’enceinte se font dans `validate_map`.

## Plan d’implémentation
1. Ajouter `t_map_buffer` (size, capacity, char **lines) + helpers init/push/free.
2. Implementer `parse_map` avec les contrôles immédiats (charset, joueur unique, pas de lignes vides après début).
3. Implémenter `validate_map` (padding, localisation joueur, flood/voisinage).
4. Documenter `parse_map` et `validate_map` dans `docs/parseur/parseur_map/` selon la charte `doc.md`.

## Exemple concret (parsing pas à pas)
Cartes entrée :
```
NO ./tex/no.xpm
SO ./tex/so.xpm
WE ./tex/we.xpm
EA ./tex/ea.xpm
F 220,100,0
C 225,30,0
11111
10N01
10001
11111
```
Séquence `parse_map` :
- `first_map_line` = `"11111"` (transmise par `parse_header`).
- `init_map_buffer` → buffer vide (size=0).
- `push_line` du `"11111"` initial → buffer[0] = "11111".
- Boucle `get_next_line` :
  - lit `"10N01"` → `handle_map_line` vérifie charset + joueur unique (N), puis `push_line`.
  - lit `"10001"` → contrôle charset, push.
  - lit `"11111"` → contrôle charset, push.
  - EOF atteint → sortie boucle.
- À ce stade, le buffer contient 4 lignes de carte. `parse_map` renvoie `true`.
- `validate_map` (plus tard) prendra ce buffer, le remplira en rectangle si besoin et fera le flood/voisinage pour garantir que les `0` ne touchent ni le vide ni le bord.
