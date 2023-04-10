function
trace_input()
{
	var p1;
	p1 = get_binding(symbol(TRACE));
	if (p1 != symbol(TRACE) && !iszero(p1))
		printbuf(instring.substring(trace1, trace2), BLUE);
}
