#ifndef _FIELD_H_
#define _FIELD_H_

#include <iostream>
#include "config.h"
#include "schema.h"

using namespace std;

struct field
{
	int N; // height
	int M; // width
	char* data; // field
	char* partition; // partition matrix
	char* data_buf; // buffer for folded chains (data)
	char* part_buf; // buffer for folded chains (partition)
	int hcyclic; // cyclic horizontal boundary
	int vcyclic; // cyclic vertical boundary
	double omega;
	int t;
	int dt;
	string output;
};

void makefield(field& F, config& C);
void clearfield(field& F);
void updatefield(field& F, schema& H, schema& V);
void printfield(field& F, ostream& f);

#endif
