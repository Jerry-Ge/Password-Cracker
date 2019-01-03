#include "SrcMain.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <tbb/parallel_for.h>
#include <tbb/blocked_range.h>

void DictionaryAttack(const size_t tableSize, const std::string& dictFileName,
                      const std::string& passFileName)
{
    HashTable<std::string> dictTable(tableSize);
    
    std::ifstream dictFile(dictFileName);
    std::string dictLine;
    while(static_cast<bool>(getline(dictFile, dictLine)))
    {
        std::stringstream ss(dictLine);
        dictLine.clear();
        ss >> dictLine;
        
        
        //dictLine is decrepted
        unsigned char hash[20];
        sha1::Calc(dictLine.c_str(), dictLine.size(), hash);
        std::string hexStr;
        hexStr.resize(40);
        sha1::ToHexString(hash, &hexStr[0]);
       
        //put <encrypted word, plain text work> into hashTable
        dictTable.Insert(hexStr, dictLine);

        dictLine.clear();
    }

    dictFile.close();
    
    
    std::ofstream solvedFile("solved.txt");
    std::ifstream passFile(passFileName);
    std::string passLine;
    
    while(static_cast<bool>(getline(passFile, passLine)))
    {
        std::stringstream ss(passLine);
        passLine.clear();
        ss >> passLine;
        
        std::string* find = dictTable.Find(passLine);
        
        
        if (find != nullptr) {
            solvedFile << passLine << "," << (*find) << std::endl;
        }
        else {
            solvedFile << passLine << "," << "" << std::endl;
        }
    }
    
    solvedFile.close();
}


void PermutationHelper(std::vector<std::string>& res, const std::vector<std::string>& input,
                       std::string& build, int totSize)
{
    //last combination
    if (totSize == 1) {
        for (const auto& s : input)
        {
            res.emplace_back(build + s);
        }
        return;
    } else {
        for (const auto& s : input)
        {
            std::string partialString = build + s;
            PermutationHelper(res, input, partialString, totSize - 1);
        }
    }
}


void GeneratePermutation(const std::vector<std::string>& vec, std::vector<std::string>& allPermutes)
{
    
    std::string initString;
    PermutationHelper(allPermutes, vec, initString, 4);
    
}


void PhrasesBruteForce(const size_t tableSize, const std::string& wordsFileName,
                       const std::string& passFileName)
{
    HashTable<size_t> dictTable(tableSize); //<encrypted text, index of plain text>
    
    std::ifstream passFile(passFileName);
    
    int textIndex = 0;
    std::string passLine;
    //creat the hashTable
    while(static_cast<bool>(passFile >> passLine))
    {
        dictTable.Insert(passLine, textIndex++);
    }

    std::vector<std::string> decreptedText(textIndex, ""); //store decrepted text by input order
    
    //read in all the input words
    std::vector<std::string> wordsVec;
    std::ifstream wordsFile(wordsFileName);
    std::string wordLine;
    while(static_cast<bool>(wordsFile >> wordLine))
    {
        wordsVec.emplace_back(wordLine);
    }

    wordsFile.close();
    
    std::vector<std::string> allPermutations;
    GeneratePermutation(wordsVec, allPermutations);
    
    //iterate through all permuatations
    tbb::parallel_for(tbb::blocked_range<size_t>(0, allPermutations.size()),
                      [&dictTable, &decreptedText, &allPermutations](const tbb::blocked_range<size_t>& r)
    {
        unsigned char hash[20];
        std::string hexStr(40, 0);
        for (size_t i = r.begin(); i != r.end(); ++i) {
            sha1::Calc(allPermutations[i].c_str(), allPermutations[i].size(), hash);
            sha1::ToHexString(hash, &hexStr[0]);
    
            size_t* find = dictTable.Find(hexStr);
            if (find != nullptr)
            {
                decreptedText[*find] = hexStr + "," + allPermutations[i];
            }
        }
    });
    
    std::ofstream decreptedFile("solved.txt");
    for (const auto& s : decreptedText) {
        decreptedFile << s << std::endl;
    }
    decreptedFile.close();
}

void ProcessCommandArgs(int argc, const char* argv[])
{
	// TODO: Implement
    std::string commandType = argv[1];
    size_t tableSize = atoi(argv[2]);
    std::string dictFileName = argv[3];
    std::string passFileName = argv[4];
    
    if (commandType == "dictionary") {
        DictionaryAttack(tableSize, dictFileName, passFileName);
    }
    else if (commandType == "phrases")
    {
        PhrasesBruteForce(tableSize, dictFileName, passFileName);
    }
}
