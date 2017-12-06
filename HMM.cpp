/*===============================================================
*   Copyright (C) 2017 All rights reserved.
*   
*   FileName:HMM.cpp
*   creator:yuliu1@microsoft.com
*   Time:12/03/2017
*   Description:
*   Notice: 
*   Updates:
*
================================================================*/
 
#include "HMM.h"
#include <limits>
#include <math.h>
#include <iostream>
// please add your code here!
#define MAX_COST -1000000.0
HMM::HMM(std::vector<std::string> sentVec,HMMProbability *hmmModel)
{
   int len = int(sentVec.size());
   nodemap = new Node*[len];
   sentLen = len;
   for (int i = 0; i < len;i++)
   {
     nodemap[i] = new Node[hmmModel->statesNum];
     for (int j = 0; j < hmmModel->statesNum; j++)
     {
       nodemap[i][j].labelId = j;
     }
   }
    
   for (int j = 0; j < len; j++)
   {
     symbolIds.push_back(hmmModel->symbols2Id[sentVec[j]]);
   }
   for (int j=0; j < len; j++)
   {
     std::cerr << symbolIds[j]<<" ";
   }
   std::cerr << std::endl;

   gammas.resize(sentLen,std::vector<double>(hmmModel->statesNum));
   epsilons.resize(sentLen-1,NULL);

   for (int i = 0; i < sentLen-1;i++)
   {
      epsilons[i] = new double*[hmmModel->statesNum];
      for (int j = 0; j < hmmModel->statesNum;j++)
      {
        epsilons[i][j]=new double [hmmModel->statesNum];
      }
   }
}
HMM::~HMM()
{
  if (nodemap!=NULL)
  {
    for (int i = 0; i < sentLen; i++)
    {
      delete [] nodemap[i];
      nodemap[i] = NULL;
    }
    delete [] nodemap;
    nodemap = NULL;
  }
  for (int i = 0; i < sentLen-1;i++)
  {

    if (epsilons[i]!=NULL)
    {
      for (int j = 0; j < 4;j++)
      {
        if (epsilons[i][j]!=NULL)
        {
           delete [] epsilons[i][j];
           epsilons[i][j] = NULL;
        }
      }
      delete [] epsilons[i];
      epsilons[i] = NULL;
    }
  }
}

std::vector<std::string> HMM::GetMaxProbSequence(int index,HMMProbability* hmmModel)
{
  std::vector<std::string> sequence;
  int k = sentLen - 1;
  int stateId = index;
  while (k>=0)
  {
    sequence.push_back(hmmModel->Id2States[stateId]);
    stateId = nodemap[k][stateId].leftIndex;
    k--;
  }
  std::reverse(sequence.begin(),sequence.end());
  return sequence;
}
std::vector<std::string> HMM::Predict(HMMProbability* hmmModel)
{
  std::pair<double,int> info = Viterbi(hmmModel);
  return GetMaxProbSequence(info.second,hmmModel);
}

