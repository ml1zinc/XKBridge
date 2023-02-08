#include "keysym_custom.h"

#define TEXT         "TEXT"
#define TARGETS      "TARGETS"
#define CLIPBOARD    "CLIPBOARD"
#define UTF8_STRING  "UTF8_STRING"


UTF8**
allocate_array(size_t N, size_t M);

void
open_xkb_session(Display **dpy, XkbDescPtr *kb_desc, XkbStateRec *state);

static UTF8*
get_selected_text();

int 
main(int argc, char **argv);
