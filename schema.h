#ifndef _SCHEMA_H_
#define _SCHEMA_H_

#include "config.h"

struct schema
{
	char* buf; // all left and right side chains
	int maxlen; // length of the longest left side chain

	// left side chains with probabilities
	short* lhs; // index of the first character of the given left side chain in the buf array

	// right side chains with probabilities
	short* pos; // index of the first substition of the given rule in the rhs and prob arrays
	short* rhs; // index of the first character of the given right side chain in the buf array
	float* prob; // probabilities (accumulative)

	// sizes
	int lnum; // number of rules
	int llen; // total length of all left side chains
	int rnum; // number of all right side chains
	int rlen; // total length of all right side chains
};

void makeschema(schema& M, config& C, char* name);
void clearschema(schema& M);
int findrule(schema& M, char* a, int n);
int applyrule(schema& M, char* a, int n);

#endif