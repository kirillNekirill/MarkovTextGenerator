#include "MarkovTextGen.h"

#include<algorithm>
#include <fstream>
#include <vector>
#include <string>
#include <time.h>
#include <array>
#include <sstream>

#define NEXT_DICTOGRAM_TAG L"*NEXT*"

MarkovTextGen::MarkovTextGen(unsigned iOrder): order_(iOrder)
{
}

bool MarkovTextGen::Update(const std::list<std::wstring>& iWords)
{
	if(iWords.size() < order_)
		return false;
	
	std::wstring window;
	unsigned i = 0;
	auto wordsIt = iWords.begin();
	while(i < order_)
	{
		if(i != 0)
			window.push_back(' ');
		
		window.append(*wordsIt);
		wordsIt++;
		i++;
	}
	
	while(wordsIt != iWords.end())
	{
		auto dictogramIt = _wordsMap.emplace(window, dictogram()).first;
		
		auto weightedWordIt = dictogramIt->second.emplace(*wordsIt,0).first;
		weightedWordIt->second++;

		while(!window.empty() && window[0] != ' ')
			window.erase(0, 1);
		if(!window.empty())
			window.erase(0, 1);

		if(order_ > 1)
			window.push_back(' ');
		window.append(*wordsIt);

		wordsIt++;
		if(wordsIt == iWords.end())
			dictogramIt->second[END_TAG] = 1;
	}

	return true;
}

std::wstring MarkovTextGen::GenRandWord(const std::list<std::wstring>& iPrev) const
{
	if(iPrev.size() != order_)
		return END_TAG;

	std::wstring window;
	unsigned i = 0;
	for(const auto& item : iPrev)
	{
		if(i != 0)
			window.push_back(' ');
		
		window.append(item);
		i++;
	}
	
	auto dictogramIt = _wordsMap.find(window);
	
	if(dictogramIt == _wordsMap.end())
		return END_TAG;
	
	int totalWeight = 0;
	std::for_each(dictogramIt->second.begin(), dictogramIt->second.end(), [&totalWeight](std::pair<std::wstring, int> i){ totalWeight += i.second; });

	int index2 = rand() % totalWeight;
	totalWeight = 0;
	for (auto weightedWordPair : dictogramIt->second)
	{
		if (index2 >= totalWeight && index2 < totalWeight + weightedWordPair.second)
			return weightedWordPair.first;

		totalWeight += weightedWordPair.second;
	}

	return END_TAG;
}


bool MarkovTextGen::LoadFromFile(std::string iFileToRead)
{
	std::wifstream modelFile;
	modelFile.open(iFileToRead);

	if (modelFile.is_open())
	{
		std::wstring curr, word, num;
		std::wstring keyWindow;
		std::wstringstream lineStream(curr);
		unsigned currOrder = 0;
		
		if(getline(modelFile, curr))
		{
			keyWindow = curr;
			lineStream = std::wstringstream(curr);
			while (getline(lineStream, word, L' '))
				currOrder++;
		}

		if(currOrder == 0)
			return false;

    order_ = currOrder;

		auto currentDictogramIt = _wordsMap.end();

		while(getline(modelFile, curr))
		{
			lineStream = std::wstringstream(curr);
			if(curr == NEXT_DICTOGRAM_TAG)
			{
				currentDictogramIt = _wordsMap.end();
				currOrder = 0;
				keyWindow.clear();
				if(getline(modelFile, curr))
				{
					keyWindow = curr;
					lineStream = std::wstringstream(curr);
					while (getline(lineStream, word, L' '))
						currOrder++;
				}
				continue;
			}
			else
			{
				if(!getline(lineStream, word, L' '))
					return false;
				if(!getline(lineStream, num, L' '))
					return false;

				if(std::stoi(num) <= 0)
					return false;

				if(currOrder == 0)
					return false;

				if(currOrder != order_)
					return false;

				if(currentDictogramIt == _wordsMap.end())
					currentDictogramIt = _wordsMap.emplace(keyWindow, dictogram()).first;

				currentDictogramIt->second[word] = std::stoi(num);
			}
		}
		modelFile.close();
	}
	else
		return false;

	return true;
}
bool MarkovTextGen::SaveToFile(std::string iFileToSave) const
{
	if(iFileToSave.size() == 0)
		return false;

	std::wofstream modelFile;
	modelFile.open(iFileToSave);

	if (modelFile.is_open())
	{
		for(auto i : _wordsMap)
		{
			modelFile << i.first << std::endl;

			for(auto j: i.second)
			{
				modelFile << j.first << L' ' << std::to_wstring(j.second) << std::endl;
			}
			modelFile << NEXT_DICTOGRAM_TAG << std::endl;
		}
		modelFile.close();
	}
	else
		return false;

	return true;
}

unsigned MarkovTextGen::order() const
{
  return order_;
}