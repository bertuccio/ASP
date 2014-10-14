#include <stdio.h>

#define LONGITUD 20000

void mult(long fact[],int j)
{
 int i,ll;
 long x;

 ll=0;
 for(i=0;i<=(LONGITUD-1);i++)
 {
  x=fact[i]*j+ll;
  fact[i]=x%10000;
  ll=x/10000;
 }
}


void factorial(int num,long fact[])
{
 int j;

 for(j=0;j<LONGITUD;j++)
  fact[j]=0L;

 fact[0]=1L;
 for(j=1;j<=num;j++)
  mult(fact,j);
} 

void imprime_fact ( long fact[] )
{
	int p, j;

	p = 1;
	for ( j = LONGITUD-1; j>=0; j-- )
		if ( fact[j] || !p )
		{
			fprintf(stdout,"%04ld",fact[j]);
			p = 0;
		}
	fprintf ( stdout,"\n");
}

void main()
{
	long f[LONGITUD];
	int i;

	for ( i = 0; i<20; i+=3 )
	{
		factorial ( i, f );
		fprintf(stdout,"fact(%d) :",i);
		imprime_fact ( f );
	}
        
}