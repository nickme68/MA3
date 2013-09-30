#include <iostream>
#include "schema.h"

using namespace std;

int scopy(char* dst, int pos, const char* src, int n)
{
	for( int i=0; i<n; i++ )
		dst[pos+i] = src[i];
	return pos+n;
}

// makes new schema M by its name 
void makeschema(schema& M, config& C, char* name)
{
	int k = C.findsec(name);
	int n = C.secsize(k); 
	//cout << "[" << name << "] " << k << " " << n << endl;

	M.llen = 0;
	M.rlen = 0;
	M.lnum = 0;
	M.rnum = 0;
	M.maxlen = 0;
	for( int i=0; i<n; i++ )
	{
		if( C.key(k,i)=="rule" )
		{
			M.lnum += 1;
			int rn = (C.recsize(k,i)+1)/2; 
			M.rnum += rn;
			int ll = C.value(k,i,0).length();
			if( ll>M.maxlen )
				M.maxlen = ll;
			M.llen += ll;
			M.rlen += rn*ll;
		}
	}
	//cout << lnum << " " << llen << ", " << rnum << " " << rlen << endl;

	M.buf = new char[M.llen+M.rlen];
	M.lhs = new short[M.lnum+1];
	M.pos = new short[M.lnum];
	M.rhs = new short[M.rnum];
	M.prob = new float[M.rnum];
	int jbuf = 0, ibuf = M.llen, ipos = 0, irhs = 0, ilhs = 0;
	int rn = 0;
	for( int i=0; i<n; i++ )
	{
		if( C.key(k,i) != "rule" )
			continue;
		int ll = C.value(k,i,0).length();
		M.lhs[ilhs] = jbuf;
		ilhs += 1;
		jbuf = scopy(M.buf, jbuf, C.value(k,i,0).c_str(), ll);
		if( ipos==0 )
			M.pos[ipos] = 0;
		else
			M.pos[ipos] = M.pos[ipos-1] + rn;
		rn = (C.recsize(k,i)+1)/2; 
		ipos += 1;
		float p = 0.0;
		for( int j=0; j<rn-1; j++ )
		{
			p += tofloat(C.value(k,i,2+2*j));
			M.rhs[irhs] = ibuf;
			M.prob[irhs] = p;
			ibuf = scopy(M.buf,ibuf,C.value(k,i,1+2*j).c_str(),ll);
			irhs += 1;
		}
		M.rhs[irhs] = ibuf;
		M.prob[irhs] = 1.0;
		ibuf = scopy(M.buf,ibuf,C.value(k,i,0).c_str(),ll);
		irhs += 1;
	}
	M.lhs[M.lnum] = M.llen;
	/*
	for( int i=0; i<llen+rlen; i++ )
		cout << M.buf[i];
	cout << endl;
	for( int i=0; i<lnum+1; i++ )
		cout << M.lhs[i] << " ";
	cout << endl;
	for( int i=0; i<lnum; i++ )
		cout << M.pos[i] << " ";
	cout << endl;
	for( int i=0; i<rnum; i++ )
		cout << M.rhs[i] << " ";
	cout << endl;
	for( int i=0; i<rnum; i++ )
		cout << M.prob[i] << " ";
	cout << endl;
	getchar();
	//*/
}

// clears all data in the schema M
void clearschema(schema& M)
{
	delete[] M.buf;
	delete[] M.lhs;
	delete[] M.pos;
	delete[] M.rhs;
	delete[] M.prob;
}

int cmp(char* a, int n, int dk, char* b, int m)
{
	for( int i=0; i < n && i < m; i++ )
	{
		if( a[i*dk] < b[i] )
			return -1;
		if( a[i*dk] > b[i] )
			return 1;
	}
	if( n < m )
		return -1;
	if( n > m )
		return 1;
	return 0; // equal
}

// finds rule's number by its left side chain a 
// n - length(a)
int findrule(schema& M, char* a, int n, int dk)
{
	/* serial search
	for( int i=0; i<M.lnum; i++ )
	{
		char* b = M.buf + M.lhs[i];
		int m = M.lhs[i+1] - M.lhs[i];
		if( cmp(a,n,dk,b,m)==0 )
		{
			//cerr << i;
			//getchar();
			return i;
		}
	}
	return -1;
	//*/
	//* binary search
	if( n > M.maxlen )
		return -1;
	int l = 0;
	int r = M.lnum;
	while( l<r )
	{
		int q = (l+r)/2;
		char* b = M.buf + M.lhs[q];
		int m = M.lhs[q+1] - M.lhs[q];
		int c = cmp(a, n, dk, b, m);
		if( c == 0 )
		{
			//cerr << q;
			//getchar();
			return q;
		}
		if( c < 0 )
			r = q;
		else
			l = q + 1;
	}
	return -1;
	//*/
}

void applyrule(schema& M, char* a, int n, int dk)
{
	int rule = findrule(M, a, n, dk);
	if( rule == -1 )
		return;
	int k = M.pos[rule];
	double r = frand();
	while( r > M.prob[k] )
		k += 1;
	int j = M.rhs[k];
	for( int i=0; i<n; i++ )
		a[i*dk] = M.buf[j+i];
}
