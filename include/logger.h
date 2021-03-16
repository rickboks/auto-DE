#pragma once
#include <fstream>
#include "Eigen/Dense"
#include <iomanip>

class Logger {
	private:
		std::ofstream out;
		std::string const filename;
		bool activated = false;
	public:
		Logger(std::string filename) : filename(filename){};
		~Logger();
		void checkActive();

		template <typename T>
		void log(T const& t, bool const newline = true){
			checkActive();
			out << t;
			if (newline)
				out << "\n";
		}
		void flush();
};
