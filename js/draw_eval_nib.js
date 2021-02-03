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

		drawmode = 2;
		evalf();
		drawmode = 1;

		floatf();
	}

	catch(err) {

		if (drawmode == 1) {
			drawmode = 0;
			stopf("draw");
		}

		drawmode = 1;
		expanding = 1;

		stack.splice(save_stack_length);
		frame.splice(save_frame_length);

		push_symbol(NIL); // return value
	}

	finally {
		//
	}
}
