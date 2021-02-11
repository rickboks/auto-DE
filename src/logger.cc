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
		activated = true;
	}
}
