#!/bin/sh
# Lance cub3d sur toutes les cartes map/bad et vérifie que le parseur détecte une erreur.

set -u

BIN="./cub3d"
DIR="map/bad"
ok=0
ko=0

if [ ! -x "$BIN" ]; then
	echo "Binaire introuvable : $BIN (lance 'make' avant)" >&2
	exit 1
fi

for f in "$DIR"/*; do
	if [ ! -f "$f" ]; then
		continue
	fi
	printf "%-25s : " "$(basename "$f")"
	$BIN "$f" >/dev/null 2>&1
	if [ $? -eq 0 ]; then
		echo "KO (aucune erreur detectee)"
		ko=$((ko + 1))
	else
		echo "OK (erreur attendue)"
		ok=$((ok + 1))
	fi
done

echo "Bilan: $ok OK / $ko KO"

if [ $ko -ne 0 ]; then
	exit 1
fi

exit 0
