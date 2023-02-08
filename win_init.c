#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <X11/Xlocale.h>

#include <X11/XKBlib.h>

#include "keysym_custom.h"
#include "win_init.h"


size_t DEF_BUTTON_WIDTH;

void
hide_decoration(Display *dpy, Window win){
    Hints hints;
    Atom property;

    hints.flags = 2;
    hints.decorations = 0;
    property = XInternAtom(dpy, "_MOTIF_WM_HINTS", True);
    XChangeProperty(dpy, win, property, property, 32, PropModeReplace, (unsigned char *)&hints, 5);
}

int
create_main_window(Display *dpy, Window root, UTF8 **array_str, XFontSet font, size_t x, size_t y, size_t buttons_num, uint32_t Button_first){
    Window main_win;

    int screen;
    screen = DefaultScreen(dpy);

    main_win = XCreateSimpleWindow(dpy, root, x, y, (DEF_BUTTON_WIDTH + DEF_INDENT*2 + DEF_BORDER_WIDTH), HEIGHT(buttons_num), 2, BlackPixel(dpy, screen), WhitePixel(dpy, screen));

    XSelectInput(dpy, main_win, ExposureMask | KeyPressMask | ButtonPressMask | ButtonReleaseMask);

    hide_decoration(dpy, main_win);

    XMapWindow(dpy, main_win);
    XMoveWindow(dpy, main_win, x, y); 

    // colormap
    Colormap colormap;
    XColor button_color;
    XColor lightgray_color, darkgray_color;
    XGCValues gcv_lightgray, gcv_darkgray;
    GC gc_lightgray, gc_darkgray;

    colormap = DefaultColormap(dpy, screen);
    XParseColor(dpy, colormap, "rgb:cc/cc/cc", &button_color);
    XAllocColor(dpy, colormap, &button_color);

    XParseColor(dpy, colormap, "rgb:ee/ee/ee", &lightgray_color);
    XAllocColor(dpy, colormap, &lightgray_color);
    gcv_lightgray.foreground = lightgray_color.pixel; 
    gcv_lightgray.background = button_color.pixel;
    gc_lightgray = XCreateGC(dpy, root, GCForeground | GCBackground, & gcv_lightgray);

    XParseColor(dpy, colormap, "rgb:88/88/88", &darkgray_color);
    XAllocColor(dpy, colormap, &darkgray_color);
    gcv_darkgray.foreground = darkgray_color.pixel; 
    gcv_darkgray.background = button_color.pixel;
    gc_darkgray = XCreateGC(dpy, root, GCForeground | GCBackground, & gcv_darkgray);
    // colormap End

  
    XRectangle inkrect, logrect;
    XFontSetExtents *extents;
    extents = XExtentsOfFontSet(font);

    Button buttons[buttons_num];
    for(size_t i = 0; i < buttons_num; i++){
        buttons[i] = create_button_obj(dpy, main_win, i, screen, button_color);
    }

    XFlush(dpy);

    int key_i;
    int current_button_num;
    KeySym key;
    char text[10];
    int done = 1;

    XEvent e;
    while(done){
        XNextEvent(dpy, &e);
        current_button_num = in_check(e.xany.window, buttons, buttons_num);

        if(e.type == KeyPress){
            key_i = XLookupString(&e.xkey, text, 10, &key, 0);

            if((key_i == 1 && text[0] == 'q') || (key_i == 1 && key == XKB_KEY_Escape)) {
                exit(0);
            }

            if(key_i == 1 && ((size_t)text[0] >= Button_first) && ((size_t)text[0] < (Button_first + buttons_num))){
                current_button_num = (size_t)text[0] - Button_first;
                done = 0;
                continue;
            }
        }

        if(current_button_num+1){
            if(e.type == Expose){
                button_press_release(dpy, buttons[current_button_num].button, gc_darkgray, gc_lightgray); // ButtonRelease
                Xutf8TextExtents(font, (char *)array_str[current_button_num], byte_len(array_str[current_button_num]), &inkrect, &logrect);
                
                Xutf8DrawString(dpy, 
                                buttons[current_button_num].button, 
                                font, 
                                DefaultGC(dpy, screen), 
                                DEF_BORDER_WIDTH,
                                (((DEF_BUTTON_HEIGHT) - logrect.height)/2 - extents->max_logical_extent.y), 
                                (char *)array_str[current_button_num], 
                                byte_len(array_str[current_button_num]));
            }

            if(e.type == ButtonPress){
                if(e.xbutton.button == 1){
                    button_press_release(dpy, buttons[current_button_num].button, gc_lightgray, gc_darkgray); // ButtonPress
                    continue;
                }
            }

            if(e.type == ButtonRelease){
                if(e.xbutton.button == 1){
                    button_press_release(dpy, buttons[current_button_num].button, gc_darkgray, gc_lightgray); // ButtonRelease
                    done = 0;
                }
            }
        }
    }


    XFreeColormap(dpy, colormap);
    XFreeFontSet(dpy, font);

    free_buttons_obj(dpy, buttons, buttons_num);

    XDestroyWindow(dpy, main_win);
    XCloseDisplay(dpy);

    return current_button_num;   
}

