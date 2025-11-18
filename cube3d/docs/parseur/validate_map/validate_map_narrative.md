# Revue de Conception Narrative – validate_map.c

## 1. But du module
- Assurer que la carte déjà lue par `parse_map` est fermée : pas de trous aux bords, pas de débordements ouverts, espaces correctement encadrés.
- Ne modifie pas la carte : renvoie `true/false` et imprime une erreur via `print_error` en cas de violation.
- Renforce l’invariant “un seul joueur” avant d’aller plus loin dans l’exécution.

## 2. Structures importantes
- `t_parser_state` : on lit `map_lines`, `map_height`, et `player_count`. Aucune écriture, uniquement des checks.
- Les helpers internes (`get_content_start`, `safe_char`, `space_neighbors_ok`) travaillent à partir des chaînes stockées dans `map_lines`.

## 3. Fonctions principales
### 3.1 validate_map(t_parser_state *state)
- **Objectif** : orchestrer les contrôles de fermeture sur toutes les lignes.
- **Stratégie** :
  - Rejette immédiatement si la carte est absente ou si `player_count != 1`.
  - Parcourt chaque ligne : bord haut/bas via `check_border_line`, lignes internes via `scan_row`.
  - S’arrête au premier échec (retour false).
- **Ce que ça modifie** : rien ; lecture seule sur `state`.
- **Exemple** : une première ligne contenant un `0` fait échouer `check_border_line`.

### 3.2 check_border_line(const char *line)
- **Objectif** : garantir des bords supérieurs/inférieurs constitués uniquement de `1` ou d’espaces.
- **Stratégie** : boucle sur la ligne ; tout caractère hors `{1, ' '}` déclenche une erreur “Carte ouverte sur le bord”.
- **Ce que ça modifie** : rien.
- **Exemple** : `"11 110"` est accepté, `"11 10"` échoue.

### 3.3 scan_row(t_parser_state *st, size_t y)
- **Objectif** : vérifier une ligne interne : bords latéraux fermés, débordements murés, espaces entourés.
- **Stratégie** :
  - `get_content_start` trouve le premier/dernier non-espace et impose `'1'`.
  - Pour chaque espace, `space_neighbors_ok` contrôle que N/E/S/O sont `1`, espaces ou `0`.
  - Pour chaque `0`, `zero_neighbors_closed` garantit qu’il n’est pas en débordement (lignes du haut/bas suffisamment longues, pas sur les bords gauche/droite immédiats).
- **Ce que ça modifie** : rien.
- **Exemple** : un `0` placé sur la première colonne ou au-delà de la longueur de la ligne du haut échoue avec “Carte non fermee verticalement”.

### 3.4 Helpers internes
- **get_content_start** : saute les espaces de tête/fin, échoue si la ligne est vide ou si les extrémités ne sont pas `1`.
- **safe_char** : retourne `' '` si la colonne dépasse la longueur de la ligne ciblée, sinon le caractère demandé.
:colon: Permet de tester les voisins sans segfault.
- **space_neighbors_ok** : contrôle que les voisins d’un espace (N/E/S/O) sont dans `{1, ' '}` en s’appuyant sur `safe_char`.

## 4. Guide de lecture du code
1. Lire `validate_map` pour l’orchestration (checks globaux puis boucle lignes).
2. Voir `check_border_line` pour comprendre le contrôle des bords haut/bas.
3. Parcourir `scan_row` : ordre des tests (bords latéraux, débordements, espaces internes).
4. Noter que `safe_char` renvoie `' '` si on sort de la ligne, évitant toute lecture hors borne.

## 5. Check-list de compréhension
- Je sais ce qui déclenche une erreur immédiate (carte absente, joueur absent/multiple).
- Je peux expliquer pourquoi les bords haut/bas sont limités à `{1, ' '}`.
- Je sais que pour un espace interne, tous les voisins N/E/S/O doivent être `1` ou espace.
- Je sais que pour un `0`, il ne doit pas toucher un débordement (pas au bord, lignes voisines assez longues).
- Je sais que `validate_map` ne modifie rien et se contente de valider.

## Pièges / invariants importants
- `player_count` doit déjà valoir 1 (double-check ici).
- Une ligne faite uniquement d’espaces échoue car `get_content_start` impose des `1` en extrémités non-espaces.
- Les espaces de tête ne sont pas inspectés pour les voisins ; seuls les espaces après `get_content_start` sont soumis au voisinage (et tolèrent `0`/`1`/espace).
- Les `0` placés en bord ou dépassant la longueur d’une ligne voisine échouent via “Carte non fermee verticalement”.
