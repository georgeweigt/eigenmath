function
guess()
{
	var p = pop();
	push(p);
	if (find(p, symbol(SYMBOL_X)))
		push_symbol(SYMBOL_X);
	else if (find(p, symbol(SYMBOL_Y)))
		push_symbol(SYMBOL_Y);
	else if (find(p, symbol(SYMBOL_Z)))
		push_symbol(SYMBOL_Z);
	else if (find(p, symbol(SYMBOL_T)))
		push_symbol(SYMBOL_T);
	else if (find(p, symbol(SYMBOL_S)))
		push_symbol(SYMBOL_S);
	else
		push_symbol(SYMBOL_X);
}
