//#define CURSES

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
#include <curl/curl.h>

#include "grid.h"


long int difficulty = -1;

#define MIN_DIFFICULTY  0
#define MAX_DIFFICULTY  4

#define BUF_LEN         64


/* return integer between 1 and range, inclusive */
long int
random_in_range(int range) {
    unsigned long int num_bins = (unsigned long)range;
    unsigned long int num_rand = (unsigned long)RAND_MAX + 1;
    unsigned long int bin_size = num_rand / num_bins;
    unsigned long int defect = num_rand % num_bins;

    long int x;

    do {
        x = random();
    } while (num_rand - defect <= (unsigned long)x);

    return (int)((x / bin_size) + 1);
}

void
calculate_grid_backtrack(void)
{
    int next_cell;

    for (int m = 0; m < 9; m++) {
        for (int n = 0; n < 9; n++) {
            next_cell = (int)random_in_range(9);
            insert_cell(m, n, next_cell);
        }
    }
}

/* download a websudoku puzzle */
int
get_sudoku()
{
    CURL *curl;
    CURLcode res;
    char fn[] = "ws.html";
    FILE *f = fopen(fn, "w+");
    //FILE *f = tmpfile();
    char str[9999], cheat[82];
    int w_c;
    int count = 0;

    if (!(curl = curl_easy_init())) {
        fprintf(stderr, "curl_easy_init failed\n");
        return 1;
    }

    curl_easy_setopt(curl, CURLOPT_WRITEDATA, f);
    //http://view.websudoku.com/?level=3&set_id=8542113115 for example
    curl_easy_setopt(curl, CURLOPT_URL, "http://view.websudoku.com/");

    res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
    }

#if 0
    if (f) {
        printf("hello\n");
        while (fgetc(f, "%s") != EOF) {
            printf("%s", str);
        }
    }
    printf("goodbye\n");
#endif
    rewind(f);
    while (fscanf(f,"%s", str) == 2)
    {
        printf("%s\n", str);
    }
#if 0
    while (!feof(f)) {
        fscanf(f, " ");
        if (fscanf(f, "w_c=%d;", &w_c)) {
            printf("%d\n", w_c);
            break;
        } else {
            printf("c %d\n", count);
        }
        //if (fscanf(f, "            var cheat='%s';", cheat)) {
            //break;
        //}
        count++;
    }
    //printf("cheat %s\n", cheat);
    printf("wc %d\n", w_c);
    printf("count %d\n", count);
#endif
    
    fclose(f);
    curl_easy_cleanup(curl);

    return 0;
}

void
calculate_sudoku(void)
{
    get_sudoku();
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

    do {
        /* TODO clear buf for each iteration
         * while ((ch = getchar()) != '\n' && ch != EOF);
         */
        fprintf(stdout, "Choose a difficulty level [1-4]: ");
        fgets(buf, BUF_LEN - 1, stdin);
        buf[strnlen(buf, BUF_LEN) - 1] = '\0';
    } while (parse_difficulty(buf));
}

int
main(int argc, char *argv[])
{
    parse_options(argc, argv);
    if (difficulty < MIN_DIFFICULTY) {
        choose_difficulty();
    }

    calculate_sudoku();
    print_grid();
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

    set_cell(GY_0, GX_0);

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

