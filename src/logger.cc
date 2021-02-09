#include "logger.h"

Logger::~Logger(){
	out.close();
};

void Logger::flush(){
	out.flush();
}
