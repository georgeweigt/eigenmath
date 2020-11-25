#include "app.h"

double document_height;
double document_width;

int cleared;
int malloc_kaput_flag;
double total_h; // sum of the heights of all the items in the display queue
double total_w; // the maximum width including right and left margins
struct display *first;
struct display *mark;
struct display *last;

struct display *emit_display;
int emit_level;
int emit_index;
int emit_count;

CTFontRef font_ref_tab[5];
