// local includes
#include "CUnit.h"
#include "utils.h"
// system includes
#include <assert.h>
#include <string>
#include <sstream>
#include <fstream>

// The purpose of these tests are to test the file writing functions.
// Having this test ensures we can easily maintain the cross platform
// nature of the file writing function.
int main(int argc, char *argv[])
{
    std::vector<int> schematic{0, 1, 2, 2, 0, 3, 4};
    std::vector<double> flows{100.0, 20, 30, 12, 81, 11, 14, 29, 88, 50};
    int n{3};
    int iter{99};
    int perf{14};

    // as this this is a test file, need to go up two levels to reach `data/`
    std::string data_dir = utils::Get_Exe_Path() + utils::File_Sep() +
                           ".." + utils::File_Sep() + ".." + utils::File_Sep() + "data";
    utils::Print_Circuit_To_File(data_dir, schematic, flows, flows, iter, perf);

    // now test that the exe was saved in the correct location and read the file
    std::string filename = data_dir + utils::File_Sep() + "circuit_" +
                           std::to_string(n) + "_" + std::to_string(iter) + ".txt";
    // string to store incoming file as vector separated by \n
    std::vector<std::string> contents{};
    // string to store each line
    std::string line{""};
    std::ifstream in;
    // open the file
    in.open(filename);
    if (in.good())
    {
        while (std::getline(in, line))
        {
            contents.push_back(line);
        };
    }
    // now check the first and last lines of the file
    for (size_t i = 0; i < schematic.size(); i++)
    {
        // use j as an easy way of accounting for the comma+space separators in the file
        int j = i * 3;
        assert(contents[0][j] == *std::to_string(schematic[i]).c_str());
    }
    assert(contents[3] == std::to_string(perf));
}