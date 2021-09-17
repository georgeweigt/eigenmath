/* exported run */

function
run()
{
	try {
		run_nib();
	} catch (errmsg) {
		if (errmsg.length > 0) {
			if (trace1 < trace2 && inbuf[trace2 - 1] == '\n')
				trace2--;
			printbuf(inbuf.substring(trace1, trace2) + "\nStop: " + errmsg, RED);
		}
	}
}

function
run_nib()
{
	var k = 0;

	inbuf = document.getElementById("stdin").value;
	stdout = document.getElementById("stdout");
	stdout.innerHTML = "";

	init();
	initscript();

	for (;;) {

		k = scan_inbuf(k);

		if (k == 0)
			break; // end of input

		eval_and_print_result();
	}
}
