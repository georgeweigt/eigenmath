function
stop(s)
{
	if (s.length > 0) {
		if (trace1 < trace2 && inbuf[trace2 - 1] == '\n')
			trace2--;
		print_buf(inbuf.substring(trace1, trace2) + "\nStop: " + s, RED);
	}

	throw s;
}
