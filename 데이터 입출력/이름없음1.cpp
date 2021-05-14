#include<stdio.h>
#include<stdlib.h>

int main()
{
	while(1)
	{	
		int i; 

	    printf("0 : 0보내기, 1 : 1보내기\n선택하세요. "); 
	
	    scanf("%d",&i); 
	
	    if(i==1) 
	    { 
	       printf("1 보내기\n"); 	
	       system("cpp인자받기 1"); 	
	    } 
	
	    else 
	    { 	
	       printf("0 보내기\n"); 	
	       system("cpp인자받기 0"); 
	    }
	}
}


