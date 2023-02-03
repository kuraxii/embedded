#include <stdio.h>
#include"mymath.h"
int main(void)
{
	int a = 14,b = 7;

	printf("%d + %d = %d\n",a,b,add(a,b));
	printf("%d - %d = %d\n",a,b,sub(a,b));
	printf("%d * %d = %d\n",a,b,div1(a,b));

}
