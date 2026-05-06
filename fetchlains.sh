#!/bin/zsh

GIF_DIR="$HOME/Descargas/gifs"
MARGIN_TXT="$HOME/margin.txt"
GIF_INDEX="$HOME/.gif-index"
FF_TMP="/tmp/.fastfetch_out_$$"

fastfetch --logo "$MARGIN_TXT" --logo-width 1 >"$FF_TMP" &
FF_PID=$!

if [[ -n "$KITTY_WINDOW_ID" || "$TERM" == "xterm-kitty" ]]; then
  PROTO="kitty"
elif [[ "$TERM" == "foot" || "$TERM" == "foot-extra" ]]; then
  PROTO="sixel"
elif [[ "$TERM_PROGRAM" == "WezTerm" ]]; then
  PROTO="kitty"
else
  PROTO="kitty" # default: kitty protocol (GPU accelerated)
fi

typeset -A gif_sizes
while IFS=: read -r name w h; do
  gif_sizes[$name]="$w:$h"
done <"$GIF_INDEX"

names=(${(k)gif_sizes})
SELECTED_NAME=${names[$((RANDOM % ${#names} + 1))]}
SELECTED_GIF="$GIF_DIR/$SELECTED_NAME"
ANCHO_GIF=${${gif_sizes[$SELECTED_NAME]}%%:*}
ALTO_GIF=${${gif_sizes[$SELECTED_NAME]}##*:}

# ── 4. Esperar fastfetch, imprimir su output y limpiar tmpfile ────────────
wait $FF_PID
clear
cat "$FF_TMP"
rm -f "$FF_TMP"

printf "\e[H\e[2B"
timg -p "$PROTO" --loops 0 -g ${ANCHO_GIF}x${ALTO_GIF} "$SELECTED_GIF"
