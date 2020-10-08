function
scan_inbuf(k)
{
	trace1 = k;
	k = scan(inbuf, k);
	if (k) {
		trace2 = k;
		trace_input();
	}
	return k;
}
