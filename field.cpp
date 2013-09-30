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

// updates field once using random partition of the field (by rows or columns)
void updatefield(field& F, schema& H, schema& V)
{
	int parH[] = {F.N, F.M, 1, F.M};	// number of chains, length of the chain, delta, delta'
	int parV[] = {F.M, F.N, F.M, 1};	// ...
	int r = rand()%2;					// 0 - rows, 1 - columns
	int* par = r ? parH : parV;			// current parameters
	schema* S = r ? &H : &V;			// current schema
	for( int i=0; i<par[0]; i++ )		// loop over all chains
	{
		int start, end = 0;
		while( end < par[1] )			// loop over symbols in the current chain
		{
			start = end;
			end = randchain(start, par[1], F.omega); // cut subchain
			applyrule(*S, F.data+start*par[2]+i*par[3], end-start, par[2]); // replace subchain
		}
	}
}