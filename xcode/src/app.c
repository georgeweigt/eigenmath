#include "app.h"

int running;

double document_height;	// parent process
double document_width;

double total_height;	// thread process
double total_width;

struct display *fence;

int draw_count;
int draw_max;
struct draw_buf_t *draw_buf;

double tmin, tmax;
double xmin, xmax;
double ymin, ymax;

int emit_level;
int emit_index;
int emit_count;
struct display *emit_display;

void
eval_exit(struct atom *p1)
{
	(void) p1;
	push_symbol(NIL);
}

void
echo_input(char *s)
{
	emit_text(s, (int) strlen(s), BLUE);
}

int
check_display(void)
{
	if (fence == get_display_ptr())
		return 0; // nothing changed
	else
		return 1;
}

void
prep_display(void)
{
	fence = get_display_ptr();

	if (fence) {
		document_height = fence->total_height;
		document_width = fence->total_width;
	} else {
		document_height = 0.0;
		document_width = 0.0;
	}
}

void
clear_display(void)
{
	struct display *p, *t;

	if (running)
		return;

	p = get_display_ptr();
	set_display_ptr(NULL);

	while (p) {
		t = p;
		p = p->next;
		free(t);
	}

	fence = NULL;

	total_height = 0.0;
	total_width = 0.0;
}
