# fetch

> fastfetch + lain gifs for my Hyprland setup.

The original zsh script worked fine, but I wanted to eliminate shell overhead and take advantage of the fact that `fastfetch` and GIF selection can run in parallel..


<img width="406" height="500" alt="lain10" src="https://github.com/user-attachments/assets/72a2b08c-8a43-4a29-adab-cef65cdfd683" /> <img width="458" height="566" alt="lain1" src="https://github.com/user-attachments/assets/712ae26b-090b-4d03-acb2-0d0a346b5988" />



---

## What it does

1. **Immediate hehe** — launches `fastfetch` in the background, redirecting its output to a tmpfile, without waiting.
2. **In parallel** — reads `~/.gif-index` and picks a random GIF with its precomputed dimensions.
3. **Sync** — waits for `fastfetch`, clears the screen, dumps the output.
4. **`execlp` timg** — replaces the process itself with `timg` (no extra fork, no zombie process). Animated GIF with `--loops 0`.

---

## Files

```
fetch.c        — source
fetch.sh       — original zsh script (reference)
gif-index      — GIF index with precomputed dimensions → copy to ~/.gif-index
margin.txt     — fastfetch logo → copy to ~/margin.txt

```

---

## Dependencies

```sh
fastfetch timg gcc
```

---

## Setup

```sh
# 1. Place the index
mkdir ~/Descargas && cp -r ~/fetchlains/gifs ~/Descargas/

# 2. Place the index
cp gif-index ~/.gif-index

# 3. Place the logo
cp margin.txt ~/margin.txt

# 4. Compile
gcc -O2 -march=native -flto -pipe -s -o fetch fetch.c

# 5. Install
sudo mv fetch /usr/local/bin/fetch
```

---

## gif-index

Format: `name.gif:width:height`. One GIF per line, no spaces.

```
lain-gift.gif:36:36
lain8.gif:33:33
lain9.gif:37:37
lain1.gif:35:35
lain10.gif:35:35
lain2.gif:35:35
lain4.gif:35:35
lain5.gif:35:35
```

GIFs live in `~/Descargas/gifs/`. To add a new one just append a line to the index — no recompilation needed.

---

## Hyprland integration

In `hyprland.conf`:

```ini
bind = $mainMod, H, exec, footclient fetch
```

---

## Why not the zsh script

| | `fetch.sh` | `fetch` (C) |
|---|---|---|
| Startup overhead | ~15ms (zsh) | ~1ms |
| fastfetch + GIF selection | Sequential | **Parallel** |
| Launching timg | fork + exec | `execlp` (replaces the process) |
| GIF dimensions | `case` at runtime | Read from index |

