// this way matches strcmp (localeCompare differs from strcmp)

function
cmp_strings(s1, s2)
{
	if (s1 < s2)
		return -1;
	if (s1 > s2)
		return 1;
	return 0;
}
