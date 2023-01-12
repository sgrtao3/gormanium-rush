#include "Genetic_Algorithm.h"
#include "utils.h"

using namespace std;

/* -------------- Circuit Modeling Part----------------*/
void Evaluate_Flows(
    vector<double> &new_feed_gormanium,
    vector<double> &new_feed_waste,
    const vector<int> &circuit_vector,
    double tolerance,
    int max_iterations,
    double gormanium_price,
    double waste_cost,
    double input_gormanium,
    double input_waste)
{
    int n = (circuit_vector.size() - 1) / 2;
    vector<double> feed_waste(n + 2, 0.0);
    vector<double> feed_gormanium(n + 2, 0.0);

    // Fractions going to concentrate
    double fraction_gormanium = 0.2;
    double fraction_waste = 0.05;

    // This will later be used to check for mass continuity
    double total_mass = 0.0;

    // set initial feed rate - entry unit index is given by circuit_vector[0]
    feed_gormanium[circuit_vector[0]] = input_gormanium;
    feed_waste[circuit_vector[0]] = input_waste;

    // Initialise relative error variables and iteration counter
    double gormanium_error;
    double waste_error;
    int it = 0;

    while (it < max_iterations)
    {
        // New feed vector should be set to 0 at start of every iteration
        std::fill(new_feed_gormanium.begin(), new_feed_gormanium.end(), 0.0);
        std::fill(new_feed_waste.begin(), new_feed_waste.end(), 0.0);

        // Update gormanium and waste feeds based on current feeds into each unit
        for (int i = 0; i < n; i++)
        {
            // gormanium to concentrate
            new_feed_gormanium[circuit_vector[i * 2 + 1]] += feed_gormanium[i] * fraction_gormanium;
            // waste to concentrate
            new_feed_waste[circuit_vector[i * 2 + 1]] += feed_waste[i] * fraction_waste;

            // gormanium to tailings
            new_feed_gormanium[circuit_vector[i * 2 + 2]] += feed_gormanium[i] * (1 - fraction_gormanium);
            // waste to tailings
            new_feed_waste[circuit_vector[i * 2 + 2]] += feed_waste[i] * (1 - fraction_waste);
        }

        // this is true if any difference between previous and current waste or
        // gormanium feed arrays exceeds our given tolerance
        bool exceeds_tolerance = false;

        // Feed mass into the overall circuit
        new_feed_gormanium[circuit_vector[0]] += input_gormanium;
        new_feed_waste[circuit_vector[0]] += input_waste;

        for (int i = 0; i < n; i++)
        {
            // Calculate relative errors in gormanium and waste feed vectors with respect to previous iteration
            gormanium_error = std::abs(new_feed_gormanium[i] - feed_gormanium[i]) / feed_gormanium[i];
            waste_error = std::abs(new_feed_waste[i] - feed_waste[i]) / feed_waste[i];

            // As soon as any value exceeds the tolerance, proceed to the next iteration in the circuit
            if (gormanium_error > tolerance || waste_error > tolerance)
            {
                exceeds_tolerance = true;
                break;
            }
        }

        // this means we've reached steady state and can calculate the performance
        if (exceeds_tolerance == false)
        {
            break;
        }

        // Update feed vectors for next iteration
        for (int i = 0; i < n + 2; i++)
        {
            feed_gormanium[i] = new_feed_gormanium[i];
            feed_waste[i] = new_feed_waste[i];
        }

        // Store destination tailing and concentrate
        total_mass += new_feed_gormanium[n] + new_feed_gormanium[n + 1] + new_feed_waste[n] + new_feed_waste[n + 1];

        // Take destination mass out of the circuit (i.e. set to 0)
        feed_gormanium[n] = 0.0;
        feed_gormanium[n + 1] = 0.0;
        feed_waste[n] = 0.0;
        feed_waste[n + 1] = 0.0;

        // increment iteration count
        it++;
    }
    // check for mass continuity
    for (int i = 0; i < n; i++)
    {
        total_mass += new_feed_gormanium[i];
        total_mass += new_feed_waste[i];
    }

    // Print message and return negative performance if the algorithm does not converge
    if (it == max_iterations)
    {
        throw 1;
    }

    // Total mass in the circuit should be equal to the mass fed into it
    double sum_check = (it + 1) * (input_gormanium + input_waste);

    if (std::abs(total_mass - sum_check) / sum_check > 1e-4)
        throw 2;
}

