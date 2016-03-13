/**
 * Grid things
 */

int grid[9][9] = { 0 };

void
set_cell(int row, int col)
{
    int y, x;

    y = 4 * row - 1;
    x = 2 * col;
    move(y, x);
}
