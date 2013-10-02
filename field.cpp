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
	F.partition = new char[F.N*F.M];
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
	delete[] F.partition;
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

void makepartition(char* partition, int n, int m, double omega, int dir)
{
	for( int i=0; i<n*m; i++ )
		partition[i] = (frand()<omega)?0:1;
	if( dir==1 ) // horizontal
		for( int i=0; i<n; i++ )
			partition[(i+1)*m-1] = 1;
	else // vertical
		for( int j=0; j<m; j++ )
			partition[(n-1)*m+j] = 1;
}

// updates field once using random partition of the field (by rows or columns)
void updatefield(field& F, schema& H, schema& V)
{
	int parH[] = {F.N, F.M, 1, F.M};	// number of chains, length of the chain, delta, delta'
	int parV[] = {F.M, F.N, F.M, 1};	// ...
	int r = randbit();					// random choice of the partition type: 1 - rows, 0 - columns
	int* par = r ? parH : parV;			// current parameters
	schema* S = r ? &H : &V;			// current schema
	makepartition(F.partition, F.N, F.M, F.omega, r);
	for( int i=0; i<par[0]; i++ )		// loop over all chains
	{
		int pos = 0;
		while( pos < par[1] )			// loop over symbols in the current chain
		{
			int start = pos*par[2]+i*par[3]; 
			pos += applyrule(*S, F.data+start, F.partition+start, par[2]); // replace subchain
		}
	}
}