double Evaluate_Circuit(
    const vector<int> &circuit_vector,
    bool write_to_file,
    int current_it,
    double tolerance,
    int max_iterations,
    double gormanium_price,
    double waste_cost,
    double input_gormanium,
    double input_waste)
{
    // Initialise vectors of gormanium and waste feeds into units. We use n+2
    // to account for the destinations of the final concentrate and tailings

    int n = (circuit_vector.size() - 1) / 2;
    vector<double> new_feed_waste(n + 2);
    vector<double> new_feed_gormanium(n + 2);

    try
    {
        Evaluate_Flows(
            new_feed_gormanium,
            new_feed_waste,
            circuit_vector,
            tolerance,
            max_iterations,
            gormanium_price,
            waste_cost,
            input_gormanium,
            input_waste);
    }
    catch (const int error_code)
    {
        if (error_code == 1)
        {

            // This means the algorithm didn't converge
            return -input_waste * waste_cost;
        }
        else if (error_code == 2)
        {
            throw "Mass continuity FAILED!";
        }
        else
        {
            throw "Something went wrong";
        }
    }

    // Calculate performance as difference of gormanium income and waste
    // charge from the concentrate
    double performance = new_feed_gormanium[n] * gormanium_price - new_feed_waste[n] * waste_cost;

    if (write_to_file == true)
    {

        // as this this should only be used in the main executable,
        // only need to go up one level to reach `data/`
        std::string data_dir = utils::Get_Exe_Path() + utils::File_Sep() +
            ".." + utils::File_Sep() + "data";
        utils::Print_Circuit_To_File(data_dir, circuit_vector,
            new_feed_gormanium, new_feed_waste,
            current_it, performance);
    }

    return performance;
}

/* -------------- Genetic Algorithm Part----------------*/

// random initial function
void Generate_Initial(int population_size, vector<vector<int>> &parents, int num_units)
{
    parents.reserve(population_size * (2 * num_units + 1));
    while (parents.size() < population_size)
    {
        vector<int> circuit_vector;
        circuit_vector.reserve(2 * num_units + 1);
        circuit_vector.push_back(0);
        for (int j = 0; j < num_units * 2; j++)
        {
            if (j < num_units + 1)
            {
                circuit_vector.push_back(j + 1);
            }
            else
            {
                circuit_vector.push_back(rand() % (num_units + 2));
            }
        }
        unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
        shuffle(circuit_vector.begin() + 1, circuit_vector.end(), std::default_random_engine(seed));
        if (utils::Check_Validity(circuit_vector) == 0)
        {
            parents.push_back(circuit_vector);
        }
    }
    return;
}

void Performance(
    int population_size,
    const vector<vector<int>> &parents,
    vector<double> &performance,
    double flow_rate_gormanium,
    double flow_rate_waste,
    double price_gormanium,
    double cost_waste
)
{
    for (int i = 0; i < parents.size(); i++)
    {
        vector<int> temp = parents[i];
        double r = Evaluate_Circuit(
            temp,
            false,
            0,
            1e-4,
            1000,
            price_gormanium,
            cost_waste,
            flow_rate_gormanium,
            flow_rate_waste
        );
        performance.push_back(r);
    }
    return;
}

void Fitness(int population_size, vector<double> &performance, vector<double> &fitness)
{
    for (int i = 0; i < population_size; i++)
    {
        fitness.push_back(performance[i] + 50000);
    }

    return;
}

