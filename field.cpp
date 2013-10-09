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
	string cyclic = C.safe_value(k,C.findrec(k,"cyclic"),0,"no");
	F.vcyclic = !( cyclic=="no" || cyclic=="horizontal" );
	F.hcyclic = !( cyclic=="no" || cyclic=="vertical" );
	int j = C.findrec(k,"load");
	if( C.value(k,j,0)=="file" )
		loadfield(F, C.value(k,j,1).c_str());
	else if( C.value(k,j,0)=="script" )
	{
		string prompt = "python makefield.py " + C.filename;
		system(prompt.c_str());
		loadfield(F, "_input_.dat");
	}
	int sz = (F.N>F.M) ? F.N : F.M;
	F.data_buf = new char[sz];
	F.part_buf = new char[sz];
}

void clearfield(field& F)
{
	delete[] F.data;
	delete[] F.partition;
	delete[] F.data_buf;
	delete[] F.part_buf;
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

void makepartition(char* partition, int n, int m, double omega, int dir, int hc, int vc)
{
	for( int i=0; i<n*m; i++ )
		partition[i] = (frand()<omega)?0:1;
	if( dir==1 && hc==0 ) // horizontal
		for( int i=0; i<n; i++ )
			partition[(i+1)*m-1] = 1;
	if( dir==0 && vc==0 ) // vertical
		for( int j=0; j<m; j++ )
			partition[(n-1)*m+j] = 1;
}

// updates field once using random partition of the field (by rows or columns)

void updatebuf(schema& S, char* data_buf, char* part_buf, int n)
{
	int pos = 0;
	for( int j=0; j<n; j++ )
		if( part_buf[j]==1 )
		{
			applyrule(S, data_buf+pos, j-pos+1);	
			pos = j+1;
		}
}

void updatefield(field& F, schema& H, schema& V)
{
	int pos;
	int r = randbit();
	makepartition(F.partition, F.N, F.M, F.omega, r, F.hcyclic, F.vcyclic);
	if( r==1 ) // rows
	{
		for( int i=0; i<F.N; i++ )
		{
			int end = 0;
			while( end<F.M && F.partition[i*F.M+end]==0 )
				end++;
			if( end==F.M )
				return;
			for( int j=0; j<F.M; j++)
			{
				pos = j-end-1;
				if( pos<0 )
					pos += F.M;
				F.data_buf[pos] = F.data[i*F.M + j];
				F.part_buf[pos] = F.partition[i*F.M + j];
			}
			updatebuf(H,F.data_buf,F.part_buf,F.M);
			for( int j=0; j<F.M; j++)
			{
				pos = j-end-1;
				if( pos<0 )
					pos += F.M;
				F.data[i*F.M + j] = F.data_buf[pos];
				F.partition[i*F.M + j] = F.part_buf[pos];
			}
		}
	}
	else // columns
	{
		for( int i=0; i<F.M; i++ )
		{
			int end = 0;
			while( end<F.N && F.partition[end*F.M+i]==0 )
				end++;
			if( end==F.N )
				return;
			for( int j=0; j<F.N; j++)
			{
				pos = j-end-1;
				if( pos<0 )
					pos += F.N;
				F.data_buf[pos] = F.data[j*F.M + i];
				F.part_buf[pos] = F.partition[j*F.M + i];
			}
			updatebuf(V,F.data_buf,F.part_buf,F.N);
			for( int j=0; j<F.N; j++)
			{
				pos = j-end-1;
				if( pos<0 )
					pos += F.N;
				F.data[j*F.M + i] = F.data_buf[pos];
				F.partition[j*F.M + i] = F.part_buf[pos];
			}
		}
	}
}