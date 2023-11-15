#include<stdio.h>
#include<string.h>
 
int main(void)
{
    	char a[] = "abc cnczx csdv vsd";;
        printf("%p", a);
    	char aa[1000];
    	char c[] = " ";
 
    	
 
    	char *p = strtok(a,c);

    	while(p)
    	{
        	printf("%p\n", p);
        	p = strtok(NULL,c); 
    	}
}