/*===============================================================
*   Copyright (C) 2017 All rights reserved.
*   
*   FileName:HMMProbability.cpp
*   creator:yuliu1@microsoft.com
*   Time:12/02/2017
*   Description:
*   Notice: 
*   Updates:
*
================================================================*/
 
#include "HMMProbability.h"
#include "HMM.h"
// please add your code here!
#include<fstream>
#include<stdlib.h>
#include<iostream>
std::vector<std::string> HMMProbability::StringSplit(std::string sstr, const char* delim)
{
  std::vector<std::string> results;
  char *src = new char [sstr.length() + 1];
  strncpy(src,sstr.c_str(),sstr.length());
  src[sstr.length()] = 0;
  char *p = strtok(src,delim);
  if ( p!= NULL)
  {
    results.push_back(p);
  }
  while ( (p=strtok(NULL,delim)) != NULL )
  {
    results.push_back(p);
  }
  if (src != NULL )
  {
    delete [] src;
    src = NULL;
  }
  return results;
}

void HMMProbability::GetStatesInfo(const char * filename)
{
   std::ifstream fin(filename); 
   std::string line = "";
   int id = 0;
   while (std::getline(fin,line))
   {
     std::vector<std::string> lines = StringSplit(line,"\t");
     states2Id.insert(std::make_pair(lines[0],id));
     Id2States.push_back(lines[0]);
     id++;
   }
   statesNum=id;
   std::cerr << "statesNum="<<statesNum<<std::endl;
   fin.close();
 }
 
 void HMMProbability::GetSymbolsInfo(const char * filename)
 {
    std::ifstream fin(filename); 
    std::string line = "";
    int id = 0;
    while (std::getline(fin,line))
    {
      std::vector<std::string> lines = StringSplit(line,"\t");
      symbols2Id.insert(std::make_pair(lines[0],id));
      Id2Symbols.push_back(lines[0]);
      id++;
    }
    symbolsNum=id;
   std::cerr << "symbolsNum="<<symbolsNum<<std::endl;
    fin.close();
 }

 void HMMProbability::InitializeProbabilities()
 {
   transits = new double* [statesNum];
   for (int i = 0 ; i<statesNum;i++)
   {
      transits[i] = new double[statesNum];
      memset(transits[i],0,sizeof(double)*statesNum);
   }
   states = new double [statesNum];
   memset(states,0,sizeof(double)*statesNum);
   endstates = new double [statesNum];
   memset(endstates,0,sizeof(double)*statesNum);
   emits = new double* [statesNum];
   for (int i = 0; i< statesNum;i++)
   {
     emits[i] = new double[symbolsNum];
     memset(emits[i],0, sizeof(double)*symbolsNum);
   }
 }

 void HMMProbability::BuildProbabilities(const char* filename)
 {
   
   InitializeProbabilities();
   
   for (int i = 0; i < statesNum;i++)
   {
     states[i] = 1.0/double(statesNum);
   }
   for (int i = 0; i < statesNum;i++)
   {
     endstates[i] = 1.0/double(statesNum);
   }

   for (int i = 0; i < statesNum;i++)
   {
     for (int j = 0; j < statesNum;j++)
     {
       transits[i][j]=1.0/double(statesNum);
     }
   }
   for (int i = 0; i < statesNum;i++)
   {
     for (int j = 0; j < symbolsNum;j++)
     {
       emits[i][j] = 1.0/double(symbolsNum);
     }
   }
   
   std::cerr <<"initial"<<std::endl;
   PrintProbabilities();
   Train(filename);
 }
 void HMMProbability::LoadProbabilities(const char *filename)
 {
   InitializeProbabilities();
   std::ifstream fin(filename); 
   std::string line = "";
   int id = 0;
   while (std::getline(fin,line))
   {
        if (line=="")
        {
          id = 0;
          continue;
        }
        std::vector<std::string> lines = StringSplit(line,"\t");
        if (lines[0]=="states")
        {
             for (int i = 1; i < lines.size();i++)
             {
               states[i-1] = atof(lines[i].c_str());
             }
        }
        if (lines[0]=="endstates")
        {
             for (int i = 1; i < lines.size();i++)
             {
               endstates[i-1] = atof(lines[i].c_str());
             }
        }
        if (lines[0]=="transits")
        {
             for (int i = 1; i < lines.size();i++)
             {
               transits[id][i-1] = atof(lines[i].c_str());
             }
        }
        if (lines[0]=="emits")
        {
          for(int i = 1; i < lines.size();i++)
          {
            emits[id][i-1] = atof(lines[i].c_str());
          }
        }
        id++;
   }
   fin.close();
}
void HMMProbability::clear()
{
  if (states!= NULL)
  {
    delete [] states;
    states = NULL;
  }
  if (endstates!= NULL)
  {
    delete [] endstates;
    endstates = NULL;
  }
  if (transits != NULL)
  {
    for (int i = 0; i < statesNum;i++)
    {
      if (transits[i] != NULL)
      {
          delete [] transits[i];
          transits[i] = NULL; 
      }
    }
    delete [] transits;
    transits = NULL;
  }
  if (emits != NULL)
  {
    for (int i = 0; i < statesNum; i++)
    {
      if (emits[i] != NULL)
      {
         delete [] emits[i];
         emits[i] = NULL;
      }
    }
    delete [] emits;
    emits = NULL;
  }
}
void HMMProbability:: Initialize(bool EMmode)
{
    GetStatesInfo("Labels.txt");
    GetSymbolsInfo("Symbols.txt");
    if (!EMmode)
    {
      LoadProbabilities("hmm.model.txt");
    }
    else
    {
      BuildProbabilities("train.txt");
    }

}
void HMMProbability::PrintProbabilities()
{
    std::cerr<<"states";
   for (int i = 0; i < statesNum;i++)
   {
     std::cerr<<"\t"<<states[i];
   }
   std::cerr<<std::endl;
   std::cerr<<"endstates";
   for (int i = 0; i < statesNum;i++)
   {
     std::cerr<<"\t"<<endstates[i];
   }
   std::cerr<<std::endl;
   for (int i = 0; i < statesNum;i++)
   {
     std::cerr<<"transits";
     for (int j = 0; j < statesNum;j++)
     {
       std::cerr<<"\t"<<transits[i][j];
     }
     std::cerr << std::endl;
   }
   for (int i = 0; i < statesNum;i++)
   {
     std::cerr<<"emits";
     for (int j = 0; j < symbolsNum;j++)
     {
       std::cerr<<"\t"<<emits[i][j];
     }
     std::cerr << std::endl;
   }
}
// notice here should has constraints 
void HMMProbability::Train(const char *filename)
{
   std::ifstream fin(filename); 
   std::string line = "";
   int linecount = 0;
   while (std::getline(fin,line))
   {
     std::vector<std::string> lines = StringSplit(line,"#");
     if (lines.size()<=1)
     {
       continue;
     }
     linecount++;
     if (linecount%100==0)
     {
       std::cerr<<"EM training linecount="<<linecount<< std::endl;
     }
     HMM hmm(lines,this);
     hmm.Train(this);
     std::cerr << "training sample count="<<linecount<<std::endl;
     PrintProbabilities();
   }
   fin.close();
}

HMMProbability::~HMMProbability()
{
  clear();
}

