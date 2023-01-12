#include <cmath>
#include <iostream>
#include <vector>

#include "CUnit.h"
#include "Genetic_Algorithm.h"

bool all_Close(std::vector<double> &v1, std::vector<double> &v2, double tol = 0.1)
{
    bool is_close = true;
    int size = v1.size();
    if (size != v2.size())
    {
        return false;
    }
    for (int i = 0; i < size; i++)
    {
        if (std::abs(v1[i] - v2[i]) > tol)
        {
            is_close = false;
            break;
        }
    }
    return is_close;
}

bool test_Evaluate_Circuit1()
{
    std::vector<int> circuit_vector = {0, 4, 3, 2, 0, 5, 4, 4, 6, 2, 1};

    double p = Evaluate_Circuit(circuit_vector, false, 0, 1e-6, 300);

    return std::abs(p - 24.82) / 24.82 <= 1e-3;
}

bool test_Evaluate_Circuit2()
{
    std::vector<int> circuit_vector = {12, 7, 12, 7, 0, 7, 14, 10, 1, 10,
        3, 14, 6, 13, 5, 10, 4, 1, 11, 1,
        8, 15, 7, 1, 5, 7, 2, 0, 16, 1, 9};

    double p = Evaluate_Circuit(circuit_vector, false, 0, 1e-6, 1000);

    return std::abs(p - 408.55) / 408.55 <= 1e-3;
}

bool test_Evaluate_Circuit3()
{
    std::vector<int> circuit_vector = {18, 14, 2, 14, 16, 14, 9, 8, 19, 20,
        15, 8, 18, 18, 12, 14, 0, 4, 13, 14,
        10, 3, 11, 19, 17, 18, 21, 4, 14, 8,
        3, 20, 8, 14, 7, 5, 6, 14, 1, 8, 5};

    double p = Evaluate_Circuit(circuit_vector, false, 0, 1e-6, 1000);

    return std::abs(p - 630.77) / 630.77 <= 1e-3;
}

bool test_Evaluate_Flows1()
{
    std::vector<int> circuit_vector = {0, 1, 2, 3, 0, 0, 4};
    int n = 3;

    std::vector<double> flow_gormanium(n + 2);
    std::vector<double> flow_waste(n + 2);

    std::vector<double> flow_gormanium_expected{
        14.71, 2.94, 11.76, 0.59, 9.41};
    std::vector<double> flow_waste_expected{
        110.5, 5.52, 104.97, 0.28, 99.72};

    Evaluate_Flows(flow_gormanium, flow_waste, circuit_vector);

    return all_Close(flow_gormanium, flow_gormanium_expected) && all_Close(flow_waste, flow_waste_expected);
}

bool test_Evaluate_Flows2()
{
    std::vector<int> circuit_vector = {0, 4, 3, 2, 0, 5, 4, 4, 6, 2, 1};
    int n = 5;

    std::vector<double> flow_gormanium(n + 2);
    std::vector<double> flow_waste(n + 2);

    std::vector<double> flow_gormanium_expected{
        14.78, 5.98, 2.69, 11.83, 7.47, 0.54, 9.46};
    std::vector<double> flow_waste_expected{
        110.74, 11.30, 1.16, 105.2, 11.9, 0.06, 99.94};

    Evaluate_Flows(flow_gormanium, flow_waste, circuit_vector);

    return all_Close(flow_gormanium, flow_gormanium_expected) && all_Close(flow_waste, flow_waste_expected);
}

bool test_Generate_Initial()
{
    int population_size = 15;
    std::vector<std::vector<int>> parents;
    Generate_Initial(population_size, parents);

    // Check that the correct number of parent circuits are generated
    bool correct_number = parents.size() == population_size;

    // Check that default number of units is 10, so circuit length is 21
    bool correct_circuit_length = parents[0].size() == 21;

    return correct_circuit_length && correct_number;
}

bool test_Performance()
{
    std::vector<int> circuit_vector = {0, 4, 3, 2, 0, 5, 4, 4, 6, 2, 1};

    std::vector<std::vector<int>> parents{circuit_vector, circuit_vector, circuit_vector};

    std::vector<double> perf;
    Performance(3, parents, perf);

    // check that it returns a vector of length 3
    bool check_length = perf.size() == 3;
    bool check_performance = std::abs(perf[0] - 24.82) / 24.82 <= 0.1;

    return check_length && check_performance;
}

bool test_Fitness()
{
    std::vector<double> performance = {-30000.0, -20000.0, -10000.0, 0.0, 10000.0, 20000.0, 30000.0};
    std::vector<double> fitness;
    std::vector<double> answer = {20000.0, 30000.0, 40000.0, 50000.0, 60000.0, 70000.0, 80000.0};
    Fitness(performance.size(), performance, fitness);

    return all_Close(fitness, answer) && (performance.size() == fitness.size());
}

bool test_Probability()
{
    std::vector<double> fitness = {5, 15, 20, 25, 15, 5, 15};
    std::vector<double> probability;
    std::vector<double> answer = {0.05, 0.2, 0.4, 0.65, 0.8, 0.85, 1};
    Probability(fitness.size(), fitness, probability);
    return all_Close(probability, answer) && (probability.size() == answer.size());
}

void print_Result(bool result, std::string title)
{
    std::cout << title;
    if (result)
    {
        std::cout << ": pass\n";
    }
    else
    {
        std::cout << ": fail\n";
    }
}

int main(int argc, char *argv[])
{
    print_Result(test_Evaluate_Circuit1(), "Circuit Evaluation Test 1");
    print_Result(test_Evaluate_Circuit2(), "Circuit Evaluation Test 2");
    print_Result(test_Evaluate_Circuit3(), "Circuit Evaluation Test 3");
    print_Result(test_Evaluate_Flows1(), "Flows Evaluation Test 1");
    print_Result(test_Evaluate_Flows2(), "Flows Evaluation Test 2");
    print_Result(test_Generate_Initial(), "Generate_Initial Test");
    print_Result(test_Performance(), "Performance Test");
    print_Result(test_Fitness(), "Fitness Test");
    print_Result(test_Probability(), "Probability Test");
}
