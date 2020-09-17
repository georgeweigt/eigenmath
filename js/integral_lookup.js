function
integral_lookup(F, h)
{
	var t, table;

	t = integral_classify(F);

	if ((t & 1) && integral_search(F, h, integral_tab_exp))
		return;

	if ((t & 2) && integral_search(F, h, integral_tab_log))
		return;

	if ((t & 4) && integral_search(F, h, integral_tab_trig))
		return;

	if (car(F) == symbol(POWER))
		table = integral_tab_power;
	else
		table = integral_tab;

	if (integral_search(F, h, table))
		return;

	stopf("integral: no solution found");
}
