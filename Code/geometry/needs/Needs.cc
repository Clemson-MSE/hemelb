
// This file is part of HemeLB and is Copyright (C)
// the HemeLB team and/or their institutions, as detailed in the
// file AUTHORS. This software is provided under the terms of the
// license in the file LICENSE.

#include "geometry/needs/Needs.h"
#include "log/Logger.h"
#include "util/utilityFunctions.h"

namespace hemelb
{
  namespace geometry
  {
    Needs::Needs(const site_t blockCount,
                 const std::vector<bool>& readBlock,
                 const proc_t readingGroupSize,
                 comm::Communicator::ConstPtr comm,
                 bool shouldValidate_) :
        procsWantingBlocksBuffer(blockCount), communicator(comm), readingGroupSize(readingGroupSize), shouldValidate(shouldValidate_)
    {
      // Compile the blocks needed here into an array of indices, instead of an array of bools
      std::vector<std::vector<site_t> > blocksNeededHere(readingGroupSize);
      for (site_t block = 0; block < blockCount; ++block)
      {
        if (readBlock[block])
        {
          blocksNeededHere[GetReadingCoreForBlock(block)].push_back(block);
        }
      }

      // Share the counts of needed blocks
      int blocksNeededSize[readingGroupSize];
      std::vector<int> blocksNeededSizes;

      for (proc_t readingCore = 0; readingCore < readingGroupSize; readingCore++)
      {
        blocksNeededSize[readingCore] = blocksNeededHere[readingCore].size();
	auto tmp = communicator->Gather(blocksNeededSize[readingCore], readingCore);
	if (readingCore == communicator->Rank())
	  blocksNeededSizes = std::move(tmp);
      }
      
      // Communicate the arrays of needed blocks
      std::vector<site_t> blocksNeededOn;
      for (proc_t readingCore = 0; readingCore < readingGroupSize; readingCore++)
      {
	auto tmp = communicator->GatherV(blocksNeededHere[readingCore], blocksNeededSizes, readingCore);
	if (readingCore == communicator->Rank())
	  blocksNeededOn = std::move(tmp);
      }

      if (communicator->Rank() < readingGroupSize)
      {
        int needsPassed = 0;
        // Transpose the blocks needed on cores matrix
        for (proc_t sendingCore = 0; sendingCore < communicator->Size(); sendingCore++)
        {
          for (int needForThisSendingCore = 0; needForThisSendingCore < blocksNeededSizes[sendingCore];
              ++needForThisSendingCore)
          {
            procsWantingBlocksBuffer[blocksNeededOn[needsPassed]].push_back(sendingCore);

            ++needsPassed;
          }
        } //for sendingCore
      } //if a reading core

      // If in debug mode, also reduce the old-fashioned way, and check these are the same.
      if (shouldValidate)
      {
        Validate(blockCount, readBlock);
      }
    }

    void Needs::Validate(const site_t blockCount, const std::vector<bool>& readBlock)
    {
      for (site_t block = 0; block < blockCount; ++block)
      {
        int neededHere = readBlock[block];
        proc_t readingCore = GetReadingCoreForBlock(block);
        std::vector<int> procsWantingThisBlockBuffer = communicator->Gather(neededHere, readingCore);

        if (communicator->Rank() == readingCore)
        {

          for (proc_t needingProcOld = 0; needingProcOld < communicator->Size(); needingProcOld++)
          {
            bool found = false;
            for (std::vector<proc_t>::iterator needingProc = procsWantingBlocksBuffer[block].begin();
                needingProc != procsWantingBlocksBuffer[block].end(); needingProc++)
            {
              if (*needingProc == needingProcOld)
              {
                found = true;
              }
            }
            if (found && (!procsWantingThisBlockBuffer[needingProcOld]))
            {
              log::Logger::Log<log::Critical, log::OnePerCore>("Was not expecting block %i to be needed on proc %i, but was",
                                                            block,
                                                            needingProcOld);
            }
            if ( (!found) && procsWantingThisBlockBuffer[needingProcOld] && (needingProcOld != readingCore))
            {
              log::Logger::Log<log::Critical, log::OnePerCore>("Was expecting block %i to be needed on proc %i, but was not",
                                                            block,
                                                            needingProcOld);
            } //if problem
          } // for old proc
        } // if reading group
      } // for blocks
    } //constructor

    proc_t Needs::GetReadingCoreForBlock(const site_t blockNumber) const
    {
      return proc_t(blockNumber % readingGroupSize);
    }
  } //namespace
} //namespace
