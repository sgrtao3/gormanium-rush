/*
ACSE-4 Group 4.2 - Galena
First Created: 2021-03-23

Imperial College London
Department of Earth Science and Engineering

Group members:
    Iñigo Basterretxea Jacob
    Gordon Cheung
    Nina Kahr
    Miguel Pereira
    Ranran Tao
    Suyan Shi
    Jihao Xin
    Jie Zhu
*/

#ifndef __UTILS__
#define __UTILS__
/*
ACSE-4 Group 4.2 - Galena
First Created: 2021-03-24
References:
The BFS and check_vallidity functions were implented based on the algorithm outlined
in: I. Baidari and A. Hanagawadimath, "Traversing directed cyclic and acyclic graphs
using modified BFS algorithm," 2014 Science and Information Conference, London, UK,
2014, pp. 175-181, doi: 10.1109/SAI.2014.6918187.
*/

// local includes
#include "CUnit.h"

// system includes
#include <string>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#define PATH_MAX MAX_PATH
#else
#include <limits.h>
#include <unistd.h>
#endif

namespace utils
{

    /* 
    Write the circuit into a data file, follwing the
    data structure and naming convention specified in `root/data/Note`

    @param path: std::string, path of the folder the data file should be stored in
    @param schematic: std::vector<int>, circuit to be exported
    @param gormanium: std::vector<double> (optional), flow volumes of gormanium into each cell,
                        size num_units + 2, with the last two values representing
                        the concentrate output and tailings output of the whole circuit,
                        default to an empty vector
    @param waste: std::vector<double> (optional), flow volumes of waste into each cell,
                        size num_units + 2, with the last two values representing
                        the concentrate output and tailings output of the whole circuit,
                        default to an empty vector
    @param iter: int (optional), the current generation, default to -1
    @param performance: double (optional), the performance of the circuit, default to -1e9
    */
    void Print_Circuit_To_File(std::string path,
                               const std::vector<int> schematic,
                               const std::vector<double> gormanium = {},
                               const std::vector<double> waste = {},
                               const int iter = 0,
                               const double performance = -1e9);

    /*
    Helper function for handling the different file separators in different systems
    
    @return path: std::string, "/" for unix systems, "\\" for windows
    */
    std::string File_Sep();

    /*
    Get the path of the executable

    @return path: std::string, absolute path of the executable
    */
    std::string Get_Exe_Path();

    /*
    Algorithm which determines whether or not the given input schematic is valid.
    
    @param schematic: std::vector<int>, the schematic specified in the coded vector form.
    
    @return status: int, an int that codes for the outcome. The possible outputs are as follows:
        0 - success, it's a valid circuit.
        1 - invalid, there's an error in the actual circuit design, meaning that either not all separation
            units are accesible from the feed, or every unit is not forward connected to both outputs
            (which leads to lack of convergence at worst and at best, means some of your units are redundant).
        2 - invalid, there's an error in one or more of the separation units, meaning that at least one unit
            is trying to do a self-recycle or both its C and T outputs are connected to the same unit.
    */
    int Check_Validity(const std::vector<int> &schematic);

    /*
    Conducts single BFS search of a circuit defined as a vector of SeparationUnits.
    Do not call this directly unless you know what you're doing.
    
    @param units: std::vector<SeparationUnit>, The circuit specified as a vector of Separation Units.
    @param output_nodes: std::vector<int>, Blank vector (size = 0) that will be populated with the 
                        set of nodes connected to output pipes.
    @param root: int, The node that the breadth first search will start at.
    @param num_units: int, Total number of separation units in the circuit, aka 'nodes in the graph'.
    @param level: int, The code on each separation unit which is considered to mean 'unvisited'.
    
    @return status: bool, true if all nodes were visited
    */
    bool BFS(std::vector<SeparationUnit> &units, std::vector<int> &output_nodes, int root, int num_units, int level);

    /*
    Conducts single BFS search of a circuit defined as a vector of SeparationUnits inthe reverse direction
    Do not call this directly unless you know what you're doing.
    
    @param units: std::vector<SeparationUnit>, The circuit specified as a vector of Separation Units.
    @param output_nodes: std::vector<int>, Blank vector (size = 0) that will be populated with the 
                        set of nodes connected to output pipes.
    @param root: int, The node that the breadth first search will start at.
    @param num_units: int, Total number of separation units in the circuit, aka 'nodes in the graph'.
    @param level: int, The code on each separation unit which is considered to mean 'unvisited'.
    
    @return status: bool, true if all nodes were visited backwards
    */
    int BFS_Reverse(std::vector<SeparationUnit> &units, std::vector<int> &output_nodes, int root, int num_units, int level);
}

#endif // !__UTILS__
