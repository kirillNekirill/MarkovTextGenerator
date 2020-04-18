#include "MarkovTextGen.h"

#include <iostream>
#include <memory>
#include <fstream>
#include <algorithm>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#define TMP_FILENAME "tmp_file_from_url.txt"
#define CURRENTLOCALE "en_US.UTF-8"

std::string exec(std::string iCmdLine) {
	std::array<char, 128> buffer;
	std::string result;
	std::shared_ptr<FILE> pipe(popen(iCmdLine.c_str(), "r"), pclose);
	if (!pipe) throw std::runtime_error("popen() failed!");
	while (!feof(pipe.get())) {
		if (fgets(buffer.data(), 128, pipe.get()) != nullptr)
			result += buffer.data();
	}
	return result;
}

void printHelp()
{
	std::cout << "USAGE: <File with URLs> <Model order> <File to save model>" << std::endl;
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
		std::cout << "First argument must be a file to read" << std::endl;
		return EXIT_FAILURE;
	}

	if (argc == 2)
	{
		std::cout << "Three arguments are expected" << std::endl;
		return EXIT_FAILURE;
	}

	if (argc == 3)
	{
		std::cout << "Three arguments are expected" << std::endl;
		return EXIT_FAILURE;
	}

	bool isNum = true;
	std::string numArg(argv[2]);
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
		std::cout << "Second argument must be a natural num" << std::endl;
		return EXIT_FAILURE;
	}

	unsigned n = std::stoi(numArg);

	if (n < 1)
	{
		std::cout << "Second argument must be a natural num" << std::endl;
		return EXIT_FAILURE;
	}

	MarkovTextGen model(n);

	std::ifstream urlsfile;
	urlsfile.open(argv[1]);
	std::string textToProcess;
	if (urlsfile.is_open())
	{
		std::string url;
		while(getline(urlsfile, url))
		{
			std::string cmd_call;
			cmd_call += "curl ";
			cmd_call += url;
			cmd_call += " -o ";
			cmd_call += TMP_FILENAME;
			textToProcess = exec(cmd_call);

			std::wifstream filetoprocess;
			filetoprocess.open(TMP_FILENAME);

			std::wstring line;
			std::list<std::wstring> wordsRow;
			if(filetoprocess.is_open())
			{
				while (getline(filetoprocess, line, L' '))
				{
					std::transform(line.begin(), line.end(), line.begin(), ::tolower);

					std::wstring nextW;
					for (auto it = line.begin(); it != line.end(); ++it)
					{
						if (isalnum(*it, loc) || *it == L'\'' || *it == L'-')
							nextW += *it;
					}

					if(nextW.size() > 0)
						wordsRow.push_back(nextW);
				}

				filetoprocess.close();
				unlink(TMP_FILENAME);

				if(wordsRow.size() < n)
				{
					std::cout << "Not enough words to build Markov chain" << std::endl;
					return EXIT_FAILURE;
				}

				model.Update(wordsRow);

				if(!model.SaveToFile(argv[3]))
				{
					std::cout << "Unsuccessful saving of Markov Text Gen model in file: " << argv[3] << std::endl;
					return EXIT_FAILURE;
				}
			}
			else
			{
				std::cout << "Unable to open file " << TMP_FILENAME << std::endl;
				return EXIT_FAILURE;
			}
		}

		urlsfile.close();
	}
	else
	{
		std::cout << "Unable to open file " << argv[1] << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
