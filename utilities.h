#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <string.h>
#include <algorithm>

#include <vector>
#include <map>
#include <unordered_map>
#include <unordered_set>

using namespace std;

#define LINUX
#define DIM 3
#define PROPS 9
#define SVARS 216
#define LOG(x) std::cout << x << std::endl;

struct user_element
{
    int unodes;         // NODES=unodes
    int utype;          // TYPE=U'utype'
    int uproperties;    // PROPERTIES=uproperties
    int ucoordinates;   // COORDINATES=ucoordinates
    int uvariables;     // VARIABLES=uvariables
    // unordered_map <int, vector <int> > uIEN;
};

struct vec3
{
	double x, y, z;
};


template <typename ValueType>
int ReadLine(string tmp, vector <ValueType>& row);

int no(char* param);

char* StringToUpper(char* str);

int MatchKeyword(const char* str,const char* key2);

int MatchString(const char* str,const char* key2);

void DivideTerms(string tmp, vector<string>* Terms);

inline void setVec(const std::vector<double> &inVec, vec3 &outVec);

inline void interpolateNode(
    const double &t,    // value between (0, 1)
    const vec3 &A,      // bottom node
    const vec3 &B,      // top node
    vec3 &C);            // interpolated node

int ReadABAQUSInputs(
	string OUTDIR, 
	string JOBNAME, 
	map <int, vector <double> > &Nodes,
	unordered_map <int, vector <int> > &U_Elements);

void ReadTSplineData(
	const char* filename, vector<double>& t, 
	int& nlayers, vector<double>& fiber_angle, 
	int& nsections, vector<int>& section_limits, 
	vector<double>& knotW, vector<double>& elRangeW, 
	vector<double>& coords, vector<double>& weight, 
	unordered_map<int, vector<int>>& IEN, unordered_map<int,vector<vector<double>>>& BEXT,
	const int& nsolids, const int& poly3);

void generateThruThicknessData(
	const int &NL, 
	const int &poly3,
    const std::map <int, vector <double> > &NODES_bot, 
    const std::unordered_map <int, vector <int> > &IEN_newbot,
    const std::map <int, vector <double> > &NODES_top, 
    const std::unordered_map <int, vector <int> > &IEN_top,
    const std::unordered_map <int, vector<vector<double>> > BEXT,
    std::unordered_map <int, vector <double> > &NODES, 
    std::unordered_map <int, user_element> &MESH);
