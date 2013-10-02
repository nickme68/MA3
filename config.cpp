#include "config.h"

#include <iostream>
#include <fstream>

void config::load(char * fname)
{
	filename = fname;
	char ch;
	ifstream f(fname);
	int state = 0;
	string buf, subbuf;
	int S = -1; // current section number
	int R; // currrent record number
	bool waskey;
	while( f.get(ch) && state!=-1 )
	{
		switch(state)
		{
		case 0:
			buf = "";
			switch(ch)
			{
			case ' ':
				state = 0;
				break;
			case '\n':
				state = 0;
				break;
			case '#':
				state = 1;
				break;
			case '[':
				state = 2;
				break;
			case '"':
				state = 3;
				break;
			default:
				buf += ch;
				state = 4;
			}
			break;
		case 1:
			if( ch=='\n' )
			{
				waskey = false;
				state = 0;
			}
			break;
		case 2:
			if( ch==']' )
			{
				if( buf=="end" )
					state = -1;
				else
				{
					S += 1;
					data.push_back(section());
					data[S].name = buf;
					R = -1;
					waskey = false;
					state = 0;
				}
			}
			else
				buf += ch;
			break;
		case 3:
			if( ch=='"' )
			{
				if( !waskey )
				{
					R += 1;
					data[S].records.push_back(record());
					data[S].records[R].key = buf;
					waskey = true;
				}
				else
					data[S].records[R].values.push_back(buf);
				state = 0;
			}
			else
				buf += ch;
			break;
		case 4:
			if( ch==' ' || ch=='\n' || ch=='#' )
			{
				if( !waskey )
				{
					R += 1;
					data[S].records.push_back(record());
					data[S].records[R].key = buf;
					waskey = true;
				}
				else
					data[S].records[R].values.push_back(buf);
			}
			switch( ch )
			{
			case ' ':
				state = 0;
				break;
			case '\n':
				waskey = false;
				state = 0;
				break;
			case '#':
				waskey = false;
				state = 1;
				break;
			case '$':
				subbuf = "";
				state = 5;
				break;
			default:
				buf += ch;
			}
			break;
		case 5:
			if( ch=='$' )
			{
				buf += subbuf;
				state = 4;
			}
			else
				subbuf += ch;
			break;
		}
	}
	f.close();	
}

record& config::getrec(string sname, string rname)
{
	for( int S = 0; S<data.size(); S++ )
	{
		if( data[S].name == sname )
		{
			for( int R = 0; R<data[S].records.size(); R++ )
				if( data[S].records[R].key == rname )
					return data[S].records[R];
			return dummy;
		}
	}
	return dummy;
}

int config::findsec(string sname)
{
	for( int S = 0; S<data.size(); S++ )
		if( data[S].name == sname )
			return S;
	return -1;
}

int config::findrec(int S, string rname)
{
	for( int R = 0; R<data[S].records.size(); R++ )
		if( data[S].records[R].key == rname )
			return R;
	return -1;
}

/////// utils

float tofloat(string s)
{
	return float(atof(s.c_str()));
}

double todouble(string s)
{
	return double(atof(s.c_str()));
}

int toint(string s)
{
	return atoi(s.c_str());
}

double frand()
{
	return rand()/(double)RAND_MAX;
}

unsigned int randbit_x = 1;
unsigned int randbit_mask = 1<<16; 

void randbitseed(unsigned int x)
{
	randbit_x = x;
}

int randbit()
{
	// Linear congruential generator 
	// Source: http://en.wikipedia.org/wiki/Linear_congruential_generator
	const unsigned int A = 69069;
	const unsigned int B = 1; 
	randbit_x = A*randbit_x + B;
	return (randbit_x & randbit_mask) ? 1 : 0;
}