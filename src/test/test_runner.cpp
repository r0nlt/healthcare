/**
 * Test Runner for rad-tolerant-ml tests
 *
 * This file provides a simple interface for running the tests in the project.
 */

#include <functional>
#include <iostream>
#include <map>
#include <string>

// Define macro to disable main function in test files
#define QUANTUM_WAVE_MC_TEST_NO_MAIN

// External function declarations for test entry points
extern int run_quantum_wave_mc_test(int argc, char** argv);

// Main test runner
int main(int argc, char** argv)
{
    // Map of test names to their entry point functions
    std::map<std::string, std::function<int(int, char**)>> tests = {
        {"quantum_wave_mc", run_quantum_wave_mc_test}};

    // If no arguments, show available tests
    if (argc < 2) {
        std::cout << "Test Runner for rad-tolerant-ml\n";
        std::cout << "------------------------------\n";
        std::cout << "Usage: " << argv[0] << " <test_name> [test_args...]\n";
        std::cout << "Available tests:\n";

        for (const auto& test : tests) {
            std::cout << "  " << test.first << "\n";
        }

        return 1;
    }

    // Get the test name from the command line
    std::string test_name = argv[1];

    // Look up the test function
    auto it = tests.find(test_name);
    if (it == tests.end()) {
        std::cerr << "Error: Unknown test '" << test_name << "'\n";
        return 1;
    }

    // Prepare arguments for the test
    int test_argc = argc - 1;
    char** test_argv = new char*[test_argc];

    // Make argv[0] the program name, then copy the rest of the arguments
    test_argv[0] = argv[0];
    for (int i = 1; i < test_argc; ++i) {
        test_argv[i] = argv[i + 1];
    }

    // Run the test
    int result = it->second(test_argc, test_argv);

    // Clean up
    delete[] test_argv;

    return result;
}

// Implementation of the test entry points
int run_quantum_wave_mc_test(int argc, char** argv)
{
    // This function will be defined when quantum_wave_mc_test.cpp is included
    extern int main(int argc, char** argv);
    return main(argc, argv);
}

// Include test implementations AFTER defining NO_MAIN macros
#include "quantum_wave_mc_test.cpp"
