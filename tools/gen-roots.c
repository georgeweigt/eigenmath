// generate self test for roots function

#include <stdio.h>
#include <stdlib.h>

int
urand(void)
{
	int t;
	t = rand() % 9 + 1; // t = 1,2,...,9
	if (rand() % 2 == 1)
		t = -t; // random sign
	return t;
}

int
main()
{
	int a1, b1;
	int a2, b2;
	int a3, b3;

	int c, d, i, t;

	for (i = 0; i < 100; i++) {

		for (;;) {

			a1 = urand();
			b1 = urand();

			a2 = urand();
			b2 = urand();

			a3 = urand();
			b3 = urand();

			// want unique roots

			if (a1 * b2 == a2 * b1)
				continue;

			if (a1 * b3 == a3 * b1)
				continue;

			if (a2 * b3 == a3 * b2)
				continue;

			break;
		}

		printf("p = ((%d) x - (%d)) ((%d) x - (%d)) ((%d) x - (%d))\n", a1, b1, a2, b2, a3, b3);

		// sort

		if (a1 < 0) {
			a1 = -a1;
			b1 = -b1;
		}

		if (a2 < 0) {
			a2 = -a2;
			b2 = -b2;
		}

		if (a3 < 0) {
			a3 = -a3;
			b3 = -b3;
		}

		c = b1 * a2;
		d = b2 * a1;

		if (c > d) {
			t = a1;
			a1 = a2;
			a2 = t;
			t = b1;
			b1 = b2;
			b2 = t;
		}

		c = b2 * a3;
		d = b3 * a2;

		if (c > d) {
			t = a2;
			a2 = a3;
			a3 = t;
			t = b2;
			b2 = b3;
			b3 = t;
		}

		c = b1 * a2;
		d = b2 * a1;

		if (c > d) {
			t = a1;
			a1 = a2;
			a2 = t;
			t = b1;
			b1 = b2;
			b2 = t;
		}

		printf("r = (%d/%d,%d/%d,%d/%d)\n", b1, a1, b2, a2, b3, a3);

		printf("check(roots(p) == r)\n");
	}
}
