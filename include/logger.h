#include <fstream>
#include "Eigen/Dense"

class Logger {
	private:
		std::ofstream out;
	public:
		Logger(std::string filename): out(filename){};
		~Logger();

		template <typename T>
		void log(T const& t, bool newline = true){
			out << t;
			if (newline)
				out << "\n";
		}
};
