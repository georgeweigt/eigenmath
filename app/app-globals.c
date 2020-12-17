#include "app.h"

double document_height;	// for parent process
double document_width;

double total_height;	// for thread process
double total_width;

struct display *fence;

int draw_count;
struct draw_buf_t draw_buf[DRAW_MAX];

double tmin, tmax;
double xmin, xmax;
double ymin, ymax;

int running;
struct display *emit_display;
int emit_level;
int emit_index;
int emit_count;
