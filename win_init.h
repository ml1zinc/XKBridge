#define DEF_FONT_SIZE 18
#define DEF_BORDER_WIDTH 2
#define DEF_INDENT 4
#define DEF_BUTTON_HEIGHT (DEF_INDENT + DEF_FONT_SIZE) 

#define BUTTON_WIDTH(str_width) (DEF_BORDER_WIDTH * 2 + (str_width)) 

#define HEIGHT(str_num) (DEF_INDENT * (str_num + 1) + (DEF_BORDER_WIDTH * 2 + DEF_FONT_SIZE) * str_num + DEF_BORDER_WIDTH)
#define MAX_BUTTON_TEXT_LEN 30


typedef struct{
    unsigned long flags;
    unsigned long functions;
    unsigned long decorations;
    long inputMode;
    unsigned long status;
} Hints;


typedef struct{
	int x, y;
	int button_num;
	
	Window button;
} Button;

void
hide_decoration(Display *dpy, Window win);

int 
create_main_window(Display *dpy, Window root, UTF8 **array_str, XFontSet font, size_t x, size_t y, size_t buttons_num, uint32_t Button_first);

void 
get_mouse_position(Display *dpy, Window win, size_t *mouse_x, size_t *mouse_y);

int 
create_choose_win(UTF8 **array_str, size_t string_num, uint32_t Button_first);

Button 
create_button_obj(Display *dpy, Window parent_win, int button_num, int screen_num, XColor b_color);

void
free_buttons_obj(Display *dpy, Button *buttons, size_t buttons_num);

int
in_check(Window value, Button *list, int num_in_list);

void 
button_press_release(Display *dpy, Window button, GC light_gc, GC dark_gc);