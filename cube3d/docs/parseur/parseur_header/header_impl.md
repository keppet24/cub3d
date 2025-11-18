# Parser Header – Implémentation (version actuelle)

Ce document suit la même structure visuelle que `design/parser/design_parser.md` : d’abord un diagramme aéré des appels, puis des fiches fonctions détaillées.

## Vue rapide des appels (ASCII)
```
┌───────────────────────────────┐
│ parse_header(fd, state, line) │
└───────────────┬───────────────┘
                │ boucle
                ▼
      ┌─────────────────┐
      │ get_next_line() │
      └────────┬────────┘
               │
               ▼
      ┌───────────────────┐
      │ trim_newline      │
      └────────┬──────────┘
               │
   ┌───────────▼───────────┐
   │ line_is_empty ?       │
   └───────────┬───────────┘
               │non
               ▼
   ┌───────────────────────┐
   │ looks_like_map_line ? │
   └──────┬────────┬───────┘
          │non     │oui
          ▼        ▼
┌────────────────┐  ┌─────────────────────┐
│handle_identifier│  │ header_complete ?  │
└──────┬─────────┘  └──────────┬──────────┘
       │                      │
       ▼                      ▼
┌──────────────┐      ┌────────────────────┐
│parse_texture │      │return first_map_line│
│parse_color   │      └────────────────────┘
└──────────────┘
```
Idée clé : tant que la ligne n’est pas une map, on reste dans les identifiants. Si une ligne de map arrive mais que le header est incomplet → erreur immédiate.

## Sous-graphes par fonction (même style que la vue rapide)

### get_next_line (boucle de lecture)
```
┌──────────────────────────┐
│    get_next_line(fd)     │
└───────────┬──────────────┘
            ▼
┌──────────────────────────┐
│ line == NULL ?           │
└──────┬─────────┬─────────┘
       │oui      │non
       ▼         ▼
┌──────────────┐ ┌────────────────────┐
│ return (EOF) │ │ ligne disponible   │
└──────────────┘ └────────────────────┘
```

### trim_newline
```
┌──────────────────────────┐
│    trim_newline(line)    │
└───────────┬──────────────┘
            ▼
┌──────────────────────────┐
│ fin == '\n' ?            │
└──────┬─────────┬─────────┘
       │oui      │non
       ▼         ▼
┌────────────────┐ ┌────────────────┐
│ line[len-1]=0  │ │ ligne inchangée│
└────────────────┘ └────────────────┘
```

### line_is_empty
```
┌──────────────────────────┐
│   line_is_empty(line)    │
└───────────┬──────────────┘
            ▼
┌──────────────────────────┐
│ parcourir chaque char    │
└──────────┬───────┬───────┘
           │trouve │fin
           ▼       ▼
┌────────────────┐ ┌────────────────┐
│ char non-espace│ │ return true    │
│ → return false │ └────────────────┘
└────────────────┘
```

### looks_like_map_line
```
┌──────────────────────────┐
│  looks_like_map_line()   │
└───────────┬──────────────┘
            ▼
┌──────────────────────────┐
│ skip espaces de tête     │
└──────────┬───────────────┘
           ▼
┌──────────────────────────┐
│ fin de chaîne ?          │
└──────┬─────────┬─────────┘
       │oui      │non
       ▼         ▼
   ┌────────┐ ┌──────────────────┐
   │false   │ │ préfixe NO/SO/…? │
   └────────┘ └──────┬─────┬─────┘
                     │oui  │non
                     ▼     ▼
                 ┌─────┐ ┌──────────────────────────┐
                 │false│ │ char ∈ {0,1,N,S,E,W,' '}?│
                 └─────┘ └──────┬─────────┬─────────┘
                                 │oui      │non
                                 ▼         ▼
                             ┌─────┐   ┌──────┐
                             │true │   │false│
                             └─────┘   └──────┘
```

### handle_identifier
```
┌───────────────────────────────┐
│handle_identifier(line, state) │
└───────────────┬───────────────┘
                ▼
┌──────────────────────────┐
│ espaces en début ?       │
└──────┬─────────┬─────────┘
       │oui      │non
       ▼         ▼
┌──────────────┐ ┌───────────────┬───────────────┬──────────────────────┐
│ erreur       │ │ NO/SO/WE/EA   │   F ou C     │  sinon                │
└──────────────┘ └──────┬────────┴─────┬────────┴──────────────┬───────┘
                        │              │                      │
                        ▼              ▼                      ▼
                 ┌────────────────┐ ┌───────────────────┐ ┌────────────────────┐
                 │parse_texture_path│ │parse_color_value │ │erreur identifiant  │
                 └────────────────┘ └───────────────────┘ └────────────────────┘
```

