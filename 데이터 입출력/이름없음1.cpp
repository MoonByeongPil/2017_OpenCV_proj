#include<stdio.h>
#include<stdlib.h>

int main()
{
	while(1)
	{	
		int i; 

	    printf("0 : 0������, 1 : 1������\n�����ϼ���. "); 
	
	    scanf("%d",&i); 
	
	    if(i==1) 
	    { 
	       printf("1 ������\n"); 	
	       system("cpp���ڹޱ� 1"); 	
	    } 
	
	    else 
	    { 	
	       printf("0 ������\n"); 	
	       system("cpp���ڹޱ� 0"); 
	    }
	}
}


