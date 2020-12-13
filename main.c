#include <stdio.h>
#include "random.h"

int main() {
	double ra = 0;
	printf("Hello, World!\n");
	
	ra = urand01();
	printf("is : %lf\n",ra);
	
	return 0;
}
