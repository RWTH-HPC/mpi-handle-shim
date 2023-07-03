#ifndef NOTOOL

#include <assert.h>
#include <mpi.h>
#include <stdio.h>

#include "tracking.h"

{{fnalltype fn_name MPI_Request* MPI_Start MPI_Startall MPI_Cancel MPI_Request_free MPI_Wait MPI_Waitall MPI_Waitany MPI_Waitsome MPI_Test MPI_Testall MPI_Testany MPI_Testsome}}
  debug_printf("Executing %s\n",__PRETTY_FUNCTION__);
   {{apply_to_type MPI_Comm preComm}} {{apply_to_type MPI_Datatype preType}} {{apply_to_type MPI_Group preGroup}} {{apply_to_type MPI_File preFile}} {{apply_to_type MPI_Win preWin}} {{apply_to_type MPI_Op preOp}} {{apply_to_type MPI_Session preSession}}
   {{ret_val}} = P{{fn_name}}({{args}});
   {{apply_to_type MPI_Request* postRequest}} {{apply_to_type MPI_Comm* postComm}} {{apply_to_type MPI_Datatype* postType}} {{apply_to_type MPI_Group* postGroup}} {{apply_to_type MPI_Win* postWin}} {{apply_to_type MPI_File* postFile}} {{apply_to_type MPI_Op* postOp}} {{apply_to_type MPI_Session* postSession}}
{{endfnalltype}}

#include "completion-wrappers.cpp"

#endif