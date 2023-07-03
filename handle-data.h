enum nbFunction {
  nbf_unknown = -1,
  nbf_MPI_Isend,
  nbf_MPI_Issend,
  nbf_MPI_Irsend,
  nbf_MPI_Ibsend,
  nbf_MPI_Irecv,

  nbf_MPI_Send_init,
  nbf_MPI_Ssend_init,
  nbf_MPI_Rsend_init,
  nbf_MPI_Bsend_init,
  nbf_MPI_Recv_init,

  nbf_MPI_Iallgather,
  nbf_MPI_Iallgatherv,
  nbf_MPI_Iallreduce,
  nbf_MPI_Ialltoall,
  nbf_MPI_Ialltoallv,
  nbf_MPI_Ialltoallw,
  nbf_MPI_Ibarrier,
  nbf_MPI_Ibcast,
  nbf_MPI_Iexscan,
  nbf_MPI_Igather,
  nbf_MPI_Igatherv,
  nbf_MPI_Ireduce,
  nbf_MPI_Ireduce_scatter,
  nbf_MPI_Ireduce_scatter_block,
  nbf_MPI_Iscan,
  nbf_MPI_Iscatter,
  nbf_MPI_Iscatterv,

  nbf_MPI_Allgather_init,
  nbf_MPI_Allgatherv_init,
  nbf_MPI_Allreduce_init,
  nbf_MPI_Alltoall_init,
  nbf_MPI_Alltoallv_init,
  nbf_MPI_Alltoallw_init,
  nbf_MPI_Barrier_init,
  nbf_MPI_Bcast_init,
  nbf_MPI_Exscan_init,
  nbf_MPI_Gather_init,
  nbf_MPI_Gatherv_init,
  nbf_MPI_Reduce_init,
  nbf_MPI_Reduce_scatter_init,
  nbf_MPI_Reduce_scatter_block_init,
  nbf_MPI_Scan_init,
  nbf_MPI_Scatter_init,
  nbf_MPI_Scatterv_init,

  nbf_MPI_Neighbor_iallgather,
  nbf_MPI_Neighbor_iallgatherv,
  nbf_MPI_Neighbor_iallreduce,
  nbf_MPI_Neighbor_ialltoall,
  nbf_MPI_Neighbor_ialltoallv,
  nbf_MPI_Neighbor_ialltoallw,
  nbf_MPI_Neighbor_ibarrier,
  nbf_MPI_Neighbor_ibcast,
  nbf_MPI_Neighbor_iexscan,
  nbf_MPI_Neighbor_igather,
  nbf_MPI_Neighbor_igatherv,
  nbf_MPI_Neighbor_ireduce,
  nbf_MPI_Neighbor_ireduce_scatter,
  nbf_MPI_Neighbor_ireduce_scatter_block,
  nbf_MPI_Neighbor_iscan,
  nbf_MPI_Neighbor_iscatter,
  nbf_MPI_Neighbor_iscatterv,

  nbf_MPI_Neighbor_allgather_init,
  nbf_MPI_Neighbor_allgatherv_init,
  nbf_MPI_Neighbor_allreduce_init,
  nbf_MPI_Neighbor_alltoall_init,
  nbf_MPI_Neighbor_alltoallv_init,
  nbf_MPI_Neighbor_alltoallw_init,
  nbf_MPI_Neighbor_barrier_init,
  nbf_MPI_Neighbor_bcast_init,
  nbf_MPI_Neighbor_exscan_init,
  nbf_MPI_Neighbor_gather_init,
  nbf_MPI_Neighbor_gatherv_init,
  nbf_MPI_Neighbor_reduce_init,
  nbf_MPI_Neighbor_reduce_scatter_init,
  nbf_MPI_Neighbor_reduce_scatter_block_init,
  nbf_MPI_Neighbor_scan_init,
  nbf_MPI_Neighbor_scatter_init,
  nbf_MPI_Neighbor_scatterv_init
};


#define NUM_UC_TIMERS 4

struct p2pRequest {
  nbFunction nbf{nbf_unknown};
#ifdef TRACK_DATA
  const void *buf{nullptr};
  int count{-1};
  MPI_Datatype dt{MPI_DATATYPE_NULL};
  int remote{-1};
  int tag{-1};
  MPI_Comm comm{MPI_COMM_NULL};
#endif
  p2pRequest(nbFunction nbf, const void *buf, int count, MPI_Datatype dt,
             int remote, int tag, MPI_Comm comm)
      : nbf(nbf)
#ifdef TRACK_DATA
        ,
        buf(buf), count(count), dt(dt), remote(remote), tag(tag), comm(comm)
#endif
  {
  }
  p2pRequest(const p2pRequest &o)
      : nbf(o.nbf)
#ifdef TRACK_DATA
        ,
        buf(o.buf), count(o.count), dt(o.dt), remote(o.remote), tag(o.tag),
        comm(o.comm)
#endif
  {
  }
  p2pRequest(p2pRequest &&o) noexcept
      : nbf(o.nbf)
#ifdef TRACK_DATA
        ,
        buf(o.buf), count(o.count), dt(o.dt), remote(o.remote), tag(o.tag),
        comm(o.comm)
#endif
  {
  }
  p2pRequest() {}
};

