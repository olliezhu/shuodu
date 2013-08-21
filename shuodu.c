/*
 * shuodu, a terminal-based Sudoku game and solver
 * with ncurses
 * http://invisible-island.net/ncurses/ncurses-intro.html
 * http://www.writeka.com/ed/ncurses_library.html
 * tint
 */
#include <stdlib.h>
#include <curses.h>
#include <signal.h>

#define CELL_WIDTH 3
#define BOX_WIDTH (3*CELL_WIDTH + 2) // 11
#define GRID_WIDTH (3*BOX_WIDTH + 2) // 35

static void finish(int sig);
static void draw_grid(void);

int
main(int argc, char *argv[])
{
    int num = 0;

    /* initialize your non-curses data structures here */

    (void) signal(SIGINT, finish);      /* arrange interrupts to terminate */

    initscr();      /* initialize the curses library */
    draw_grid();
    //(void) newterm();
    keypad(stdscr, TRUE);  /* enable keyboard mapping */
    (void) nonl();         /* tell curses not to do NL->CR/NL on output */
    (void) cbreak();       /* take input chars one at a time, no wait for \n */
    (void) echo();         /* echo input - in color */

    if (has_colors())
    {
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

    for (;;)
    {
        int c = getch();     /* refresh, accept single keystroke of input */
        attrset(COLOR_PAIR(num % 8));
        num++;

        /* process the command keystroke */
    }

    finish(0);               /* we're done */
}

static void
draw_grid(void)
{
    static const char regionrow[] = " | | ";
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
