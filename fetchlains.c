#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <fcntl.h>

#define MAX_GIFS  64
#define MAX_NAME  256
#define MAX_PATH  2048
#define MAX_LINE  512
#define TMPFILE   "/tmp/.fastfetch_out"

typedef struct {
    char name[MAX_NAME];
    int  w, h;
} GifEntry;

static void die(const char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

static void expand_home(const char *in, char *out, size_t outlen)
{
    const char *home = getenv("HOME");
    if (!home) { fputs("HOME not set\n", stderr); exit(1); }
    if (in[0] == '~' && (in[1] == '/' || in[1] == '\0'))
        snprintf(out, outlen, "%s%s", home, in + 1);
    else
        snprintf(out, outlen, "%s", in);
}

static void dump_and_remove(const char *path)
{
    FILE *f = fopen(path, "r");
    if (!f) return;
    char buf[4096];
    size_t n;
    while ((n = fread(buf, 1, sizeof(buf), f)) > 0)
        fwrite(buf, 1, n, stdout);
    fflush(stdout);
    fclose(f);
    unlink(path);
}

static void xwrite(int fd, const char *s, size_t n)
{
    while (n > 0) {
        ssize_t r = write(fd, s, n);
        if (r < 0) die("write");
        s += r; n -= (size_t)r;
    }
}

static const char *detect_protocol(void)
{
    if (getenv("KITTY_WINDOW_ID")) return "kitty";

    const char *term = getenv("TERM");
    if (term) {
        if (strcmp(term, "xterm-kitty") == 0)               return "kitty";
        if (strcmp(term, "foot") == 0 ||
            strcmp(term, "foot-extra") == 0)                 return "sixel";
    }

    const char *term_program = getenv("TERM_PROGRAM");
    if (term_program && strcmp(term_program, "WezTerm") == 0) return "kitty";

    return "kitty";
}


int main(void)
{
    char gif_dir[MAX_PATH], margin_txt[MAX_PATH], gif_index[MAX_PATH];
    expand_home("~/Descargas/gifs", gif_dir,    sizeof(gif_dir));
    expand_home("~/margin.txt",     margin_txt, sizeof(margin_txt));
    expand_home("~/.gif-index",     gif_index,  sizeof(gif_index));

    pid_t ff_pid = fork();
    if (ff_pid < 0) die("fork");

    if (ff_pid == 0) {
        int fd = open(TMPFILE, O_WRONLY | O_CREAT | O_TRUNC, 0600);
        if (fd < 0) die("open tmpfile");
        if (dup2(fd, STDOUT_FILENO) < 0) die("dup2");
        close(fd);
        execlp("fastfetch", "fastfetch",
               "--logo", margin_txt, "--logo-width", "1", (char *)NULL);
        die("execlp fastfetch");
    }

    GifEntry gifs[MAX_GIFS];
    int gif_count = 0;

    FILE *idx = fopen(gif_index, "r");
    if (!idx) die("fopen ~/.gif-index");

    char line[MAX_LINE];
    while (fgets(line, sizeof(line), idx) && gif_count < MAX_GIFS) {
        line[strcspn(line, "\r\n")] = '\0';
        if (line[0] == '\0') continue;
        char name[MAX_NAME];
        int  w, h;
        if (sscanf(line, "%255[^:]:%d:%d", name, &w, &h) != 3) continue;
        memcpy(gifs[gif_count].name, name, MAX_NAME);
        gifs[gif_count].w = w;
        gifs[gif_count].h = h;
        gif_count++;
    }
    fclose(idx);

    if (gif_count == 0) {
        fputs("gif-index empty or malformed\n", stderr);
        exit(1);
    }

    srand((unsigned)time(NULL) ^ (unsigned)getpid());
    GifEntry *sel = &gifs[rand() % gif_count];

    char gif_path[MAX_PATH + MAX_NAME + 1];
    snprintf(gif_path, sizeof(gif_path), "%s/%s", gif_dir, sel->name);

    char geom[32];
    snprintf(geom, sizeof(geom), "%dx%d", sel->w, sel->h);

    const char *proto = detect_protocol();

    waitpid(ff_pid, NULL, 0);
    xwrite(STDOUT_FILENO, "\033[2J\033[H", 7);
    dump_and_remove(TMPFILE);

    xwrite(STDOUT_FILENO, "\033[H\033[2B", 7);

    execlp("timg", "timg",
           "-p", proto, "--loops", "0", "-g", geom, gif_path, (char *)NULL);
    die("execlp timg");
}
