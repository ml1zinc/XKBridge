#include <err.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/XKBlib.h>

#include "xkbridge.h"
#include "keysym_custom.h"
#include "win_init.h"


/* Our X Display and Window */
Display *display;
static Window window;

static Atom text_atom; /* The TEXT atom */
static Atom utf8_atom; /* The UTF8 atom */
static Atom targets_atom; /* The TAGRET atom */

/* bool from X11 */
static Bool check_case = True;

static void
usage(void){
    printf("Usage: xkbridge [OPTION] \n\n");
    printf("Convert text from some locale to all of your keyboard's layouts.\n");
    printf("E.g. \"Руддщ цщкдв!\" --> \"Hello world\" \n\n");
    printf("  -c    Dont keep case, by default keeped\n");
    printf("  -n    Set button numbering from 1, by default from 0\n");
}


UTF8**
allocate_array(size_t N, size_t M){
    UTF8 **array = (UTF8 **)malloc(N * sizeof(UTF8*) + sizeof(UTF32) * N * M * sizeof(UTF8));
    UTF8 *start = (UTF8 *)((UTF8 *)array + N * sizeof(UTF8*));

    for (size_t i = 0; i < N; i++){
        array[i] = start + sizeof(UTF32) * i * M;
        array[i][0] = 0x0;
    }

    return array;
}

# define xs_strcat(to, from) ((UTF8 *) strcat((char *)to, (const char *)from))

#define xs_strdup(s) ((unsigned char *) _xs_strdup ((const char *)s))
static char*
_xs_strdup(const char *s){
    char *ret;

    if (s == None) return None;
    if ((ret = strdup(s)) == None) {
        err(1, "strdup error");
    }

    return ret; 
}


void
open_display(Display **display){
    int return_code;
    
    *display = XkbOpenDisplay("", None, None, None, None, &return_code); 
    if (!*display) {
        err(1, "Can't connect to X server: %s\n",
            getenv("DISPLAY"));
        switch (return_code) {
            case XkbOD_ConnectionRefused :
                err(1, "Connection refused\n");
            case XkbOD_NonXkbServer:
                err(1, "XKB extension not present\n");
            default:
                err(1, "Error %d from XkbOpenDisplay\n",
                    return_code);
        }
        exit(1);
    }       
}


void
open_xkb_session(Display **display, XkbDescPtr *kb_desc, XkbStateRec *state){
    *kb_desc = XkbGetMap(*display, XkbAllMapComponentsMask, XkbUseCoreKbd);
    if (!*kb_desc)
        err(1, "Error from XkbGetMap");

    if (XkbGetState(*display, XkbUseCoreKbd, state) != Success)
        err(1, "Error from XkbGetState");

    if (XkbGetControls(*display, XkbGroupsWrapMask, *kb_desc) != Success)
        err(1, "Error from XkbGetControls");

    if (XkbGetNames(*display, (XkbSymbolsNameMask | XkbGroupNamesMask), *kb_desc) != Success)
        err(1, "Error from XkbGetNames");
}


static UTF8*
get_selection (Atom selection, Atom request_target){
    Atom prop;
    XEvent event;
    Atom target;
    int format;
    unsigned long bytesafter, length;
    unsigned char *value, *retval = None;
    static Time timestamp;
    
    prop = XInternAtom (display, "SEL_DATA", False);
    XConvertSelection (display, selection, request_target, prop, window, timestamp);
    XSync (display, False);

    Bool keep_waiting = True;

    while (keep_waiting) {
        XNextEvent (display, &event);

        switch (event.type) {
        case SelectionNotify:
            if (event.xselection.selection != selection) break;

            if (event.xselection.property == None) {
                value = None;
                keep_waiting = False;
            } else {
                XGetWindowProperty (event.xselection.display,
                    event.xselection.requestor,
                    event.xselection.property, 0L, 1000000,
                    False, (Atom)AnyPropertyType, &target,
                    &format, &length, &bytesafter, &value);

                    retval = xs_strdup(value);
                    XFree(value);
                    keep_waiting = False;

                XDeleteProperty (event.xselection.display,
                                 event.xselection.requestor,
                                 event.xselection.property);
            }
            break;
        default:
            break;
        }
    }
    return retval;
}


static UTF8*
get_selected_text(){
    UTF8 *retval;
    Atom selection = XA_PRIMARY;

    if ((retval = get_selection (selection, utf8_atom)) == None)
        retval = get_selection (selection, XA_STRING);

    return retval;
}


