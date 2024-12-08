#ifndef UTILITS_H
#define UTILITS_H

/*! \page utilits
 *  \file utilits.h
 *  \brief Header file for utility functions. 
 */
#include "fila1s.h"

/*! 
 * @brief Prompts the user to enter two unique seeds, one for arrivals and one for departures.
 *
 * This function requests seeds within a specific range [1, 100], ensuring the seeds are positive,
 * within the specified range, and distinct from each other.
 *
 * @param init Pointer to the initial values structure where the seeds will be stored.
 */
void ask_streams(InitialValues *init);

/*! 
 * @brief Generates additional seed streams for each server in the system.
 *
 * This function fills the `streams` array in `state` with unique seeds for each server. 
 * It ensures that the seeds are sequential, distinct from the arrival stream seed, 
 * and wraps around if the seed exceeds 100.
 *
 * @param state Pointer to the system state structure containing the number of servers 
 *              and an array to store the generated seeds.
 */
void generate_other_streams(InitialValues *init, int index, SystemState * state);

/*!
 * @brief Reads input parameters from a specified file and verifies their validity.
 *
 * This function takes the command-line arguments, opens the input file, and reads the system parameters into the `state` structure. 
 * It validates each parameter to ensure they meet the required conditions and outputs error messages if any parameter is invalid. 
 * If any issue is encountered, the function exits with `EXIT_FAILURE`.
 *
 * @param argc The count of command-line arguments.
 * @param argv The array of command-line arguments, where `argv[1]` is expected to be the input file name.
 * @param state Pointer to the SystemState structure that will hold the simulation parameters.
 * @param files Pointer to the Files structure containing file pointers.
 * @param q1 Pointer to the circular queue structure.
 * @param init Pointer to the InitialValues structure that will store the random seeds.
 * @return `EXIT_SUCCESS` if all parameters are valid; otherwise, `EXIT_FAILURE`.
 */
int receive_input_file(int argc, char *argv[], Files *files, circular_queue *q1, InitialValues *init);

/*!
 * @brief Prompts the user for simulation parameters, validates inputs, and writes to an input file.
 *
 * This function collects all necessary simulation parameters from the user, validating each one to ensure 
 * it meets the required conditions. If any parameter is invalid, it prompts the user to re-enter a valid value. 
 * After all values are correctly entered, it writes the parameters and random seeds to the input file.
 *
 * The parameters collected include:
 * - Number of servers
 * - Mean interarrival time
 * - Mean service time
 * - Number of customers (delays) required
 * - Queue option (without queue or infinite queue)
 * 
 * This function also calculates the offered traffic load (`A`) based on the interarrival and service times, 
 * and ensures it is less than the number of servers. It calls `ask_streams` to collect random seeds and 
 * `generate_other_streams` to generate additional seeds for each server.
 *
 * @param files Pointer to the Files structure that holds the input file pointer.
 * @param q Pointer to the circular queue structure.
 * @param init Pointer to the InitialValues structure that will store the random seeds.
 */
void ask_for_par( Files *files, circular_queue *q, InitialValues *init);

/*!
 * @brief Clears the console screen based on the operating system.
 *
 * This function clears the console screen by calling the appropriate system command 
 * depending on the operating system. It uses "cls" for Windows and "clear" for Unix-based systems.
 * 
 * This function is platform-dependent and relies on the system's command-line tools.
 */
void clear_screen();

/*!
 * @brief Calculates the blocking probability in an M/M/n/0 system using the Erlang-B formula.
 *
 * This function computes the blocking probability (or customer loss) in an M/M/n/0 queuing system, where customers arrive according to a Poisson process and are served by `n` servers. The calculation is based on the Erlang-B formula, which uses the offered traffic `A` (arrival rate multiplied by the average service time) and the number of channels `n` (the number of servers) to determine the probability of a customer being rejected (blocked).
 *
 * The Erlang-B formula is especially used for loss systems, where there is no waiting queue, and customers are rejected if all servers are busy.
 *
 * @param A The offered traffic to the system, which is the arrival rate multiplied by the average service time (A = λ * T).
 * @param n The number of channels (servers) available in the system.
 * @return The blocking probability (customer loss) in the M/M/n/0 system.
 */
double erlang_B(double A, unsigned int n);

/*!
 * @brief Calculates the waiting on queue probability in an M/M/n/inf system using the Erlang-B formula.
 *
 * This function computes the waiting on queue probability (or customer loss) in an M/M/n/inf queuing system, where customers arrive according to a Poisson process and are served by `n` servers. The calculation is based on the Erlang-C formula, which uses the offered traffic `A` (arrival rate multiplied by the average service time) and the number of channels `n` (the number of servers) to determine the probability of a customer going to a queue.
 *
 * The Erlang-B formula is especially used for loss systems, where there is no waiting queue, and customers are rejected if all servers are busy.
 *
 * @param A The offered traffic to the system, which is the arrival rate multiplied by the average service time (A = λ * T).
 * @param n The number of channels (servers) available in the system.
 * @return The waiting on queue probability in the M/M/n/inf system.
 */
double erlang_C(double A, unsigned int n);

/*!
 * @brief Calculates the confidence interval for a given sample using the t-distribution.
 *
 * This function computes the confidence interval for a set of values, based on the t-distribution, which is commonly used when the sample size is small and the population variance is unknown. The calculation uses the sample mean, standard deviation, and a critical value from the t-distribution (from a file `t_student_95.txt`) to determine the interval around the sample mean that would contain the true population mean with a certain level of confidence.
 * 
 * The confidence interval is calculated using the formula:
 * 
 *      CI = [mean - (t * (std_dev / sqrt(n))), mean + (t * (std_dev / sqrt(n)))]
 *
 * Where:
 * - `mean` is the average of the sample values.
 * - `t` is the critical value from the t-distribution for a 95% confidence level.
 * - `std_dev` is the standard deviation of the sample.
 * - `n` is the number of samples.
 *
 * The critical value `t` is read from a file containing t-distribution values for the desired confidence level (95% in this case).
 *
 * @param array The array containing the sample data.
 * @param init A pointer to an `InitialValues` structure, which holds the number of repetitions (`number_of_reps`) for the sample.
 * @param inferior A pointer to a float that will hold the lower bound of the confidence interval.
 * @param superior A pointer to a float that will hold the upper bound of the confidence interval.
 */
void intervalo_confianca(float array[], InitialValues * init, float *inferior, float *superior );

#endif






