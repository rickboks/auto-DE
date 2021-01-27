#include "logger.h"
#include "util.h"
#include <numeric>

void Logger::log(int const function, int const D, ArrayXd const percCorrected, 
		ArrayXd const bestX, double const bestF, int const numEvals){
	out.precision(3);

	out << function << " " << D << " ";
	for (double d : percCorrected)
		out << d << " ";
	for (double d : bestX)
		out << d << " ";
	out << bestF << " " << numEvals << "\n";
}

void Logger::start(int const function, int const D){
	out << function << " " << D << ":";
}

void Logger::log(ArrayXd const& F, ArrayXd const& Cr){
	out.precision(3);
	out << F.mean() << " " << Cr.mean() << ","; 
}

void Logger::newLine(){
	out << "\n";
}

Logger::~Logger(){
	out.close();
};
