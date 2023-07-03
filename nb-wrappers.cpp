#include <assert.h>
#include <mpi.h>
#include <stdio.h>

#ifndef NOTOOL
#include "tracking.h"

/* P2P Communication */

int MPI_Isend(const void *buf, int count, MPI_Datatype datatype, int dest,
              int tag, MPI_Comm comm, MPI_Request *request) {
  MPI_Comm myComm = cf.getHandlePool()->getHandle(comm);
  int ret = PMPI_Isend(buf, count, datatype, dest, tag, myComm, request);
#ifdef TRACK_DATA
  p2pRequest reqData{nbf_MPI_Isend, buf, count, datatype, dest, tag, comm};
#else
  p2pRequest reqData{};
#endif
  *request = rf.getHandlePool()->newRequest(*request, reqData);
  return ret;
}

int MPI_Issend(const void *buf, int count, MPI_Datatype datatype, int dest,
               int tag, MPI_Comm comm, MPI_Request *request) {
  MPI_Comm myComm = cf.getHandlePool()->getHandle(comm);
  int ret = PMPI_Issend(buf, count, datatype, dest, tag, myComm, request);
  p2pRequest reqData{nbf_MPI_Issend, buf, count, datatype, dest, tag, comm};
  *request = rf.getHandlePool()->newRequest(*request, reqData);
  return ret;
}

int MPI_Ibsend(const void *buf, int count, MPI_Datatype datatype, int dest,
               int tag, MPI_Comm comm, MPI_Request *request) {
  MPI_Comm myComm = cf.getHandlePool()->getHandle(comm);
  int ret = PMPI_Ibsend(buf, count, datatype, dest, tag, myComm, request);
  p2pRequest reqData{nbf_MPI_Ibsend, buf, count, datatype, dest, tag, comm};
  *request = rf.getHandlePool()->newRequest(*request, reqData);
  return ret;
}

int MPI_Irsend(const void *buf, int count, MPI_Datatype datatype, int dest,
               int tag, MPI_Comm comm, MPI_Request *request) {
  MPI_Comm myComm = cf.getHandlePool()->getHandle(comm);
  int ret = PMPI_Irsend(buf, count, datatype, dest, tag, myComm, request);
  p2pRequest reqData{nbf_MPI_Irsend, buf, count, datatype, dest, tag, comm};
  *request = rf.getHandlePool()->newRequest(*request, reqData);
  return ret;
}

int MPI_Irecv(void *buf, int count, MPI_Datatype datatype, int source, int tag,
              MPI_Comm comm, MPI_Request *request) {
  MPI_Comm myComm = cf.getHandlePool()->getHandle(comm);
  int ret = PMPI_Irecv(buf, count, datatype, source, tag, myComm, request);
#ifdef TRACK_DATA
  p2pRequest reqData{nbf_MPI_Irecv, buf, count, datatype, source, tag, comm};
#else
  p2pRequest reqData{};
#endif
  *request = rf.getHandlePool()->newRequest(*request, reqData);
  return ret;
}

/* Persistent P2P Communication */

int MPI_Send_init(const void *buf, int count, MPI_Datatype datatype, int dest,
                  int tag, MPI_Comm comm, MPI_Request *request) {
  MPI_Comm myComm = cf.getHandlePool()->getHandle(comm);
  int ret = PMPI_Send_init(buf, count, datatype, dest, tag, myComm, request);
  p2pRequest reqData{nbf_MPI_Send_init, buf, count, datatype, dest, tag, comm};
  *request = rf.getHandlePool()->newRequest(*request, reqData, true);
  return ret;
}

int MPI_Ssend_init(const void *buf, int count, MPI_Datatype datatype, int dest,
                   int tag, MPI_Comm comm, MPI_Request *request) {
  MPI_Comm myComm = cf.getHandlePool()->getHandle(comm);
  int ret = PMPI_Ssend_init(buf, count, datatype, dest, tag, myComm, request);
  p2pRequest reqData{nbf_MPI_Ssend_init, buf, count, datatype, dest, tag, comm};
  *request = rf.getHandlePool()->newRequest(*request, reqData, true);
  return ret;
}

int MPI_Bsend_init(const void *buf, int count, MPI_Datatype datatype, int dest,
                   int tag, MPI_Comm comm, MPI_Request *request) {
  MPI_Comm myComm = cf.getHandlePool()->getHandle(comm);
  int ret = PMPI_Bsend_init(buf, count, datatype, dest, tag, myComm, request);
  p2pRequest reqData{nbf_MPI_Bsend_init, buf, count, datatype, dest, tag, comm};
  *request = rf.getHandlePool()->newRequest(*request, reqData, true);
  return ret;
}

