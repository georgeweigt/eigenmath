// generate hadamard, not, and cnot operators for 4 qbits

#include <stdio.h>

void kets(void);
void hadamard(int t);
void not(int t);
void cnot(int c, int t);

int
main()
{
	int i, j;

	kets();

	hadamard(0);
	hadamard(1);
	hadamard(2);
	hadamard(3);

	not(0);
	not(1);
	not(2);
	not(3);

	for (i = 0; i < 4; i++)
		for (j = 0; j < 4; j++)
			if (i != j)
				cnot(i, j);
}

void
kets(void)
{
	int i, k, q[4];

	k = 0;

	for (q[0] = 0; q[0] < 2; q[0]++)
	for (q[1] = 0; q[1] < 2; q[1]++)
	for (q[2] = 0; q[2] < 2; q[2]++)
	for (q[3] = 0; q[3] < 2; q[3]++) {

		printf("ket%d%d%d%d = (", q[0], q[1], q[2], q[3]);

		for (i = 0; i < k; i++)
			printf("0,");

		printf("1");

		for (i = k + 1; i < 16; i++)
			printf(",0");

		printf(")\n");

		k++;
	}

	printf("\n");
}

void
hadamard(int t)
{
	int k = 0, q[4], r[4];

	printf("H%d = ", t);

	for (q[0] = 0; q[0] < 2; q[0]++)
	for (q[1] = 0; q[1] < 2; q[1]++)
	for (q[2] = 0; q[2] < 2; q[2]++)
	for (q[3] = 0; q[3] < 2; q[3]++) {

		r[0] = q[0];
		r[1] = q[1];
		r[2] = q[2];
		r[3] = q[3];

		r[t] = q[t] ? 0 : 1;

		if (k > 0)
			printf("     ");

		printf("sqrt(1/2) outer(ket%d%d%d%d,", q[0], q[1], q[2], q[3]);

		if (q[t] == 0)
			// q = |0>, r = |1>, print |0> + |1>
			printf("ket%d%d%d%d + ket%d%d%d%d)", q[0], q[1], q[2], q[3], r[0], r[1], r[2], r[3]);
		else
			// q = |1>, r = |0>, print |0> - |1>
			printf("ket%d%d%d%d - ket%d%d%d%d)", r[0], r[1], r[2], r[3], q[0], q[1], q[2], q[3]);

		if (k < 15)
			printf(" +\n");
		else
			printf("\n\n");

		k++;
	}
}

void
not(int t)
{
	int k = 0, q[4], r[4];

	printf("X%d = ", t);

	for (q[0] = 0; q[0] < 2; q[0]++)
	for (q[1] = 0; q[1] < 2; q[1]++)
	for (q[2] = 0; q[2] < 2; q[2]++)
	for (q[3] = 0; q[3] < 2; q[3]++) {

		r[0] = q[0];
		r[1] = q[1];
		r[2] = q[2];
		r[3] = q[3];

		r[t] = q[t] ? 0 : 1; // flip target

		if (k > 0)
			printf("     ");

		printf("outer(ket%d%d%d%d,ket%d%d%d%d)", q[0], q[1], q[2], q[3], r[0], r[1], r[2], r[3]);

		if (k < 15)
			printf(" +\n");
		else
			printf("\n\n");

		k++;
	}
}

void
cnot(int c, int t)
{
	int k = 0, q[4], r[4];

	printf("C%d%d = ", c, t);

	for (q[0] = 0; q[0] < 2; q[0]++)
	for (q[1] = 0; q[1] < 2; q[1]++)
	for (q[2] = 0; q[2] < 2; q[2]++)
	for (q[3] = 0; q[3] < 2; q[3]++) {

		r[0] = q[0];
		r[1] = q[1];
		r[2] = q[2];
		r[3] = q[3];

		if (q[c] == 1)
			r[t] = q[t] ? 0 : 1; // flip target

		if (k > 0)
			printf("      ");

		printf("outer(ket%d%d%d%d,ket%d%d%d%d)", q[0], q[1], q[2], q[3], r[0], r[1], r[2], r[3]);

		if (k < 15)
			printf(" +\n");
		else
			printf("\n\n");

		k++;
	}
}