struct nbcRequest {
  nbFunction nbf{nbf_unknown};
#ifdef TRACK_DATA
  const void *sbuf{nullptr};
  std::vector<int> scounts{};
  std::vector<int> sdispls{};
  std::vector<MPI_Datatype> stypes{};
  const void *rbuf{nullptr};
  std::vector<int> rcounts{};
  std::vector<int> rdispls{};
  std::vector<MPI_Datatype> rtypes{};
  MPI_Op op{MPI_OP_NULL};
  int root{-1};
  MPI_Comm comm{MPI_COMM_NULL};
#endif

  nbcRequest(nbFunction nbf, const void *sbuf, std::vector<int> scounts,
             std::vector<int> sdispls, std::vector<MPI_Datatype> stypes,
             const void *rbuf, std::vector<int> rcounts,
             std::vector<int> rdispls, std::vector<MPI_Datatype> rtypes,
             MPI_Comm comm, int root = -1, MPI_Op op = MPI_OP_NULL)
      : nbf(nbf)
#ifdef TRACK_DATA
        ,
        sbuf(sbuf), scounts(scounts), sdispls(sdispls), stypes(stypes),
        rbuf(rbuf), rcounts(rcounts), rdispls(rdispls), rtypes(rtypes), op(op),
        root(root), comm(comm)
#endif
  {
  }
  nbcRequest(nbFunction nbf, MPI_Comm comm)
#ifdef TRACK_DATA
      : comm(comm)
#endif
  {
  }

  nbcRequest(nbcRequest &&o) noexcept
      : nbf(o.nbf)
#ifdef TRACK_DATA
        ,
        sbuf(o.sbuf), scounts(std::move(o.scounts)),
        sdispls(std::move(o.sdispls)), stypes(std::move(o.stypes)),
        rbuf(o.rbuf), rcounts(std::move(o.rcounts)),
        rdispls(std::move(o.rdispls)), rtypes(std::move(o.rtypes)),
        root(o.root), op(o.op)
#endif
  {
  }
  nbcRequest(const nbcRequest &o)
      : nbf(o.nbf)
#ifdef TRACK_DATA
        ,
        sbuf(o.sbuf), scounts(std::move(o.scounts)),
        sdispls(std::move(o.sdispls)), stypes(std::move(o.stypes)),
        rbuf(o.rbuf), rcounts(std::move(o.rcounts)),
        rdispls(std::move(o.rdispls)), rtypes(std::move(o.rtypes)),
        root(o.root), op(o.op)
#endif
  {
  }
  nbcRequest() {}
};

template <typename M, auto E> class alignas(64) HandleData {
protected:
public:
  static M nullHandle;
  M handle{nullHandle};
#ifdef FORTRAN_SUPPORT
  int fHandle{-1};
#endif
  HandleData() {}
  HandleData(M group
#ifdef FORTRAN_SUPPORT
             ,
             MPI_Fint fGroup
#endif
             )
      : handle(group)
#ifdef FORTRAN_SUPPORT
        ,
        fHandle(fGroup)
#endif
  {
  }
  HandleData(const HandleData &o) { this->init(o); }
  HandleData(HandleData &&o) noexcept { this->init(o); }
  virtual void init(M group
#ifdef FORTRAN_SUPPORT
                    ,
                    MPI_Fint fGroup
#endif
  ) {
    handle = group;
#ifdef FORTRAN_SUPPORT
    fHandle = fGroup;
#endif
  }
#ifdef FORTRAN_SUPPORT
  virtual void init(M group);
#endif

  virtual void init(const HandleData &o) {
    handle = o.handle;
#ifdef FORTRAN_SUPPORT
    fHandle = o.fHandle;
#endif
  }
  virtual void fini() {}
};

/*class alignas(64) CommData {
  MPI_Comm dupComm{MPI_COMM_NULL};
  int size{0};
  int rank{-1};
public:
  static MPI_Comm nullHandle;
  MPI_Comm handle{nullHandle};
  void init(MPI_Comm comm) {
    this->handle = comm;
    if(comm == MPI_COMM_NULL)
      return;
    PMPI_Comm_dup(comm, &dupComm);
    PMPI_Comm_size(comm, &size);
    PMPI_Comm_rank(comm, &rank);
  }
  void fini() {
    PMPI_Comm_free(&dupComm);
  }
  int getSize(){return size;}
  int getRank(){return rank;}
  MPI_Comm getDupComm(){return dupComm;}
};*/

