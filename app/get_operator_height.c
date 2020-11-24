#include "app.h"

double
get_operator_height(int font_num)
{
	return 0.7 * get_xheight(font_num); // approximate height of '-'
}
