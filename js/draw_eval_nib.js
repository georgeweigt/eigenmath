function
draw_eval_nib(F, X, x)
{
	var p1, tos, tof;

	try {
		push(get_binding(F)); // on error, binding may be lost
		tos = stack.length;
		tof = frame.length;
		push_double(x);
		x = pop();
		set_binding(X, x);
		push(F);
		evalf();
		floatf();
		p1 = pop();
		pop(); // pop saved binding
		push(p1);
	}

	catch(err) {
		stack.splice(tos); // pop all
		frame.splice(tof); // pop all
		expanding = 1;
		p1 = pop();
		set_binding(F, p1);
		push_symbol(NIL); // return value
	}

	finally {
		//
	}
}
