function
draw_eval_nib(F, X, x)
{
	var save_stack_length, save_frame_length;

	try {
		save_stack_length = stack.length;
		save_frame_length = frame.length;

		push_double(x);
		x = pop();
		set_binding(X, x);

		push(F);
		evalf();
		floatf();
	}

	catch(err) {
		stack.splice(save_stack_length);
		frame.splice(save_frame_length);
		expanding = 1;
		push_symbol(NIL); // return value
	}

	finally {
		//
	}
}
