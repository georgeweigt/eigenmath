#include "app.h"

void
malloc_kaput(void)
{
	malloc_kaput_flag = 1;
	longjmp(stop_return, 1);
}
