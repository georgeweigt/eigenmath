/* exported run */

function
run()
{
	inbuf = document.getElementById("stdin").value;
	stdout = document.getElementById("stdout");
	stdout.innerHTML = "";

	try {
		run_nib();
	}

	catch(err) {
		//
	}

	finally {
		//
	}
}
