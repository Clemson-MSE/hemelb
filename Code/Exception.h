//
// Copyright (C) University College London, 2007-2012, all rights reserved.
//
// This file is part of HemeLB and is CONFIDENTIAL. You may not work
// with, install, use, duplicate, modify, redistribute or share this
// file, or any part thereof, other than as allowed by any agreement
// specifically made by you with University College London.
//

#ifndef HEMELB_EXCEPTION_H
#define HEMELB_EXCEPTION_H

#include <sstream>
#include <stdexcept>
#include <string>

namespace hemelb
{
  /**
   * Exception class for HemeLB
   *
   * Implements an ostream interface so you can perform formatted output to
   * it easily like:
   *
   * throw Exception() << "Simulation unstable at t = " << currentTime;
   *
   */
  class Exception : public std::exception
  {
    public:
      /**
       * ctor
       */
      Exception()
      {
      }
      ;
      /**
       * Copy ctor required -- note that it appends the old content of the
       * stream to the message of the new one and gives the new one an empty
       * stream.
       *
       * @param that
       */
      Exception(const Exception& that)
      {
        mWhat += that.mStream.str();
      }
      virtual ~Exception() throw ()
      {
      }
      ;

      /**
       * Return combined error string.
       * @return
       */
      virtual const char *what() const throw ()
      {
        if (mStream.str().size())
        {
          mWhat += mStream.str();
          mStream.str("");
        }
        return mWhat.c_str();
      }

      /**
       * Delegate output operations to the Stream.
       * @param t
       * @return
       */
      template<typename T>
      Exception& operator<<(const T& t)
      {
        mStream << t;
        return *this;
      }

    private:
      // Note that these are mutable
      mutable std::stringstream mStream;
      mutable std::string mWhat;
  };
}
#endif // HEMELB_EXCEPTION_H
