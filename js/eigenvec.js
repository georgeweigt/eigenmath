function
eigenvec(D, Q, n)
{
	var i;

	for (i = 0; i < 100; i++)
		if (eigenvec_step(D, Q, n) == 0)
			return;

	stopf("eigenvec: convergence error");
}
