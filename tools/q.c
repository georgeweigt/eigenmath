// generate kets, operators, and a measurement function

#include <stdio.h>

// N is number of qbits

#define N 4

// M is number of states

#define M (1 << N)

void kets(void);
void X(int n);
void Y(int n);
void Z(int n);
void H(int n);
void C(int m, int n);

int
main()
{
	int i, j;

	kets();

	for (i = 0; i < N; i++) {
		X(i);
		Y(i);
		Z(i);
		H(i);
		for (j = 0; j < N; j++)
			if (i != j)
				C(j, i);
	}

	printf("M(psi) = do(xrange = (0,%d), yrange = (0,1), draw(psi[ceiling(x)] conj(psi[ceiling(x)]),x))\n", M);
}

void
kets(void)
{
	int i, j;

	for (i = 0; i < M; i++) {

		printf("ket%d = (", i);

		for (j = 0; j < i; j++)
			printf("0,");

		printf("1");

		for (j = i + 1; j < M; j++)
			printf(",0");

		printf(")\n");
	}

	printf("\n");
}

void
X(int n)
{
	int i, j;

	printf("X%d =\n", n);

	n = 1 << n;

	for (i = 0; i < M; i++) {

		j = i ^ n; // flip bit n

		printf("outer(ket%d,ket%d)", i, j);

		if (i < M - 1)
			printf(" +");

		printf("\n");
	}

	printf("\n");
}

void
Y(int n)
{
	int i, j;

	printf("Y%d =\n", n);

	n = 1 << n;

	for (i = 0; i < M; i++) {

		j = i ^ n; // flip bit n

		if (i & n)
			printf("outer(ket%d,-i ket%d)", i, j);
		else
			printf("outer(ket%d,i ket%d)", i, j);

		if (i < M - 1)
			printf(" +");

		printf("\n");
	}

	printf("\n");
}


void
Z(int n)
{
	int i, j;

	printf("Z%d =\n", n);

	n = 1 << n;

	for (i = 0; i < M; i++) {

		j = i;

		if (i & n)
			printf("outer(ket%d,-ket%d)", i, j);
		else
			printf("outer(ket%d,ket%d)", i, j);

		if (i < M - 1)
			printf(" +");

		printf("\n");
	}

	printf("\n");
}

void
H(int n)
{
	int i, j;

	printf("H%d =\n", n);

	n = 1 << n;

	for (i = 0; i < M; i++) {

		j = i ^ n; // flip bit n

		printf("sqrt(1/2) outer(ket%d,", i);

		if (i & n)
			// i = |1>, j = |0>, print |0> - |1>
			printf("ket%d - ket%d)", j, i);
		else
			// i = |0>, j = |1>, print |0> + |1>
			printf("ket%d + ket%d)", i, j);

		if (i < M - 1)
			printf(" +");

		printf("\n");
	}

	printf("\n");
}

void
C(int m, int n)
{
	int i, j;

	printf("C%d%d =\n", m, n);

	m = 1 << m;
	n = 1 << n;

	for (i = 0; i < M; i++) {

		j = i;

		if (i & m)
			j ^= n; // flip bit n

		printf("outer(ket%d,ket%d)", i, j);

		if (i < M - 1)
			printf(" +");

		printf("\n");
	}

	printf("\n");
}
