function
trace_input()
{
	if (!iszero(get_binding(symbol(TRACE))))
		printbuf(instring.substring(trace1, trace2), BLUE);
}
