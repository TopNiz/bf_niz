#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
/* #include <ncursesw/curses.h> */
#include <curses.h>
#include <string.h>
/* #include <unistd.h> */
#include <getopt.h>

#define DEBUG 0

#define MAX_MEM_SIZE 4096
#define MAX_CODE_SIZE 256
#define MAX_STACK_SIZE 64

#define WACS_BBBB (const cchar_t *) "\u00A0"

/* 176 ░ 177 ▒ 178 ▓ */
/* 218 ┌ 194 ┬ 196 ─ 191 ┐ 192 └ 193 ┴ 217 ┘ 195 ├ 197 ┼ 179 │ 180 ┤ */
/* 201 ╔ 203 ╦ 205 ═ 187 ╗ 200 ╚ 202 ╩ 188 ╝ 204 ╠ 206 ╬ 186 ║ 185 ╣ */
/* 219 █ 220 ▄ 254 ■ 223 ▀ */
/* 221 ¦ 238 ¯ 242 ‗ */

typedef struct {
    WINDOW *w_mem;
    WINDOW *w_code;
    WINDOW *w_output;
} BF_WINDOW;

typedef struct {
    size_t mem_size;
    size_t code_size;
    size_t mem_pos;
    size_t code_pos;
    size_t output_pos;
    size_t stack_pos;
    BF_WINDOW *bf_window;
    char *code;
    char memory[MAX_MEM_SIZE];
    char stack[MAX_STACK_SIZE];
    char output[MAX_MEM_SIZE];
} Memory;

void printo(Memory *mem) {
    size_t o_pos = 0;
    while(o_pos < mem->output_pos) {
        printf("%d ", mem->output[o_pos]);
        o_pos++;
    }
    printf("\n");
}

void printwo(WINDOW *win, Memory *mem) {
    size_t o_pos = 0;
    while(o_pos < mem->output_pos) {
        wprintw(win, "%d ", mem->output[o_pos]);
        o_pos++;
    }
    printf("\n");
}

WINDOW * create_newwin(int height, int width, int starty, int startx)
{
    WINDOW *local_win;
	local_win = newwin(height, width, starty, startx);
	/* box_set(local_win, WACS_BDBD, WACS_DBDB); */
	/* box(local_win, (int)'-', (int)'|');		0, 0 gives default characters
					 * for the vertical and horizontal
					 * lines			*/
	/* wrefresh(local_win);		* Show that box */
	return local_win;
}

void destroy_win(WINDOW * local_win)
{
	/* box(local_win, ' ', ' '); : This won't produce the desired
	 * result of erasing the window. It will leave it's four corners
	 * and so an ugly remnant of window.
	 */
	wborder(local_win, ' ', ' ', ' ',' ',' ',' ',' ',' ');
	/* The parameters taken are
	 * 1. win: the window on which to operate
	 * 2. ls: character to be used for the left side of the window
	 * 3. rs: character to be used for the right side of the window
	 * 4. ts: character to be used for the top side of the window
	 * 5. bs: character to be used for the bottom side of the window
	 * 6. tl: character to be used for the top left corner of the window
	 * 7. tr: character to be used for the top right corner of the window
	 * 8. bl: character to be used for the bottom left corner of the window
	 * 9. br: character to be used for the bottom right corner of the window
	 */
	wrefresh(local_win);
	delwin(local_win);
}

BF_WINDOW * create_bfwin()
{
    BF_WINDOW * bf_win = malloc(sizeof(BF_WINDOW));
    bf_win->w_code   = newwin(4, 60, 0, 0);
    bf_win->w_output = newwin(3, 60, 4, 0);
    bf_win->w_mem    = newwin(14, 60, 7, 0);
    return bf_win;
}

void destroy_bfwin(Memory *mem)
{
    BF_WINDOW *bfwin = mem->bf_window;
    destroy_win(bfwin->w_code);
    destroy_win(bfwin->w_output);
    destroy_win(bfwin->w_mem);
}

