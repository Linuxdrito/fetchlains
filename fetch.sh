#!/bin/zsh

GIF_DIR="$HOME/Descargas/gifs"
MARGIN_TXT="$HOME/margin.txt"

gifs=($GIF_DIR/*.gif)
SELECTED_GIF=$gifs[$((RANDOM % ${#gifs} + 1))]


case "$SELECTED_GIF" in
    *lain-gift.gif)
        ANCHO_GIF=36; ALTO_GIF=36 ;;
    *lain8.gif)
        ANCHO_GIF=33; ALTO_GIF=33 ;;
    *lain9.gif)
        ANCHO_GIF=37; ALTO_GIF=37 ;;
    *)
        ANCHO_GIF=35; ALTO_GIF=35 ;;
esac

clear

fastfetch --logo "$MARGIN_TXT" --logo-width 1

printf "\e[H\e[2B"

timg -p sixel --loops 0 -g ${ANCHO_GIF}x${ALTO_GIF} "$SELECTED_GIF"
