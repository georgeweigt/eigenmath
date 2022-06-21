#include "app.h"

int history_index;
int history_count;
char *history_instring;
char **history_tab;

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
		kaput("malloc");
	history_tab[history_count - 1] = strdup(instring);
	history_index = history_count;
}
