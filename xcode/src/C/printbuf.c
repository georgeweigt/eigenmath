#include "app.h"

void
printbuf(char *s, int color)
{
	int len;
	char *t;

	for (;;) {

		t = strchr(s, '\n');

		if (t == NULL)
			break;

		len = (int) (t - s);

		emit_text(s, len, color);

		s = t + 1;
	}

	if (*s) {
		len = (int) strlen(s);
		emit_text(s, len, color);
	}
}
