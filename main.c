#include <curses.h>
#include <stdlib.h>
#include <string.h>

#define CGA_BLACK 0
#define CGA_BLUE 1
#define CGA_GREEN 2
#define CGA_CYAN 3
#define CGA_RED 4
#define CGA_MAGENTA 5
#define CGA_YELLOW 6
#define CGA_WHITE 7
#define CGA_H_BLACK 8
#define CGA_H_BLUE 9
#define CGA_H_GREEN 10
#define CGA_H_CYAN 11
#define CGA_H_RED 12
#define CGA_H_MAGENTA 13
#define CGA_H_YELLOW 14
#define CGA_H_WHITE 15

#define max(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })

#define MAXOPTIONS 11
#define XPAD 4
#define YPAD 2


typedef unsigned int uint;
typedef unsigned char uchar;


short curs_color(int fg)
{
    switch (7 & fg) {           /* RGB */
        case 0:                     /* 000 */
            return (COLOR_BLACK);
        case 1:                     /* 001 */
            return (COLOR_BLUE);
        case 2:                     /* 010 */
            return (COLOR_GREEN);
        case 3:                     /* 011 */
            return (COLOR_CYAN);
        case 4:                     /* 100 */
            return (COLOR_RED);
        case 5:                     /* 101 */
            return (COLOR_MAGENTA);
        case 6:                     /* 110 */
            return (COLOR_YELLOW);
        case 7:                     /* 111 */
            return (COLOR_WHITE);
    }
}

int colornum(int fg, int bg)
{
    int B, bbb, ffff;

    B = 1 << 7;
    bbb = (7 & bg) << 4;
    ffff = 7 & fg;

    return (B | bbb | ffff);
}

void init_colorpairs(void)
{
    int fg, bg;
    int colorpair;

    for (bg = 0; bg <= 7; bg++) {
        for (fg = 0; fg <= 7; fg++) {
            colorpair = colornum(fg, bg);
            init_pair(colorpair, curs_color(fg), curs_color(bg));
        }
    }
}

int is_bold(int fg)
{
    /* return the intensity bit */

    int i;

    i = 1 << 3;
    return (i & fg);
}

void setcolor(int fg, int bg)
{
    /* set the color pair (colornum) and bold/bright (A_BOLD) */

    attron(COLOR_PAIR(colornum(fg, bg)));
    if (is_bold(fg)) {
        attron(A_BOLD);
    }
}

void unsetcolor(int fg, int bg)
{
    /* unset the color pair (colornum) and
       bold/bright (A_BOLD) */

    attroff(COLOR_PAIR(colornum(fg, bg)));
    if (is_bold(fg)) {
        attroff(A_BOLD);
    }
}

char* itoa(int val, int base)
{
    static char buf[32] = {0};
    int i = 30;
    for(; val && i; --i, val /= base) buf[i] = "0123456789abcdef"[val % base];
    return &buf[i+1];
}

int main(int argc, char *argv[])
{
    if(argc < 3)
    {
        printf("At least a title, and one option is required!");
        exit(1);
    }
    if(argc > 99 + 2)
    {
        printf("No more than 99 options are allowed!");
        exit(1);
    }

    /* initialize curses */


    initscr();
    cbreak();
    //noecho();

    if (has_colors() == FALSE) {
        endwin();
        puts("Your terminal does not support color");
        exit(1);
    }

    start_color();
    init_colorpairs();
    keypad(stdscr, TRUE);

    clear();
    setcolor(CGA_H_WHITE, CGA_BLUE);
    for(int y = 0; y < LINES; y++) for(int x = 0; x < COLS; x++) mvaddch(y, x, ' ');
    unsetcolor(CGA_H_WHITE, CGA_BLUE);

    char *options[MAXOPTIONS];
    for(int i = 0; i < argc - 2; i++) options[i] = argv[i + 2];

    int WIDTH = (int)strlen(argv[1]);
    for(int i = 0; i < argc - 2; i++) WIDTH = max(WIDTH, strlen(options[i]) + 4);
    WIDTH = max(WIDTH, 30);
    WIDTH += (XPAD * 2);

    int HEIGHT = argc + (YPAD * 2);

    int COLSTART = (COLS / 2) - (WIDTH / 2);

    int ROWSTART = (LINES / 2) - (HEIGHT / 2);
    if(LINES % 2 == 0) ROWSTART--;


    setcolor(CGA_BLACK, CGA_BLACK);
    for (int y = ROWSTART; y < ROWSTART + HEIGHT; y++)
        for (int x = COLSTART; x < COLSTART + WIDTH; x++)
            mvaddch(y + 1, x + 1, ' ');
    unsetcolor(CGA_BLACK, CGA_BLACK);


    bool stop = false;
    uchar choice = 0;
    while(!stop)
    {
        setcolor(CGA_BLACK, CGA_WHITE);
        for (int y = ROWSTART; y < ROWSTART + HEIGHT; y++)
            for (int x = COLSTART; x < COLSTART + WIDTH; x++)
                mvaddch(y, x, ' ');
        mvaddstr(ROWSTART + YPAD, COLSTART + XPAD, argv[1]);

        char num[4];
        for (int y = 0; y < argc - 2; y++)
        {
            strcpy(num, itoa(y + 1, 10));
            mvaddstr(ROWSTART + YPAD + y + 2, COLSTART + XPAD, strcat(num, "."));
            mvaddstr(ROWSTART + YPAD + y + 2, COLSTART + XPAD + 4, options[y]);
        }
        unsetcolor(CGA_BLACK, CGA_WHITE);

        setcolor(CGA_WHITE, CGA_BLUE);
        strcpy(num, itoa(choice + 1, 10));
        for(int x = COLSTART + XPAD; x < COLSTART + WIDTH - XPAD; x++) mvaddch(ROWSTART + YPAD + choice + 2, x, ' ');
        mvaddstr(ROWSTART + YPAD + choice + 2, COLSTART + XPAD, strcat(num, "."));
        mvaddstr(ROWSTART + YPAD + choice + 2, COLSTART + XPAD + 4, options[choice]);
        unsetcolor(CGA_WHITE, CGA_BLUE);

        move(0, 0);
        refresh();

        int in = getch();
        if(in == '\n') stop = true;
        else if(in == KEY_UP && choice > 0) choice--;
        else if(in == KEY_DOWN && choice < argc - 3) choice++;
    }




    move(0, 0);
    refresh();

    move(0, 0);

    //getch();
    endwin();

    printf("%hhu", choice);

    exit(0);

}
