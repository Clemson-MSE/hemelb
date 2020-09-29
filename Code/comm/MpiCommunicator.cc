// This file is part of HemeLB and is Copyright (C)
// the HemeLB team and/or their institutions, as detailed in the
// file AUTHORS. This software is provided under the terms of the
// license in the file LICENSE.

#include "comm/MpiCommunicator.h"
#include "comm/MpiError.h"
#include "comm/MpiFile.h"
#include "comm/MpiRequest.h"
#include "comm/MpiGroup.h"

namespace hemelb
{
  namespace comm
  {
    
    namespace
    {
      void Deleter(MPI_Comm* comm)
      {
        int finalized;
        HEMELB_MPI_CALL(MPI_Finalized, (&finalized));
        if (!finalized)
          HEMELB_MPI_CALL(MPI_Comm_free, (comm));
        delete comm;
      }
    }

    MpiCommunicator::MpiCommunicator() : commPtr()
    {
    }

    MpiCommunicator::MpiCommunicator(MPI_Comm communicator, bool owner) : commPtr()
    {
      if (communicator == MPI_COMM_NULL)
        return;

      if (owner)
      {
        commPtr.reset(new MPI_Comm(communicator), Deleter);
      }
      else
      {
        commPtr.reset(new MPI_Comm(communicator));
      }
    }
    
    MpiCommunicator::operator MPI_Comm() const
    {
      return *commPtr;
    }
    
    MpiCommunicator::~MpiCommunicator()
    {
    }
    int MpiCommunicator::Rank() const
    {
      int rank;
      HEMELB_MPI_CALL(MPI_Comm_rank, (*commPtr, &rank));
      return rank;
    }

    int MpiCommunicator::Size() const
    {
      int size;
      HEMELB_MPI_CALL(MPI_Comm_size, (*commPtr, &size));
      return size;
    }

    void MpiCommunicator::Abort(int errCode) const
    {
      HEMELB_MPI_CALL(MPI_Abort, (*commPtr, errCode));
    }

    Communicator::Ptr MpiCommunicator::Duplicate() const
    {
      MPI_Comm newComm;
      HEMELB_MPI_CALL(MPI_Comm_dup, (*commPtr, &newComm));
      return std::make_shared<MpiCommunicator>(newComm, true);
    }
    Group::Ptr MpiCommunicator::GetGroup() const
    {
      MPI_Group grp;
      HEMELB_MPI_CALL(MPI_Comm_group, (*commPtr, &grp));
      return std::make_shared<MpiGroup>(grp, true);
    }
    
    Communicator::Ptr MpiCommunicator::Create(std::shared_ptr<const Group> grp) const
    {
      auto mpiGrp = std::dynamic_pointer_cast<const MpiGroup>(grp);
      
      MPI_Comm newComm;
      HEMELB_MPI_CALL(MPI_Comm_create, (*commPtr, *mpiGrp, &newComm));
      return std::make_shared<MpiCommunicator>(newComm, true);
    }
    
    MpiFile::Ptr MpiCommunicator::OpenFile(const std::string& filename, int mode,
				       const MPI_Info info) const
    {
      MPI_File ans;
      HEMELB_MPI_CALL(
          MPI_File_open,
          (*this, filename.c_str(), mode, info, &ans)
      );
      return std::make_shared<MpiFile>(shared_from_this(), ans);
    }

    void MpiCommunicator::Barrier() const
    {
      HEMELB_MPI_CALL(MPI_Barrier, (*commPtr));
    }

    std::shared_ptr<Request> MpiCommunicator::Ibarrier() const
    {
      MPI_Request req;
      HEMELB_MPI_CALL(MPI_Ibarrier, (*commPtr, &req));
      return std::make_shared<MpiRequest>(req);
    }

    bool MpiCommunicator::Iprobe(int source, int tag, MPI_Status* stat) const
    {
      int flag;
      HEMELB_MPI_CALL(
          MPI_Iprobe,
          (source, tag, *commPtr, &flag, stat)
      );
      return flag;
    }

    
    void MpiCommunicator::BcastImpl(void* buf, int count, MPI_Datatype dt,
				    int root) const
    {
      HEMELB_MPI_CALL(
		      MPI_Bcast,
		      (buf, count, dt, root, *commPtr)
      );
    }
    
    std::shared_ptr<Request> MpiCommunicator::IbcastImpl(void* buf, int count, MPI_Datatype dt,
					 int root) const
    {
      MPI_Request req;
      HEMELB_MPI_CALL(
          MPI_Ibcast,
          (buf, count, dt, root, *commPtr, &req)
      );
      return std::make_shared<MpiRequest>(req);
    }
    
    void MpiCommunicator::AllreduceImpl(const void* send, void* ans, int count, MPI_Datatype dt,
					MPI_Op op) const
    {
      HEMELB_MPI_CALL(
          MPI_Allreduce,
          (send, ans, count, dt, op, *commPtr)
      );
    }
    
