/**
 * Drawing, navigating, and modifying the Sudoku grid
 */
#include <stdlib.h>
#include <curses.h>

#include "grid.h"

int grid[9][9] = { 0 };
int gy = GY_0, gx = GX_0;

void finish(int sig)
{
    endwin();

    /* do your non-curses wrapup here */

    exit(0);
}

void
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

void
print_grid(void)
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

void
set_cell(int row, int col)
{
    int y, x;

    y = 2 * row - 1;
    x = 4 * col - 2;
    move(y, x);
}

int
insert_cell(int row, int col, int ans)
{
    /* TODO check if insert is a legal move */
    //check_block(row, col, ans);
    //check_row(row, col, ans);
    //check_col(row, col, ans);
    grid[row][col] = ans;

    return 0;
}
