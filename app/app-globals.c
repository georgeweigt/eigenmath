#include "app.h"

double document_height;	// for parent process
double document_width;

double total_height;	// for thread process
double total_width;

int running;
int malloc_kaput_flag;
struct display *first;
struct display *fence;
struct display * volatile last; // (read backwards) volatile pointer to...

struct display *emit_display;
int emit_level;
int emit_index;
int emit_count;

CTFontRef font_ref_tab[5];
