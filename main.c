#include <locale.h>
#include <curses.h>
#include <stdlib.h>
#include <string.h>

#define BACKGROUND 1
#define FOREGROUND 2
#define SHADOW 3
#define SELECTED 4

#define max(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })

#define MAXOPTIONS 11
#define XPAD 4
#define YPAD 2


typedef unsigned int uint;
typedef unsigned char uchar;


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

    setlocale(LC_ALL, "");
    initscr();
    cbreak();
    noecho();

    if (has_colors() == FALSE) {
        endwin();
        puts("Your terminal does not support color");
        exit(1);
    }

    start_color();
    init_pair(BACKGROUND, COLOR_BLUE, COLOR_BLUE);
    init_pair(FOREGROUND, COLOR_BLACK, COLOR_WHITE);
    init_pair(SHADOW, COLOR_BLACK, COLOR_BLACK);
    init_pair(SELECTED, COLOR_WHITE, COLOR_BLUE);
    keypad(stdscr, TRUE);

    clear();
    attron(COLOR_PAIR(BACKGROUND));
    for(int y = 0; y < LINES; y++) for(int x = 0; x < COLS; x++) mvaddch(y, x, ' ');
    attroff(COLOR_PAIR(BACKGROUND));


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


    attron(COLOR_PAIR(SHADOW));
    for (int y = ROWSTART; y < ROWSTART + HEIGHT; y++)
        for (int x = COLSTART; x < COLSTART + WIDTH; x++)
            mvaddch(y + 1, x + 1, ' ');
    attroff(COLOR_PAIR(SHADOW));


    bool stop = false;
    uchar choice = 0;
    while(!stop)
    {
        attron(COLOR_PAIR(FOREGROUND));
        for (int y = ROWSTART; y < ROWSTART + HEIGHT; y++)
        {
            for (int x = COLSTART; x < COLSTART + WIDTH; x++)
            {
                bool yStart = y == ROWSTART;
                bool yEnd = y == ROWSTART + HEIGHT - 1;
                bool xStart = x == COLSTART;
                bool xEnd = x == COLSTART + WIDTH - 1;
                if(yStart)
                {
                    if(xStart) mvaddstr(y, x, "┌");
                    else if(xEnd) mvaddstr(y, x, "┐");
                    else mvaddstr(y, x, "─");
                }
                else if(yEnd)
                {
                    if(xStart) mvaddstr(y, x, "└");
                    else if(xEnd) mvaddstr(y, x, "┘");
                    else mvaddstr(y, x, "─");
                }
                else if(xStart || xEnd) mvaddstr(y, x, "│");
                else mvaddch(y, x, ' ');
            }
        }

        mvaddstr(ROWSTART + YPAD, COLSTART + XPAD, argv[1]);

        char num[4];
        for (int y = 0; y < argc - 2; y++)
        {
            strcpy(num, itoa(y + 1, 10));
            mvaddstr(ROWSTART + YPAD + y + 2, COLSTART + XPAD, strcat(num, "."));
            mvaddstr(ROWSTART + YPAD + y + 2, COLSTART + XPAD + 4, options[y]);
        }
        attroff(COLOR_PAIR(FOREGROUND));

        attron(COLOR_PAIR(SELECTED));
        strcpy(num, itoa(choice + 1, 10));
        for(int x = COLSTART + XPAD; x < COLSTART + WIDTH - XPAD; x++) mvaddch(ROWSTART + YPAD + choice + 2, x, ' ');
        mvaddstr(ROWSTART + YPAD + choice + 2, COLSTART + XPAD, strcat(num, "."));
        mvaddstr(ROWSTART + YPAD + choice + 2, COLSTART + XPAD + 4, options[choice]);
        attroff(COLOR_PAIR(SELECTED));

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

    exit(choice);

}
