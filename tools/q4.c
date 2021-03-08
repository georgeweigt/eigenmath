// generate operators for 4 qbits

#include <stdio.h>

void kets(void);
void X(int t);
void Y(int t);
void Z(int t);
void H(int t);
void C(int c, int t);

int q[4];
int r[4];

int
main()
{
	int i, j;

	kets();

	for (i = 0; i < 4; i++) {
		X(i);
		Y(i);
		Z(i);
		H(i);
	}

	for (i = 0; i < 4; i++)
		for (j = 0; j < 4; j++)
			if (i != j)
				C(i, j);

	puts("M(psi) = do(xrange = (0,16), yrange = (0,1), draw(psi[ceiling(x)] conj(psi[ceiling(x)]),x))");
}

void
kets(void)
{
	int i, k;

	for (k = 0; k < 16; k++) {

		q[0] = (k & 8) ? 1 : 0;
		q[1] = (k & 4) ? 1 : 0;
		q[2] = (k & 2) ? 1 : 0;
		q[3] = (k & 1) ? 1 : 0;

		printf("ket%d%d%d%d = (", q[0], q[1], q[2], q[3]);

		for (i = 0; i < k; i++)
			printf("0,");

		printf("1");

		for (i = k + 1; i < 16; i++)
			printf(",0");

		printf(")\n");
	}

	printf("\n");
}

void
X(int t)
{
	int k;

	printf("X%d = ", t);

	for (k = 0; k < 16; k++) {

		q[0] = (k & 8) ? 1 : 0;
		q[1] = (k & 4) ? 1 : 0;
		q[2] = (k & 2) ? 1 : 0;
		q[3] = (k & 1) ? 1 : 0;

		r[0] = q[0];
		r[1] = q[1];
		r[2] = q[2];
		r[3] = q[3];

		r[t] = q[t] ? 0 : 1;

		if (k > 0)
			printf("     ");

		printf("outer(ket%d%d%d%d,ket%d%d%d%d)", q[0], q[1], q[2], q[3], r[0], r[1], r[2], r[3]);

		if (k < 15)
			printf(" +\n");
		else
			printf("\n\n");
	}
}

void
Y(int t)
{
	int k;

	printf("Y%d = ", t);

	for (k = 0; k < 16; k++) {

		q[0] = (k & 8) ? 1 : 0;
		q[1] = (k & 4) ? 1 : 0;
		q[2] = (k & 2) ? 1 : 0;
		q[3] = (k & 1) ? 1 : 0;

		r[t] = q[t] ? 0 : 1;

		if (k > 0)
			printf("     ");

		if (q[t] == 0)
			printf("outer(ket%d%d%d%d,i ket%d%d%d%d)", q[0], q[1], q[2], q[3], r[0], r[1], r[2], r[3]);
		else
			printf("outer(ket%d%d%d%d,-i ket%d%d%d%d)", q[0], q[1], q[2], q[3], r[0], r[1], r[2], r[3]);

		if (k < 15)
			printf(" +\n");
		else
			printf("\n\n");
	}
}


void
Z(int t)
{
	int k;

	printf("Z%d = ", t);

	for (k = 0; k < 16; k++) {

		q[0] = (k & 8) ? 1 : 0;
		q[1] = (k & 4) ? 1 : 0;
		q[2] = (k & 2) ? 1 : 0;
		q[3] = (k & 1) ? 1 : 0;

		if (k > 0)
			printf("     ");

		if (q[t] == 0)
			printf("outer(ket%d%d%d%d,ket%d%d%d%d)", q[0], q[1], q[2], q[3], q[0], q[1], q[2], q[3]);
		else
			printf("outer(ket%d%d%d%d,-ket%d%d%d%d)", q[0], q[1], q[2], q[3], q[0], q[1], q[2], q[3]);

		if (k < 15)
			printf(" +\n");
		else
			printf("\n\n");
	}
}

void
H(int t)
{
	int k;

	printf("H%d = ", t);

	for (k = 0; k < 16; k++) {

		q[0] = (k & 8) ? 1 : 0;
		q[1] = (k & 4) ? 1 : 0;
		q[2] = (k & 2) ? 1 : 0;
		q[3] = (k & 1) ? 1 : 0;

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
	}
}

void
C(int c, int t)
{
	int k;

	printf("C%d%d = ", c, t);

	for (k = 0; k < 16; k++) {

		q[0] = (k & 8) ? 1 : 0;
		q[1] = (k & 4) ? 1 : 0;
		q[2] = (k & 2) ? 1 : 0;
		q[3] = (k & 1) ? 1 : 0;

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
	}
}
