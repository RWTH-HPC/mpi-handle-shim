#include "tracking.h"

#ifdef FORTRAN_SUPPORT
#ifdef HANDLE_OP
template <> void OpData::init(MPI_Op op) {
  handle = op;
  fHandle = PMPI_Op_c2f(op);
}
#ifndef MPI_Op_c2f
MPI_Fint MPI_Op_c2f(MPI_Op op) { return of.c2f(op); }
MPI_Op MPI_Op_f2c(MPI_Fint op) { return of.f2c(op); }
#endif
#endif
#ifdef HANDLE_WIN
template <> void WinData::init(MPI_Win win) {
  handle = win;
  fHandle = PMPI_Win_c2f(win);
}
#ifndef MPI_Win_c2f
MPI_Fint MPI_Win_c2f(MPI_Win win) { return wf.c2f(win); }
MPI_Win MPI_Win_f2c(MPI_Fint win) { return wf.f2c(win); }
#endif
#endif
#ifdef HANDLE_TYPE
template <> void TypeData::init(MPI_Datatype type) {
  handle = type;
  fHandle = PMPI_Type_c2f(type);
}
#ifndef MPI_Type_c2f
MPI_Fint MPI_Type_c2f(MPI_Datatype type) { return tf.c2f(type); }
MPI_Datatype MPI_Type_f2c(MPI_Fint type) { return tf.f2c(type); }
#endif
#endif
#ifdef HANDLE_FILE
template <> void FileData::init(MPI_File file) {
  handle = file;
  fHandle = PMPI_File_c2f(file);
}
#ifndef MPI_File_c2f
MPI_Fint MPI_File_c2f(MPI_File file) { return ff.c2f(file); }
MPI_File MPI_File_f2c(MPI_Fint file) { return ff.f2c(file); }
#endif
#endif
#ifdef HANDLE_COMM
template <> void CommData::init(MPI_Comm comm) {
  handle = comm;
  fHandle = PMPI_Comm_c2f(comm);
}
#ifndef MPI_Comm_c2f
MPI_Fint MPI_Comm_c2f(MPI_Comm comm) { return cf.c2f(comm); }
MPI_Comm MPI_Comm_f2c(MPI_Fint comm) { return cf.f2c(comm); }
#endif
#endif
#ifdef HANDLE_GROUP
template <> void GroupData::init(MPI_Group group) {
  handle = group;
  fHandle = PMPI_Group_c2f(group);
}
#ifndef MPI_Group_c2f
MPI_Fint MPI_Group_c2f(MPI_Group group) { return gf.c2f(group); }
MPI_Group MPI_Group_f2c(MPI_Fint group) { return gf.f2c(group); }
#endif
#endif
#endif

#ifdef HANDLE_OP
template <> void AbstractHandleFactory<MPI_Op, OpData>::initPredefined() {
  static_assert(false, "HandleFactory<MPI_Op, OpData>::initPredefined not yet implemented, but requested");
}
template <> MPI_Op OpData::nullHandle{MPI_OP_NULL};
OpFactory of;
#endif
#ifdef HANDLE_WIN
template <> MPI_Win WinData::nullHandle{MPI_WIN_NULL};
WinFactory wf;
#endif
#ifdef HANDLE_TYPE
template <>
void AbstractHandleFactory<MPI_Datatype, TypeData>::initPredefined() {
  static_assert(false, "HandleFactory<MPI_Datatype, TypeData>::initPredefined not yet implemented, but requested");
}
template <> MPI_Datatype TypeData::nullHandle{MPI_DATATYPE_NULL};
TypeFactory tf{};
#endif
#ifdef HANDLE_FILE
template <> MPI_File FileData::nullHandle{MPI_FILE_NULL};
FileFactory ff;
#endif
#ifdef HANDLE_COMM
template <> bool AbstractHandleFactory<MPI_Comm, CommData>::isPredefined(MPI_Comm handle) {
  return handle == MPI_COMM_NULL || handle == MPI_COMM_WORLD ||
         handle == MPI_COMM_SELF;
}
template <> void AbstractHandleFactory<MPI_Comm, CommData>::initPredefined() {
//  printf("%s(%p,%p,%p)\n", __PRETTY_FUNCTION__, (void*)MPI_COMM_NULL, (void*)MPI_COMM_SELF ,(void*)MPI_COMM_WORLD);
  CommData *nHandle;
  nHandle = new CommData();
  nHandle->init(MPI_COMM_NULL);
  predefHandles[MPI_COMM_NULL] = nHandle;
#ifdef FORTRAN_SUPPORT
  predefFHandles[nHandle->fHandle] = nHandle;
#endif
  nHandle = new CommData();
  nHandle->init(MPI_COMM_WORLD);
  predefHandles[MPI_COMM_WORLD] = nHandle;
#ifdef FORTRAN_SUPPORT
  predefFHandles[nHandle->fHandle] = nHandle;
#endif
  nHandle = new CommData();
  nHandle->init(MPI_COMM_SELF);
  predefHandles[MPI_COMM_SELF] = nHandle;
#ifdef FORTRAN_SUPPORT
  predefFHandles[nHandle->fHandle] = nHandle;
#endif
}

template <> MPI_Comm CommData::nullHandle{MPI_COMM_NULL};
CommFactory cf;
#endif
#ifdef HANDLE_GROUP
template <>
bool AbstractHandleFactory<MPI_Group, GroupData>::isPredefined(MPI_Group handle) {
  return handle == MPI_GROUP_NULL || handle == MPI_GROUP_EMPTY;
}
template <> void AbstractHandleFactory<MPI_Group, GroupData>::initPredefined() {
  GroupData *nHandle;
  nHandle = new GroupData();
  nHandle->init(MPI_GROUP_NULL);
  predefHandles[MPI_GROUP_NULL] = nHandle;
#ifdef FORTRAN_SUPPORT
  predefFHandles[nHandle->fHandle] = nHandle;
#endif
  nHandle = new GroupData();
  nHandle->init(MPI_GROUP_EMPTY);
  predefHandles[MPI_GROUP_EMPTY] = nHandle;
#ifdef FORTRAN_SUPPORT
  predefFHandles[nHandle->fHandle] = nHandle;
#endif
}
template <> MPI_Group GroupData::nullHandle{MPI_GROUP_NULL};
GroupFactory gf{};
#endif
#ifdef HANDLE_REQUEST
MPI_Request RequestData::nullHandle{MPI_REQUEST_NULL};
RequestFactory rf{};
#endif
#if defined(HAVE_SESSION) && defined(HANDLE_SESSION)
template <> MPI_Session SessionData::nullHandle{MPI_SESSION_NULL};
SessionFactory sf{};
#endif
