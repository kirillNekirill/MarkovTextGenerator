#ifndef MARKOVTEXTGEN_H_
#define MARKOVTEXTGEN_H_

#include <unordered_map>
#include <string>
#include <list>

#define END_TAG L"*END*"

class MarkovTextGen
{
public:
	//Model constructor.
	//arguments:
	//	iOrder - Markov's model order.
	MarkovTextGen(unsigned iOrder = 1);

	//Fit model with input text.
	//input:
	//	iWords - list of strings representing input text. One word per list element.
	//return:
	//	true if succeeded, false otherwise.
	bool Update(const std::list<std::wstring>& iWords);

	//Load model from file.
	//input:
	//	iFileToRead - file name to read model data from.
	//return:
	//	true if succeeded, false otherwise.
	bool LoadFromFile(std::string iFileToRead);

	//Save model to file.
	//input:
	//	iFileToSave - file name to save the model.
	//return:
	//	true if succeeded, false otherwise.
	bool SaveToFile(std::string iFileToSave) const;

	//Generate a new random word from previous word(s).
	//input:
	//	iPrev - previous words using for generating a new one. Size must be equal to model's order.
	//return:
	//	generated word or END_TAG.
	std::wstring GenRandWord(const std::list<std::wstring>& iPrev) const;

	//Model's order
	unsigned order;

private:
	typedef std::unordered_map<std::wstring, int> dictogram;
	std::unordered_map<std::wstring, dictogram> _wordsMap;
};

#endif
