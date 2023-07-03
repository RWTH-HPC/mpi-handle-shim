/* Start & Completion */

int MPI_Start(MPI_Request *request) {
  debug_printf("Executing %s\n",__PRETTY_FUNCTION__);
#ifdef HANDLE_REQUEST
  MPI_Request* myReq = &rf.startRequest(*request);
#else
  MPI_Request* myReq = request;
#endif
  return PMPI_Start(myReq);
}

int MPI_Startall(int count, MPI_Request array_of_requests[]) {
  debug_printf("Executing %s\n",__PRETTY_FUNCTION__);
#ifdef HANDLE_REQUEST
  MPI_Request myReqs[count];
  {
    for (int i = 0; i < count; i++) {
      myReqs[i] = rf.startRequest(array_of_requests[i]);
    }
  }
#else
  MPI_Request* myReqs = array_of_requests;
#endif
  return PMPI_Startall(count, myReqs);
}

/* ================== C Wrappers for MPI_Cancel ================== */
int PMPI_Cancel(MPI_Request *request);
int MPI_Cancel(MPI_Request *request) {
  debug_printf("Executing %s\n",__PRETTY_FUNCTION__);
  int _wrap_py_return_val = 0;
#ifdef HANDLE_REQUEST
  MPI_Request* myReq = &rf.findData(*request)->handle;
#else
  MPI_Request* myReq = request;
#endif
  _wrap_py_return_val = PMPI_Cancel(myReq);

  return _wrap_py_return_val;
}

int MPI_Request_free(MPI_Request *request) {
  debug_printf("Executing %s\n",__PRETTY_FUNCTION__);
#ifdef HANDLE_REQUEST
  MPI_Request* myReq = &rf.findData(*request)->handle;
#else
  MPI_Request* myReq = request;
#endif
  int ret = PMPI_Request_free(myReq);
#ifdef HANDLE_REQUEST
  *request = rf.freeHandle(*request);
#endif
  return ret;
}

int MPI_Wait(MPI_Request *request, MPI_Status *status) {
  debug_printf("Executing %s\n",__PRETTY_FUNCTION__);
#ifdef HANDLE_REQUEST
  MPI_Request* myReq = &rf.findData(*request)->handle;
  MPI_Status tmpstatus;
  if (status == MPI_STATUS_IGNORE)
    status = &tmpstatus;
#else
  MPI_Request* myReq = request;
#endif
  int ret = PMPI_Wait(myReq, status);
#ifdef HANDLE_REQUEST
  *request = rf.completeRequest(*request, status);
#endif
  return ret;
}

int MPI_Waitall(int count, MPI_Request array_of_requests[],
                MPI_Status array_of_statuses[]) {
  debug_printf("Executing %s\n",__PRETTY_FUNCTION__);
#ifdef HANDLE_REQUEST
  MPI_Request myReqs[count];
  MPI_Status tmpstatuses[count];
  if (array_of_statuses == MPI_STATUSES_IGNORE)
    array_of_statuses = tmpstatuses;
  {
    auto lock = rf.getSharedLock();
    for (int i = 0; i < count; i++) {
      myReqs[i] = rf.getHandleLocked(array_of_requests[i]);
    }
  }
#else
  MPI_Request* myReqs = array_of_requests;
#endif
  int ret = PMPI_Waitall(count, myReqs, array_of_statuses);
#ifdef HANDLE_REQUEST
  {
    for (int i = 0; i < count; i++) {
      array_of_requests[i] =
          rf.completeRequest(array_of_requests[i], array_of_statuses + i);
    }
  }
#endif
  return ret;
}

int MPI_Waitany(int count, MPI_Request array_of_requests[], int *indx,
                MPI_Status *status) {
  debug_printf("Executing %s\n",__PRETTY_FUNCTION__);
#ifdef HANDLE_REQUEST
  MPI_Request myReqs[count];
  MPI_Status tmpstatus;
  if (status == MPI_STATUS_IGNORE)
    status = &tmpstatus;
  {
    auto lock = rf.getSharedLock();
    for (int i = 0; i < count; i++) {
      myReqs[i] = rf.getHandleLocked(array_of_requests[i]);
    }
  }
#else
  MPI_Request* myReqs = array_of_requests;
#endif
  int ret = PMPI_Waitany(count, myReqs, indx, status);
#ifdef HANDLE_REQUEST
  {
    array_of_requests[*indx] =
        rf.completeRequest(array_of_requests[*indx], status);
  }
#endif
  return ret;
}

