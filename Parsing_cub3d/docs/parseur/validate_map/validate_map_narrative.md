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
  - Pour chaque espace non-leading, `space_neighbors_ok` contrôle que N/E/S/O sont `1` ou espace et refuse un débordement vertical (colonne au-delà de la ligne du haut ou du bas).
  - Pour chaque `0`, on refuse tout débordement vertical et `zero_neighbors_closed` garantit qu’il n’est pas collé aux bords gauche/droite.
- **Ce que ça modifie** : rien.
- **Exemple** : un `0` placé sur la première colonne ou au-delà de la longueur de la ligne du haut échoue avec “Carte non fermee verticalement”.

### 3.4 Helpers internes (objectif / stratégie / modifs / exemple)
- **get_content_start(const char *line, size_t *start)**
  - Objectif : trouver le premier et le dernier non-espace d’une ligne interne et imposer qu’ils valent `'1'`.
  - Stratégie : avance `start` en sautant les espaces de tête, recule `end` en sautant ceux de fin ; si vide ou extrémités ≠ `1`, erreur “Carte non fermee sur les bords”.
  - Modifie : écrit dans `*start` uniquement.
  - Exemple : `"  1 0010  "` → `start=2`, `end=9`, OK ; `"   0  "` → échec.
- **safe_char(t_parser_state *st, size_t y, size_t x)**
  - Objectif : lire un voisin potentiel sans risque de dépassement.
  - Stratégie : si `x` dépasse `len(line[y])`, retourne `' '`, sinon retourne le caractère demandé.
  - Modifie : rien.
  - Exemple : sur une ligne plus courte, `safe_char(st, y, len+3)` renvoie `' '`.
- **space_neighbors_ok(t_parser_state *st, size_t y, size_t x)**
  - Objectif : valider les voisins immédiats N/E/S/O d’un espace interne.
  - Stratégie : récupère `up/down/left/right` via `safe_char` (gauche forcée à `' '` si `x == 0`), exige que chaque voisin ∈ `{1, ' '}`.
  - Modifie : rien.
  - Exemple : un espace jouxtant un caractère `2` ferait échouer le check.
- **zero_neighbors_closed(t_parser_state *st, size_t y, size_t x)**
  - Objectif : refuser un `0` posé sur un débordement vertical ou collé aux bords latéraux.
  - Stratégie : calcule les longueurs des lignes `y-1`, `y`, `y+1`; échoue si `x` dépasse len_up ou len_down, ou si `x` est 0 ou si `x+1` dépasse la ligne courante.
  - Modifie : rien.
  - Exemple : un `0` en bord gauche (`x == 0`) ou sous une ligne plus courte est rejeté (“Carte non fermee verticalement”).

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
- Les espaces de tête ne sont pas inspectés pour les voisins ; seuls les espaces après `get_content_start` sont soumis au voisinage (et tolèrent uniquement `1`/espace).
- Les `0` placés en bord ou dépassant la longueur d’une ligne voisine échouent via “Carte non fermee verticalement”.
