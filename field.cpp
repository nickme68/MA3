#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include "config.h"
#include "schema.h"
#include "field.h"

using namespace std;

void loadfield(field& F, const char* fname)
{
	ifstream f(fname);
	f >> F.N >> F.M;
	F.data = new char[F.N*F.M];
	for( int i=0; i<F.N; i++ )
	{
		while( f.get()!='\n' );
		for( int j=0; j<F.M; j++ )
			F.data[i*F.M+j] = f.get();
	}
	f.close();
}

void makefield(field& F, config& C)
{
	int k = C.findsec("options");
	F.omega = todouble(C.value(k,C.findrec(k,"omega"),0));
	F.t = toint(C.value(k,C.findrec(k,"t"),0)); 
	F.dt = toint(C.value(k,C.findrec(k,"dt"),0)); 
	F.output = C.value(k,C.findrec(k,"output"),0); 
	int j = C.findrec(k,"load");
	if( C.value(k,j,0)=="file" )
		loadfield(F, C.value(k,j,1).c_str());
	else if( C.value(k,j,0)=="script" )
	{
		string prompt = "python makefield.py " + C.filename;
		system(prompt.c_str());
		loadfield(F, "_input_.dat");
	}
}

void clearfield(field& F)
{
	delete[] F.data;
}

void printfield(field& F, ostream& f)
{
	for( int i=0; i<F.N; i++ )
	{
		for( int j=0; j<F.M; j++ )
			f << F.data[i*F.M+j];
		f << endl;
	}
}

int randchain(int start, int n, double omega)
{
	if( start==n ) 
		return -1;
	int l = start+1;
	while( frand() < omega && l < n) 
		l += 1;
	return l;
}

void updatefield(field& F, schema& H, schema& V)
{
	if( rand()%2 ) // vertical partition
	{
		for( int j=0; j<F.M; j++ )
		{
			int start = 0;
			int end = randchain(start, F.N, F.omega);
			while( end != -1 )
			{
				applyrule(V, F.data+start*F.M+j, end-start, F.M);
				start = end;
				end = randchain(start, F.N, F.omega);
			}
		}
	}
	else // horizontal partition
	{
		for( int i=0; i<F.N; i++ )
		{
			int start = 0;
			int end = randchain(start, F.M, F.omega);
			while( end != -1 )
			{
				applyrule(H, F.data+i*F.M+start, end-start, 1);
				start = end;
				end = randchain(start, F.M, F.omega);
			}
		}
	}
}