int MPI_Rsend_init(const void *buf, int count, MPI_Datatype datatype, int dest,
                   int tag, MPI_Comm comm, MPI_Request *request) {
  MPI_Comm myComm = cf.getHandlePool()->getHandle(comm);
  int ret = PMPI_Rsend_init(buf, count, datatype, dest, tag, myComm, request);
  p2pRequest reqData{nbf_MPI_Rsend_init, buf, count, datatype, dest, tag, comm};
  *request = rf.getHandlePool()->newRequest(*request, reqData, true);
  return ret;
}

int MPI_Recv_init(void *buf, int count, MPI_Datatype datatype, int source,
                  int tag, MPI_Comm comm, MPI_Request *request) {
  MPI_Comm myComm = cf.getHandlePool()->getHandle(comm);
  int ret = PMPI_Recv_init(buf, count, datatype, source, tag, myComm, request);
  p2pRequest reqData{
      nbf_MPI_Recv_init, buf, count, datatype, source, tag, comm};
  *request = rf.getHandlePool()->newRequest(*request, reqData, true);
  return ret;
}

/* Collective Communication */

int MPI_Ibarrier(MPI_Comm comm, MPI_Request *request) {
  MPI_Comm myComm = cf.getHandlePool()->getHandle(comm);
  int ret = PMPI_Ibarrier(myComm, request);
  nbcRequest reqData{nbf_MPI_Ibarrier, comm};
  *request = rf.getHandlePool()->newRequest(*request, reqData);
  return ret;
}

int MPI_Iallgather(const void *sendbuf, int sendcount, MPI_Datatype sendtype,
                   void *recvbuf, int recvcount, MPI_Datatype recvtype,
                   MPI_Comm comm, MPI_Request *request) {
  MPI_Comm myComm = cf.getHandlePool()->getHandle(comm);
  int ret = PMPI_Iallgather(sendbuf, sendcount, sendtype, recvbuf, recvcount,
                            recvtype, myComm, request);
  nbcRequest reqData{nbf_MPI_Iallgather,
                     sendbuf,
                     std::vector<int>{1, sendcount},
                     std::vector<int>{},
                     std::vector<MPI_Datatype>{1, sendtype},
                     recvbuf,
                     std::vector<int>{1, recvcount},
                     std::vector<int>{},
                     std::vector<MPI_Datatype>{1, recvtype},
                     comm};
  *request = rf.getHandlePool()->newRequest(*request, reqData);
  return ret;
}

int MPI_Iallgatherv(const void *sendbuf, int sendcount, MPI_Datatype sendtype,
                    void *recvbuf, const int recvcounts[], const int displs[],
                    MPI_Datatype recvtype, MPI_Comm comm,
                    MPI_Request *request) {
  MPI_Comm myComm = cf.getHandlePool()->getHandle(comm);
  int ret = PMPI_Iallgatherv(sendbuf, sendcount, sendtype, recvbuf, recvcounts,
                             displs, recvtype, myComm, request);
  int comm_size;
  PMPI_Comm_size(myComm, &comm_size);
  nbcRequest reqData{nbf_MPI_Iallgatherv,
                     sendbuf,
                     std::vector<int>{1, sendcount},
                     std::vector<int>{},
                     std::vector<MPI_Datatype>{1, sendtype},
                     recvbuf,
                     std::vector<int>{recvcounts, recvcounts + comm_size},
                     std::vector<int>{displs, displs + comm_size},
                     std::vector<MPI_Datatype>{1, recvtype},
                     comm};
  *request = rf.getHandlePool()->newRequest(*request, reqData);
  return ret;
}

int MPI_Ialltoall(const void *sendbuf, int sendcount, MPI_Datatype sendtype,
                  void *recvbuf, int recvcount, MPI_Datatype recvtype,
                  MPI_Comm comm, MPI_Request *request) {
  MPI_Comm myComm = cf.getHandlePool()->getHandle(comm);
  int ret = PMPI_Ialltoall(sendbuf, sendcount, sendtype, recvbuf, recvcount,
                           recvtype, myComm, request);
  nbcRequest reqData{nbf_MPI_Ialltoall,
                     sendbuf,
                     std::vector<int>{1, sendcount},
                     std::vector<int>{},
                     std::vector<MPI_Datatype>{1, sendtype},
                     recvbuf,
                     std::vector<int>{1, recvcount},
                     std::vector<int>{},
                     std::vector<MPI_Datatype>{1, recvtype},
                     comm};
  *request = rf.getHandlePool()->newRequest(*request, reqData);
  return ret;
}

