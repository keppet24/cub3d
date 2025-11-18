# Charte de Documentation du Projet

Ce document définit les règles de documentation pour toute implémentation de code. Il est basé sur la méthodologie qui a permis de créer header_impl.md 

L'agent IA est tenu de suivre ces principes pour garantir une documentation claire, visuelle et maintenable.

1. Principe : Le Format Visuel (Organigramme ASCII)

   Toute la logique (flux de contrôle, étapes, tests conditionnels) doit être représentée par des organigrammes ASCII.

   Le style de référence (Modèle) : Le style visuel doit être identique à celui-ci (cf. docs/parseur/parseur_header/header_impl.md) :

```
┌──────────────────────────┐
│   Nom_de_la_fonction()   │
└───────────┬──────────────┘
            ▼
┌──────────────────────────┐
│ Étape 1 (ex: init)       │
└──────────┬───────────────┘
            ▼
┌──────────────────────────┐
│ Test logique ?           │
└──────┬─────────┬─────────┘
│oui      │non
▼         ▼
┌────────┐ ┌────────────────┐
│ Action A │ │ Action B       │
└────────┘ └────────────────┘
```

   **Interdiction :** Ne pas utiliser de listes à puces ou de pseudo-code pour décrire un flux. La représentation doit être 100% visuelle sous forme d'organigramme.

2. Principe : La Structure Hiérarchique

   La documentation doit refléter l'arborescence et les "couches" d'appels de fonctions.

   - Niveau 0 (Titre ##) : Pour la "Vue d'ensemble" ou le diagramme principal d'une fonctionnalité (ex: parse_header).
   - Niveau 1 (Titre ###) : Pour les fonctions ou blocs logiques appelés directement par le Niveau 0 (ex: line_is_empty, handle_identifier).
   - Niveau 2 et plus (Titre ####) : Pour les sous-fonctions appelées par le Niveau 1 (ex: parse_texture_path), imbriquées visuellement sous leur parent.

3. Principe : La Maintenance

   À chaque ajout ou modification de code (nouvelle fonction, sous-fonction, etc.), la documentation correspondante doit être créée ou mise à jour automatiquement, en s'insérant au bon endroit dans la hiérarchie de la documentation existante.
