// @HEADER
// ***********************************************************************
//
//          Tpetra: Templated Linear Algebra Services Package
//                 Copyright (2008) Sandia Corporation
//
// Under the terms of Contract DE-AC04-94AL85000 with Sandia Corporation,
// the U.S. Government retains certain rights in this software.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
// 1. Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
//
// 3. Neither the name of the Corporation nor the names of the
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY SANDIA CORPORATION "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL SANDIA CORPORATION OR THE
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// ************************************************************************
// @HEADER

#ifndef TPETRA_DETAILS_CHECKMEMORYTYPE_HPP
#define TPETRA_DETAILS_CHECKMEMORYTYPE_HPP

/// \file Tpetra_Details_checkMemoryType.hpp
/// \brief Declaration of functions for checking whether a given
///   Kokkos::View has a pointer that matches the claimed memory
///   space.

#include "TpetraCore_config.h"

#ifdef HAVE_TPETRACORE_CUDA
#ifndef DOXYGEN_SHOULD_SKIP_THIS
namespace Kokkos {
  class CudaSpace; // forward declarations
  class CudaUVMSpace;
  class CudaHostPinnedSpace;
} // namespace Kokkos
#endif // DOXYGEN_SHOULD_SKIP_THIS
#endif // HAVE_TPETRACORE_CUDA

namespace Tpetra {
namespace Details {
namespace Impl {

template<class MemorySpace>
struct CheckMemorySpace {
  static bool inSpace (const void* /* ptr */) {
    return true;
  }
};

#ifdef HAVE_TPETRACORE_CUDA
template<>
struct CheckMemorySpace<Kokkos::CudaSpace> {
  static bool inSpace (const void* ptr);
};

template<>
struct CheckMemorySpace<Kokkos::CudaUVMSpace> {
  static bool inSpace (const void* ptr);
};

template<>
struct CheckMemorySpace<Kokkos::CudaHostPinnedSpace> {
  static bool inSpace (const void* ptr);
};
#endif // HAVE_TPETRACORE_CUDA

} // namespace Impl

/// \brief Is \c ptr in the given memory space?
///
/// This function doesn't promise exact results for anything other
/// than CudaSpace, CudaUVMSpace, or CudaHostPinnedSpace.  The point
/// of this function is for Tpetra classes to debug user error in
/// which users create a Kokkos::View with a raw pointer in the wrong
/// memory space (e.g., a host pointer, when they should have used a
/// UVM pointer).
template<class MemorySpace>
bool inMemorySpace (const void* ptr, const MemorySpace& /* space */)
{
  return Impl::CheckMemorySpace<MemorySpace>::inSpace (ptr);
}

/// \brief Return the Kokkos memory space name (without "Kokkos::")
///   corresponding to \c ptr.
///
/// This function doesn't promise exact results for anything other
/// than CudaSpace, CudaUVMSpace, or CudaHostPinnedSpace.
std::string memorySpaceName (const void* ptr);

} // namespace Details
} // namespace Tpetra

#endif // TPETRA_DETAILS_CHECKMEMORYTYPE_HPP
