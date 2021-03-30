#include <exception>
#include "logger.h"

Logger::~Logger(){
	if (activated)
		out.close();
};

void Logger::flush(){
	if (activated)
		out.flush();
}

void Logger::checkActive(){
	if (!activated){
		out.open(filename);
		if (!out.is_open())
			throw std::invalid_argument("Failed to open file " + filename);
		activated = true;
	}
}
