#include <stdint.h>
#include <X11/Xlib.h>

#define ARRAY_SIZE(arr) ((sizeof(arr) / sizeof(*(arr))))

#define XKB_KEY_NoSymbol                    0x000000  

#define XKB_KEY_BackSpace                     0xff08  

#define XKB_KEY_Clear                         0xff0b
#define XKB_KEY_Return                        0xff0d  

#define XKB_KEY_Escape                        0xff1b
#define XKB_KEY_Delete                        0xffff  

#define XKB_KEY_Space                         0x0020  

#define XKB_KEY_KP_Space                      0xff80  
#define XKB_KEY_KP_Tab                        0xff89
#define XKB_KEY_KP_Enter                      0xff8d  


#define XKB_KEY_KP_Equal                      0xffbd  
#define XKB_KEY_KP_Multiply                   0xffaa

#define XKB_KEY_KP_9                          0xffb9


#define XKB_KEY_0                             0x0030 
#define XKB_KEY_1                             0x0031 
#define XKB_KEY_9                             0x0039
#define XKB_KEY_A                             0x0041 
#define XKB_KEY_Z                             0x005a 

#define XKB_KEY_quotedbl                      0x0022
#define XKB_KEY_apostrophe                    0x0027
#define XKB_KEY_comma                         0x002c
#define XKB_KEY_colon                         0x003a
#define XKB_KEY_less                          0x003c
#define XKB_KEY_greater                       0x003e
#define XKB_KEY_question                      0x003f
#define XKB_KEY_at                            0x0040
#define XKB_KEY_backslash                     0x005c
#define XKB_KEY_asciicircum                   0x005e
#define XKB_KEY_underscore                    0x005f
#define XKB_KEY_braceleft                     0x007b
#define XKB_KEY_bar                           0x007c
#define XKB_KEY_braceright                    0x007d
#define XKB_KEY_tilde                         0x007e
#define XKB_KEY_Aogonek                       0x01a1

typedef unsigned char UTF8;   /* typically 8 bits */
typedef uint32_t UTF32;  /* at least 32 bits, 
used uint32_t instead of unsigned long because in 64bit compiler its take 8 bytes, but need 4 only */

size_t
byte_len(UTF8 *string);

size_t
characters_num(UTF8 *string);

size_t 
keysym_to_utf8(KeySym keysym, UTF8 *buffer, size_t size);

UTF32 
keysym_to_utf32(KeySym keysym);

KeySym
utf32_to_keysym(UTF32 ucs);

size_t 
utf32_to_utf8(UTF32 unichar, UTF8 *buffer);

size_t
_utf8_to_utf32(const UTF8 *src_orig, UTF32 *dst, size_t len);

UTF32*
utf8_to_utf32(UTF8 *utf8_string, size_t *str_lens);

