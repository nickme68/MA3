// ma3.cpp : Defines the entry point for the console application.
//

#include <iostream>
#include <fstream>
#include "config.h"
#include "schema.h"
#include "field.h"

using namespace std;

int main(int argc, char** argv)
{
	config C;
	C.load(argv[1]);				// Load config data

	string prompt = "python makeconfig.py " + C.filename;
	system(prompt.c_str());

	config CC;
	CC.load("_input_.cfg");

	schema V;
	makeschema(V, CC, "vertical");	// Extract from config data schema with name "vertical"

	schema H;
	makeschema(H, CC, "horizontal"); // Extract from config data schema with name "horizontal"
	
	field F;
	makefield(F,C);

	ofstream fout(F.output);

	fout << F.N << " " << F.M << endl;

	//getchar();

	printfield(F, fout);

	for( int t = 0; t<F.t*F.dt; t++ )
	{
		updatefield(F, H, V);
		if( (t+1) % F.dt == 0 )
		{
			printfield(F, fout);
			cerr << "*";
		}
	}

	fout.close();

	clearschema(H);
	clearschema(V);
	clearfield(F);

	//system("python test.py");

	return 0;
}

