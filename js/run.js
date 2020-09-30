/* exported run */

function
run()
{
	var s = document.getElementById("stdin").value;

	stdout = document.getElementById("stdout");
	stdout.innerHTML = "";

	try {
		run_nib(s);
	}

	catch(err) {
		//
	}

	finally {
		//
	}
}