char *repeat(char c, size_t count)
{
    char *tmp_result = malloc(sizeof(char) * count);
    char *result = tmp_result;
    while(count--){
        *tmp_result = c;
        tmp_result++;
    }
    *tmp_result = 0;
    return result;
}

void display(Memory *mem, int startY, int startX)
{
    WINDOW * w_mem = mem->bf_window->w_mem;
    int y = startY;
    int x = startX;
    for(int i = 0; i < 100; i++) {
        if(i % 10 == 0) {
            y++;
            x = 2;
            wmove(w_mem, y, x);
            wprintw(w_mem, "%4X: ", i);
            x += 6;
        }
        wmove(w_mem, y, x);
        wprintw(w_mem, "%X ", mem->memory[i]);
        x += 3;
    }
}

void refresh_w_code(Memory *mem)
{
    WINDOW* w_code = mem->bf_window->w_code;
    wclear(w_code);
    wborder_set(w_code, WACS_DBDB, WACS_DBDB, WACS_BDBD, WACS_BBBB,
                        WACS_BDDB, WACS_BBDD, WACS_DBDB, WACS_DBDB);
    /* Title line */
    wmove(w_code, 0, 2);
    wprintw(w_code, " Code: %zu ", mem->code_size);
    /* Rule line */
    char* rule_line = repeat('-', mem->code_size);
    rule_line[mem->code_pos] = 48 + (char)mem->stack_pos;
    wmove(w_code, 1, 2);
    wprintw(w_code, "%s", rule_line);
    /* Cursor line */
    char* cursor_line = repeat(' ', mem->code_size);
    cursor_line[mem->code_pos] = 'v';
    wmove(w_code, 2, 2);
    wprintw(w_code, "%s", cursor_line);
    /* Code line */
    wmove(w_code, 3, 2);
    wprintw(w_code, "%s", mem->code);
    wrefresh(w_code);
}

void refresh_w_output(Memory *mem)
{
    WINDOW * w_output = mem->bf_window->w_output;
    wclear(w_output);
    wborder_set(w_output, WACS_DBDB, WACS_DBDB, WACS_BDBD, WACS_BBBB,
                          WACS_DDDB, WACS_DBDD, WACS_DBDB, WACS_DBDB);
    wmove(w_output, 0, 2);
    wprintw(w_output, " Output: ");
    wmove(w_output, 1, 2);

    printwo(w_output, mem);
    wrefresh(w_output);
}

void refresh_w_memory(Memory *mem)
{
    WINDOW * w_mem = mem->bf_window->w_mem;
    wclear(w_mem);
    wborder_set(w_mem, WACS_DBDB, WACS_DBDB, WACS_BDBD, WACS_BDBD,
                       WACS_DDDB, WACS_DBDD, WACS_DDBB, WACS_DBBD);
    int x = 2;
    int y = 0;
    wmove(w_mem, y, x);
    wprintw(w_mem, " Memory snapshot ");
    y++;
    display(mem, y, x);
    wrefresh(w_mem);
}

void bf_refresh(Memory *mem)
{
    refresh_w_code(mem);
    refresh_w_output(mem);
    refresh_w_memory(mem);
    refresh();
}

Memory* init(size_t mem_size, size_t code_size)
{
    if(mem_size > MAX_MEM_SIZE || code_size > MAX_CODE_SIZE)
        return NULL;

    Memory* mem = (Memory*) malloc(sizeof(Memory));
    if(mem == NULL)
        return NULL;

    for(size_t i = 0; i < mem_size; i++) {
        mem->memory[i] = 0;
    }

    for(size_t i = 0; i < mem_size; i++) {
        mem->output[i] = 0;
    }

    mem->mem_size = mem_size;
    mem->code_size = code_size;
    mem->mem_pos = 0;
    mem->code_pos = 0;
    mem->output_pos = 0;
    mem->code = "";

    return mem;
}

