#include <iostream>

#include "utilities.h"


int main() 
{
    std::cout << "\nSTART..." << std::endl;

    // read in the .inp files
    std::string outdir_bot = "./input";
    std::string jobname_bot = "hemisphere_input_gref_lev0_lgref_lev0_Bot";
    std::map <int, vector <double> > NODES_bot;
    unordered_map <int, vector <int> > IEN_bot;
    ReadABAQUSInputs(outdir_bot, jobname_bot, NODES_bot, IEN_bot);

    // shift the element index of the bottom layer IEN because the 
    // CMU does not start at zero (rather at N = num elements in plane)
    int ele_index;
    int new_index;
    const int nelem = IEN_bot.size();
    unordered_map <int, vector <int> > IEN_newbot;
    for (auto iter = IEN_bot.begin(); iter != IEN_bot.end(); iter++)
    {
        ele_index = iter->first;
        new_index = ele_index - nelem;
        IEN_newbot[new_index] = iter->second;
    }   

    std::string outdir_top = "./input";
    std::string jobname_top = "hemisphere_input_gref_lev0_lgref_lev0_Top";
    std::map <int, vector <double> > NODES_top;
    unordered_map <int, vector <int> > IEN_top;
    ReadABAQUSInputs(outdir_top, jobname_top, NODES_top, IEN_top);

    // read in the .tsplines files
    std::string filename_bot = "./input/hemisphere_input_gref_lev0_lgref_lev0_Bot.tsplines";
    std::string filename_top = "./input/hemisphere_input_gref_lev0_lgref_lev0_Top.tsplines";
    std::unordered_map<int, vector<int>> IEN_tbot; 
    std::unordered_map<int,vector<vector<double>>> BEXT_bot;
    std::unordered_map<int, vector<int>> IEN_ttop; 
    std::unordered_map<int,vector<vector<double>>> BEXT_top;

    std::vector<double> t;
    int nlayers;
    std::vector<double> fiber_angle;
    int nsections;
    std::vector<int> section_limits;
    std::vector<double> knotW;
    std::vector<double> elRangeW; 
    std::vector<double> coords;
    std::vector<double> weight; 
    const int nsolids = 0;

    ReadTSplineData(
        filename_top.c_str(), t, nlayers, fiber_angle, 
        nsections, section_limits, 
        knotW, elRangeW, 
        coords, weight, 
        IEN_ttop, BEXT_top,
        nsolids, 0);

    const int NL = 4;
    const int poly3 = 2;

    // write the .inp and .tsplines output files
    std::unordered_map <int, vector <double> > NODES;
    std::unordered_map <int, user_element > MESH;
    generateThruThicknessData(NL, poly3,
        NODES_bot, IEN_newbot,
        NODES_top, IEN_top, BEXT_top,
        NODES, MESH);

    std::cout << "\n...END\n" << std::endl;
}
