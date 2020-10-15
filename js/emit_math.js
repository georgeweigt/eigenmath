/* exported emit_math */

function
emit_math()
{
	var p1 = pop();
	emitbuf = [];
	emit_expr(p1);
}
