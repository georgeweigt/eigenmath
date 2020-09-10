function
trace_input()
{
	if (!iszero(get_binding(symbol(TRACE))))
		print_buf(instring.substring(trace1, trace2), BLUE);
}
