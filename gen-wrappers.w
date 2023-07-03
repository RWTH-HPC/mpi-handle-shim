#ifndef NOTOOL

#include <assert.h>
#include <mpi.h>
#include <stdio.h>

#include "tracking.h"

{{fn fn_name MPI_Init MPI_Init_thread}}
   {{ret_val}} = P{{fn_name}}({{args}});
#ifdef HANDLE_OP
   of.initPredefined();
#endif
#ifdef HANDLE_WIN
   wf.initPredefined();
#endif
#ifdef HANDLE_TYPE
   tf.initPredefined();
#endif
#ifdef HANDLE_FILE
   ff.initPredefined();
#endif
#ifdef HANDLE_COMM
   cf.initPredefined();
#endif
#ifdef HANDLE_GROUP
   gf.initPredefined();
#endif
#ifdef HANDLE_REQUEST
   rf.initPredefined();
#endif
#if defined(HAVE_SESSION) && defined(HANDLE_SESSION)
   sf.initPredefined();
#endif
{{endfn}}

int MPI_Finalize(){
#ifdef HANDLE_OP
   of.clear();
#endif
#ifdef HANDLE_WIN
   wf.clear();
#endif
#ifdef HANDLE_TYPE
   tf.clear();
#endif
#ifdef HANDLE_FILE
   ff.clear();
#endif
#ifdef HANDLE_COMM
   cf.clear();
#endif
#ifdef HANDLE_GROUP
   gf.clear();
#endif
#ifdef HANDLE_REQUEST
   rf.clear();
#endif
#if defined(HAVE_SESSION) && defined(HANDLE_SESSION)
   sf.clear();
#endif
   return PMPI_Finalize();
}

{{fnallntype fn_name MPI_Request MPI_Comm_free MPI_Comm_disconnect MPI_Group_free MPI_Type_free MPI_Type_commit MPI_Op_free MPI_Win_free MPI_Session_finalize MPI_Alltoallw MPI_Neighbor_alltoallw MPI_Type_create_struct MPI_Type_get_contents MPI_Type_struct MPI_Alltoallw_c MPI_Neighbor_alltoallw_c MPI_Type_create_struct_c MPI_Type_get_contents_c   MPI_Comm_create_keyval MPI_Comm_free_keyval MPI_Keyval_create MPI_Keyval_free MPI_Type_create_keyval MPI_Type_free_keyval MPI_Win_create_keyval MPI_Win_free_keyval MPI_Get_address MPI_Address MPI_Add_error_class MPI_Add_error_code MPI_Add_error_string MPI_Comm_create_errhandler MPI_Errhandler_free MPI_Error_class MPI_Error_string MPI_File_create_errhandler MPI_Session_create_errhandler MPI_Win_create_errhandler MPI_Errhandler_create MPI_Info_create MPI_Info_create_env MPI_Info_delete MPI_Info_dup MPI_Info_free MPI_Info_get MPI_Info_get_nkeys MPI_Info_get_nthkey MPI_Info_get_string MPI_Info_get_valuelen MPI_Info_set MPI_Finalize MPI_Finalized MPI_Init MPI_Init_thread MPI_Initialized MPI_Is_thread_main MPI_Query_thread MPI_Get_library_version MPI_Get_processor_name MPI_Get_version MPI_Pcontrol MPI_Buffer_attach MPI_Buffer_detach MPI_Status_set_cancelled MPI_Test_cancelled MPI_Alloc_mem MPI_Free_mem MPI_Close_port MPI_Lookup_name MPI_Open_port MPI_Publish_name MPI_Unpublish_name MPI_Dims_create MPI_Buffer_attach_c MPI_Buffer_detach_c MPI_File_delete MPI_Register_datarep MPI_Register_datarep_c MPI_Aint_add MPI_Aint_diff }}
   {{apply_to_type MPI_Comm preComm}} {{apply_to_type MPI_Datatype preType}} {{apply_to_type MPI_Group preGroup}} {{apply_to_type MPI_File preFile}} {{apply_to_type MPI_Win preWin}} {{apply_to_type MPI_Op preOp}} {{apply_to_type MPI_Session preSession}}
   {{ret_val}} = P{{fn_name}}({{args}});
   {{apply_to_type MPI_Comm* postComm}} {{apply_to_type MPI_Datatype* postType}} {{apply_to_type MPI_Group* postGroup}} {{apply_to_type MPI_Win* postWin}} {{apply_to_type MPI_File* postFile}} {{apply_to_type MPI_Op* postOp}} {{apply_to_type MPI_Session* postSession}}
{{endfnallntype}}

#endif