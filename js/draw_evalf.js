function
draw_evalf(F, X, x)
{
	var tos = stack.length;
	var tof = frame.length;

	try {
		push_double(x);
		x = pop();
		set_binding(X, x);
		push(F);
		evalf();
		floatf();
	}

	catch(err) {
		stack.splice(tos); // pop all
		frame.splice(tof); // pop all
		expanding = 1;
		push_symbol(NIL);
	}

	finally {
		//
	}
}