UTF8**
convert_to_langs(UTF32 *symb_str, size_t str_lens, size_t group_count){
    KeySym  old_ksym;
    KeySym  new_ksym;
    KeyCode kcd;

    UTF8 **array = allocate_array(group_count, str_lens);

    // int buffer_bytes;
    UTF8 buffer_symb[8];
    Bool case_flag = False;

    for(size_t i = 0; i < str_lens; i++){
        old_ksym = utf32_to_keysym(symb_str[i]);
        kcd = XKeysymToKeycode(display, old_ksym);

        // letter case check
        if (check_case == True){
            KeyCode kcd_ch = XKeysymToKeycode(display, utf32_to_keysym(symb_str[i] + 0x20));
            
            if (kcd_ch == kcd){
                case_flag = (Bool) ((XKB_KEY_A <= symb_str[i] && symb_str[i] <= XKB_KEY_Z) || symb_str[i] >= XKB_KEY_Aogonek); 
            } else {
                case_flag = (Bool) ((symb_str[i] == XKB_KEY_braceleft || symb_str[i] == XKB_KEY_braceright) ||
                                    (symb_str[i] == XKB_KEY_less      || symb_str[i] == XKB_KEY_greater)    ||
                                    (symb_str[i] == XKB_KEY_colon     || symb_str[i] == XKB_KEY_quotedbl)   ||
                                    (symb_str[i] == XKB_KEY_question  || symb_str[i] == XKB_KEY_tilde)
                                    ); 
            }
        }

        for(size_t j = 0; j < group_count; j++){

            if(
               (symb_str[i] <= XKB_KEY_9         && symb_str[i] >= XKB_KEY_0)         ||
               (symb_str[i] < XKB_KEY_comma      && symb_str[i] > XKB_KEY_apostrophe )||
               (symb_str[i] < XKB_KEY_apostrophe && symb_str[i] > XKB_KEY_quotedbl )  ||
               symb_str[i] == XKB_KEY_at         ||
               symb_str[i] == XKB_KEY_bar        ||
               symb_str[i] < XKB_KEY_quotedbl    ||
               symb_str[i] == XKB_KEY_underscore ||
               symb_str[i] == XKB_KEY_asciicircum
               ){
                keysym_to_utf8(old_ksym, buffer_symb, 8);
            } else {
                new_ksym = XkbKeycodeToKeysym(display, kcd, j, case_flag);
                keysym_to_utf8(new_ksym, buffer_symb, 8);
            }
            xs_strcat(array[j], buffer_symb);
            // strcat(array[j], buffer_symb);
        }
    }
    return array;
}

void copy_to_clibboard(Atom selection, UTF8* text, int size){
    XEvent event;

    XSetSelectionOwner (display, selection, window, 0);
    if (XGetSelectionOwner (display, selection) != window) return;

    while (1) {
        XNextEvent (display, &event);
        switch (event.type) {
            case SelectionRequest:
                if (event.xselectionrequest.selection != selection) break;

                XSelectionRequestEvent *xsr = &event.xselectionrequest;
                XSelectionEvent ev = {0};
                int R = 0;
                ev.type = SelectionNotify, ev.display = xsr->display, ev.requestor = xsr->requestor,
                ev.selection = xsr->selection, ev.time = xsr->time, ev.target = xsr->target, ev.property = xsr->property;

                if (ev.target == targets_atom){
                    R = XChangeProperty (ev.display, ev.requestor, ev.property, XA_ATOM, 32,
                        PropModeReplace, (unsigned char*)&utf8_atom, 1);
                }
                else if (ev.target == XA_STRING || ev.target == text_atom){
                    R = XChangeProperty(ev.display, ev.requestor, ev.property, XA_STRING, 8, PropModeReplace, text, size);
                }
                else if (ev.target == utf8_atom){
                    R = XChangeProperty(ev.display, ev.requestor, ev.property, utf8_atom, 8, PropModeReplace, text, size);
                }
                else ev.property = None;
                if ((R & 2) == 0) XSendEvent (display, ev.requestor, 0, 0, (XEvent *)&ev);
                break;

            case SelectionClear:
                return;
        }
    }
}


int main(int argc, char **argv){
    int opt;

    int choosed_indx;
    size_t group_count;
    UTF8 *select_text;

    XkbDescPtr kb_desc;
    XkbStateRec state;

    int black;
    Window root;
    char *window_name = "selection";

    uint32_t Button_first = XKB_KEY_0;

    while ((opt = getopt(argc, argv, "cnh")) != -1) {
        switch (opt) {
        case 'c': check_case = False; continue;
        case 'n': Button_first = XKB_KEY_1; continue;
        case 'h': 
        default:
            usage();
            exit(-1);
        }
    }

    open_display(&display);
    open_xkb_session(&display, &kb_desc, &state);

    root = XDefaultRootWindow(display);

    /* Create an unmapped window for receiving events */
    black = BlackPixel(display, DefaultScreen (display));
    window = XCreateSimpleWindow(display, root, 0, 0, 1, 1, 0, black, black);

    /* Set window name and class */
    XStoreName(display, window, window_name);

    text_atom = XInternAtom(display, TEXT, False);
    utf8_atom = XInternAtom(display, UTF8_STRING, True);
    if (utf8_atom == None) utf8_atom = XA_STRING;

    targets_atom = XInternAtom(display, TARGETS, False);
    Atom clipboard = XInternAtom(display, CLIPBOARD, False);

    select_text = get_selected_text();

    // current_state = state.group;
    group_count = kb_desc->ctrls->num_groups;

    size_t str_lens;
    UTF32 *symb_str = utf8_to_utf32(select_text, &str_lens);

    
    /*convert to all language from keyboard layout*/
    UTF8 **array = convert_to_langs(symb_str, str_lens, group_count);
    
    choosed_indx = create_choose_win(array, (size_t)group_count, Button_first);

    // added \0 for no pasting string with newline
    strncat((char *)array[choosed_indx], "\0", 1);

    copy_to_clibboard(clipboard, (UTF8 *)array[choosed_indx], (int) byte_len(array[choosed_indx]));

    free(array);

    XkbFreeClientMap(kb_desc, XkbAllMapComponentsMask, False);
    XkbFreeNames(kb_desc, XkbGroupNamesMask, False);
    XkbFreeControls(kb_desc, XkbGroupsWrapMask, False);
    
    XkbFreeKeyboard(kb_desc, XkbAllMapComponentsMask, True);
    
    XDestroyWindow(display, root);
    XCloseDisplay(display);

    return 0;

}
    


