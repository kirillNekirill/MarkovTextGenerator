#include "MarkovTextGen.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <string.h>

#define CURRENTLOCALE "en_US.UTF-8"

void printHelp()
{
	std::cout << "USAGE: <File with model> <File with initial words> <Generated words num>" << std::endl;
}

int main(int argc, char* argv[])
{
	std::locale loc(CURRENTLOCALE);
	setlocale(LC_CTYPE, CURRENTLOCALE);

	if(argc == 2 && (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0))
	{
		printHelp();
		return EXIT_SUCCESS;
	}

	if (argc == 1)
	{
		std::cout << "First argument must be a model-file to read" << std::endl;
		return EXIT_FAILURE;
	}

	if (argc == 2)
	{
		std::cout << "Second argument must be an input file with initial words" << std::endl;
		return EXIT_FAILURE;
	}
	
	if (argc == 3)
	{
		std::cout << "Third argument must be a natural number of words to generate" << std::endl;
		return EXIT_FAILURE;
	}

	bool isNum = true;
	std::string numArg(argv[3]);
	for (auto i : numArg)
	{
		if (!isdigit(i))
		{
			isNum = false;
			break;
		}
	}

	if (!isNum)
	{
		std::cout << "Third argument must be a natural num" << std::endl;
		return EXIT_FAILURE;
	}

	unsigned m = std::stoi(numArg);

	if (m < 1)
	{
		std::cout << "Third argument must be a natural num" << std::endl;
		return EXIT_FAILURE;
	}

	srand(time(NULL));
	MarkovTextGen model;
	if(!model.LoadFromFile(argv[1]))
	{
		std::cout << "Unable to load model from file" << argv[1] << std::endl;
		return EXIT_FAILURE;
	}

	unsigned n = model.order;

	std::list<std::wstring> current;
	
	std::wifstream initialWordsFile;
	initialWordsFile.open(argv[2]);
	std::string textToProcess;
	if (initialWordsFile.is_open())
	{
		std::wstring currWord;
		while(current.size() < n && getline(initialWordsFile, currWord, L' '))
		{
			std::transform(currWord.begin(), currWord.end(), currWord.begin(), ::tolower);

			std::wstring checkedW;
			for (auto ch : currWord)
			{
				if (isalnum(ch, loc) || ch == L'\'' || ch == L'-')
					checkedW += ch;
			}
			
			if(checkedW.length() > 0)
				current.push_back(checkedW);
		}

		initialWordsFile.close();
	}
	else
	{
		std::cout << "Unable to open initial file " << argv[2] << std::endl;
		return EXIT_FAILURE;
	}
	
	if(current.size() < n)
	{
		std::cout << "Only " << current.size() << " valid words were founded in initial file " << argv[2] << ". Model's order is " << n << std::endl;
		return EXIT_FAILURE;
	}
	
	for(auto i: current)
		std::wcout << i << L' ';

	while (m > 0)
	{
		std::wstring nextW = model.GenRandWord(current);

		if (nextW == END_TAG)
			break;

		current.erase(current.begin());
		current.push_back(nextW);

		std::wcout << nextW << ' ';
		m--;
	}

	std::wcout << std::endl;

    return EXIT_SUCCESS;
}