int MPI_Waitsome(int incount, MPI_Request array_of_requests[], int *outcount,
                 int array_of_indices[], MPI_Status array_of_statuses[]) {
  debug_printf("Executing %s\n",__PRETTY_FUNCTION__);
#ifdef HANDLE_REQUEST
  MPI_Request myReqs[incount];
  MPI_Status tmpstatuses[incount];
  if (array_of_statuses == MPI_STATUSES_IGNORE)
    array_of_statuses = tmpstatuses;
  {
    auto lock = rf.getSharedLock();
    for (int i = 0; i < incount; i++) {
      myReqs[i] = rf.getHandleLocked(array_of_requests[i]);
    }
  }
#else
  MPI_Request* myReqs = array_of_requests;
#endif
  int ret = PMPI_Waitsome(incount, myReqs, outcount, array_of_indices,
                          array_of_statuses);
#ifdef HANDLE_REQUEST
  {
    for (int i = 0; i < *outcount; i++) {
      array_of_requests[array_of_indices[i]] =
          rf.completeRequest(array_of_requests[array_of_indices[i]],
                              array_of_statuses + array_of_indices[i]);
    }
  }
#endif
  return ret;
}

int MPI_Test(MPI_Request *request, int *flag, MPI_Status *status) {
  debug_printf("Executing %s\n",__PRETTY_FUNCTION__);
  debug_printf("MPI_Test(* %p = %p, %i, %i)\n",request, (void*)(intptr_t)*request, *flag, *request == MPI_REQUEST_NULL);
#ifdef HANDLE_REQUEST
  MPI_Request* myReq = &rf.findData(*request)->handle;
  MPI_Status tmpstatus;
  if (status == MPI_STATUS_IGNORE){
    debug_printf("MPI_STATUS_IGNORE: providing storage for status\n");
    status = &tmpstatus;
  }
#else
  MPI_Request* myReq = request;
#endif
  int ret = PMPI_Test(myReq, flag, status);
#ifdef HANDLE_REQUEST
  if (*flag) {
    *request = rf.completeRequest(*request, status);
  }
#endif
  if (*flag) {
    debug_printf("Test successful.\n");
  }
  return ret;
}

int MPI_Testall(int count, MPI_Request array_of_requests[], int *flag,
                MPI_Status array_of_statuses[]) {
  debug_printf("Executing %s\n",__PRETTY_FUNCTION__);
#ifdef HANDLE_REQUEST
  MPI_Request myReqs[count];
  MPI_Status tmpstatuses[count];
  if (array_of_statuses == MPI_STATUSES_IGNORE)
    array_of_statuses = tmpstatuses;
  {
    auto lock = rf.getSharedLock();
    for (int i = 0; i < count; i++) {
      myReqs[i] = rf.getHandleLocked(array_of_requests[i]);
    }
  }
#else
  MPI_Request* myReqs = array_of_requests;
#endif
  int ret = PMPI_Testall(count, myReqs, flag, array_of_statuses);
#ifdef HANDLE_REQUEST
  if (*flag) {
    for (int i = 0; i < count; i++) {
      array_of_requests[i] =
          rf.completeRequest(array_of_requests[i], array_of_statuses + i);
    }
  }
#endif
  return ret;
}

int MPI_Testany(int count, MPI_Request array_of_requests[], int *indx,
                int *flag, MPI_Status *status) {
  debug_printf("Executing %s\n",__PRETTY_FUNCTION__);
#ifdef HANDLE_REQUEST
  MPI_Request myReqs[count];
  MPI_Status tmpstatus;
  if (status == MPI_STATUS_IGNORE)
    status = &tmpstatus;
  {
    auto lock = rf.getSharedLock();
    for (int i = 0; i < count; i++) {
      myReqs[i] = rf.getHandleLocked(array_of_requests[i]);
    }
  }
#else
  MPI_Request* myReqs = array_of_requests;
#endif
  int ret = PMPI_Testany(count, myReqs, indx, flag, status);
#ifdef HANDLE_REQUEST
  if (*flag) {
    array_of_requests[*indx] =
        rf.completeRequest(array_of_requests[*indx], status);
  }
#endif
  return ret;
}

int MPI_Testsome(int incount, MPI_Request array_of_requests[], int *outcount,
                 int array_of_indices[], MPI_Status array_of_statuses[]) {
  debug_printf("Executing %s\n",__PRETTY_FUNCTION__);
#ifdef HANDLE_REQUEST
  MPI_Request myReqs[incount];
  MPI_Status tmpstatuses[incount];
  if (array_of_statuses == MPI_STATUSES_IGNORE)
    array_of_statuses = tmpstatuses;
  {
    auto lock = rf.getSharedLock();
    for (int i = 0; i < incount; i++) {
      myReqs[i] = rf.getHandleLocked(array_of_requests[i]);
    }
  }
#else
  MPI_Request* myReqs = array_of_requests;
#endif
  int ret = PMPI_Testsome(incount, myReqs, outcount, array_of_indices,
                          array_of_statuses);
#ifdef HANDLE_REQUEST
  {
    for (int i = 0; i < *outcount; i++) {
      array_of_requests[array_of_indices[i]] =
          rf.completeRequest(array_of_requests[array_of_indices[i]],
                              array_of_statuses + array_of_indices[i]);
    }
  }
#endif
  return ret;
}
