#ifndef HEMELB_UNITTESTS_NET_LABELLEDREQUEST_H
#define HEMELB_UNITTESTS_NET_LABELLEDREQUEST_H
#include "net/StoredRequest.h"
namespace hemelb
{
  namespace unittests
  {
    namespace net
    {
      class LabelledRequest : public hemelb::net::SimpleRequest
      {
        public:
          const std::string Label;
          LabelledRequest(void *pointer, int count, MPI_Datatype type, proc_t rank, const std::string &label) :
              SimpleRequest(pointer, count, type, rank), Label(label)
          {
          }

          virtual bool EnvelopeIdentical(const SimpleRequest & other)
          {
            bool this_ok = ( (Count == other.Count) && (Rank == other.Rank) && (Type == other.Type));
            if (!this_ok)
            {
              std::cerr << "Envelope different: " << Label << " R: " << Rank << " C: " << Count << " T: " << " : "
                  << " R" << other.Rank << " C " << other.Count << " T " << std::flush;
            }
            return this_ok;
          }

          virtual bool PayloadIdentical(const SimpleRequest & other)
          {
            // reduction
            bool ok = true;
            for (int element = 0; element < Count; element++)
            {
              int size;
              MPI_Type_size(other.Type, &size);
              // The below use of unsigned char is not formally correct (due to the possibility of char not having alignment 1)
              // But we cannot currently see a better solution to avoid compiler warnings from void* arithmetic.
              bool this_ok = 0
                  == std::memcmp(static_cast<unsigned char*>(other.Pointer) + size * element,
                                 static_cast<unsigned char*>(Pointer) + size * element,
                                 size);
              if (!this_ok)
              {

                std::cerr << "Unexpected data in request: " << Label << " R " << Rank << " C " << Count << " : "
                    << std::endl;
                for (int i = 0; i < size; i++)
                {
                  // The below use of unsigned char is not formally correct (due to the possibility of char not having alignment 1)
                  // But we cannot currently see a better solution to avoid compiler warnings from void* arithmetic.
                  std::cerr << i << " : "
                      << static_cast<int>(* (static_cast<unsigned char*>(Pointer) + size * element + i)) << " "
                      << static_cast<int>(* (static_cast<char*>(other.Pointer) + size * element + i)) << std::endl;
                }
                std::cerr << std::endl;
              }
              ok = ok && this_ok;
            }
            return ok;
          }

          virtual void Unpack(SimpleRequest & other)
          {
            for (int element = 0; element < Count; element++)
            {
              int size;
              MPI_Type_size(other.Type, &size);
              // The below use of unsigned char is not formally correct (due to the possibility of char not having alignment 1)
              // But we cannot currently see a better solution to avoid compiler warnings from void* arithmetic.
              std::memcpy(static_cast<unsigned char*>(other.Pointer) + size * element,
                          static_cast<unsigned char*>(Pointer) + size * element,
                          size);
            }
          }
      };
    }
  }
}

#endif