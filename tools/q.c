// generate kets, operators, and a measurement function

#include <stdio.h>
#include <stdlib.h>

// N is number of qubits

#define N 4

// M is number of states

#define M (1 << N)

void kets(void);

void H(int n);
void P(int n);
void X(int n);
void Y(int n);
void Z(int n);

void CH(int c, int n);
void CP(int c, int n);
void CX(int c, int n);
void CY(int c, int n);
void CZ(int c, int n);

void W(int m, int n);

int
main()
{
	int i, j;

	kets();

	for (i = 0; i < N; i++) {

		H(i);
		P(i);
		X(i);
		Y(i);
		Z(i);

		for (j = 0; j < N; j++) {
			CH(j, i);
			CP(j, i);
			CX(j, i);
			CY(j, i);
			CZ(j, i);
			W(j, i);
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

// hadamard

void
H(int n)
{
	int i;

	printf("H%d =\n", n);

	n = 1 << n;

	for (i = 0; i < M; i++) {

		if (i & n)
			printf("sqrt(1/2) outer(ket%d,ket%d - ket%d)", i, i ^ n, i); // do this for state 1
		else
			printf("sqrt(1/2) outer(ket%d,ket%d + ket%d)", i, i, i ^ n); // do this for state 0

		if (i < M - 1)
			printf(" +");

		printf("\n");
	}

	printf("\n");
}

void
CH(int c, int n)
{
	int i;

	printf("H%d%d =\n", c, n);

	c = 1 << c;
	n = 1 << n;

	for (i = 0; i < M; i++) {

		if ((i & c) == 0)
			printf("outer(ket%d,ket%d)", i, i);
		else if (i & n)
			printf("sqrt(1/2) outer(ket%d,ket%d - ket%d)", i, i ^ n, i); // do this for state 1
		else
			printf("sqrt(1/2) outer(ket%d,ket%d + ket%d)", i, i, i ^ n); // do this for state 0

		if (i < M - 1)
			printf(" +");

		printf("\n");
	}

	printf("\n");
}

// phase

void
P(int n)
{
	int i;

	printf("P%d(phi) = I - P%dM + exp(i phi) P%dM\n\n", n, n, n);

	printf("P%dM =\n", n);

	n = 1 << n;

	for (i = 0; i < M; i++) {

		if ((i & n) == 0)
			continue;

		printf("outer(ket%d,ket%d)", i, i);

		if (i < M - 1)
			printf(" +");

		printf("\n");
	}

	printf("\n");
}

void
CP(int c, int n)
{
	int i;

	printf("P%d%d(phi) = I - P%d%dM + exp(i phi) P%d%dM\n\n", c, n, c, n, c, n);

	printf("P%d%dM =\n", c, n);

	c = 1 << c;
	n = 1 << n;

	for (i = 0; i < M; i++) {

		if ((i & c) == 0 || (i & n) == 0)
			continue;

		printf("outer(ket%d,ket%d)", i, i);

		if (i < M - 1)
			printf(" +");

		printf("\n");
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

		if (i & n)
			printf("outer(ket%d,ket%d)", i, i ^ n); // do this for state 1
		else
			printf("outer(ket%d,ket%d)", i, i ^ n); // do this for state 0

		if (i < M - 1)
			printf(" +");

		printf("\n");
	}

	printf("\n");
}

void
CX(int c, int n)
{
	int i;

	printf("X%d%d =\n", c, n);

	c = 1 << c;
	n = 1 << n;

	for (i = 0; i < M; i++) {

		if ((i & c) == 0)
			printf("outer(ket%d,ket%d)", i, i);
		else if (i & n)
			printf("outer(ket%d,ket%d)", i, i ^ n); // do this for state 1
		else
			printf("outer(ket%d,ket%d)", i, i ^ n); // do this for state 0

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
			printf("outer(ket%d,-i ket%d)", i, i ^ n); // do this for state 1
		else
			printf("outer(ket%d,i ket%d)", i, i ^ n); // do this for state 0

		if (i < M - 1)
			printf(" +");

		printf("\n");
	}

	printf("\n");
}

void
CY(int c, int n)
{
	int i;

	printf("Y%d%d =\n", c, n);

	c = 1 << c;
	n = 1 << n;

	for (i = 0; i < M; i++) {

		if ((i & c) == 0)
			printf("outer(ket%d,ket%d)", i, i);
		else if (i & n)
			printf("outer(ket%d,-i ket%d)", i, i ^ n); // do this for state 1
		else
			printf("outer(ket%d,i ket%d)", i, i ^ n); // do this for state 0

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
			printf("outer(ket%d,-ket%d)", i, i); // do this for state 1
		else
			printf("outer(ket%d,ket%d)", i, i); // do this for state 0

		if (i < M - 1)
			printf(" +");

		printf("\n");
	}

	printf("\n");
}

void
CZ(int c, int n)
{
	int i;

	printf("Z%d%d =\n", c, n);

	c = 1 << c;
	n = 1 << n;

	for (i = 0; i < M; i++) {

		if ((i & c) == 0)
			printf("outer(ket%d,ket%d)", i, i);
		else if (i & n)
			printf("outer(ket%d,-ket%d)", i, i); // do this for state 1
		else
			printf("outer(ket%d,ket%d)", i, i); // do this for state 0

		if (i < M - 1)
			printf(" +");

		printf("\n");
	}

	printf("\n");
}

// swap

void
W(int m, int n)
{
	int i, t;

	printf("W%d%d =\n", m, n);

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