int MPI_Ialltoallv(const void *sendbuf, const int sendcounts[],
                   const int sdispls[], MPI_Datatype sendtype, void *recvbuf,
                   const int recvcounts[], const int rdispls[],
                   MPI_Datatype recvtype, MPI_Comm comm, MPI_Request *request) {
  MPI_Comm myComm = cf.getHandlePool()->getHandle(comm);
  int ret = PMPI_Ialltoallv(sendbuf, sendcounts, sdispls, sendtype, recvbuf,
                            recvcounts, rdispls, recvtype, myComm, request);
  int comm_size;
  PMPI_Comm_size(myComm, &comm_size);
  nbcRequest reqData{nbf_MPI_Ialltoallv,
                     sendbuf,
                     std::vector<int>{sendcounts, sendcounts + comm_size},
                     std::vector<int>{sdispls, sdispls + comm_size},
                     std::vector<MPI_Datatype>{1, sendtype},
                     recvbuf,
                     std::vector<int>{recvcounts, recvcounts + comm_size},
                     std::vector<int>{rdispls, rdispls + comm_size},
                     std::vector<MPI_Datatype>{1, recvtype},
                     comm};
  *request = rf.getHandlePool()->newRequest(*request, reqData);
  return ret;
}

int MPI_Ialltoallw(const void *sendbuf, const int sendcounts[],
                   const int sdispls[], const MPI_Datatype sendtypes[],
                   void *recvbuf, const int recvcounts[], const int rdispls[],
                   const MPI_Datatype recvtypes[], MPI_Comm comm,
                   MPI_Request *request) {
  MPI_Comm myComm = cf.getHandlePool()->getHandle(comm);
  int ret = PMPI_Ialltoallw(sendbuf, sendcounts, sdispls, sendtypes, recvbuf,
                            recvcounts, rdispls, recvtypes, myComm, request);
  int comm_size;
  PMPI_Comm_size(myComm, &comm_size);
  nbcRequest reqData{
      nbf_MPI_Ialltoallw,
      sendbuf,
      std::vector<int>{sendcounts, sendcounts + comm_size},
      std::vector<int>{sdispls, sdispls + comm_size},
      std::vector<MPI_Datatype>{sendtypes, sendtypes + comm_size},
      recvbuf,
      std::vector<int>{recvcounts, recvcounts + comm_size},
      std::vector<int>{rdispls, rdispls + comm_size},
      std::vector<MPI_Datatype>{recvtypes, recvtypes + comm_size},
      comm};
  *request = rf.getHandlePool()->newRequest(*request, reqData);
  return ret;
}

int MPI_Iallreduce(const void *sendbuf, void *recvbuf, int count,
                   MPI_Datatype datatype, MPI_Op op, MPI_Comm comm,
                   MPI_Request *request) {
  MPI_Comm myComm = cf.getHandlePool()->getHandle(comm);
  int ret =
      PMPI_Iallreduce(sendbuf, recvbuf, count, datatype, op, myComm, request);
  nbcRequest reqData{nbf_MPI_Iallreduce,
                     sendbuf,
                     std::vector<int>{1, count},
                     std::vector<int>{},
                     std::vector<MPI_Datatype>{1, datatype},
                     recvbuf,
                     std::vector<int>{},
                     std::vector<int>{},
                     std::vector<MPI_Datatype>{},
                     comm,
                     -1,
                     op};
  *request = rf.getHandlePool()->newRequest(*request, reqData);
  return ret;
}

int MPI_Ireduce_scatter_block(const void *sendbuf, void *recvbuf, int recvcount,
                              MPI_Datatype datatype, MPI_Op op, MPI_Comm comm,
                              MPI_Request *request) {
  MPI_Comm myComm = cf.getHandlePool()->getHandle(comm);
  int ret = PMPI_Ireduce_scatter_block(sendbuf, recvbuf, recvcount, datatype,
                                       op, myComm, request);
  nbcRequest reqData{nbf_MPI_Ireduce_scatter_block,
                     sendbuf,
                     std::vector<int>{},
                     std::vector<int>{},
                     std::vector<MPI_Datatype>{1, datatype},
                     recvbuf,
                     std::vector<int>{1, recvcount},
                     std::vector<int>{},
                     std::vector<MPI_Datatype>{},
                     comm,
                     -1,
                     op};
  *request = rf.getHandlePool()->newRequest(*request, reqData);
  return ret;
}

