#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <vector>
#include <string>

using namespace std;

struct record
{
	string key;
	vector<string> values;
};

struct section
{
	string name;
	vector<record> records;
};

class config
{
	vector<section> data;
	record dummy;
public:
	string filename;
	void load(char* fname);
	record& getrec(string sname, string rname);

	int size() { return data.size(); }
	int secsize(int i) { return data[i].records.size(); }
	int recsize(int i, int j) { return data[i].records[j].values.size(); }
	string name(int i) { return data[i].name; }
	string key(int i, int j) { return data[i].records[j].key; }
	string value(int i, int j, int k) { return data[i].records[j].values[k]; }
	int findsec(string sname);
	int findrec(int s, string rname);
};

extern float tofloat(string);
extern double todouble(string);
extern int toint(string);
extern double frand();

#endif