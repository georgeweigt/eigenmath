function
derivative()
{
	var p1, p2;

	p2 = pop();
	p1 = pop();

	if (istensor(p1))
		if (istensor(p2))
			d_tensor_tensor(p1, p2);
		else
			d_tensor_scalar(p1, p2);
	else
		if (istensor(p2))
			d_scalar_tensor(p1, p2);
		else
			d_scalar_scalar(p1, p2);
}
