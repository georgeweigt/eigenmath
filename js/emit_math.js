/* exported emit_math */

function
emit_math()
{
	var p1 = pop();
	emitbuf = [];
	emit_data_begin();
	emit_expr(p1);
	emit_data_end();
}