std::pair<double,int> HMM::Viterbi(HMMProbability *hmmModel)
{
  double maxcost = MAX_COST;
  int bestIndex = -1;
  for (int k = 0; k < sentLen;k++)
  {
      for (int j = 0; j<hmmModel->statesNum;j++)
      {
        if (k==0)
          {
            nodemap[k][j].cost=log(hmmModel->states[j])+log(hmmModel->emits[j][symbolIds[k]]);
            //std::cerr<<"k-1="<<k-1<<",j="<<j<<"\t"<<log(hmmModel->states[j])<<"\t"<<log(hmmModel->emits[j][symbolIds[k]])<<"\t symbolId="<<symbolIds[k]<<"\t"<<maxcost<<std::endl;
            //std::cerr<<nodemap[k][j].cost<<std::endl;
          }
          else
          {
            maxcost = MAX_COST;
            bestIndex = -1;
            for (int i = 0; i < hmmModel->statesNum;i++)
            {
                //std::cerr<<"k-1="<<k-1<<",i="<<i<<",j="<<j<<" "<<nodemap[k-1][i].cost<<"\t"<<log(hmmModel->transits[i][j])<<"\t"<<log(hmmModel->emits[j][symbolIds[k]])<<"\t symbolId="<<symbolIds[k]<<"\t"<<maxcost<<std::endl;
                //std::cerr << nodemap[k-1][i].cost+log(hmmModel->transits[i][j]) << std::endl;

              if ((nodemap[k-1][i].cost+log(hmmModel->transits[i][j])) > maxcost)
              {
                  bestIndex = i;
                  maxcost = nodemap[k-1][i].cost + log(hmmModel->transits[i][j]);
                  //std::cerr << "in loop bestIndex="<<bestIndex<<" maxcost="<<maxcost<<std::endl;
              }
            }
            maxcost += log(hmmModel->emits[j][symbolIds[k]]);
            nodemap[k][j].cost = maxcost;
            nodemap[k][j].leftIndex = bestIndex;
            nodemap[k-1][bestIndex].rightIndex=j;
            if (k==sentLen-1)
            {
              nodemap[k][j].cost+=log(hmmModel->endstates[j]);
            }
          }
      }
      if (k==sentLen-1)
      {
        maxcost = MAX_COST;
        bestIndex = -1;
        for (int i = 0; i < hmmModel->statesNum;i++)
        {
           if (nodemap[k][i].cost > maxcost)
           {
             maxcost = nodemap[k][i].cost;
             bestIndex = i;
           }
        }
      }
  }
  /*
  for (int i = 0 ; i < sentLen; i++)
  {
    for (int j = 0; j <hmmModel->statesNum;j++)
    {
          std::cerr<<"i="<<i<<","<<"j="<<j<<" "<< nodemap[i][j].leftIndex<<" "<<nodemap[i][j].rightIndex<<" "<<nodemap[i][j].cost << std::endl;
    }
  }
  */
  return std::make_pair(maxcost, bestIndex);
}

/************************************************
 * calculate alpha
 ************************************************/
void HMM::Forward(HMMProbability* hmmModel)
{
  for (int i = 0; i < sentLen;i++)
  {
    for (int j = 0; j < hmmModel->statesNum;j++)
    {
      if (i==0)
      {
        nodemap[i][j].alpha=log(hmmModel->states[j])+log(hmmModel->emits[j][symbolIds[i]]);
        //std::cerr<<"state"<<j<<":"<<log(hmmModel->states[j])<<std::endl;
        //std::cerr<<log(hmmModel->emits[j][symbolIds[i]])<<std::endl;
        std::cerr<<"i="<<i<<",j="<<j<<" alpha="<<nodemap[i][j].alpha<<std::endl;
      }
      else
      {
         for (int k = 0; k < hmmModel->statesNum;k++)
         {
          // std::cerr <<nodemap[i-1][k].alpha<<"\t"<<log(hmmModel->transits[k][j])<<std::endl;
           nodemap[i][j].alpha+=exp(nodemap[i-1][k].alpha+log(hmmModel->transits[k][j]));   
         }
         nodemap[i][j].alpha=log(nodemap[i][j].alpha)+log(hmmModel->emits[j][symbolIds[i]]);
         //std::cerr << log(hmmModel->emits[j][symbolIds[i]])<<std::endl;
         std::cerr<<"i="<<i<<",j="<<j<<" alpha="<<nodemap[i][j].alpha<<std::endl;
      }
    }
  }

}


/******************************************************
 *calculate beta
 *******************************************************/
 void HMM::Backward(HMMProbability* hmmModel)
 {
   for (int i = sentLen - 1; i>=0; i--)
   {
     for (int j = 0; j < hmmModel->statesNum;j++)
     {
       if (i==sentLen-1)
       {
         nodemap[i][j].beta = 0.0;
         std::cerr<<"i="<<i<<",j="<<j<<" beta="<<nodemap[i][j].beta<<std::endl;
       }
       else
       {
         for (int k = 0; k < hmmModel->statesNum;k++)
         {
           nodemap[i][j].beta+=exp(nodemap[i+1][k].beta+log(hmmModel->transits[j][k])+log(hmmModel->emits[k][symbolIds[i+1]]));
         }
         nodemap[i][j].beta = log(nodemap[i][j].beta);
         std::cerr<<"i="<<i<<",j="<<j<<" beta="<<nodemap[i][j].beta<<std::endl;
       }
     }
   }
 }


