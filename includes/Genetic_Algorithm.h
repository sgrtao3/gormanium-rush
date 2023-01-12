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

#ifndef Genetic_Algorithm
#define Genetic_Algorithm
#include <vector>
#include <cmath>
#include <algorithm>
#include <numeric>
#include <time.h>
#include <iostream>
#include <random>
#include <chrono>
#include "utils.h"
#include "CUnit.h"
#endif

/*
This function calculates the mass flow rates in the circuit. We make use
of the successive substitution algorithm, where we feed a steady mass flow
rate of gormanium and waste into the system through the first unit at each
iteration. The mass flow rates feeding into each unit get updated with
every iteration according to the circuit connectivity, whilst ensuring mass
continuity.

Its "raison d'etre" is to output the mass flow rates independently from the
performance calculation, if the user decides so.

@param new_feed_gormanium: std::vector<double>, gormanium mass flow rates (kg/s),
                            passed by reference
@param new_feed_waste: std::vector<double>, feed mass flow rates (kg/s),
                            passed by reference
@param circuit_vector: std::vector<int>, gene of the circuit
@param tolerance: double (optional), double (optional), maximum relative error
                    allowed for convergence, default to 1e-4
@param max_iterations: int (optional), number of iterations of the algorithm within which
                        converge is expected. Terminate the algorithm if convergence
                        is not met after this iteration, default to 1000
@param gormanium_price: double (optional), price of gormanium in the concentrate [GBP/kg],
                        default to £100/kg
@param waste_cost: double (optional), cost of waste disposal in the concentrate [GBP/kg],
                        default to £500/kg
@param input_gormanium: double (optional), mass flow rate of gormanium fed into circuit [kg/s],
                        default to 10kg/s
@param input_waste: double (optional), mass flow rate of waste fed into circuit [kg/s],
                        default to 100kg/s
*/
void Evaluate_Flows(
    std::vector<double> &new_feed_gormanium,
    std::vector<double> &new_feed_waste,
    const std::vector<int> &circuit_vector,
    double tolerance = 1e-4,
    int max_iterations = 1000,
    double gormanium_price = 100.0,
    double waste_cost = 500.0,
    double input_gormanium = 10.0,
    double input_waste = 100.0);

/*
This function calculates the performance of the circuit as the difference
in income derived from the sale of gormanium and the charge derived from
the waste disposal in the concentrate.

If the algorithm does not converge (i.e. the mass flow rates keep
changing), the function returns a negative value for the performance
equal to the waste flow rate to the feed times the cost of waste
disposal in the concentrate.

@param circuit_vector: std::vector<int>, integer vector representing the circuit
                        of size 2*No.Units+1
@param write_to_file: bool (optional), whether to store the circuit in evaluation into
                        a data file, default to false
@param current_it: int (optional), current generation, default to 0
@param tolerance: double (optional), maximum relative error allowed for convergence,
                    default to 1e-4
@param max_iterations: int (optional), number of iterations of the algorithm within which
                        convergence is expected. Terminate the algorithm if convergence
                        is not met pass this iteration, default to 1000
@param gormanium_price: double (optional), price of gormanium in the concentrate [GBP/kg],
                        default to £100/kg
@param waste_cost: double (optional), cost of waste disposal in the concentrate [GBP/kg],
                        default to £500/kg
@param input_gormanium: double (optional), mass flow rate of gormanium fed into circuit [kg/s],
                        default to 10kg/s
@param input_waste: double (optional), mass flow rate of waste feed into circuit [kg/s],
                        default to 100kg/s

@return performance: double, earnings from the gormanium in output -
                            cost to dispose waste in output
*/
double Evaluate_Circuit(
    const std::vector<int> &circuit_vector,
    bool write_to_file = false,
    int current_it = 0,
    double tolerance = 1e-4,
    int max_iterations = 1000,
    double gormanium_price = 100.0,
    double waste_cost = 500.0,
    double input_gormanium = 10.0,
    double input_waste = 100.0);

/*
Generate the initial population to start the Genetic Algorithm.

We set the population size as a population_size and randomly create
circuit vectors. If the circuit vectors pass the validity check,
we put them into the initial population.

@param population_size: int, the number of genes in each generation
@param parents: vector<vector<int>>, vector of vectors to store the initial spopulation
@param num_units: int, number of units in a circuit
*/
void Generate_Initial(int population_size, std::vector<std::vector<int>> &parents, int num_units = 10);

/*
This function calculates the performance vector for all the circuits.

We loop over every circuit and make use of the Evaluate_Circuit function written before
to get each individual performance. Then put all the performances together into a vector.

@param population_size: int, the size of population
@param population: vector<vector<int>>, all genes in the current generation
@param performance: vector<double>, vector to store all the performances values
                    of the current generation
@param flow_rate_gormanium: double, kg/s gormanium flowing into the circuit
@param flow_rate_waste: double, kg/s wasteflowing into the circuit
@param price_gormanium: double, £/kg of gormanium in the concentrate
@param cost_waste: double, £/kg of waste in the concentrate
*/
void Performance(
    int population_size,
    const std::vector<std::vector<int>> &population,
    std::vector<double> &performance,
    double flow_rate_gormanium = 10.0,
    double flow_rate_waste = 100.0,
    double price_gormanium = 100.0,
    double cost_waste = 500.0
);

