#include <assert.h>
#include <mpi.h>
#include <stdio.h>

#ifndef NOTOOL
#include "tracking.h"
#endif

#define COUNT 400
int main() {
  int rank, size, rbuf[COUNT], total = 0;
  MPI_Request allreq[2 * COUNT], *rreq = allreq, *sreq = allreq + COUNT;
  int provided;
  MPI_Init_thread(NULL, NULL, MPI_THREAD_MULTIPLE, &provided);
  if(provided != MPI_THREAD_MULTIPLE){
    printf("Threadlevel insufficient\n");
    MPI_Abort(MPI_COMM_WORLD, -1);
  }
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  for (int i = 0; i < COUNT; i++) {
    rreq[i] = sreq[i] = MPI_REQUEST_NULL;
  }

  /*******************
         wait
  *******************/

  double time, stime, ctime;
  time = -MPI_Wtime();
  stime = ctime = 0;
  for (int it = 0; it < COUNT; it++) {
#pragma omp parallel
    {
#pragma omp for schedule(static) nowait
      for (int i = 0; i < COUNT; i++) {
        MPI_Irecv(rbuf + i, 1, MPI_INT, size - rank - 1, 42, MPI_COMM_WORLD,
                  rreq + i);
        MPI_Isend(&rank, 1, MPI_INT, size - rank - 1, 42, MPI_COMM_WORLD,
                  sreq + i);
      }
#pragma omp for schedule(static) reduction(+ : total)
      for (int i = 0; i < COUNT; i++) {
        MPI_Wait(rreq + i, MPI_STATUS_IGNORE);
        MPI_Wait(sreq + i, MPI_STATUS_IGNORE);
        total += rbuf[i];
      }
    }
  }
  printf("%i: p2p-wait %i, %lf\n", rank, total, MPI_Wtime() + time);
  time = -MPI_Wtime();
  for (int it = 0; it < COUNT; it++) {
#pragma omp parallel
    {
#pragma omp for schedule(static) nowait
      for (int i = 0; i < COUNT; i++) {
        PMPI_Irecv(rbuf + i, 1, MPI_INT, size - rank - 1, 42, MPI_COMM_WORLD,
                   rreq + i);
        PMPI_Isend(&rank, 1, MPI_INT, size - rank - 1, 42, MPI_COMM_WORLD,
                   sreq + i);
      }
#pragma omp for schedule(static) reduction(+ : total)
      for (int i = 0; i < COUNT; i++) {
        PMPI_Wait(rreq + i, MPI_STATUS_IGNORE);
        PMPI_Wait(sreq + i, MPI_STATUS_IGNORE);
        total += rbuf[i];
      }
    }
  }
  printf("%i: p2p-wait-notool %i, %lf\n", rank, total, MPI_Wtime() + time);

  /*******************
        waitany
  *******************/
  time = -MPI_Wtime();
  stime = ctime = 0;
  for (int it = 0; it < COUNT; it++) {
    stime -= MPI_Wtime();
#pragma omp parallel for
    for (int i = 0; i < COUNT; i++) {
      MPI_Irecv(rbuf + i, 1, MPI_INT, size - rank - 1, 42, MPI_COMM_WORLD,
                rreq + i);
      MPI_Isend(&rank, 1, MPI_INT, size - rank - 1, 42, MPI_COMM_WORLD,
                sreq + i);
    }
    stime += MPI_Wtime();
    ctime -= MPI_Wtime();
    for (int i = 0; i < COUNT; i++) {
      int ri, si;
      MPI_Waitany(COUNT, rreq, &ri, MPI_STATUS_IGNORE);
      MPI_Waitany(COUNT, sreq, &si, MPI_STATUS_IGNORE);
      total += rbuf[ri];
    }
    ctime += MPI_Wtime();
  }
  printf("%i: p2p-waitany %i, %lf, %lf, %lf\n", rank, total, MPI_Wtime() + time, stime, ctime);
  time = -MPI_Wtime();
  stime = ctime = 0;
  for (int it = 0; it < COUNT; it++) {
    stime -= MPI_Wtime();
#pragma omp parallel for
    for (int i = 0; i < COUNT; i++) {
      PMPI_Irecv(rbuf + i, 1, MPI_INT, size - rank - 1, 42, MPI_COMM_WORLD,
                 rreq + i);
      PMPI_Isend(&rank, 1, MPI_INT, size - rank - 1, 42, MPI_COMM_WORLD,
                 sreq + i);
    }
    stime += MPI_Wtime();
    ctime -= MPI_Wtime();
    for (int i = 0; i < COUNT; i++) {
      int ri, si;
      PMPI_Waitany(COUNT, rreq, &ri, MPI_STATUS_IGNORE);
      PMPI_Waitany(COUNT, sreq, &si, MPI_STATUS_IGNORE);
      total += rbuf[ri];
    }
    ctime += MPI_Wtime();
  }
  printf("%i: p2p-waitany-notool %i, %lf, %lf, %lf\n", rank, total, MPI_Wtime() + time, stime, ctime);

  /*******************
        waitsome
  *******************/
  time = -MPI_Wtime();
  stime = ctime = 0;
  for (int it = 0; it < COUNT; it++) {
    stime -= MPI_Wtime();
#pragma omp parallel for
    for (int i = 0; i < COUNT; i++) {
      MPI_Irecv(rbuf + i, 1, MPI_INT, size - rank - 1, 42, MPI_COMM_WORLD,
                rreq + i);
      MPI_Isend(&rank, 1, MPI_INT, size - rank - 1, 42, MPI_COMM_WORLD,
                sreq + i);
    }
    stime += MPI_Wtime();
    ctime -= MPI_Wtime();
    int ri[COUNT], si[COUNT];
    for (int sc = 0, rc = 0, is = 0; is < COUNT && (sc < COUNT || rc < COUNT);) {
      int soutcount = 0, routcount = 0;
      if (rc < COUNT)
        MPI_Waitsome(COUNT, rreq, &routcount, ri, MPI_STATUSES_IGNORE);
      if (sc < COUNT)
        MPI_Waitsome(COUNT, sreq, &soutcount, si, MPI_STATUSES_IGNORE);
      for (int i = 0; i < routcount; i++)
        total += rbuf[ri[i]];
      if (soutcount != MPI_UNDEFINED)
        sc += soutcount;
      if (routcount != MPI_UNDEFINED)
        rc += routcount;
      is++;
    }
    ctime += MPI_Wtime();
  }
  printf("%i: p2p-waitsome %i, %lf, %lf, %lf\n", rank, total, MPI_Wtime() + time, stime, ctime);
  for (int i = 0; i < COUNT; i++) {
    rreq[i] = sreq[i] = MPI_REQUEST_NULL;
  }
  time = -MPI_Wtime();
  stime = ctime = 0;
  for (int it = 0; it < COUNT; it++) {
    stime -= MPI_Wtime();
#pragma omp parallel for
    for (int i = 0; i < COUNT; i++) {
      PMPI_Irecv(rbuf + i, 1, MPI_INT, size - rank - 1, 42, MPI_COMM_WORLD,
                 rreq + i);
      PMPI_Isend(&rank, 1, MPI_INT, size - rank - 1, 42, MPI_COMM_WORLD,
                 sreq + i);
    }
    stime += MPI_Wtime();
    ctime -= MPI_Wtime();
    int ri[COUNT], si[COUNT];
    for (int sc = 0, rc = 0, is = 0; is < COUNT && (sc < COUNT || rc < COUNT);) {
      int soutcount = 0, routcount = 0;
      if (rc < COUNT)
        PMPI_Waitsome(COUNT, rreq, &routcount, ri, MPI_STATUSES_IGNORE);
      if (sc < COUNT)
        PMPI_Waitsome(COUNT, sreq, &soutcount, si, MPI_STATUSES_IGNORE);
      for (int i = 0; i < routcount; i++)
        total += rbuf[ri[i]];
      if (soutcount != MPI_UNDEFINED)
        sc += soutcount;
      if (routcount != MPI_UNDEFINED)
        rc += routcount;
      is++;
    }
    ctime += MPI_Wtime();
  }
  printf("%i: p2p-waitsome-notool %i, %lf, %lf, %lf\n", rank, total, MPI_Wtime() + time, stime, ctime);

  /*******************
        waitall
  *******************/
  time = -MPI_Wtime();
  stime = ctime = 0;
  for (int it = 0; it < COUNT; it++) {
    stime -= MPI_Wtime();
#pragma omp parallel for
    for (int i = 0; i < COUNT; i++) {
      MPI_Irecv(rbuf + i, 1, MPI_INT, size - rank - 1, 42, MPI_COMM_WORLD,
                rreq + i);
      MPI_Isend(&rank, 1, MPI_INT, size - rank - 1, 42, MPI_COMM_WORLD,
                sreq + i);
    }
    stime += MPI_Wtime();
    ctime -= MPI_Wtime();
    MPI_Waitall(2 * COUNT, allreq, MPI_STATUSES_IGNORE);
    for (int i = 0; i < COUNT; i++) {
      total += rbuf[i];
    }
    ctime += MPI_Wtime();
  }
  printf("%i: p2p-waitall %i, %lf, %lf, %lf\n", rank, total, MPI_Wtime() + time, stime, ctime);
  time = -MPI_Wtime();
  stime = ctime = 0;
  for (int it = 0; it < COUNT; it++) {
    stime -= MPI_Wtime();
#pragma omp parallel for
    for (int i = 0; i < COUNT; i++) {
      PMPI_Irecv(rbuf + i, 1, MPI_INT, size - rank - 1, 42, MPI_COMM_WORLD,
                 rreq + i);
      PMPI_Isend(&rank, 1, MPI_INT, size - rank - 1, 42, MPI_COMM_WORLD,
                 sreq + i);
    }
    stime += MPI_Wtime();
    ctime -= MPI_Wtime();
    PMPI_Waitall(2 * COUNT, allreq, MPI_STATUSES_IGNORE);
    for (int i = 0; i < COUNT; i++) {
      total += rbuf[i];
    }
    ctime += MPI_Wtime();
  }
  printf("%i: p2p-waitall-notool %i, %lf, %lf, %lf\n", rank, total, MPI_Wtime() + time, stime, ctime);

  MPI_Finalize();
}
