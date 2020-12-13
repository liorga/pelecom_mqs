#include <stdio.h>
#include "random.h"

int main() {
	srand(time(NULL));
	double ra = 0;
	printf("hello world\n");
	ra = urand01();
	printf("%lf\n",ra);
	return 0;
}