/*
This function creates a fitness vector for a generation using the performance value

We loop every gene and apply a fitness function:
Fit(i) = performance(i) +c,
where c is our estimated maximum, here is 50000.

@param population_size: int, the size of population
@param performance: vector<double>, performance of all genes in the current generation
@param fitness: vector<double>, vector to store fitness of all genes in the current generation
*/
void Fitness(int population_size, std::vector<double> &performance, std::vector<double> &fitness);

/*
This function creates a probability vector for a generation.

We first calculate the sum of performance of every circuit. Then use
the roulette method: probability of each circuit is calculated as a ratio of
individual performance to the sum performance.

@param population_size: int, the size of population
@param performance: vector<double>, performances of every circuit in the current generation
@param probability: vector<double>, vector to record the probabilities of all circuits
                    in this generation
*/
void Probability(int population_size, const std::vector<double> &fitness, std::vector<double> &probability);

/*
This function finds a highest value in a vector of doubles.

@param vec: vector<double>, vector of doubles (such as performance or fitness)

@return max_value: double, maximum value in the vector
*/
double Find_Best_Value(const std::vector<double> &vec);

/*
Compare the fitness value of two genes

@param father_index: int, index of 1st gene to be compared
@param mother_index: int, index of 2nd gene to be compared
@param fitness: std::vector<double>, vector of the fitness values of the current generation

@return: fitness: double, the larger fitness value of the two genes
*/
double Find_Better_Fitness(int father_index, int mother_index, std::vector<double> &fitness);

/*
Find the average fitness in the current generation

@param fitness: std::vector<double>, vector of the fitness values of the current generation

@return avg: double, the average fitness value of the current generation
*/
double Find_Avg_Fitness(std::vector<double> &fitness);

/*
Obtain the fitness value of a circuit using the performance,
fitness is defined by performance + 50000

@param circuit_vector: std::vector<int>, gene of the circuit
@param flow_rate_gormanium: double, kg/s gormanium flowing into the circuit
@param flow_rate_waste: double, kg/s wasteflowing into the circuit
@param price_gormanium: double, £/kg of gormanium in the concentrate
@param cost_waste: double, £/kg of waste in the concentrate

@return f_self: double, fitness of the circuit
*/
double Calculate_Self_Fitness(
    std::vector<int> &circuit_vector,
    double flow_rate_gormanium = 10.0,
    double flow_rate_waste = 100.0,
    double price_gormanium = 100.0,
    double cost_waste = 500.0
);

/*
This function finds a best circuit_vector from the current generation.

We get the position of the circuit with maximum performance value after
Find_Best_Value function. Then use that position to find out its
corresponding circuit.

@param parents: vector<vector<int>>, circuit population in the current generation
@param performance: vector<double>, performances of each circuit
@param max_performance: double, the maximum performance in the current generation

@return parents[index]: vector<int>, the best circuit from the current generation
*/
std::vector<int> Find_Best_Gen(const std::vector<std::vector<int>> &parents, const std::vector<double> &performance, double max_performance);

/*
This function randomly selects a pair of parents for crossover.

We look into the probability vector. Select a random
number first. Then use the binary search method to
get the position of that number.

@param probability: vector<double>, chosen probabilities of each gene
                    in the current population

@return mid: int, index of the chosen gene
*/
int Choose_Cross(const std::vector<double> &probability);

/*
Mutation function for selected gene.

Randomly select a number. If this number is smaller
than the mutation rate, do the mutation.

@param f_self: double,
@param f_max: double,
@param f_avg: double,
@param f: double,
@param son: vector<int>, the gene to be mutated
@param num_units: int, number of units in a circuit
*/
void Mutation(double f_self, double f_max, double f_avg, double f, std::vector<double> &adaptive_rate, std::vector<int> &gene, int num_units);

/*
Cross over function for selected parent genes.

Randomly select a number, if it is larger than the crossover rate,
keep the old generation. Otherwise, do the crossover and insert
mutations during the crossover process.

@param crossover_rate: double, the probability of doing crossover
@param father: vector<int>, parent gene 1
@param mother: vector<int>, parent gene 2
@param num_units: int, number of units in a circuit
*/
void Crossover(
    double f_max,
    double f_avg,
    double f, std::vector<double> &adaptive_rate,
    std::vector<int> &father,
    std::vector<int> &mother,
    int num_units
);

/*
Solver function of the Genetic Algorithm.
We get the performance, probability, best performance and best generations from
previous work. Define a subpopulation and circulate it when randomly generate
parent pairs until the subpopulation is filled. Temporarily transform the parents
into the next generation. Then check the validity of the next generation. Finally get
the best results for each reproduction.

@param population_size: int, the size of each generation in Genetic Algorithm
@param max_iterations: int, the max iterations in Genetic Algorithm, after this number of iterations,
                        optimize search will stop
@param threshold: int, if the performance of the best vector has not changed for this number
                    of iteration, the algorithm terminates

@return best_circuit: vector<int>, the best result within the iterations
@param num_units: int, number of units in a circuit
@param flow_rate_gormanium: double, input flow rate of gormanium
@param flow_rate_gormanium: double, kg/s gormanium flowing into the circuit
@param flow_rate_waste: double, kg/s wasteflowing into the circuit
@param price_gormanium: double, £/kg of gormanium in the concentrate
@param cost_waste: double, £/kg of waste in the concentrate
*/
std::vector<int> Genetic_Optimization(
    int population_size,
    int max_iterations,
    int threshold,
    std::vector<double> &adaptive_rate,
    int num_units = 10,
    double flow_rate_gormanium = 10.0,
    double flow_rate_waste = 100.0,
    double price_gormanium = 100.0,
    double cost_waste = 500.0
);
