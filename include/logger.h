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

		template <typename T>
		void log(std::vector<T*> const pop) {
			for (T* s : pop){
				for (double x : s->getX()){
					out << x << " ";
				}
				out << s->getFitness();
				out << "\n";
			}
			out << "\n";
		}

		void log(int const function, int const D, VectorXd const percCorrected, 
				VectorXd const bestX, double const bestF, int const numEvals);

		void log(VectorXd const& F, VectorXd const& Cr);
		void start(int const f, int const D);

		void newLine();
};