void set(char *code, Memory *in_mem)
{
    in_mem->code = code;
    in_mem->code_size = 0;
    while(*(code++) != '\0') {
        in_mem->code_size++;
    }
    in_mem->code_pos = 0;
}

void logg(char* c, Memory *mem)
{
    if(!DEBUG) return;
    printf("Exec: %s mem-%zu code-%zu stack-%zu\n",
            c,
            mem->mem_pos,
            mem->code_pos,
            mem->stack_pos);
    for(int i=0; i < 5; i++) {
        printf("%d - ", mem->memory[i]);
    }
    printf("\n");
    for(int i=0; i < 5; i++) {
        printf("%d - ", mem->stack[i]);
    }
    printf("\n");
}

void next(Memory *mem)
{
    if(mem->mem_pos < mem->mem_size) {
        mem->mem_pos++;
    }
    logg(">", mem);
}

void prev(Memory *mem)
{
    if(mem->mem_pos > 0) {
        mem->mem_pos--;
    }
    logg("<", mem);
}

void incr(Memory *mem)
{
    mem->memory[mem->mem_pos]++;
    logg("+", mem);
}

void decr(Memory *mem)
{
    mem->memory[mem->mem_pos]--;
    logg("-", mem);
}

void out(Memory *mem)
{
    mem->output[mem->output_pos++] = mem->memory[mem->mem_pos];
    logg(".", mem);
}

void push(Memory *mem)
{
    /* If pointed memory is 0, Jump to next instruction after ] */
    if (mem->memory[mem->mem_pos] == 0) {
        do {
            mem->code_pos++;
        } while (mem->code[mem->code_pos] != ']');
    }
    else if (mem->stack_pos < MAX_STACK_SIZE) {
        mem->stack[(mem->stack_pos)++] = mem->code_pos;
    }
    logg("[", mem);
}

void pop(Memory *mem)
{
    if(mem->stack_pos > 0) {
        if(mem->memory[mem->mem_pos] != 0) {
            mem->code_pos = mem->stack[mem->stack_pos - 1];
        } else {
            --(mem->stack_pos);
        }
    }
    logg("]", mem);
}

void process(Memory *mem)
{
    char c = mem->code[mem->code_pos];
    switch(c) {
        case '>':
            next(mem);
            break;
        case '<':
            prev(mem);
            break;
        case '+':
            incr(mem);
            break;
        case '-':
            decr(mem);
            break;
        case '.':
            out(mem);
            break;
        case '[':
            push(mem);
            break;
        case ']':
            pop(mem);
            break;
        default:
            break;
    }
}

char * compile(Memory *mem)
{
    char c = mem->code[mem->code_pos];
    switch(c) {
        case '>':
            return "GT ";
        case '<':
            return "LT ";
        case '+':
            return "PLUS ";
        case '-':
            return "MINUS ";
        case '.':
            return "DOT ";
        case '[':
            return "OBRA ";
        case ']':
            return "CBRA ";
        default:
            return "";
    }
}

void optimize (Memory *mem)
{
    char *code = mem->code;
    char *cursor = code;
    char copy[mem->code_size];
    /* Remove consecutive opposite instructions */
    while(*cursor != '\0') {
        if((*cursor == '>' && *(cursor+1) =='<')
        || (*cursor == '<' && *(cursor+1) =='>')
        || (*cursor == '+' && *(cursor+1) =='-')
        || (*cursor == '-' && *(cursor+1) =='+')) {
            strcpy(copy, cursor + 2);
            *cursor = '\0';
            strcat(code, copy);
            cursor = code;
        }
        else {
            cursor++;
        }
    }
    mem->code_size = strlen(code);
}

