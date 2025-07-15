void
eval_rand(struct atom *p1)
{
	double d;
	d = (double) rand() / ((double) RAND_MAX + 1);
	push_double(d);
}
