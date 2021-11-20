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

int history_index;
int history_count;
char *history_instring;
char **history_tab;

void
eval_exit(void)
{
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

char *
history_up(char *instring)
{
	if (history_index == 0)
		return instring; // no change

	if (history_index == history_count) {
		if (history_instring)
			free(history_instring);
		history_instring = strdup(instring);
	}

	history_index--;

	return history_tab[history_index];
}

char *
history_down(char *instring)
{
	if (history_index == history_count)
		return instring; // no change

	history_index++;

	if (history_index == history_count)
		return history_instring;
	else
		return history_tab[history_index];
}

void
history_push(char *instring)
{
	int i, n;

	// don't push if blank

	if (instring == NULL)
		return;

	n = (int) strlen(instring);

	for (i = 0; i < n; i++)
		if (instring[i] >= '!' && instring[i] <= '~')
			break;

	if (i == n)
		return; // blank

	history_count++;
	history_tab = (char **) realloc(history_tab, history_count * sizeof (char *));
	if (history_tab == NULL)
		malloc_kaput();
	history_tab[history_count - 1] = strdup(instring);
	history_index = history_count;
}
