// generate hadamard and cnot operators for 2 qbits

#include <stdio.h>

void hadamard(int t);
void cnot(int c, int t);

int
main()
{
	int i, j;

	hadamard(0);
	hadamard(1);

	for (i = 0; i < 2; i++)
		for (j = 0; j < 2; j++)
			if (i != j)
				cnot(i, j);
}

void
hadamard(int t)
{
	int k = 0, q[2], r[2];

	printf("H%d = (", t);

	for (q[0] = 0; q[0] < 2; q[0]++)
	for (q[1] = 0; q[1] < 2; q[1]++) {

		r[0] = q[0];
		r[1] = q[1];

		if (k > 0)
			printf("      ");

		if (q[t] == 0) {
			r[t] = 1;
			printf("ket%d%d + ket%d%d", q[0], q[1], r[0], r[1]);
		} else {
			r[t] = 0;
			printf("ket%d%d - ket%d%d", r[0], r[1], q[0], q[1]);
		}

		if (k < 3)
			printf(",\n");
		else
			printf(") / sqrt(2)\n\n");

		k++;
	}
}

void
cnot(int c, int t)
{
	int k = 0, q[2], r[2];

	printf("C%d%d = (", c, t);

	for (q[0] = 0; q[0] < 2; q[0]++)
	for (q[1] = 0; q[1] < 2; q[1]++) {

		r[0] = q[0];
		r[1] = q[1];

		if (q[c] == 1)
			r[t] = q[t] ? 0 : 1; // flip target

		if (k > 0)
			printf("       ");

		printf("ket%d%d", r[0], r[1]);

		if (k < 3)
			printf(",\n");
		else
			printf(")\n\n");

		k++;
	}
}