typedef enum { ISEND=0, IRECV, ICOLL, IREDUCE, IALLREDUCE, IBCAST, PSEND, PRECV, PCOLL, PREDUCE, PALLREDUCE, PBCAST } KIND;
class alignas(64) RequestData {
  enum reqKind { NONE, P2P, NBC };
  bool persistent{false};
  reqKind rKind{NONE};
  std::function<void(RequestData *)> startCallback{};
  std::function<void(RequestData *, MPI_Status *)> completionCallback{};
  bool freed{true};
  /*union {
    void *none{nullptr};
    p2pRequest p2p;
    nbcRequest nbc;
  };*/

protected:
public:
  //double uc[NUM_UC_TIMERS];
  KIND kind{ISEND};
  MPI_Request pb_req{MPI_REQUEST_NULL};
  // int remote{-42};
  // int tag{-42};
  // int root{-42};
  // CommData* comm{nullptr};

  static MPI_Request nullHandle;
  RequestData() {}
  RequestData(MPI_Request req
#ifdef FORTRAN_SUPPORT
              ,
              MPI_Fint fReq
#endif
              )
      : handle(req),
#ifdef FORTRAN_SUPPORT
        fHandle(fReq),
#endif
        freed(false)
  {
  }
  RequestData(const RequestData &o) : rKind(o.rKind) {
    this->init(o);
    //    if (o.rKind == P2P)
    //      this->init(o.handle, o.p2p, o.persistent);
    //    if (o.rKind == NBC)
    //      this->init(o.handle, o.nbc, o.persistent);
  }
  RequestData(RequestData &&o) noexcept : rKind(o.rKind) {
    this->init(o);
    //    if (o.rKind == P2P)
    //      this->init(o.handle, o.p2p, o.persistent);
    //    if (o.rKind == NBC)
    //      this->init(o.handle, o.nbc, o.persistent);
  }
  ~RequestData() {}
  MPI_Request handle{MPI_REQUEST_NULL};
#ifdef FORTRAN_SUPPORT
  MPI_Fint fHandle{-1};
#endif

  void init(const RequestData &o) {
    handle = o.handle;
#ifdef FORTRAN_SUPPORT
    fHandle = o.fHandle;
#endif
    freed = false;
  }
  // void init(MPI_Request request, KIND _kind, int _remote = -1, int _tag = -1, CommData* _comm = nullptr, int _root=-1, bool _persistent = false) {
  //   rKind = NONE;
  //   handle = request;
  //   kind = _kind;
  //   remote = _remote;
  //   tag = _tag;
  //   comm = _comm;
  //   root = _root;
  //   persistent = _persistent;
  //   freed = false;
  // }
  void init(MPI_Request request, const p2pRequest &rData,
            bool _persistent = false) {
              assert(false);
    persistent = _persistent;
    handle = request;
    rKind = P2P;
    freed = false;
//    new (&p2p) p2pRequest{std::move(rData)};
  }
  void init(MPI_Request request,
#ifdef FORTRAN_SUPPORT
            MPI_Fint fRequest = -1,
#endif
            const void *rData = nullptr, bool _persistent = false) {
    persistent = _persistent;
    handle = request;
#ifdef FORTRAN_SUPPORT
    if (fHandle == -1)
      fHandle = PMPI_Request_c2f(request);
    else
      fHandle = fRequest;
#endif
    rKind = NONE;
    freed = false;
//    new (&p2p) p2pRequest{std::move(rData)};
  }
  void init(MPI_Request request, const nbcRequest &rData,
            bool _persistent = false) {
              assert(false);
    persistent = _persistent;
    handle = request;
    rKind = NBC;
    freed = false;
//    new (&nbc) nbcRequest{std::move(rData)};
  }
/*  void init(MPI_Request request) {
    handle = request;
    freed = false;
  }*/
  void start(){
    assert(!freed);
    if (startCallback) {
      startCallback(this);
    }
  }
  void complete(MPI_Status *status) {
    assert(!freed);
    if (completionCallback) {
      completionCallback(this, status);
    }
  }
  void fini(MPI_Status *status) {
    complete(status);
    fini();
  }
  void fini() {
    handle = MPI_REQUEST_NULL;
    /*if (rKind == P2P)
      p2p.~p2pRequest();
    if (rKind == NBC)
      nbc.~nbcRequest();*/
    rKind = NONE;
    //none = nullptr;
    freed = true;
  }
  bool isFreed() { return freed; }
  bool isPersistent() {
    assert(!freed);
    return persistent;
  }
  void setStartCallback(std::function<void(RequestData *)> sc){startCallback = sc;}
  void setCompletionCallback(std::function<void(RequestData *, MPI_Status *)> cc){completionCallback = cc;}
};