int MPI_Ireduce_scatter(const void *sendbuf, void *recvbuf,
                        const int recvcounts[], MPI_Datatype datatype,
                        MPI_Op op, MPI_Comm comm, MPI_Request *request) {
  MPI_Comm myComm = cf.getHandlePool()->getHandle(comm);
  int ret = PMPI_Ireduce_scatter(sendbuf, recvbuf, recvcounts, datatype, op,
                                 myComm, request);
  int comm_size;
  PMPI_Comm_size(myComm, &comm_size);
  nbcRequest reqData{nbf_MPI_Ireduce_scatter,
                     sendbuf,
                     std::vector<int>{},
                     std::vector<int>{},
                     std::vector<MPI_Datatype>{1, datatype},
                     recvbuf,
                     std::vector<int>{recvcounts, recvcounts + comm_size},
                     std::vector<int>{},
                     std::vector<MPI_Datatype>{},
                     comm,
                     -1,
                     op};
  *request = rf.getHandlePool()->newRequest(*request, reqData);
  return ret;
}

int MPI_Iscan(const void *sendbuf, void *recvbuf, int count,
              MPI_Datatype datatype, MPI_Op op, MPI_Comm comm,
              MPI_Request *request) {
  MPI_Comm myComm = cf.getHandlePool()->getHandle(comm);
  int ret = PMPI_Iscan(sendbuf, recvbuf, count, datatype, op, myComm, request);
  nbcRequest reqData{nbf_MPI_Iscan,
                     sendbuf,
                     std::vector<int>{1, count},
                     std::vector<int>{},
                     std::vector<MPI_Datatype>{1, datatype},
                     recvbuf,
                     std::vector<int>{},
                     std::vector<int>{},
                     std::vector<MPI_Datatype>{},
                     comm,
                     -1,
                     op};
  *request = rf.getHandlePool()->newRequest(*request, reqData);
  return ret;
}

int MPI_Iexscan(const void *sendbuf, void *recvbuf, int count,
                MPI_Datatype datatype, MPI_Op op, MPI_Comm comm,
                MPI_Request *request) {
  MPI_Comm myComm = cf.getHandlePool()->getHandle(comm);
  int ret =
      PMPI_Iexscan(sendbuf, recvbuf, count, datatype, op, myComm, request);
  nbcRequest reqData{nbf_MPI_Iexscan,
                     sendbuf,
                     std::vector<int>{1, count},
                     std::vector<int>{},
                     std::vector<MPI_Datatype>{1, datatype},
                     recvbuf,
                     std::vector<int>{},
                     std::vector<int>{},
                     std::vector<MPI_Datatype>{},
                     comm,
                     -1,
                     op};
  *request = rf.getHandlePool()->newRequest(*request, reqData);
  return ret;
}

int MPI_Igather(const void *sendbuf, int sendcount, MPI_Datatype sendtype,
                void *recvbuf, int recvcount, MPI_Datatype recvtype, int root,
                MPI_Comm comm, MPI_Request *request) {
  MPI_Comm myComm = cf.getHandlePool()->getHandle(comm);
  int ret = PMPI_Igather(sendbuf, sendcount, sendtype, recvbuf, recvcount,
                         recvtype, root, myComm, request);
  nbcRequest reqData{nbf_MPI_Igather,
                     sendbuf,
                     std::vector<int>{1, sendcount},
                     std::vector<int>{},
                     std::vector<MPI_Datatype>{1, sendtype},
                     recvbuf,
                     std::vector<int>{1, recvcount},
                     std::vector<int>{},
                     std::vector<MPI_Datatype>{1, recvtype},
                     comm,
                     root};
  *request = rf.getHandlePool()->newRequest(*request, reqData);
  return ret;
}

