/**
 * Sudoku grid
 */

/* constants */
#define CELL_WIDTH 3
#define BOX_WIDTH (3*CELL_WIDTH + 2)    /* 11 */
#define GRID_WIDTH (3*BOX_WIDTH + 2)    /* 35 */
#define GY_0    1
#define GX_0    1

/* functions */
void finish(int sig);

void draw_grid(void);
void print_grid(void);

int handle_ch(int);

void set_cell(int row, int col);
int insert_cell(int row, int col, int ans);
