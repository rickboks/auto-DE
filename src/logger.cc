#include "logger.h"
#include "util.h"
#include <numeric>

Logger::~Logger(){
	out.close();
};