int MPI_Igatherv(const void *sendbuf, int sendcount, MPI_Datatype sendtype,
                 void *recvbuf, const int recvcounts[], const int displs[],
                 MPI_Datatype recvtype, int root, MPI_Comm comm,
                 MPI_Request *request) {
  MPI_Comm myComm = cf.getHandlePool()->getHandle(comm);
  int ret = PMPI_Igatherv(sendbuf, sendcount, sendtype, recvbuf, recvcounts,
                          displs, recvtype, root, myComm, request);
  int comm_size;
  PMPI_Comm_size(myComm, &comm_size);
  nbcRequest reqData{nbf_MPI_Igatherv,
                     sendbuf,
                     std::vector<int>{1, sendcount},
                     std::vector<int>{},
                     std::vector<MPI_Datatype>{1, sendtype},
                     recvbuf,
                     std::vector<int>{recvcounts, recvcounts + comm_size},
                     std::vector<int>{displs, displs + comm_size},
                     std::vector<MPI_Datatype>{1, recvtype},
                     comm,
                     root};
  *request = rf.getHandlePool()->newRequest(*request, reqData);
  return ret;
}

int MPI_Ireduce(const void *sendbuf, void *recvbuf, int count,
                MPI_Datatype datatype, MPI_Op op, int root, MPI_Comm comm,
                MPI_Request *request) {
  MPI_Comm myComm = cf.getHandlePool()->getHandle(comm);
  int ret = PMPI_Ireduce(sendbuf, recvbuf, count, datatype, op, root, myComm,
                         request);
  nbcRequest reqData{nbf_MPI_Ireduce,
                     sendbuf,
                     std::vector<int>{1, count},
                     std::vector<int>{},
                     std::vector<MPI_Datatype>{1, datatype},
                     recvbuf,
                     std::vector<int>{},
                     std::vector<int>{},
                     std::vector<MPI_Datatype>{},
                     comm,
                     root,
                     op};
  *request = rf.getHandlePool()->newRequest(*request, reqData);
  return ret;
}

int MPI_Ibcast(void *buffer, int count, MPI_Datatype datatype, int root,
               MPI_Comm comm, MPI_Request *request) {
  MPI_Comm myComm = cf.getHandlePool()->getHandle(comm);
  int ret = PMPI_Ibcast(buffer, count, datatype, root, myComm, request);
  nbcRequest reqData{nbf_MPI_Ibcast,
                     buffer,
                     std::vector<int>{1, count},
                     std::vector<int>{},
                     std::vector<MPI_Datatype>{1, datatype},
                     nullptr,
                     std::vector<int>{},
                     std::vector<int>{},
                     std::vector<MPI_Datatype>{},
                     comm,
                     root};
  *request = rf.getHandlePool()->newRequest(*request, reqData);
  return ret;
}

int MPI_Iscatter(const void *sendbuf, int sendcount, MPI_Datatype sendtype,
                 void *recvbuf, int recvcount, MPI_Datatype recvtype, int root,
                 MPI_Comm comm, MPI_Request *request) {
  MPI_Comm myComm = cf.getHandlePool()->getHandle(comm);
  int ret = PMPI_Iscatter(sendbuf, sendcount, sendtype, recvbuf, recvcount,
                          recvtype, root, myComm, request);
  nbcRequest reqData{nbf_MPI_Iscatter,
                     sendbuf,
                     std::vector<int>{1, sendcount},
                     std::vector<int>{},
                     std::vector<MPI_Datatype>{1, sendtype},
                     recvbuf,
                     std::vector<int>{1, recvcount},
                     std::vector<int>{},
                     std::vector<MPI_Datatype>{1, recvtype},
                     comm,
                     root};
  *request = rf.getHandlePool()->newRequest(*request, reqData);
  return ret;
}

int MPI_Iscatterv(const void *sendbuf, const int sendcounts[],
                  const int displs[], MPI_Datatype sendtype, void *recvbuf,
                  int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm,
                  MPI_Request *request) {
  MPI_Comm myComm = cf.getHandlePool()->getHandle(comm);
  int ret = PMPI_Iscatterv(sendbuf, sendcounts, displs, sendtype, recvbuf,
                           recvcount, recvtype, root, myComm, request);
  int comm_size;
  PMPI_Comm_size(myComm, &comm_size);
  nbcRequest reqData{nbf_MPI_Igatherv,
                     sendbuf,
                     std::vector<int>{sendcounts, sendcounts + comm_size},
                     std::vector<int>{displs, displs + comm_size},
                     std::vector<MPI_Datatype>{1, sendtype},
                     recvbuf,
                     std::vector<int>{1, recvcount},
                     std::vector<int>{},
                     std::vector<MPI_Datatype>{1, recvtype},
                     comm,
                     root};
  *request = rf.getHandlePool()->newRequest(*request, reqData);
  return ret;
}

#include "completion-wrappers.cpp"

#endif