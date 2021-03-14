// generate kets, operators, and a measurement function

#include <stdio.h>
#include <stdlib.h>

// N is number of qbits

#define N 4

// M is number of states

#define M (1 << N)

void kets(void);
void X(int n);
void Y(int n);
void Z(int n);
void H(int n);
void CX(int m, int n);
void CP(int m, int n);
void S(int m, int n);

int
main()
{
	int i, j;

	printf("-- copy to Downloads folder on Mac\n\n");

	kets();

	for (i = 0; i < N; i++) {

		X(i); // pauli x
		Y(i); // pauli y
		Z(i); // pauli z
		H(i); // hadamard

		for (j = 0; j < N; j++)
			if (i != j) {
				CX(j, i); // controlled x (cnot)
				CP(j, i); // controlled phase
				S(j, i); // swap
			}
	}

	fflush(stdout);

	system("cat qfile.txt");
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
	int i;

	printf("X%d =\n", n);

	n = 1 << n;

	for (i = 0; i < M; i++) {

		printf("outer(ket%d,ket%d)", i, i ^ n);

		if (i < M - 1)
			printf(" +");

		printf("\n");
	}

	printf("\n");
}

void
Y(int n)
{
	int i;

	printf("Y%d =\n", n);

	n = 1 << n;

	for (i = 0; i < M; i++) {

		if (i & n)
			printf("outer(ket%d,-i ket%d)", i, i ^ n);
		else
			printf("outer(ket%d,i ket%d)", i, i ^ n);

		if (i < M - 1)
			printf(" +");

		printf("\n");
	}

	printf("\n");
}

void
Z(int n)
{
	int i;

	printf("Z%d =\n", n);

	n = 1 << n;

	for (i = 0; i < M; i++) {

		if (i & n)
			printf("outer(ket%d,-ket%d)", i, i);
		else
			printf("outer(ket%d,ket%d)", i, i);

		if (i < M - 1)
			printf(" +");

		printf("\n");
	}

	printf("\n");
}

// hadamard

void
H(int n)
{
	int i;

	printf("H%d =\n", n);

	n = 1 << n;

	for (i = 0; i < M; i++) {

		printf("sqrt(1/2) outer(ket%d,", i);

		if (i & n)
			printf("ket%d - ket%d)", i ^ n, i); // |0> - |1>
		else
			printf("ket%d + ket%d)", i, i ^ n); // |0> + |1>

		if (i < M - 1)
			printf(" +");

		printf("\n");
	}

	printf("\n");
}

// controlled x (cnot)

void
CX(int m, int n)
{
	int i;

	printf("X%d%d =\n", m, n);

	m = 1 << m;
	n = 1 << n;

	for (i = 0; i < M; i++) {

		if (i & m)
			printf("outer(ket%d,ket%d)", i, i ^ n);
		else
			printf("outer(ket%d,ket%d)", i, i);

		if (i < M - 1)
			printf(" +");

		printf("\n");
	}

	printf("\n");
}

// controlled phase

void
CP(int m, int n)
{
	int i;

	printf("P%d%d(phi) = I - P%d%dM + exp(i phi) P%d%dM\n\n", m, n, m, n, m, n);

	printf("P%d%dM =\n", m, n);

	m = 1 << m;
	n = 1 << n;

	for (i = 0; i < M; i++) {

		if ((i & m) == 0 || (i & n) == 0)
			continue;

		printf("outer(ket%d,ket%d)", i, i);

		if (i < M - 1)
			printf(" +");

		printf("\n");
	}

	printf("\n");
}

// swap

void
S(int m, int n)
{
	int i, t;

	printf("S%d%d =\n", m, n);

	m = 1 << m;
	n = 1 << n;

	for (i = 0; i < M; i++) {

		t = i & (m | n);

		if (t == m || t == n)
			printf("outer(ket%d,ket%d)", i, i ^ m ^ n);
		else
			printf("outer(ket%d,ket%d)", i, i);

		if (i < M - 1)
			printf(" +");

		printf("\n");
	}

	printf("\n");
}