int
in_check(Window value, Button *list, int num_in_list){
    for(int i = 0; i < num_in_list; i++){
        if(value == list[i].button){
            return i;
        }
    }
    return -1;
}


void
get_mouse_position(Display *dpy, Window win, size_t *mouse_x, size_t *mouse_y){
    Window      root_return;
    Window      child_return;
    int         root_x_return;
    int         root_y_return;
    unsigned    mask_return;
    int ret;

    ret = XQueryPointer(dpy, win, &root_return, &child_return, &root_x_return, &root_y_return, (int *)mouse_x, (int *)mouse_y, &mask_return);
    
    if (ret == BadWindow){
        *mouse_x = 32;//DEFAULT_MOUSE_X; 
        *mouse_y = 32;//DEFAULT_MOUSE_Y;
    }
}


Button 
create_button_obj(Display *dpy, Window parent_win, int button_num, int screen_num, XColor b_color){
    Button button_win;

    button_win.button_num = button_num;
    button_win.x = DEF_INDENT;
    button_win.y = DEF_INDENT * (button_num + 1) + (DEF_BUTTON_HEIGHT) * button_num;

    button_win.button = XCreateSimpleWindow(dpy, parent_win, button_win.x, button_win.y, DEF_BUTTON_WIDTH, DEF_BUTTON_HEIGHT, 1, BlackPixel(dpy, screen_num), b_color.pixel);

    XSelectInput(dpy, button_win.button, ExposureMask | KeyPressMask | ButtonPressMask | ButtonReleaseMask);
    XMapWindow(dpy, button_win.button);

    return button_win;
}


void 
free_buttons_obj(Display *dpy, Button *buttons, size_t buttons_num){
    for(size_t i = 0; i < buttons_num; i++){
        XDestroyWindow(dpy, buttons[i].button);
    }    
}


void 
button_press_release(Display *dpy, Window button, GC light_gc, GC dark_gc){
    XDrawLine(dpy, button, dark_gc, 0, 0, (DEF_BUTTON_WIDTH) - 1, 0);
    XDrawLine(dpy, button, dark_gc, 0, 0, 0, (DEF_BUTTON_HEIGHT) - 1);
    XDrawLine(dpy, button, light_gc, (DEF_BUTTON_WIDTH) - 1, 0, (DEF_BUTTON_WIDTH) - 1, (DEF_BUTTON_HEIGHT) - 1);
    XDrawLine(dpy, button, light_gc, 0, (DEF_BUTTON_HEIGHT) - 1, (DEF_BUTTON_WIDTH) - 1, (DEF_BUTTON_HEIGHT) - 1);
}
    

XFontSet
set_font(Display *dpy, UTF8 **array_str, size_t string_num, size_t *max_width){
    XRectangle inkrect, logrect;

    XFontSet font;

    int missing_charset_count_return;
    char *def_string_return;
    char **missing_charset_list_return;

    char size_font_str[3];
    char font_string[128];

    setlocale(LC_ALL, "");

    sprintf(size_font_str, "%d", (int )DEF_FONT_SIZE);

    strcpy(font_string, "-misc-*-medium-r-normal--");
    strcat(font_string, size_font_str);
    strcat(font_string, "-*-*-*-*-*-*");

    font = XCreateFontSet(dpy, font_string,
                &missing_charset_list_return, &missing_charset_count_return,
                &def_string_return);


    int len;

    for(size_t i = 0; i < string_num; i++){

        len = byte_len(array_str[i]);

        if(byte_len(array_str[i]) > MAX_BUTTON_TEXT_LEN){
            len = MAX_BUTTON_TEXT_LEN;
        } 

        Xutf8TextExtents(font, (char *)array_str[i], len, &inkrect, &logrect);
        if(logrect.width > *max_width){
            *max_width = logrect.width;
        }
    }
    return font;
}


int
create_choose_win(UTF8 **array_str, size_t string_num, uint32_t Button_first){
    int indx;
    size_t mouse_x, mouse_y;
    Display *dpy;
    Window root_win;

    dpy = XOpenDisplay("");
    root_win = DefaultRootWindow(dpy);

    size_t string_width = 0;
    XFontSet font = set_font(dpy, array_str, string_num, &string_width);

    get_mouse_position(dpy, root_win, &mouse_x, &mouse_y);

    DEF_BUTTON_WIDTH = BUTTON_WIDTH(string_width);

    indx = create_main_window(dpy, root_win, array_str, font, mouse_x, mouse_y, string_num, Button_first);

    XDestroyWindow(dpy, root_win);

    return indx;
}
