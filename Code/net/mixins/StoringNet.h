
// This file is part of HemeLB and is Copyright (C)
// the HemeLB team and/or their institutions, as detailed in the
// file AUTHORS. This software is provided under the terms of the
// license in the file LICENSE.

#ifndef HEMELB_NET_MIXINS_STORINGNET_H
#define HEMELB_NET_MIXINS_STORINGNET_H
#include "net/BaseNet.h"
#include "log/Logger.h"
#include "net/ProcComms.h"
namespace hemelb
{
  namespace net
  {
    class StoringNet : public virtual BaseNet
    {
      public:
      StoringNet(comm::Communicator::ConstPtr comms);

        virtual void RequestSendImpl(void* pointer, int count, proc_t rank, MPI_Datatype type);
        virtual void RequestReceiveImpl(void* pointer, int count, proc_t rank, MPI_Datatype type);


      protected:
        /**
         * Struct representing all that's needed to successfully communicate with another processor.
         */

        std::map<proc_t, ProcComms> sendProcessorComms;
        std::map<proc_t, ProcComms> receiveProcessorComms;

    };
  }
}
#endif
