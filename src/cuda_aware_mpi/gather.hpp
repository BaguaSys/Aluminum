////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2018, Lawrence Livermore National Security, LLC.  Produced at the
// Lawrence Livermore National Laboratory in collaboration with University of
// Illinois Urbana-Champaign.
//
// Written by the LBANN Research Team (N. Dryden, N. Maruyama, et al.) listed in
// the CONTRIBUTORS file. <lbann-dev@llnl.gov>
//
// LLNL-CODE-756777.
// All rights reserved.
//
// This file is part of Aluminum GPU-aware Communication Library. For details, see
// http://software.llnl.gov/Aluminum or https://github.com/LLNL/Aluminum.
//
// Licensed under the Apache License, Version 2.0 (the "Licensee"); you
// may not use this file except in compliance with the License.  You may
// obtain a copy of the License at:
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
// implied. See the License for the specific language governing
// permissions and limitations under the license.
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "cuda.hpp"
#include "progress.hpp"
#include "cuda_aware_mpi_impl.hpp"
#include "mpi_impl.hpp"
#include "cuda_aware_mpi/communicator.hpp"
#include "cuda_aware_mpi/base_state.hpp"

namespace Al {
namespace internal {
namespace cuda_aware_mpi {

/** Progress engine state for CUDA-aware MPI gather. */
template <typename T>
class GatherState : public CUDAAwareMPIState {
 public:
  GatherState(const T* sendbuf_, T* recvbuf_, size_t count_, int root_,
             CUDAAwareMPICommunicator& comm_,
             cudaStream_t stream) :
    CUDAAwareMPIState(stream),
    compute_stream(comm_.get_stream()),
    comm(comm_),
    sendbuf(sendbuf_), recvbuf(recvbuf_), count(count_), root(root_) {}

  bool needs_completion() const override { return false; }
  void* get_compute_stream() const override { return compute_stream; }

  std::string get_name() const override { return "CUDAAwareGather"; }
  std::string get_desc() const override {
    return "";
  }

 protected:
  void start_mpi_op() override {
    if (sendbuf == IN_PLACE<T>()) {
        MPI_Igather(MPI_IN_PLACE, count, mpi::TypeMap<T>(),
                    recvbuf, count, mpi::TypeMap<T>(),
                    root, comm.get_comm(), get_mpi_req());
      } else {
        MPI_Igather(sendbuf, count, mpi::TypeMap<T>(),
                    recvbuf, count, mpi::TypeMap<T>(),
                    root, comm.get_comm(), get_mpi_req());
      }
  }
 private:
  cudaStream_t compute_stream;

  CUDAAwareMPICommunicator& comm;
  const T* sendbuf;
  T* recvbuf;
  size_t count;
  int root;
};

}  // namespace cuda_aware_mpi
}  // namespace internal
}  // namespace Al
