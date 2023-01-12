// local includes
#include "utils.h"
#include <vector>
// system includes
#include <assert.h>
#include <fstream>
#include <queue>
#include <omp.h>

void utils::Print_Circuit_To_File(std::string path,
                                  const std::vector<int> schematic,
                                  const std::vector<double> gormanium,
                                  const std::vector<double> waste,
                                  const int iter,
                                  const double performance)
{
    int n = (schematic.size() - 1) / 2;
    // create the filestream and print
    std::ofstream out;
    std::string filename = path + utils::File_Sep() + "circuit_" +
                           std::to_string(n) + "_" + std::to_string(iter) + ".txt";

    out.open(filename, std::ofstream::out);
    if (out.good())
    {
        // write the contents of the vector to the file.
        // no frills.
        for (int i = 0; i < schematic.size(); i++)
        {
            out << schematic[i];
            if (i != schematic.size() - 1)
            {
                out << ", ";
            }
        }
        out << std::endl;
        // write the flows, this assume either none of both of the flow
        // data will be provided, and so will the performance
        // as all three are evaluated by the same function
        if (gormanium.size() != 0)
        {
            // gormanium flow
            for (int i = 0; i < gormanium.size(); i++)
            {
                out << gormanium[i];
                if (i != gormanium.size() - 1)
                {
                    out << ", ";
                }
            }
            out << std::endl;
            // waste flow
            for (int i = 0; i < waste.size(); i++)
            {
                out << waste[i];
                if (i != waste.size() - 1)
                {
                    out << ", ";
                }
            }
            out << std::endl;
            out << performance;
        }
    }
    out.close();
}

std::string utils::File_Sep()
{
#ifdef _WIN32
    return "\\";
#else
    return "/";
#endif
}

std::string utils::Get_Exe_Path()
{
    // recall PATH_MAX macro redefined for windows to mean MAX_PATH in utils header
#ifdef _WIN32
    TCHAR result[PATH_MAX];
    // by setting module handle to null, will retrieve filepath of current process
    GetModuleFileName(NULL, result, PATH_MAX);
    std::wstring temp(&result[0]);
    // and now safely convert to string
    std::string path(temp.begin(), temp.end());
#else
    char result[PATH_MAX];
    ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
    std::string path = std::string(result, (count > 0) ? count : 0);
#endif
    return path.substr(0, path.find_last_of(utils::File_Sep()));
}

int utils::Check_Validity(const std::vector<int> &schematic)
{
    // if we pass that test, create a vector containing all the separation units
    // and as we do so, check that each separation unit is indeed valid
    int counter = 1;
    int num_units{static_cast<int>(schematic.size() - 1) / 2};
    std::vector<SeparationUnit> units{};
    try
    {
        for (int i = 0; i < num_units; i++)
        {
            SeparationUnit unit(schematic[counter], schematic[counter + 1], i);
            units.push_back(unit);
            counter += 2;
        }
    }
    catch (SeparationUnit::BadSeparationUnit &e)
    {
        return 2;
    }

    // run BFS one starting from the root and find all nodes connected to output
    // consider it a circuit error if not all nodes were reachable from the specified
    // root node, or if the traversal algorithm hasn't detected at least two nodes
    // connected to the circuit's main output pipes.
    std::vector<int> output_nodes{};
    int level{0};
    bool truth = BFS(units, output_nodes, schematic[0], num_units, level);
    if (!truth || output_nodes.size() < 2)
    {
        return 1;
    };
    // reflect that we've traversed this circuit once, and all visited nodes will have a colour code 1.
    level++;
    // now re-run the algorithm, but this time use the output units as the starting points to make
    // sure that all outputs are forward reachable from every other node
    int success{0};
    // require units to be firstprivate such that each of the threads won't interfere with each others BFS traversal
    // all other vars except `int success` are read-only within the parallel section
#pragma omp parallel num_threads(2) default(none) firstprivate(units) shared(success, output_nodes, level, num_units)
    {
        std::vector<int> outputs{}; // we don't care about what BFS_Reverse will put in here.
        int thread_id{omp_get_thread_num()};
        int res = BFS_Reverse(units, outputs, output_nodes[thread_id], num_units, level);
        // if the output node is forward reachable from all other nodes, then return 0 (since that's the success exit code)
        int outcome = !(res == (num_units - 1));
#pragma omp critical
        // make sure that both threads get a 0 code (success code)
        success += outcome;
    }
    if (success == 0)
    {
        return 0;
    }
    else
        // then there was definitely a circuit error, so return code 1
        return 1;
}

bool utils::BFS(std::vector<SeparationUnit> &units, std::vector<int> &output_nodes, int root, int num_units, int level)
{
#ifdef _DEBUG
    if (units.size() > 0)
    {
        for (size_t i = 0; i < units.size(); i++)
        {
            assert(units[i].colour() == level);
        }
    }
#endif

    // 1. push the root on to the queue
    std::queue<SeparationUnit> q;
    q.push(units[root]);
    units[root].inc_colour();
    // we start the node counter off at 1 since the root node is
    // considered "visited".
    // at most, a BFS algorithm will visit each node once
    // (including this initial visit of the root node).
    int counter{1};
    while (!q.empty())
    {
        // take the top node off the queue
        SeparationUnit u = q.front();
        q.pop();
        // look at its conc and tails connections
        if (u.conc() >= num_units)
        {
            // we have an output node
            output_nodes.push_back(u.id());
        }
        else
        {
            // set the parents
            // note that we will get duplicates here, but we don't care
            // when we need to use this information, there will be mechanisms
            // to safely handle duplicate parent values
            units[u.conc()].parents.push_back(u.id());
        }
        if (u.conc() < num_units && units[u.conc()].colour() == level)
        {
            // add the conc output unit to the frontier queue
            q.push(units[u.conc()]);
            // note down that we've encountered a previously unvisited node
            units[u.conc()].inc_colour();
            counter++;
        }
        if (u.tails() >= num_units)
        {
            output_nodes.push_back(u.id());
        }
        else
        {
            units[u.tails()].parents.push_back(u.id());
        }
        if (u.tails() < num_units && units[u.tails()].colour() == level)
        {
            // and add the tails output unit to the frontier queue
            q.push(units[u.tails()]);
            units[u.tails()].inc_colour();
            counter++;
        }
    }
    return (counter == num_units);
}

int utils::BFS_Reverse(std::vector<SeparationUnit> &units, std::vector<int> &output_nodes, int root, int num_units, int level)
{
#ifdef _DEBUG
    if (units.size() > 0)
    {
        for (size_t i = 0; i < units.size(); i++)
        {
            assert(units[i].colour() == level);
        }
    }
#endif
    // push the root on to the queue
    std::queue<SeparationUnit> q;
    q.push(units[root]);
    units[root].inc_colour();
    // keeps track of the number of nodes visited once (at most, this is all nodes)
    // note, will always be = num_units - 1 if the circuit is connected correctly
    int count{0};
    while (!q.empty())
    {
        // take the top node off the queue
        SeparationUnit u = q.front();
        q.pop();
        // iterate through the parents and add them to the frontier if not previously visited
        for (auto &&val : u.parents)
        {
            if (units[val].colour() == level)
            {
                q.push(units[val]);
                units[val].inc_colour();
                count++;
            }
        }
    }
    return count;
}
