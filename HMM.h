/*===============================================================
*   Copyright (C) 2017 All rights reserved.
*   
*   FileName:HMM.h
*   creator:yuliu1@microsoft.com
*   Time:12/03/2017
*   Description:
*   Notice: 
*   Updates:
*
================================================================*/
 
#ifndef _HMM_H
#define _HMM_H
#include "HMMProbability.h"
// please add your code here!
struct Node
{
  int labelId;
  double alpha;
  double beta;
  double cost;
  int leftIndex;
  int rightIndex;
  Node():labelId(-1), alpha(0.0),beta(0.0),cost(0.0),leftIndex(-1),rightIndex(-1) { };
};
class HMM
{
  public:
   HMM(std::vector<std::string> sentVec,HMMProbability* hmmModel);
   void Forward(HMMProbability* hmmModel);
   void Backward(HMMProbability* hmmModel);
   std::pair<double, int> Viterbi(HMMProbability* hmmModel);
   ~HMM( );
   void FillGammas(HMMProbability* hmmModel);
   void FillEpsilons(HMMProbability* hmmModel);
   void BaumWelch(HMMProbability* hmmModel);
   void Train(HMMProbability* hmmModel);
   std::vector<std::string> GetMaxProbSequence(int index,HMMProbability* hmmModel);
   std::vector<std::string> Predict(HMMProbability *hmmModel);
  private:
   Node **nodemap;
   std::vector<int> symbolIds;
   int sentLen;
   std::vector<std::vector<double> > gammas;
   std::vector<double**> epsilons;
};
#endif
