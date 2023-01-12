// local includes
#include "CUnit.h"
#include "utils.h"

// system inlcudes
#include <stdio.h>
#include <assert.h>
#include <iostream>
#include <vector>
#include <set>

// create vectors of separation unit functions
std::vector<SeparationUnit> make_circuit(const std::vector<int> &schematic)
{
	int counter = 1;
	int num_units{static_cast<int>(schematic.size() - 1) / 2};
	std::vector<SeparationUnit> units{};
	// now create the unit. If any of the specified separation units are invalid,
	// let the function throw.
	for (int i = 0; i < num_units; i++)
	{
		SeparationUnit unit(schematic[counter], schematic[counter + 1], i);
		units.push_back(unit);
		counter += 2;
	}
	return units;
}

bool test_parents(const std::vector<SeparationUnit> &test, const std::vector<std::vector<int>> &ans)
{

	for (size_t i = 0; i < test.size(); i++)
	{
		if (std::set<int>(test[i].parents.begin(), test[i].parents.end()) != std::set<int>(ans[i].begin(), ans[i].end()))
		{
			return false;
		}
	}
	return true;
}

// The purpose of this executable is to perform some tests on the circuit validity checker class.
// Ideally, we should be using some unit test library. However, to avoid issues arounding installing
// any libraries on Windows, Linux and Mac (since our dev team is truly cross platform), we are just
// keeping things simple and going through some additional effort to ensure that we use asserts only.
int main(int argc, char *argv[])
{
	// bread and butter cases
	std::vector<int> bb1{0, 4, 3, 2, 0, 5, 4, 4, 6, 2, 1};			   // pass
	std::vector<int> bb2{0, 4, 3, 2, 0, 5, 4, 4, 6, 2, 3};			   // not all accessible from feed. bad circuit.
	std::vector<int> bb3{0, 2, 2};									   // both leading to same output. bad unit.
	std::vector<int> bb4{0, 1, 2, 0, 2, 1, 3};						   // one output pipe. bad circuit.
	std::vector<int> bb5{0, 4, 3, 2, 0, 5, 4, 4, 6, 2, 4};			   // self-recycle. bad unit.
	std::vector<int> bb6{0, 1, 2, 3, 5, 4, 6, 5, 1, 6, 2, 7, 1, 4, 8}; // output can't forward reach all other output. bad circuit.
	// collect them all into a central vector
	std::vector<std::vector<int>> bread_and_butter_cases{bb1, bb2, bb3, bb4, bb5, bb6};
	// the associated answers (0 - pass, 1 - bad circuit, 2 - bad unit)
	std::vector<int> bread_and_butter_slns{0, 1, 2, 1, 2, 1};

	// edge cases:
	// this is primarily to check that outputs and nodes that lead to outputs are being counted correctly
	std::vector<int> ec1{0, 1, 2}; // pass
	// pass. valid example taken from slide 29. It's interesting because there are now three nodes that lead to the output T and C
	std::vector<int> ec2{6, 16, 7, 7, 13, 11, 12, 15, 5, 3, 6, 0, 2, 14, 12, 1, 12, 14, 11, 5, 16, 11, 9, 4, 1, 0, 8, 5, 10, 2, 6};
	std::vector<std::vector<int>> edge_cases{ec1, ec2};
	std::vector<int> edge_cases_answers{0, 0};

	// test bb cases
	for (size_t i = 0; i < bread_and_butter_cases.size(); i++)
	{
		assert(bread_and_butter_slns[i] == utils::Check_Validity(bread_and_butter_cases[i]));
	}
	// test edge cases
	for (size_t i = 0; i < edge_cases.size(); i++)
	{
		assert(edge_cases_answers[i] == utils::Check_Validity(edge_cases[i]));
	}

	// now test that the parents are being correctly recognised in a single forward iteration
	std::vector<SeparationUnit> test1 = make_circuit(std::vector<int>{0, 1, 2}); // answers: 0:none
	std::vector<std::vector<int>> test1_sol{{}};

	std::vector<SeparationUnit> test2 = make_circuit(std::vector<int>{0, 1, 2, 2, 0, 3, 4}); // answers: 0:1; 1:0; 2:1,0
	std::vector<std::vector<int>> test2_sol{{1}, {0}, {0, 1}};

	std::vector<SeparationUnit> test3 = make_circuit(std::vector<int>{2, 1, 2, 0, 3, 1, 4}); // answers: 0:1; 1:0,2; 2:0
	std::vector<std::vector<int>> test3_sol{{1}, {0, 2}, {0}};

	std::vector<SeparationUnit> test4 = make_circuit(std::vector<int>{0, 1, 2, 3, 5, 4, 3, 5, 1, 7, 5, 6, 3});
	// answers: 0:none; 1:0,3; 2:0, 3:1,2,5; 4:2; 5:1,3,4
	std::vector<std::vector<int>> test4_sol{{}, {0, 3}, {0}, {1, 2, 5}, {2}, {1, 3, 4}};

	std::vector<SeparationUnit> test5 = make_circuit(std::vector<int>{0, 1, 2, 3, 5, 4, 6, 5, 1, 6, 2, 7, 3, 4, 8});
	// answers: 0:none; 1:0,3; 2:0,4; 3:1,5; 4:2,6; 5:1,3; 6:2,4
	std::vector<std::vector<int>> test5_sol{{}, {0, 3}, {0, 4}, {1, 5}, {2, 6}, {1, 3}, {2, 4}};

	std::vector<int> output_nodes{};
	// note, can't collect and loop over since that would require a nested, nested vector of ints (to represent the sols)
	// test 1
	utils::BFS(test1, output_nodes, 0, test1.size(), 0);
	assert(test_parents(test1, test1_sol));
	output_nodes.clear();
	// test 2
	utils::BFS(test2, output_nodes, 0, test2.size(), 0);
	assert(test_parents(test2, test2_sol));
	output_nodes.clear();
	// test 3
	utils::BFS(test3, output_nodes, 2, test3.size(), 0);
	assert(test_parents(test3, test3_sol));
	output_nodes.clear();
	// test 4
	utils::BFS(test4, output_nodes, 0, test4.size(), 0);
	assert(test_parents(test4, test4_sol));
	output_nodes.clear();
	// test 5
	utils::BFS(test5, output_nodes, 0, test5.size(), 0);
	assert(test_parents(test5, test5_sol));
	output_nodes.clear();

	// check the reverse BFS traversal
	// note the level value increases as we traverse the same circuit again
	int reachable = utils::BFS_Reverse(test4, output_nodes, 4, test4.size(), 1);
	assert(reachable == 2);

	reachable = utils::BFS_Reverse(test5, output_nodes, 5, test4.size(), 1);
	assert(reachable == 3);
}
