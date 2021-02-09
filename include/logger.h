#include <fstream>
#include <iostream>
#include <vector>
#include "util.h" 

class Solution;

class Logger { // Used to log arbitrary stuff
	private:
		std::ofstream out;
	public:
		Logger(std::string filename): out(filename, std::ios::app){};
		~Logger();
};
