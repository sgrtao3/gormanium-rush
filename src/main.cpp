// local includes
#include "Genetic_Algorithm.h"
// system includes
#include <omp.h>

using namespace std;

int main(int argc, char *argv[])
{
    // Pre defined parameters
    int population_size = 200;           // fixed population size
    int max_iterations = 10000;          //max number of generations
    int threshold = 300;                 // threshhold to stop iteration
    int run_times = 20;                  // multiple executions
    int num_procs = omp_get_num_procs(); // get the node's total process
    double ever_best_performance = 0.0;  // Mark the best performance.

    double price_gormanium = 100.0;
    double cost_waste = 500.0;
    double flow_rate_gormanium = 10.0;
    double flow_rate_waste = 100.0;

    // Adaptive Genetic Algotirhm parameters
    vector<double> adaptive_rate;
    adaptive_rate.push_back(1.0);
    adaptive_rate.push_back(0.5);
    adaptive_rate.push_back(1.0);
    adaptive_rate.push_back(0.5);
    vector<int> ever_best_circuit;

    // Try multi times get the best result
    cout << "Multithreads started..." << endl;
#pragma omp parallel for num_threads(2 * num_procs - 1)
    for (int i = 0; i < run_times; i++)
    {
        vector<int> result = Genetic_Optimization(
            population_size,
            max_iterations,
            threshold,
            adaptive_rate,
            10,
            price_gormanium,
            cost_waste,
            flow_rate_gormanium,
            flow_rate_waste
        );
        double current_best_performance = Evaluate_Circuit(
            result,
            false,
            0,
            1e-4,
            1000,
            price_gormanium,
            cost_waste,
            flow_rate_gormanium,
            flow_rate_waste
        );
        if (ever_best_performance < current_best_performance)
        {
            ever_best_performance = current_best_performance;
            ever_best_circuit = result;
        }
#pragma omp critical
        {
            cout << "-------------------------------------------------------" << endl;
            cout << "Run time: " << i << ", the best performance is: " << current_best_performance << endl;
            for (int i = 0; i < result.size(); i++)
            {
                cout << result[i] << " ";
            }
            cout << endl;
        }
    }
    // output best performance and result
    Evaluate_Circuit(
        ever_best_circuit,
        true,
        0,
        1e-4,
        1000,
        price_gormanium,
        cost_waste,
        flow_rate_gormanium,
        flow_rate_waste
    );
    cout << "-------------------------------------------------------" << endl;
    cout << "After " << run_times << " executions, "
         << "the best performance is: " << ever_best_performance << endl;
    cout << "The best circuit is: " << endl;

    for (int i = 0; i < ever_best_circuit.size(); i++)
    {
        cout << ever_best_circuit[i] << " ";
    }
    return 0;
}