/**
 * shuodu, a terminal-based Sudoku game and solver with ncurses
 */
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <errno.h>
#include <string.h>
#include <curses.h>
#include <signal.h>

#define CELL_WIDTH 3
#define BOX_WIDTH (3*CELL_WIDTH + 2)    /* 11 */
#define GRID_WIDTH (3*BOX_WIDTH + 2)    /* 35 */

static void finish(int sig);
static void draw_grid(void);
void set_cell(int, int);
int handle_ch(int);
int gy = 1, gx = 1;

long int difficulty = -1;
#define MIN_DIFFICULTY  0
#define MAX_DIFFICULTY  4

#define BUF_LEN         64

/*
 * Grid things
 */

int grid[9][9] = { 0 };

void
set_cell(int row, int col)
{
    int y, x;

    y = 2 * row - 1;
    x = 4 * col - 2;
    move(y, x);
}

void
printgrid(void)
{
    int i, j;
    for (i = 0; i < 9; i++) {
        if (i && i % 3 == 0) {
            printf(" ---------------------\n");
        }
        for (j = 0; j < 9; j++) {
            if (j && j % 3 == 0)
                printf(" |");
            if (grid[i][j] > 0 && grid[i][j] <= 9) {
                printf(" %d", grid[i][j]);
            } else {
                printf("  ");
            }
        }
        printf("\n");
    }
}

int
handle_ch(int c)
{
    switch (c) {
    case 'h':
        set_cell(gy, --gx);
        break;
    case 'j':
        set_cell(++gy, gx);
        break;
    case 'k':
        set_cell(--gy, gx);
        break;
    case 'l':
        set_cell(gy, ++gx);
        break;
    default:
        if (c >= '1' && c <= '9') {
            echochar(c);
            set_cell(gy, gx);
        }
        break;
    }
}

static void
draw_grid(void)
{
    char a[] = "   |   |   ";
    char b[] = "---|---|---";
    int r = 0, row;
    int i;

    addch(ACS_ULCORNER);
    hline(ACS_HLINE, GRID_WIDTH);
    mvaddch(r, BOX_WIDTH + 1, ACS_TTEE);
    mvaddch(r, 2*BOX_WIDTH + 2, ACS_TTEE);
    mvaddch(r, GRID_WIDTH + 1, ACS_URCORNER);

    for (r = 1; r <= 9; r++) {
        row = 2 * r - 1;
        mvaddch(row, 0, ACS_VLINE);
        for (i = 0; i < 3; i++) {
            printw("%s", a);
            addch(ACS_VLINE);
        }

        row++;
        if (r % 3) {
            mvaddch(row, 0, ACS_VLINE);
            for (i = 0; i < 3; i++) {
                printw("%s", b);
                addch(ACS_VLINE);
            }
        } else if (r < 9) {
            mvaddch(row, 0, ACS_VLINE);
            hline(ACS_HLINE, GRID_WIDTH);
            mvaddch(row, BOX_WIDTH + 1, ACS_PLUS);
            mvaddch(row, 2*BOX_WIDTH + 2, ACS_PLUS);
            mvaddch(row, GRID_WIDTH + 1, ACS_VLINE);
        } else {
            mvaddch(row, 0, ACS_LLCORNER);
            hline(ACS_HLINE, GRID_WIDTH);
            mvaddch(row, BOX_WIDTH + 1, ACS_BTEE);
            mvaddch(row, 2*BOX_WIDTH + 2, ACS_BTEE);
            mvaddch(row, GRID_WIDTH + 1, ACS_LRCORNER);
        }
    }
}

static void finish(int sig)
{
    endwin();

    /* do your non-curses wrapup here */

    exit(0);
}

void
print_usage(FILE *stream)
{
    fprintf(stream, "Usage: shuodu [-h | --help] [-d N]\n");
    fprintf(stream, "  -h, --help    - Print this help message and exit.\n");
    fprintf(stream, "  -d <N>,       - Specify the difficulty N (1-4).\n");
}

