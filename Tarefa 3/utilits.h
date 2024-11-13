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
 * @param state Pointer to the system state structure where the seeds are stored.
 */
void ask_streams(SystemState *state);

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
void generate_other_streams(SystemState *state);

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
 * @return `EXIT_SUCCESS` if all parameters are valid; otherwise, `EXIT_FAILURE`.
 */
int receive_input_file(int argc, char *argv[], SystemState *state, Files *files);

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
 * @param state Pointer to the SystemState structure where the simulation parameters will be stored.
 * @param files Pointer to the Files structure that holds the input file pointer.
 */
void ask_for_par(SystemState *state, Files *files);

/*!
 * @brief Clears the console screen based on the operating system.
 *
 * This function clears the console screen by calling the appropriate system command 
 * depending on the operating system. It uses "cls" for Windows and "clear" for Unix-based systems.
 * 
 * This function is platform-dependent and relies on the system's command-line tools.
 */
void clear_screen();



#endif






