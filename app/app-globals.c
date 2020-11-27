#include "app.h"

double document_height;	// for parent process
double document_width;

double total_height;	// for thread process
double total_width;

struct display * volatile display_list; // (read backwards) volatile pointer to...
struct display *fence;

int running;
int malloc_kaput_flag;
struct display *emit_display;
int emit_level;
int emit_index;
int emit_count;

CTFontRef font_ref_tab[5];
