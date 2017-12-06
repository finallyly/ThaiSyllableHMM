/*===============================================================
*   Copyright (C) 2017 All rights reserved.
*   
*   FileName:OffLineUseHMM.cpp
*   creator:yuliu1@microsoft.com
*   Time:11/25/2017
*   Description:
*   Notice: 
*   Updates:
*
================================================================*/
 
#include "HMMProbability.h"
#include "HMM.h"
#include<iostream>
#include<list>
#include<fstream>
// please add your code here!
// notice StringSplit2 is the same as StringSplit,  I just get lazy here
std::vector<std::string> StringSplit2(std::string sstr, const char* delim)
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
void TestLoading(const char* infilename, const char* ofilename)
{
  HMMProbability hmmModel;
  hmmModel.Initialize(true);
  hmmModel.PrintProbabilities();
  int linecount = 0;
  std::ifstream fin(infilename);
  std::ofstream fout(ofilename);
  std::string line="";
  std::cerr <<"begin pyredicting"<<std::endl;
  while (std::getline(fin,line))
  {
    if (line == "")
    {
      fout<<std::endl;
      continue;
    }
    linecount++;
    if (linecount%1000==0)
    {
      std::cerr <<"linecount="<<linecount<<std::endl;
    }
    std::vector<std::string> lines = StringSplit2(line,"\t");
    std::vector<std::string> sequence = StringSplit2(lines[0],"#");
    HMM hmm(sequence,&hmmModel);
    std::vector<std::string> labels = hmm.Predict(&hmmModel);
    std::string newline = "";
    for (int i = 0; i < labels.size();i++)
    {
      if (newline != "")
      {
        newline+="#";
      }
      newline+=labels[i];
    }
    fout <<lines[0]<<"\t"<<lines[1]<<"\t"<<newline<<std::endl;
  }
  fin.close();
  fout.close();
}
int main(int argc, char **argv)
{
  if (argc <3)
  {
    std::cerr << "no enough params"<<std::endl;
    exit(1);
  }
  TestLoading(argv[1],argv[2]);
  return 0;
}