void parse(Memory *mem, bool interactive)
{
    do {
        process(mem);
        if(interactive) {
             bf_refresh(mem);
            getch();
        }
        mem->code_pos++;
        if(DEBUG) getchar();
    } while(mem->code_size - mem->code_pos);
    mem->code_pos = 0;
}

int main(int argc, char *argv[])
{
    /* Sample BF code */
    char * pg1 = "+++[>[-]++++[>++<-]>.<<-]";
    /* Arguments and flags */
    /* TO BE DONE ... */
    bool i_flag = false;     /* Interactive mode */
    bool c_flag = false;     /* Compile mode */
    bool f_flag = false;     /* File input */
    char *file_name;         /* File name */
    char code[MAX_CODE_SIZE];       /* Code text */
    int opt;                 /* Option from getopt */
    FILE *code_file, *compile_file;
    while ((opt = getopt (argc, argv, "icf:")) != -1) {
        switch (opt) {
            case 'i':
                i_flag = true;
                break;
            case 'f':
                f_flag = true;
                file_name = optarg;
                break;
            case 'c':
                c_flag = true;
                break;
            case '?':
                if (optopt == 'f') {
                    fprintf (stderr, "Option -%c requires a filename.\n", optopt);
                }
                else if (isprint (optopt)) {
                    fprintf (stderr, "Unknown option `-%c'.\n", optopt);
                }
                else {
                    fprintf (stderr,
                             "Unknown option character `\\x%x'.\n",
                             optopt);
                }
                return EXIT_FAILURE;
            default:
                abort();
        }
    }
    if (optind < argc) {
        strcpy(code, argv[optind]);
    }
    if (f_flag) {
        code_file = fopen(file_name, "r");
        char c;
        int c_pos = 0;
        char * allowed = "+-><[].";
        while((c = fgetc(code_file)) != EOF
            && c_pos < MAX_CODE_SIZE) {
            if (strchr(allowed, c) != NULL) {
                code[c_pos++] = c;
            }
        }
        code[c_pos] = '\0';
    }
    /* Initialize memory */
    Memory * mem = init(256, 256);
    /* Set BF code: Sample code or input code */
    if(code[0] == 0) {
        set(pg1, mem);
    }
    else {
        set(code, mem);
    }
    /* Compile code if requested */
    if (c_flag) {
        /* Output filename */
        char *out_file = malloc((size_t)strlen(file_name) + 2);
        strcpy(out_file, file_name);
        size_t ext_pos = strcspn(file_name, ".");
        *(out_file + ext_pos) = '\0';
        strcat(out_file, ".c");
        printf("Out filename: %s\n", out_file);
        compile_file = fopen(out_file, "w");
        if (compile_file != NULL)
        {
            fputs ("#include \"bf.h\"\n",     compile_file);
            fputs ("int main() {\n",          compile_file);
            fputs ("    char mem[256];\n",    compile_file);
            fputs ("    char *ptr = mem;\n",  compile_file);
            while(mem->code_pos < mem->code_size) {
                fputs(compile(mem), compile_file);
                mem->code_pos++;
            }
            mem->mem_pos = 0;
            fputs("\n", compile_file);
            fputs("printf(\"result: %d\\n\", mem[1]);\n", compile_file);
            fputs("return 0;", compile_file);
            fputs("}", compile_file);
            fclose (compile_file);
        }
        return EXIT_SUCCESS;
    }
    /* Refresh windows when interactive mode */
    if (i_flag) {
        /* Initialize windows */
        setlocale(LC_ALL, "");
        initscr();
        clear();
        noecho();
        cbreak();
        mem->bf_window = create_bfwin();
    }
    /* Optimize code */
    optimize(mem);
    /* Execute BF code */
    parse(mem, i_flag);
    /* Wait user input to exit using CTRL+C */
    if(i_flag) {
        getch();
        getch();
    }
    endwin();
    /* Output result to stdout */
    if(!i_flag) {
        printo(mem);
    }
    return EXIT_SUCCESS;
}