void Probability(int population_size, const vector<double> &fitness, vector<double> &probability)
{
    double sum = accumulate(fitness.begin(), fitness.end(), 0);
    double pro = 0.0;

    // Roulette method
    for (int i = 0; i < population_size; i++)
    {
        pro += ((fitness[i]) / sum);
        probability.push_back(pro);
    }

    return;
}

double Find_Best_Value(const vector<double> &vec)
{
    double max_value = *max_element(vec.begin(), vec.end());

    return max_value;
}

double Find_Better_Fitness(int father_index, int mother_index, vector<double> &fitness)
{
    if (fitness[father_index] > fitness[mother_index])
    {
        return fitness[father_index];
    }
    else
    {
        return fitness[mother_index];
    }
}

double Find_Avg_Fitness(vector<double> &fitness)
{
    double sum = accumulate(begin(fitness), end(fitness), 0.0);
    double avg = sum / fitness.size();

    return avg;
}

double Calculate_Self_Fitness(
    vector<int> &circuit_vector,
    double flow_rate_gormanium,
    double flow_rate_waste,
    double price_gormanium,
    double cost_waste)
{
    double r = Evaluate_Circuit(
        circuit_vector,
        false,
        0,
        1e-4,
        1000,
        price_gormanium,
        cost_waste,
        flow_rate_gormanium,
        flow_rate_waste
    );
    double f_self = r + 50000;

    return f_self;
}

vector<int> Find_Best_Gen(const vector<vector<int>> &parents, const vector<double> &performance, double max_performance)
{
    int max_index = max_element(performance.begin(), performance.end()) - performance.begin();

    return parents[max_index];
}

int Choose_Cross(const vector<double> &probability)
{
    double num = (rand() % 1000) * 0.001 + 0.001;
    if (num < probability[0])
    {
        return 0;
    }
    // Binary search
    int low = 1;
    int high = probability.size();
    int mid = (low + high) / 2;

    while (low < high)
    {
        if (num > probability[mid])
        {
            low = mid;
            mid = (high + low) / 2;
        }
        else if (num <= probability[mid - 1])
        {
            high = mid;
            mid = (high + low) / 2;
        }
        else
        {
            return mid;
        }
    }
    //return high;
    return mid;
}

void Mutation(double f_self, double f_max, double f_avg, double f, vector<double> &adaptive_rate, vector<int> &gene, int num_units)
{
    double k2 = adaptive_rate[1];
    double k4 = adaptive_rate[3];
    double pm;

    if (f >= f_avg)
    {
        pm = k2 * ((f_max - f_self) / (f_max - f_avg));
    }
    else
    {
        pm = k4;
    }

    for (int i = 1; i < gene.size(); i++)
    {
        double num = (rand() % 10000) * 0.0001;
        if (num < pm)
        {
            // Pay attention to the step size here, it may need to be adjusted
            gene[i] = (gene[i] + rand() % (num_units + 2)) % (num_units + 2);
        }
    }
    return;
}

void Crossover(double f_max, double f_avg, double f, vector<double> &adaptive_rate, vector<int> &father, vector<int> &mother, int num_units)
{
    double k1 = adaptive_rate[0];
    double k3 = adaptive_rate[2];
    double num = (rand() % 10000) * 0.0001;
    double pc;
    if (f >= f_avg)
    {
        pc = k1 * ((f_max - f) / (f_max - f_avg));
    }
    else
    {
        pc = k3;
    }

    if (num > pc)
    {
        return;
    }

    int point = rand() % (2 * num_units + 1) + 1;
    for (int i = 1; i < point; i++)
    {
        swap(father[i], mother[i]);
    }
    return;
}