int
parse_difficulty(char *str)
{
    char *endptr;

    errno = 0;
    difficulty = strtol(str, &endptr, 10);

    if ((errno == ERANGE && (difficulty == LONG_MAX || difficulty == LONG_MIN)) || (errno != 0 && difficulty == 0)) {
        perror("strtol");
        return 1;
    }
    if (endptr == str) {
        fprintf(stderr, "No digits were found.\n");
        return 1;
    }
    if (*endptr != '\0') {
        fprintf(stderr, "Invalid argument %s.\n", endptr);
        return 1;
    }
    if (difficulty < 0 || difficulty > MAX_DIFFICULTY) {
        fprintf(stderr, "Difficulty level %ld out of range.\n", difficulty);
        return 1;
    }

    return 0;
}

static void
parse_options(int argc, char *argv[])
{
    char *option;

    for (int i = 1; i < argc; i++) {
        option = argv[i];
        if (!strcmp(option, "-h") || !strcmp(option, "--help")) {
            fprintf(stdout, "shuodu, a sudoku player in the terminal.\n\n");
            print_usage(stdout);
            exit(EXIT_SUCCESS);
        } else if (!strcmp(option, "-d")) {
            i++;
            if (i >= argc || parse_difficulty(argv[i])) {
                print_usage(stderr);
                exit(EXIT_FAILURE);
            }
        } else {
            fprintf(stderr, "%s: Invalid option `%s'\n", argv[0], option);
            print_usage(stderr);
            exit(EXIT_FAILURE);
        }
    }
}

static void
choose_difficulty(void)
{
    char buf[BUF_LEN];
    char ch;

    do {
        //while ((ch = getchar()/*fgetc(stdin)*/) != '\n' && ch != EOF);
        fprintf(stdout, "Choose a difficulty level [1-4]: ");
        fgets(buf, BUF_LEN - 1, stdin);
        //buf[strlen(buf) - 1] = '\0';
        buf[strnlen(buf) - 1, BUF_LEN] = '\0';
    } while (parse_difficulty(buf));
}

//#define CURSES
int
main(int argc, char *argv[])
{
    parse_options(argc, argv);
    if (difficulty < MIN_DIFFICULTY) {
        choose_difficulty();
    }

    return 0;

#ifdef CURSES
    int num = 0;

    /* initialize your non-curses data structures here */
    //printgrid();

    (void) signal(SIGINT, finish);      /* arrange interrupts to terminate */

    initscr();      /* initialize the curses library */
    draw_grid();
    //(void) newterm();
    keypad(stdscr, TRUE);  /* enable keyboard mapping */
    (void) nonl();         /* tell curses not to do NL->CR/NL on output */
    (void) cbreak();       /* take input chars one at a time, no wait for \n */
#if 0
    (void) echo();         /* echo input - in color */
#else
    (void) noecho();
#endif

    if (has_colors()) {
        start_color();

        /*
         * Simple color assignment, often all we need.  Color pair 0 cannot
         * be redefined.  This example uses the same value for the color
         * pair as for the foreground color, though of course that is not
         * necessary:
         */
        init_pair(1, COLOR_RED,     COLOR_BLACK);
        init_pair(2, COLOR_GREEN,   COLOR_BLACK);
        init_pair(3, COLOR_YELLOW,  COLOR_BLACK);
        init_pair(4, COLOR_BLUE,    COLOR_BLACK);
        init_pair(5, COLOR_CYAN,    COLOR_BLACK);
        init_pair(6, COLOR_MAGENTA, COLOR_BLACK);
        init_pair(7, COLOR_WHITE,   COLOR_BLACK);
    }
    init_pair(8, COLOR_WHITE, COLOR_BLACK);
    bkgd(COLOR_PAIR(8));//wbkgd();

    set_cell(gy, gx);

    for (;;)
    {
        int c = getch();     /* refresh, accept single keystroke of input */
        handle_ch(c);
        attrset(COLOR_PAIR(num % 8));
        num++;

        /* process the command keystroke */
    }

    finish(0);               /* we're done */
#endif /* CURSES */
}