    std::shared_ptr<Request> MpiCommunicator::IallreduceImpl(const void* send, void* ans, int count, MPI_Datatype dt,
					     MPI_Op op) const
    {
      MPI_Request req;
      HEMELB_MPI_CALL(
         MPI_Iallreduce,
	 (send, ans, count, dt, op, *commPtr, &req)
      );
      return std::make_shared<MpiRequest>(req);
    }
    
    std::shared_ptr<Request> MpiCommunicator::IreduceImpl(const void* send, void* ans, int count, MPI_Datatype dt,
					  MPI_Op op, int root) const
    {
      MPI_Request req;
      HEMELB_MPI_CALL(
          MPI_Ireduce,
          (send, ans, count, dt, op, root, *commPtr, &req)
      );
      return std::make_shared<MpiRequest>(req);
    }
    
    void MpiCommunicator::ReduceImpl(const void* send, void* ans, int count, MPI_Datatype dt,
				     MPI_Op op, int root) const
    {
      HEMELB_MPI_CALL(
          MPI_Reduce,
          (send, ans, count, dt, op, root, *commPtr)
      );
    }
    
    void MpiCommunicator::GatherImpl(const void* send, int sendcount, MPI_Datatype sendtype,
				     void* recv, int recvcount, MPI_Datatype recvtype,
				     int root) const
    {
      HEMELB_MPI_CALL(
          MPI_Gather,
          (send, sendcount, sendtype,
	   recv, recvcount, recvtype,
	   root, *commPtr)
      );
    }
    
    void MpiCommunicator::GathervImpl(const void *sendbuf, int sendcount, MPI_Datatype sendtype,
				      void *recvbuf, const int* recvcounts, const int* displs, MPI_Datatype recvtype,
				      int root) const
    {
      HEMELB_MPI_CALL(
          MPI_Gatherv,
          (sendbuf, sendcount, sendtype,
	   recvbuf, recvcounts, displs, recvtype,
	   root, *commPtr)
      );
    }
    void MpiCommunicator::AllgatherImpl(const void* send, int sendcount, MPI_Datatype sendtype,
					void* recv, int recvcount, MPI_Datatype recvtype) const
    {
      HEMELB_MPI_CALL(
          MPI_Allgather,
          (send, sendcount, sendtype,
	   recv, recvcount, recvtype,
	   *commPtr)
      );
    }
    
    void MpiCommunicator::AlltoallImpl(const void* send, int sendcount, MPI_Datatype sendtype,
				       void* recv, int recvcount, MPI_Datatype recvtype) const
    {
      HEMELB_MPI_CALL(
          MPI_Alltoall,
          (send, sendcount, sendtype,
	   recv, recvcount, recvtype,
	   *commPtr)
      );
    }
    
    void MpiCommunicator::SendImpl(const void *sendbuf, int sendcount, MPI_Datatype sendtype,
				   int dest, int tag) const
    {
      HEMELB_MPI_CALL(
		      MPI_Send,
		      (sendbuf, sendcount, sendtype,
		       dest, tag, *commPtr)
		      );
    }
    void MpiCommunicator::RecvImpl(void* recvbuf, int recvcount, MPI_Datatype recvtype,
				   int src, int tag, MPI_Status* stat) const
    {
      HEMELB_MPI_CALL(
		      MPI_Recv,
		      (recvbuf, recvcount, recvtype,
		       src, tag, *commPtr, stat)
		      );
    }
    
     std::shared_ptr<Request> MpiCommunicator::IsendImpl(const void* sendbuf, int sendcount, MPI_Datatype sendtype,
				 int dest, int tag) const
     {
       MPI_Request ans;
       HEMELB_MPI_CALL(
		       MPI_Isend,
		       (sendbuf, sendcount, sendtype,
			dest, tag, *commPtr, &ans)
		      );
       return std::make_shared<MpiRequest>(ans);
     }
    
     std::shared_ptr<Request> MpiCommunicator::IssendImpl(const void* sendbuf, int sendcount, MPI_Datatype sendtype,
				  int dest, int tag) const
     {
       MPI_Request ans;
       HEMELB_MPI_CALL(
		       MPI_Issend,
		       (sendbuf, sendcount, sendtype,
			dest, tag, *commPtr, &ans)
		      );
       return std::make_shared<MpiRequest>(ans);
     }
    
     std::shared_ptr<Request> MpiCommunicator::IrecvImpl(void* recvbuf, int recvcount, MPI_Datatype recvtype,
				 int source, int tag) const
     {
       MPI_Request ans;
       HEMELB_MPI_CALL(
		       MPI_Irecv,
		       (recvbuf, recvcount, recvtype,
			source, tag, *commPtr, &ans)
		       );
       return std::make_shared<MpiRequest>(ans);
     }
   
  }
}