vector<int> Genetic_Optimization(
    int population_size,
    int max_iterations,
    int threshold,
    vector<double> &adaptive_rate,
    int num_units,
    double flow_rate_gormanium,
    double flow_rate_waste,
    double price_gormanium,
    double cost_waste
)
{
    //Step 0. Define parameters
    int count_for_threshold = 1;                                 // This is a counter, if the peformance doesn't rise this generation, it will increase by 1;
    int prematurity_iterations = min(300, max_iterations / 100); // The generation before which we won't directly pass the best performance to;
    vector<vector<int>> parents;                                 // The 2D vector to store parents gene
    vector<vector<int>> children;                                // The 2D vector to store children gene
    vector<double> performance;                                  // vector for performance
    vector<double> fitness;                                      // vector for fitness
    vector<double> probability;                                  // vector for performance
    vector<int> best_circuit;                                    // vector to store vest solution vestperformance
    double current_best_performance = 0;                         // Current best performance, update every iteration
    double old_best_performance = 0;                             // Last time's best performation, update current best is larger than it
    srand((unsigned)time(NULL));

    // Step 1. Initial parents
    Generate_Initial(population_size, parents, num_units);

    // start iteration
    for (int i = 0; i < max_iterations; i++)
    {
        performance.clear();
        fitness.clear();
        probability.clear();
        best_circuit.clear();
        // Step 2. Calculate Fitness Value as probability
        Performance(
            population_size,
            parents,
            performance,
            flow_rate_gormanium,
            flow_rate_waste,
            price_gormanium,
            cost_waste
        );
        Fitness(population_size, performance, fitness);
        Probability(population_size, fitness, probability);
        double f_avg = Find_Avg_Fitness(fitness);
        double f_max = Find_Best_Value(fitness);
        double f_self;

        // Step3. Take best vector directly to children
        current_best_performance = Find_Best_Value(performance);
        best_circuit = Find_Best_Gen(parents, performance, current_best_performance);
        // To prevent Prematurity, we won't directly take best into next generation in the begining
        if (i >= prematurity_iterations)
        {
            children.push_back(best_circuit);
        }
        children.push_back(best_circuit);

        // Generate next generation with the same size
        while (children.size() < population_size)
        {
            // Step 4. Select a pair of the parents
            int father_index = Choose_Cross(probability);
            int mother_index = Choose_Cross(probability);
            // Prevent father and mother are the same.
            if (father_index == mother_index)
            {
                continue;
            }
            vector<int> father(parents[father_index]);
            vector<int> mother(parents[mother_index]);
            // Step 5. Randomly crossover.
            double f = Find_Better_Fitness(father_index, mother_index, fitness);
            Crossover(f_max, f_avg, f, adaptive_rate, father, mother, num_units);
            vector<int> &child_1 = father;
            vector<int> &child_2 = mother;
            // Step 6. Go over each of the numbers in both two vectors and decide whether to mutate them
            f_self = Calculate_Self_Fitness(
                child_1,
                flow_rate_gormanium,
                flow_rate_waste,
                price_gormanium,
                cost_waste
            );
            Mutation(f_self, f_max, f_avg, f, adaptive_rate, child_1, num_units);
            f_self = Calculate_Self_Fitness(
                child_2,
                flow_rate_gormanium,
                flow_rate_waste,
                price_gormanium,
                cost_waste
            );
            Mutation(f_self, f_max, f_avg, f, adaptive_rate, child_2, num_units);
            // Step 7. Check that each of these potential new vectors are valid and, if they are, add them to the list of child vectors.
            if (utils::Check_Validity(child_1) == 0)
            {
                children.push_back(child_1);
                if (children.size() == population_size)
                    break;
            }
            if (utils::Check_Validity(child_2) == 0)
            {
                children.push_back(child_2);
            }
            // Step 8. Repeat this process from step 4 until there are n child vectors
        }
        if (abs(current_best_performance - old_best_performance) <= 0.1)
            count_for_threshold += 1;
        else
            count_for_threshold = 1;
        if (count_for_threshold == threshold)
        {
            break;
        }
        old_best_performance = current_best_performance;
        // Step 9. Replace the parent vectors with these child vectors
        parents.swap(children);
        children.clear();
    }
    return best_circuit;
}