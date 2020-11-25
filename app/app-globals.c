#include "app.h"

int cleared;
int malloc_kaput_flag;
int total_h; // sum of the heights of all the items in the display queue
int total_w; // the maximum width including right and left margins
struct display *first;
struct display *mark;
struct display *last;

struct display *emit_display;
int emit_level;
int emit_index;
int emit_count;

CTFontRef font_ref_tab[5];
