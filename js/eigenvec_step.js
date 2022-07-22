function
eigenvec_step(D, Q, n)
{
	var count, i, j;

	count = 0;

	// for each upper triangle "off-diagonal" component do step_nib

	for (i = 0; i < n - 1; i++) {
		for (j = i + 1; j < n; j++) {
			if (D[n * i + j] != 0.0) {
				eigenvec_step_nib(D, Q, n, i, j);
				count++;
			}
		}
	}

	return count;
}
