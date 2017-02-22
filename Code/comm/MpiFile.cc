
// This file is part of HemeLB and is Copyright (C)
// the HemeLB team and/or their institutions, as detailed in the
// file AUTHORS. This software is provided under the terms of the
// license in the file LICENSE.

#include "comm/MpiFile.h"
#include "comm/MpiCommunicator.h"

namespace hemelb
{
  namespace comm
  {
    namespace
    {
      void Deleter(MPI_File* fh)
      {
        HEMELB_MPI_CALL(MPI_File_close, (fh));
        delete fh;
      }
    }
    MpiFile::MpiFile() : comm(NULL)
    {

    }

    MpiFile::MpiFile(Communicator::ConstPtr parentComm, MPI_File fh) :
        comm(parentComm)
    {
      filePtr.reset(new MPI_File(fh), Deleter);
    }


    void MpiFile::Close()
    {
      if (filePtr)
      {
        filePtr.reset();
      }
    }

    MpiFile::operator MPI_File() const
    {
      return *filePtr;
    }

    Communicator::ConstPtr MpiFile::GetCommunicator() const
    {
      return comm;
    }

    void MpiFile::SetView(MPI_Offset disp, MPI_Datatype etype, MPI_Datatype filetype, const std::string& datarep, MPI_Info info)
    {
      HEMELB_MPI_CALL(
          MPI_File_set_view,
          (*filePtr, disp, etype, filetype, datarep.c_str(), info)
      );
    }
  }
}

