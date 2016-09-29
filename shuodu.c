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
#include <tidy/tidy.h>
#include <tidy/buffio.h>
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

/* curl write callback, to fill tidy's input buffer...  TODO */
uint write_cb(char *in, uint size, uint nmemb, TidyBuffer *out)
{
	uint r;
	r = size * nmemb;
	tidyBufAppend(out, in, r);
	return r;
}

/* download a websudoku puzzle */
int
get_sudoku()
{
    CURL *curl;
    CURLcode res;
    char url[2084], level[2];
    char websudoku[] = "http://view.websudoku.com/", level_select[] = "?level=";
    char fn[] = "ws.html"; /* save in local file for parsing for now TODO */
    FILE *f = fopen(fn, "w+");//FILE *f = tmpfile();
    char str[9999], cheat[82];
    int w_c;
    int count = 0, err;

    TidyBuffer docbuf = { 0 };
    TidyBuffer errbuf = { 0 };

    TidyDoc tdoc = tidyCreate();
    Bool ok;

	char curl_errbuf[CURL_ERROR_SIZE];

    if (!(curl = curl_easy_init())) {
        fprintf(stderr, "curl_easy_init failed\n");
        return 1;
    }

    /* http://view.websudoku.com/?level=<difficulty>&set_id=<id> */
    /* TODO get random id */
    strncpy(url, websudoku, sizeof(websudoku));
    strncat(url, level_select, sizeof(level_select));
    if (difficulty > MIN_DIFFICULTY && difficulty <= MAX_DIFFICULTY) {
        sprintf(level, "%d", (int)difficulty);
        strncat(url, level, sizeof(level));
    }

    if (!(ok = tidyOptSetBool(tdoc, TidyForceOutput, true))) {
        fprintf(stderr, "tidyOptSetBool() failed\n");
        goto finish;
    }
    tidyOptSetInt(tdoc, TidyWrapLen, 1048576/*4096*/);
    tidySetErrorBuffer(tdoc, &errbuf);
    tidyBufInit(&docbuf);

    curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, curl_errbuf);
    //curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
    //curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
    //curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_cb);

#if 1
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, f);
#else
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &docbuf);
#endif
    curl_easy_setopt(curl, CURLOPT_URL, url);

    if ((res = curl_easy_perform(curl)) != CURLE_OK) {
        fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        goto finish;
    }

    //err = tidyParseBuffer(tdoc, &docbuf/*f*/);
#if 0
    if (err >= 0) {
        err = tidyCleanAndRepair(tdoc);
    }
    if (err >= 0) {
        err = tidyRunDiagnostics(tdoc);
    }
    if (err >= 0) {
        fprintf(stdout, "TIDY! ozhu\n");
    }
#endif

finish:
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

#ifdef CURSED
    int num = 0;

    printf("fuck\n");
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
#endif /* CURSED */
}