void HMM::FillGammas(HMMProbability* hmmModel)
{

  for (int i = 0; i < sentLen;i++)
  {
     double denominator = 0.0;
     for (int j = 0; j < hmmModel->statesNum;j++)
     {
      denominator+=exp(nodemap[i][j].alpha+nodemap[i][j].beta);
     }
     for (int j = 0;j < hmmModel->statesNum;j++)
     {
       gammas[i][j]=(nodemap[i][j].alpha+nodemap[i][j].beta)-log(denominator);
       std::cerr<<"i="<<i<<",j="<<j<<" gammas"<<gammas[i][j]<<std::endl;
     }
  }
}
void HMM:: FillEpsilons(HMMProbability *hmmModel)
{
  for (int k = 0; k < sentLen-1;k++)
  {
    double denominator = 0.0;
    for (int i = 0; i < hmmModel->statesNum;i++)
    {
      for (int j = 0; j < hmmModel->statesNum;j++)
      {
        epsilons[k][i][j]=(nodemap[k][i].alpha+log(hmmModel->transits[i][j])+log(hmmModel->emits[j][symbolIds[k+1]])+nodemap[k+1][j].beta);
        denominator+=exp(epsilons[k][i][j]);
      }
    }
    for (int i = 0; i < hmmModel->statesNum;i++)
    {
      for (int j = 0; j < hmmModel->statesNum;j++)
      {
          epsilons[k][i][j]=epsilons[k][i][j]-log(denominator);
          std::cerr<<"k="<<k<<",i="<<i<<",j="<<j<<" epsilons="<<epsilons[k][i][j]<<std::endl;
      }
    }
  }
}

void HMM::BaumWelch(HMMProbability *hmmModel)
{
  //estimation for states;
  for (int i = 0; i < hmmModel->statesNum;i++)
  {
    hmmModel->states[i]=exp(gammas[0][i]);
    hmmModel->endstates[i]=exp(gammas[sentLen-1][i]);
  }
  
  //estimation for transits;
  for (int i = 0; i < hmmModel->statesNum;i++)
  {
     double denominator = 0.0;
     for (int k = 0;k<sentLen-1;k++)
     {
       denominator+=exp(gammas[k][i]);
     }
     for (int j = 0; j < hmmModel->statesNum;j++)
     {
         double numerator = 0.0;
         for (int k = 0; k < sentLen-1;k++)
         {
            numerator+=exp(epsilons[k][i][j]);    
         }
         hmmModel->transits[i][j]=numerator/denominator;
     }
  }

  //estimation for emits
  for (int i = 0; i < hmmModel->statesNum;i++)
  {
    double denominator = 0.0;
    for (int k = 0; k < sentLen;k++)
    {
       denominator+=exp(gammas[k][i]);
    }
    for (int j = 0; j < hmmModel->symbolsNum;j++)
    {
      double numerator = 0.0;
      for (int k = 0; k < sentLen;k++)
      {
         if (symbolIds[k]==j)
          {
            numerator+=exp(gammas[k][i]);
          }
      }
      hmmModel->emits[i][j]=numerator/denominator;
    }
    int zerocount = 0;
    for (int j = 0; j < hmmModel->symbolsNum;j++)
    {
      if (hmmModel->emits[i][j]==0)
      {
        zerocount++;
      }
    }
    if (zerocount!=0)
    {
        for (int j = 0; j < hmmModel->symbolsNum;j++)
        {
          if (hmmModel->emits[i][j]!=0)
          {
            hmmModel->emits[i][j]=hmmModel->emits[i][j]*0.95; 
          }
       }
      for (int j = 0; j < hmmModel->symbolsNum;j++)
      {
        if (hmmModel->emits[i][j]==0)
        {
           hmmModel->emits[i][j]=0.05/zerocount; 
        }
      }
    }
  }
}

void HMM::Train(HMMProbability* hmmModel)
{
  Forward(hmmModel);
  Backward(hmmModel);
  FillGammas(hmmModel);
  FillEpsilons(hmmModel);
  BaumWelch(hmmModel);
}
