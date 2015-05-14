#include <iostream>
#include <fstream>

#include <sstream>
#include <string>

#include <vector>

typedef unsigned int uint;

void printvec(std::vector<uint>& vec) {

	for (auto it = vec.begin(); it != vec.end(); it++) {
		std::cout << *it << ",";
	}

	std::cout << std::endl;

}

void printvec(std::vector <std::vector<uint>>& vec) {

	for (auto it = vec.begin(); it != vec.end(); it++) {
		printvec(*it);
		std::cout << " ";
	}
	std::cout << std::endl;
}

void main( std::string path ) {

	std::vector< std::vector< uint > > stackvector;

	std::ifstream infile;
	infile.open("C:/workspace/ds/testdata/input3.data");

	std::ofstream outfile;
	outfile.open("C:/workspace/ds/out/out.data");
	
	std::string line,token;
	uint linecount = 0;
	while (std::getline(infile, line))
	{
		
		stackvector.clear();
		std::istringstream iss(line);
		while (std::getline(iss, token, ','))
		{
			uint newContainer = stoi(token);

			//bestfitstackptr muss auf höchsten initialisiert werden
			//damit "lastContainerOnThisStack <= lastContainerOnBestStack"
			//mit initalwert wahr sein kann.
			uint highestcontainer = 0;
			auto bestfitstackptr = stackvector.begin();
			for (auto it = stackvector.begin(); it != stackvector.end(); it++) {
				if (stackvector.size() == 0) {
					break;
				}

				if ((*it)[it->size() - 1] > highestcontainer || highestcontainer == 0) {
					bestfitstackptr = it;
					highestcontainer = (*it)[it->size() - 1];
				}
			}

			
			uint lastContainerOnThisStack = 0;
			uint lastContainerOnBestStack = 0;
			bool foundstack = false;
			for (auto it = stackvector.begin(); it != stackvector.end(); it++) {

				if (stackvector.size() == 0) {
					break;
				}

				lastContainerOnThisStack = (*it)[ it->size() - 1 ];
				lastContainerOnBestStack = (*bestfitstackptr)[bestfitstackptr->size() - 1];

				//std::cout << lastContainerOnThisStack << " " << lastContainerOnBestStack << std::endl;

				if (lastContainerOnThisStack == newContainer) {
					bestfitstackptr = it;
					foundstack = true;
					break;
				}

				if (lastContainerOnThisStack > newContainer && lastContainerOnThisStack <= lastContainerOnBestStack ) {
					bestfitstackptr = it;
					foundstack = true;
				}
			}

			if (foundstack) {
				bestfitstackptr->push_back(newContainer);
			} else {

				std::vector<uint> newstack;
				newstack.push_back(newContainer);

				stackvector.push_back(newstack);

				//std::cout << "stack: ";
				//printvec(stack);

				//std::cout << "stackvector: ";
				std::cout << newContainer << std::endl;
				printvec(stackvector);
			}
		}

		outfile << stackvector.size();

		for (auto stackvectorit = stackvector.begin(); stackvectorit != stackvector.end(); stackvectorit++) {

			outfile << " ";

			for (auto stackit = stackvectorit->begin(); stackit != stackvectorit->end(); stackit++) {

				if (stackit != stackvectorit->begin()) {
					outfile << ",";
				}

				outfile << *stackit;
			}
		}

		outfile << std::endl;

	}
	infile.close();
	outfile.close();

}