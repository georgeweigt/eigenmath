function
eval_nonstop()
{
	var save_expanding, save_stack_length, save_frame_length;

	try {
		save_expanding = expanding;

		save_stack_length = stack.length - 1;
		save_frame_length = frame.length;

		evalf();
	}

	catch(errmsg) {

		expanding = save_expanding;

		stack.splice(save_stack_length);
		frame.splice(save_frame_length);

		push_symbol(NIL); // return value
	}

	finally {
		//
	}
}
