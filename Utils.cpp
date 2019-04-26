#include <fstream>
#include <vector>

#include "Utils.h"
#include "Exceptions.h"

namespace trading {

std::vector<std::string> readFile2Vector(const std::string& filename) {

	std::ifstream is("C:\\Users\\wijekoonw\\Desktop\\RangaNew\\feed.txt");

	if (!is) {
		throw trading::BadMarketDataFile();
	}

	std::string str;
	std::vector<std::string> lines;

	while (!is.eof()) {
		std::getline(is, str);
		lines.push_back(str);
	}

	is.close();

	return lines;
}

}