#### Sous-fonction de handle_identifier : parse_texture_path
```
┌──────────────────────────┐
│ parse_texture_path(...)  │
└───────────┬──────────────┘
            ▼
┌──────────────────────────┐
│ skip espaces de tête     │
└──────────┬───────────────┘
           ▼
┌──────────────────────────┐
│ champ vide ?           │
└──────┬─────────┬───────┘
       │oui      │non
       ▼         ▼
┌────────┐ ┌────────────────────────┐
│erreur  │ │ déjà défini ?          │
└────────┘ └──────┬─────────┬───────┘
                  │oui      │non
                  ▼         ▼
         ┌────────┐ ┌────────────────────────┐
         │erreur  │ │ trim_trailing_spaces   │
         └────────┘ └─────────┬──────────────┘
                               ▼
                      ┌────────────────────────┐
                      │ espaces internes ?     │
                      └──────┬─────────┬───────┘
                             │oui      │non
                             ▼         ▼
                      ┌────────┐ ┌────────────────┐
                      │erreur  │ │ check .xpm ?   │
                      └────────┘ └──────┬─────────┬──────────┐
                                         │non      │oui       │
                                         ▼         ▼          ▼
                                ┌────────┐ ┌────────┐ ┌────────────────┐
                                │erreur  │ │ open ok│ │ ft_strdup → tex│
                                └────────┘ │ ?       │ └────────────────┘
                                           └──┬──────┘
                                              ▼
                                         ┌────────┐
                                         │erreur  │
                                         └────────┘
```

#### Sous-fonction de handle_identifier : parse_color_value
```
┌──────────────────────────┐
│ parse_color_value(...)   │
└───────────┬──────────────┘
            ▼
┌──────────────────────────┐
│ doublon ?                │
└──────┬─────────┬─────────┘
       │oui      │non
       ▼         ▼
  ┌────────┐ ┌──────────────────────────┐
  │erreur  │ │ ft_split(payload, ',')   │
  └────────┘ └─────────┬────────────────┘
                       ▼
┌──────────────────────────┐
│ 3 valeurs ?             │
└──────┬─────────┬─────────┘
       │non      │oui
       ▼         ▼
  ┌────────┐ ┌──────────────────────────┐
  │erreur  │ │ parse_component sur chaque│
  └────────┘ └─────────┬────────────────┘
                       ▼
┌──────────────────────────┐
│ composante invalide ?    │
└──────┬─────────┬─────────┘
       │oui      │non
       ▼         ▼
  ┌────────┐ ┌────────────────────────┐
  │erreur  │ │ set r/g/b et set=true  │
  └────────┘ └────────────────────────┘
```

##### parse_component (sous-fonction de parse_color_value)
```
┌──────────────────────────┐
│ parse_component(str)     │
└───────────┬──────────────┘
            ▼
┌──────────────────────────┐
│ skip espaces de tête     │
└──────────┬───────────────┘
           ▼
┌──────────────────────────┐
│ entier numérique ?       │
└──────┬─────────┬─────────┘
       │non      │oui
       ▼         ▼
  ┌────────┐ ┌────────────────────────┐
  │false   │ │ accumuler 0..255       │
  └────────┘ └─────────┬──────────────┘
                       ▼
┌──────────────────────────┐
│ skip espaces de fin      │
└──────────┬───────────────┘
           ▼
┌──────────────────────────┐
│ fin de chaîne ? et 0..255?│
└──────┬─────────┬─────────┘
       │oui      │non
       ▼         ▼
   ┌────────┐ ┌────────┐
   │ true   │ │ false  │
   └────────┘ └────────┘
```

### header_complete
```
┌──────────────────────────┐
│    header_complete()     │
└───────────┬──────────────┘
            ▼
┌──────────────────────────┐
│ NO/SO/WE/EA définies ?   │
└──────┬─────────┬─────────┘
       │non      │oui
       ▼         ▼
   ┌────────┐ ┌──────────────────────────┐
   │false   │ │ floor.set && ceiling.set?│
   └────────┘ └──────┬─────────┬─────────┘
                     │non      │oui
                     ▼         ▼
                 ┌──────┐  ┌────────┐
                 │false │  │ true   │
                 └──────┘  └────────┘
```


## Hypothèses actuelles
- Les lignes vides avant la carte sont autorisées et ignorées.
- Aucun identifiant après le démarrage de la carte n’est toléré : la détection d’une ligne de map avant un header complet provoque une erreur.
- Les erreurs passent toujours par `print_error` pour respecter le format `"Error\n..."`.

Ce document doit être mis à jour dès que la logique du header change ou qu’un nouveau contrôle est ajouté.
