/*===============================================================
*   Copyright (C) 2017 All rights reserved.
*   
*   FileName:HMM.h
*   creator:yuliu1@microsoft.com
*   Time:12/01/2017
*   Description:
*   Notice: 
*   Updates:
*
================================================================*/
 
#ifndef _HMMPROBABILITY_H
#define _HMMPROBABILITY_H
// please add your code here!
#include<map>
#include<vector>
#include<string>
class HMMProbability
{
  public:
    ~HMMProbability();
    void clear();
    std::vector<std::string> StringSplit(std::string line, const char* delim);
    void GetStatesInfo(const char* filename);
    void GetSymbolsInfo(const char* filename);
    void InitializeProbabilities();
    void BuildProbabilities(const char * filename);
    void LoadProbabilities(const char *filename);
    void Initialize(bool EMmode=false);
    void PrintProbabilities();
    void Train(const char * filename);
   friend class HMM; 
  private:
    int statesNum;
    int symbolsNum;
    // the states transition probability between states(A)
    double **transits;
    // the emition probability of symbols in states (B)
    double **emits;
    //  the states probility;
    double *states;
    //end states
    double *endstates;
    std::map<std::string, int > symbols2Id;
    std::vector<std::string> Id2Symbols;
    std::map<std::string,int> states2Id;
    std::vector<std::string> Id2States;
};
#endif
