#pragma once

#include <string>

std::string findDefaultConfigFile();
void printUsage(const char* program_name);
void printConfigNotFoundError();
std::string parseArguments(int argc, char* argv[]);
