#ifndef NOTOOL

#ifndef TRACKING_H
#define TRACKING_H

#include <assert.h>
#include <mpi.h>
#include <stdio.h>

#include <functional>
#include <list>
#include <map>
#include <mutex>
#include <shared_mutex>
#include <unordered_map>
#include <vector>
#include <atomic>

//#define TRACK_DATA 1
//#define HANDLE_OP 1
//#define HANDLE_WIN 1
//#define HANDLE_FILE 1
//#define HANDLE_TYPE 1
//#define HANDLE_COMM 1
//#define HANDLE_GROUP 1
#define HANDLE_REQUEST 1
#define REAL_DATAPOOL 1

#ifdef PRINT_DEBUG
#define debug_printf printf
#else
static inline void debug_printf(...){}
#endif


#ifdef __cplusplus
#define _EXTERN_C_ extern "C"
#endif

#ifdef HANDLE_OP
#define preOp(o) o = of.getHandle(o)
#define postOp(o) *o = of.newHandle(*o)
#else
#define preOp(o) (void)o
#define postOp(o) (void)o
#endif

#ifdef HANDLE_WIN
#define preWin(w) w = wf.getHandle(w)
#define postWin(w) *w = wf.newHandle(*w)
#else
#define preWin(w) (void)w
#define postWin(w) (void)w
#endif

#ifdef HANDLE_TYPE
#define preType(t) t = tf.getHandle(t)
#define postType(t) *(t) = tf.newHandle(*(t))
#else
#define preType(t) (void)t
#define postType(t) (void)t
#endif

#ifdef HANDLE_FILE
#define preFile(f) f = ff.getHandle(f)
#define postFile(f) *f = ff.newHandle(*f)
#else
#define preFile(f) (void)f
#define postFile(f) (void)f
#endif

#ifdef HANDLE_COMM
#define preComm(c) c = cf.getHandle(c)
#define postComm(c) *c = cf.newHandle(*c)
#else
#define preComm(c) (void)c
#define postComm(c) (void)c
#endif

#ifdef HANDLE_GROUP
#define preGroup(g) g = gf.getHandle(g)
#define postGroup(g) *g = gf.newHandle(*g)
#else
#define preGroup(g) (void)g
#define postGroup(g) (void)g
#endif

#if defined(HAVE_SESSION) && defined(HANDLE_SESSION)
#define preSession(s) s = sf.getHandle(s)
#define postSession(s) *s = sf.newHandle(*s)
#else
#define preSession(s) (void)s
#define postSession(s) (void)s
#endif

#ifdef HANDLE_REQUEST
#define postRequest(r) *r = rf.newRequest(*r, false)
#define postRequestInit(r) *r = rf.newRequest(*r, true)
#else
#define postRequest(r) (void)r
#define postRequestInit(r) (void)r
#endif

enum toolDataEnum {
  toolOpData=0,
  toolWinData=1,
  toolTypeData=2,
  toolFileData=3,
  toolCommData=4,
  toolGroupData=5,
  toolSessionData=6,
  toolRequestData=7
};

#include "handle-data.h"

template <typename M, typename T> class AbstractHandleFactory {
  using A = M; // A is the application facing handle, M is the MPI facing handle.
protected:
  std::unordered_map<M, T *> predefHandles{};
#ifdef FORTRAN_SUPPORT
  std::unordered_map<MPI_Fint, T *> predefFHandles{};
#endif
  virtual bool isPredefined(A handle) { return handle == T::nullHandle; }
  virtual T *findPredefinedData(A handle) {
    auto iter = predefHandles.find(handle);
    if (iter == predefHandles.end())
      return nullptr;
    return iter->second;
  }

public:
  virtual A newHandle(M& handle) = 0;
  virtual A newHandle(M& handle, T* data) = 0;
  virtual A freeHandle(A handle) = 0;
#ifdef FORTRAN_SUPPORT
  virtual M f2c(MPI_Fint fhandle) = 0;
  virtual MPI_Fint c2f(M handle) = 0;
#endif
  virtual T *findData(A handle) = 0;
  virtual T *newData() = 0;
  virtual M& getHandle(A& handle) = 0;
  virtual const M &getHandle(const A &handle) = 0;
  virtual M& getHandleLocked(A& handle) = 0;
  virtual const M &getHandleLocked(const A &handle) = 0;
  virtual std::shared_lock<std::shared_mutex> getSharedLock() = 0;
  virtual ~AbstractHandleFactory<M, T>(){};
  virtual void initPredefined() {
    T *nHandle = new T();
    nHandle->init(T::nullHandle);
    predefHandles[T::nullHandle] = nHandle;
#ifdef FORTRAN_SUPPORT
    predefFHandles[nHandle->fHandle] = nHandle;
#endif
  }
  virtual void clear() = 0;
  virtual void printInfo() = 0;
};

class AbstractRequestFactory : public virtual AbstractHandleFactory<MPI_Request, RequestData> {
  using A = MPI_Request;
  using M = MPI_Request;
  using T = RequestData;
public:
  virtual MPI_Request newRequest(MPI_Request req, bool persistent = false) = 0;
  //  virtual MPI_Request newRequest(MPI_Request req, p2pRequest &reqData,
  //                                 bool persistent = false) = 0;
  //  virtual MPI_Request newRequest(MPI_Request req, nbcRequest &reqData,
  //                                 bool persistent = false) = 0;
  virtual MPI_Request completeRequest(MPI_Request req, MPI_Status *status) = 0;
  virtual MPI_Request& startRequest(MPI_Request& req) = 0;
  virtual ~AbstractRequestFactory(){};
};

#ifdef REAL_DATAPOOL
template <typename M, typename T> class DataPool {
private:
  mutable std::shared_mutex DPMutex{};
  std::list<void *> memory;
  virtual void newDatas() {
    assert(sizeof(T) % 64 == 0);
    int ndatas = 4096 / sizeof(T);
    totalDatas += ndatas;
    char *datas = (char *)malloc(ndatas * sizeof(T));
    memory.push_back(datas);
    for (int i = 0; i < ndatas; i++) {
      dataPointer.push_back(new (datas + i * sizeof(T)) T());
    }
  }

protected:
  std::atomic<size_t> totalDatas{0};
  std::vector<T *> dataPointer{};
  virtual void clearPool(){
    for (auto i : this->dataPointer)
      if (i)
        i->~T();
    this->dataPointer.clear();
    for (auto i : this->memory)
      if (i)
        free(i);
    this->memory.clear();
  }
public:
  virtual T *getData() {
    std::unique_lock<std::shared_mutex> lock(DPMutex);
    if (dataPointer.empty())
      newDatas();
    T *ret = dataPointer.back();
    dataPointer.pop_back();
    return ret;
  }
  virtual void returnData(T *data) {
    std::unique_lock<std::shared_mutex> lock(DPMutex);
    dataPointer.emplace_back(data);
  }
  virtual ~DataPool() {
    this->clearPool();
  }
};
#else
template <typename M, typename T> class DataPool {
protected:
  virtual void clearPool(){}
public:
  virtual T *getData() { return new T(); }
  virtual void returnData(T *data) { delete data; }
};
#endif

template <typename M, typename T, typename D>
class HandleFactory {
  // Instanciating the primary class should fail because of the private default constructor
  HandleFactory(){ /*static_assert(false, "primary template of HandleFactory instanciated unexpectedly");*/ }
};
template <typename D>
class RequestFactoryInst {
  // Instanciating the primary class should fail because of the private default constructor
  RequestFactoryInst(){ /*static_assert(false, "primary template of RequestFactory instanciated unexpectedly");*/ }
};


template <typename M, typename T>
class HandleFactory<M,T,std::multimap<M,T*>> : public virtual AbstractHandleFactory<M, T>,
                                      public DataPool<M, T> {
protected:
  mutable std::shared_mutex MapMutex{};
  std::unordered_map<M, std::list<T *>> handleMap{};

public:
  void printInfo()
  {printf("handleMap.size() = %li\n", handleMap.size());}
  void clear(){
    this->clearPool();
    handleMap.clear();
    for (auto i : this->predefHandles)
      if (i.second)
        delete(i.second);
    this->predefHandles.clear();
#ifdef FORTRAN_SUPPORT
    this->predefFHandles.clear();
#endif
  }
#ifdef FORTRAN_SUPPORT
  M f2c(MPI_Fint fhandle) {
    assert(false);
    return 0;
  }
  MPI_Fint c2f(M handle) {
    assert(false);
    return 0;
  }
#endif
  T* newData(){
    return this->getData();
  }
  M newHandle(M& handle) {
    if (this->isPredefined(handle))
      return handle;
    T *ret = this->getData();
    return newHandle(handle, ret);
  }
  M newHandle(M& handle, T* ret) {
    ret->init(handle);
    {
      std::unique_lock<std::shared_mutex> lock(MapMutex);
      auto retEmplace = handleMap.emplace(handle, std::list<T *>{});
      retEmplace.first->second.emplace_back(ret);
    }
    return handle;
  }
  M freeHandle(M handle) {
    if (this->isPredefined(handle))
      return handle;
    T *rData;
    {
      std::unique_lock<std::shared_mutex> lock(MapMutex);
      auto hList = handleMap[handle];
      rData = hList.front();
      hList.pop_front();
      if (hList.empty())
        handleMap.erase(handle);
    }
    rData->fini();
    this->returnData(rData);
    return T::nullHandle;
  }
  T *findData(M handle) {
    T* ret = this->findPredefinedData(handle);
    if (ret != nullptr)
      return ret;
    std::shared_lock<std::shared_mutex> lock(MapMutex);
    return handleMap[handle].front();
  }
  M& getHandle(M& handle) { return handle; }
  M& getHandleLocked(M& handle) { return handle; }
  const M &getHandle(const M &handle) { return handle; }
  const M &getHandleLocked(const M &handle) { return handle; }
  std::shared_lock<std::shared_mutex> getSharedLock() {
    return std::shared_lock<std::shared_mutex>{MapMutex, std::defer_lock};
  }
  virtual ~HandleFactory<M,T,std::multimap<M,T*>>() {}
};

template<>
class RequestFactoryInst<std::multimap<MPI_Request, RequestData*>> : public HandleFactory<MPI_Request, RequestData, std::multimap<MPI_Request, RequestData*>>,
                           public AbstractRequestFactory {
  template <typename D>
  MPI_Request _newRequest(MPI_Request req, D &reqData, bool persistent) {
    if (req == MPI_REQUEST_NULL)
      return MPI_REQUEST_NULL;
    RequestData *ret = this->getData();
    {
      std::unique_lock<std::shared_mutex> lock(MapMutex);
      auto retEmplace = handleMap.emplace(req, std::list<RequestData *>{});
      retEmplace.first->second.emplace_back(ret);
//      std::unique_lock<std::shared_mutex> lock(MapMutex);
//      handleMap[req].emplace_back(ret);
    }
    ret->init(req,
#ifdef FORTRAN_SUPPORT
              -1,
#endif
              reqData, persistent);
    assert(!ret->isFreed());
    return req;
  }
public:
  // get data from the pool
  MPI_Request newRequest(MPI_Request req, bool persistent) {
    const void *reqData = nullptr;
    return _newRequest(req, reqData, persistent);
  }
  /*  MPI_Request newRequest(MPI_Request req, p2pRequest &reqData,
                           bool persistent) {
      return _newRequest(req, reqData, persistent);
    }
    MPI_Request newRequest(MPI_Request req, nbcRequest &reqData,
                           bool persistent) {
      return _newRequest(req, reqData, persistent);
    }*/

  // returning to the datapool using lock
  MPI_Request completeRequest(MPI_Request req, MPI_Status *status) {
    if (req == MPI_REQUEST_NULL)
      return MPI_REQUEST_NULL;
    RequestData *rData;
    {
      std::unique_lock<std::shared_mutex> lock(MapMutex);
      rData = handleMap[req].front();
      if (!rData->isPersistent()) {
      handleMap[req].pop_front();
      if (handleMap[req].empty())
        handleMap.erase(req);
      }
    }
      if (rData->isPersistent()) {
        rData->complete(status);
        return req;
      }
    rData->fini(status);
    this->returnData(rData);
    return MPI_REQUEST_NULL;
  }
  MPI_Request& startRequest(MPI_Request& req) {
    if (req == MPI_REQUEST_NULL)
      return req;
    RequestData * ret;
    {
    std::shared_lock<std::shared_mutex> lock(MapMutex);
    ret = handleMap[req].front();
    }
    assert (ret->isPersistent());
    ret->start();
    return req;
  }
};

template <typename M, typename T>
class HandleFactory<M,T,std::map<M,T*>> : public virtual AbstractHandleFactory<M, T>,
                                 public DataPool<M, T> {
protected:
  mutable std::shared_mutex MapMutex{};
  std::unordered_map<M, T *> handleMap{};

public:
  void printInfo()
  {printf("handleMap.size() = %li\n", handleMap.size());}
  void clear(){
    this->clearPool();
    handleMap.clear();
    for (auto i : this->predefHandles)
      if (i.second)
        delete(i.second);
    this->predefHandles.clear();
#ifdef FORTRAN_SUPPORT
    this->predefFHandles.clear();
#endif
  }
#ifdef FORTRAN_SUPPORT
  M f2c(MPI_Fint fhandle) {
    assert(false);
    return 0;
  }
  MPI_Fint c2f(M handle) {
    assert(false);
    return 0;
  }
#endif
  T* newData(){
    return this->getData();
  }
  M newHandle(M& handle) {
    if (this->isPredefined(handle))
      return handle;
    T *ret = this->getData();
    return newHandle(handle, ret);
  }
  M newHandle(M& handle, T* ret) {
    {
      std::unique_lock<std::shared_mutex> lock(MapMutex);
      auto retEmplace = handleMap.emplace(handle, ret);
    }
    ret->init(handle);
    return handle;
  }
  // returning to the datapool using lock
  M freeHandle(M handle) {
    if (this->isPredefined(handle))
      return T::nullHandle;
    T *rData;
    {
      std::unique_lock<std::shared_mutex> lock(MapMutex);
      rData = handleMap[handle];
      handleMap.erase(handle);
    }
    rData->fini();
    this->returnData(rData);
    return T::nullHandle;
  }
  T *findData(M handle) {
    T* ret = this->findPredefinedData(handle);
    if (ret != nullptr)
      return ret;
    std::shared_lock<std::shared_mutex> lock(MapMutex);
    return handleMap[handle];
  }
  M& getHandle(M& handle) { return handle; }
  M& getHandleLocked(M& handle) { return handle; }
  const M &getHandle(const M &handle) { return handle; }
  const M &getHandleLocked(const M &handle) { return handle; }
  std::shared_lock<std::shared_mutex> getSharedLock() {
    return std::shared_lock<std::shared_mutex>{MapMutex, std::defer_lock};
  }
  virtual ~HandleFactory<M, T, std::map<M,T*>>() {}
};

template<>
class RequestFactoryInst<std::map<MPI_Request, RequestData*>> : public HandleFactory<MPI_Request, RequestData, std::map<MPI_Request, RequestData*>>,
                       public AbstractRequestFactory {
  template <typename D>
  MPI_Request _newRequest(MPI_Request req, D &reqData, bool persistent) {
    if (req == MPI_REQUEST_NULL)
      return MPI_REQUEST_NULL;
    RequestData *ret = this->getData();
    ret->init(req,
#ifdef FORTRAN_SUPPORT
              -1,
#endif
              reqData, persistent);
    {
      std::unique_lock<std::shared_mutex> lock(MapMutex);
      auto retEmplace = handleMap.emplace(req, ret);
      assert(retEmplace.second);
    }
    return req;
  }
public:
  // get data from the pool
  MPI_Request newRequest(MPI_Request req, bool persistent) {
    const void *reqData = nullptr;
    return _newRequest(req, reqData, persistent);
  }
  /*  MPI_Request newRequest(MPI_Request req, p2pRequest &reqData,
                           bool persistent) {
      return _newRequest(req, reqData, persistent);
    }
    MPI_Request newRequest(MPI_Request req, nbcRequest &reqData,
                           bool persistent) {
      return _newRequest(req, reqData, persistent);
    }*/
  MPI_Request completeRequest(MPI_Request req, MPI_Status *status) {
    if (req == MPI_REQUEST_NULL)
      return MPI_REQUEST_NULL;
    RequestData *rData;
    {
      std::unique_lock<std::shared_mutex> lock(MapMutex);
      rData = handleMap[req];
      if (!rData->isPersistent())
      handleMap.erase(req);
    }
      if (rData->isPersistent()){
        rData->complete(status);
        return req;}
    rData->fini(status);
    this->returnData(rData);
    return MPI_REQUEST_NULL;
  }
  MPI_Request& startRequest(MPI_Request& req) {
    if (req == MPI_REQUEST_NULL)
      return req;
    RequestData * ret;
    {
    std::shared_lock<std::shared_mutex> lock(MapMutex);
    ret = handleMap[req];
    }
    assert (ret->isPersistent());
    ret->start();
    return req;
  }
};

template <typename M, typename T, typename MI>
class HandleFactory<M,T,MI*> : public virtual AbstractHandleFactory<M, T>,
                           public DataPool<M, T> {
protected:
  mutable std::shared_mutex DummyMutex{};

#ifdef FORTRAN_SUPPORT
  mutable std::shared_mutex DTMutex{};
  mutable std::shared_mutex AHMutex{};
  std::vector<T *> dataTable{};
  std::vector<MPI_Fint> availableHandles{};

  void newAH() {
    int ndatas = 4096 / 64;
    size_t oldSize = dataTable.size(), newSize = oldSize + ndatas;
    {
      // Only the resize actually modifies the vector and
      // can lead to a reallocation of the elements
      std::unique_lock<std::shared_mutex> lock(DTMutex);
      dataTable.resize(newSize);
    }
    // Getting the lock here is not necessary:
    // unique_lock can only be taken while having unique AHMutex
    // std::shared_lock<std::shared_mutex> lock(DTMutex);
    for (size_t i = oldSize, j = 0; i < newSize; i++, j++) {
      dataTable[i] = nullptr;
      availableHandles.emplace_back((int)(i));
    }
  }
#endif

public:
  void printInfo()
  {printf("handleMap.size() = %li\n", this->totalDatas - this->dataPointer.size());}
  void clear(){
    this->clearPool();
#ifdef FORTRAN_SUPPORT
    dataTable.clear();
    availableHandles.clear();
#endif
    for (auto i : this->predefHandles)
      if (i.second)
        delete(i.second);
    this->predefHandles.clear();
#ifdef FORTRAN_SUPPORT
    this->predefFHandles.clear();
#endif
  }
#ifdef FORTRAN_SUPPORT
  M f2c(MPI_Fint fhandle) {
    std::shared_lock<std::shared_mutex> lock(DTMutex);
    return (M)(uintptr_t)dataTable[(size_t)(fhandle)];
  }
  MPI_Fint c2f(M handle) { return ((T *)(uintptr_t)(handle))->fHandle; }
#endif
  T* newData(){
    return this->getData();
  }
  M newHandle(M& handle) {
    if (this->isPredefined(handle))
      return handle;
    T *ret = this->getData();
    return newHandle(handle, ret);
  }
  M newHandle(M& handle, T* ret) {
#ifdef FORTRAN_SUPPORT
    MPI_Fint fId;
    {
      std::unique_lock<std::shared_mutex> lock(AHMutex);
      if (availableHandles.size()<5)
        newAH();
      fId = availableHandles.back();
      availableHandles.pop_back();
      // Getting the lock here is not necessary:
      // unique_lock can only be taken while having unique AHMutex
      // std::shared_lock<std::shared_mutex> lock(DTMutex);
      dataTable[(size_t)(fId)] = ret;
    }
    ret->init(handle, fId);
#else
    ret->init(handle);
#endif
    return (M)(uintptr_t)ret;
  }
  M freeHandle(M handle) {
    if (this->isPredefined(handle))
      return T::nullHandle;
#ifdef FORTRAN_SUPPORT
    {
      auto fId = ((T *)(uintptr_t)(handle))->fHandle;
      std::unique_lock<std::shared_mutex> lock(AHMutex);
      availableHandles.emplace_back(fId);
    }
#endif
    ((T *)(uintptr_t)(handle))->fini();
    this->returnData((T *)(uintptr_t)(handle));
    return T::nullHandle;
  }
  T *findData(M handle) {
    T* ret = this->findPredefinedData(handle);
    if (ret != nullptr)
      return ret;
    return (T *)(uintptr_t)(handle);
  }
  M& getHandle(M& handle) {
    if (this->isPredefined(handle))
      return handle;
    return ((T *)(uintptr_t)(handle))->handle;
  }
  M& getHandleLocked(M& handle) {
    if (this->isPredefined(handle))
      return handle;
    return ((T *)(uintptr_t)(handle))->handle;
  }
  const M &getHandle(const M &handle) {
    if (this->isPredefined(handle))
      return handle;
    return ((T *)(uintptr_t)(handle))->handle;
  }
  const M &getHandleLocked(const M &handle) {
    if (this->isPredefined(handle))
      return handle;
    return ((T *)(uintptr_t)(handle))->handle;
  }
  std::shared_lock<std::shared_mutex> getSharedLock() {
    return std::shared_lock<std::shared_mutex>{DummyMutex, std::defer_lock};
  }
  virtual ~HandleFactory<M,T,MI*>() {}
};

template<typename MI>
class RequestFactoryInst<MI*> : public HandleFactory<MPI_Request, RequestData, MI*>,
                           public AbstractRequestFactory {
  template <typename D>
  MPI_Request _newRequest(MPI_Request req, D &reqData, bool persistent) {
    if (req == MPI_REQUEST_NULL)
      return MPI_REQUEST_NULL;
    RequestData *ret = this->getData();
#ifdef FORTRAN_SUPPORT
    MPI_Fint fId;
    {
      std::unique_lock<std::shared_mutex> lock(this->AHMutex);
      if (this->availableHandles.size()<5)
        this->newAH();
      fId = this->availableHandles.back();
      this->availableHandles.pop_back();
      // Getting the lock here is not necessary:
      // unique_lock can only be taken while having unique AHMutex
      // std::shared_lock<std::shared_mutex> lock(DTMutex);
      this->dataTable[(size_t)(fId)] = ret;
    }
    ret->init(req, fId, reqData, persistent);
#else
    ret->init(req, reqData, persistent);
#endif
    return (MPI_Request)(uintptr_t)ret;
  }

public:
  // get data from the pool
  MPI_Request newRequest(MPI_Request req, bool persistent) {
    const void *reqData = nullptr;
    return _newRequest(req, reqData, persistent);
  }
  /*  MPI_Request newRequest(MPI_Request req, p2pRequest &reqData,
                           bool persistent) {
      return _newRequest(req, reqData, persistent);
    }
    MPI_Request newRequest(MPI_Request req, nbcRequest &reqData,
                           bool persistent) {
      return _newRequest(req, reqData, persistent);
    }*/

  MPI_Request completeRequest(MPI_Request req, MPI_Status *status) {
    if (req == MPI_REQUEST_NULL)
      return MPI_REQUEST_NULL;
    if (((RequestData *)(uintptr_t)(req))->isPersistent()){
      ((RequestData *)(uintptr_t)(req))->complete(status);
      return req;
    }
#ifdef FORTRAN_SUPPORT
    {
      auto fId = ((RequestData *)(uintptr_t)(req))->fHandle;
      std::unique_lock<std::shared_mutex> lock(this->AHMutex);
      this->availableHandles.emplace_back(fId);
    }
#endif
    ((RequestData *)(uintptr_t)(req))->fini(status);
    this->returnData((RequestData *)(uintptr_t)(req));
    return MPI_REQUEST_NULL;
  }
  MPI_Request& startRequest(MPI_Request& req) {
    if (req == MPI_REQUEST_NULL)
      return req;
    RequestData * ret = ((RequestData *)(uintptr_t)(req));
    assert (ret->isPersistent());
    ret->start();
    return ret->handle;
  }
};

template <typename M, typename T>
class HandleFactory<M,T,int> : public virtual AbstractHandleFactory<M, T>, public DataPool<M, T> {
protected:
  mutable std::shared_mutex DTMutex{};
  mutable std::shared_mutex AHMutex{};
  std::vector<T *> dataTable{};
  std::vector<M> availableHandles{};

  void newAH() {
    int ndatas = 4096 / 64;
    size_t oldSize = dataTable.size(), newSize = oldSize + ndatas;
    {
      // Only the resize actually modifies the vector and
      // can lead to a reallocation of the elements
      std::unique_lock<std::shared_mutex> lock(DTMutex);
      dataTable.resize(newSize);
    }
    // Getting the lock here is not necessary:
    // unique_lock can only be taken while having unique AHMutex
    // std::shared_lock<std::shared_mutex> lock(DTMutex);
    for (size_t i = oldSize, j = 0; i < newSize; i++, j++) {
      dataTable[i] = nullptr;
      availableHandles.emplace_back((M)(i));
    }
  }

public:
  void printInfo()
  {printf("handleMap.size() = %li\n", this->totalDatas - this->dataPointer.size());}
  void clear(){
    this->clearPool();
    dataTable.clear();
    availableHandles.clear();
    for (auto i : this->predefHandles)
      if (i.second)
        delete(i.second);
    this->predefHandles.clear();
#ifdef FORTRAN_SUPPORT
    this->predefFHandles.clear();
#endif
  }
#ifdef FORTRAN_SUPPORT
  MPI_Request f2c(MPI_Fint fhandle) { return (MPI_Request)fhandle; }
  MPI_Fint c2f(MPI_Request handle) { return (MPI_Fint)handle; }
#endif
  T* newData(){
    return this->getData();
  }
  M newHandle(M& handle) {
    if (this->isPredefined(handle))
      return handle;
    T *ret = this->getData();
    return newHandle(handle, ret);
  }
  M newHandle(M& handle, T* dp) {
    debug_printf("%s(%p)\n", __PRETTY_FUNCTION__, (void*)(intptr_t)handle);
    M ret;
    {
      std::unique_lock<std::shared_mutex> lock(AHMutex);
      if (availableHandles.size()<5)
        newAH();
      ret = availableHandles.back();
      availableHandles.pop_back();
      // Getting the lock here is not necessary:
      // unique_lock can only be taken while having unique AHMutex
      // std::shared_lock<std::shared_mutex> lock(DTMutex);
      dataTable[(size_t)(ret)] = dp;
    }
    dp->init(handle);
    return ret;
  }

  M freeHandle(M handle) {
    if (this->isPredefined(handle))
      return T::nullHandle;
    T* ret;
    {
      std::unique_lock<std::shared_mutex> lock(AHMutex);
      ret = dataTable[(size_t)(handle)];
      availableHandles.emplace_back(handle);
    }
    ret->fini();
    this->returnData(ret);
    return T::nullHandle;
  }
  T *findData(M handle) {
    T* ret = this->findPredefinedData(handle);
    if (ret != nullptr)
      return ret;
    std::shared_lock<std::shared_mutex> lock(DTMutex);
    return dataTable[(size_t)(handle)];
  }
  M& getHandle(M& handle) {
    debug_printf("%s(%p)\n", __PRETTY_FUNCTION__, (void*)(intptr_t)handle);
    if (this->isPredefined(handle))
      return handle;
    std::shared_lock<std::shared_mutex> lock(DTMutex);
    return dataTable[(size_t)(handle)]->handle;
  }
  M& getHandleLocked(M& handle) {
    if (this->isPredefined(handle))
      return handle;
    return dataTable[(size_t)(handle)]->handle;
  }
  const M &getHandle(const M &handle) {
    if (this->isPredefined(handle))
      return handle;
    std::shared_lock<std::shared_mutex> lock(DTMutex);
    return dataTable[(size_t)(handle)]->handle;
  }
  const M &getHandleLocked(const M &handle) {
    if (this->isPredefined(handle))
      return handle;
    return dataTable[(size_t)(handle)]->handle;
  }
  std::shared_lock<std::shared_mutex> getSharedLock() {
    return std::shared_lock<std::shared_mutex>{DTMutex};
  }
  virtual ~HandleFactory<M,T,int>(){}
};

template<>
class RequestFactoryInst<int> : public HandleFactory<MPI_Request, RequestData, int>,
                       public AbstractRequestFactory {
  template <typename D>
  MPI_Request _newRequest(MPI_Request req, D &reqData, bool persistent) {
    if (req == MPI_REQUEST_NULL)
      return MPI_REQUEST_NULL;
    MPI_Request ret;
    RequestData *dp = this->getData();
    {
      std::unique_lock<std::shared_mutex> lock(AHMutex);
      if (availableHandles.size()<5)
        newAH();
      ret = availableHandles.back();
      availableHandles.pop_back();
      dataTable[(size_t)(ret)] = dp;
    }
    dp->init(req, 
#ifdef FORTRAN_SUPPORT
    (size_t)ret, 
#endif
    reqData, persistent);
    return ret;
  }
public:
  ~RequestFactoryInst<int>(){debug_printf("RequestList size=%li\n", this->availableHandles.size());}
  // get data from the pool
  MPI_Request newRequest(MPI_Request req, bool persistent, RequestData *data) {
    if (req == MPI_REQUEST_NULL)
      return MPI_REQUEST_NULL;
    MPI_Request ret;
      std::unique_lock<std::shared_mutex> lock(AHMutex);
      if (availableHandles.size()<5)
        newAH();
      ret = availableHandles.back();
      availableHandles.pop_back();
      dataTable[(size_t)(ret)] = data;
    return ret;
  }
  MPI_Request newRequest(MPI_Request req, bool persistent) {
    const void *reqData = nullptr;
    return _newRequest(req, reqData, persistent);
  }
  /*  MPI_Request newRequest(MPI_Request req, p2pRequest &reqData,
                           bool persistent) {
      return _newRequest(req, reqData, persistent);
    }
    MPI_Request newRequest(MPI_Request req, nbcRequest &reqData,
                           bool persistent) {
      return _newRequest(req, reqData, persistent);
    }*/

  // returning to the datapool using lock
  MPI_Request completeRequest(MPI_Request req, MPI_Status *status) {
    if (req == MPI_REQUEST_NULL)
      return MPI_REQUEST_NULL;
    RequestData * ret;
    bool persistent{false};
    {
    std::unique_lock<std::shared_mutex> lock(AHMutex);
    ret = dataTable[(size_t)(req)];
    persistent = ret->isPersistent();
    if(!persistent){
    availableHandles.emplace_back(req);
    }}
    if (persistent){
      ret->complete(status);
      return req;
    }
    ret->fini(status);
    this->returnData(ret);
    return MPI_REQUEST_NULL;
  }
  MPI_Request& startRequest(MPI_Request& req) {
    if (req == MPI_REQUEST_NULL)
      return req;
    RequestData * ret;
    {
      std::shared_lock<std::shared_mutex> lock(DTMutex);
      ret = dataTable[(size_t)(req)];
    }
    assert (ret->isPersistent());
    ret->start();
    return ret->handle;
  }
};

using OpData = HandleData<MPI_Op, toolOpData>;
using WinData = HandleData<MPI_Win, toolWinData>;
using TypeData = HandleData<MPI_Datatype, toolTypeData>;
using FileData = HandleData<MPI_File, toolFileData>;
using CommData = HandleData<MPI_Comm, toolCommData>;
using GroupData = HandleData<MPI_Group, toolGroupData>;
#ifdef HAVE_SESSION
using SessionData = HandleData<MPI_Session, toolSessionData>;
#endif

#ifndef HANDLE_MAP
using OpFactory = HandleFactory<MPI_Op, OpData, MPI_Op>;
using WinFactory = HandleFactory<MPI_Win, WinData, MPI_Win>;
using TypeFactory = HandleFactory<MPI_Datatype, TypeData, MPI_Datatype>;
using FileFactory = HandleFactory<MPI_File, FileData, MPI_File>;
using CommFactory = HandleFactory<MPI_Comm, CommData, MPI_Comm>;
using GroupFactory = HandleFactory<MPI_Group, GroupData, MPI_Group>;
using RequestFactory = RequestFactoryInst<MPI_Request>;
#else
using OpFactory = HandleFactory<MPI_Op, OpData, std::multimap<MPI_Op, OpData*>>;
using WinFactory = HandleFactory<MPI_Win, WinData, std::multimap<MPI_Win, WinData*>>;
using TypeFactory = HandleFactory<MPI_Datatype, TypeData, std::multimap<MPI_Datatype, TypeData*>>;
using FileFactory = HandleFactory<MPI_File, FileData, std::multimap<MPI_File, FileData*>>;
using CommFactory = HandleFactory<MPI_Comm, CommData, std::multimap<MPI_Comm, CommData*>>;
using GroupFactory = HandleFactory<MPI_Group, GroupData, std::multimap<MPI_Group, GroupData*>>;
using RequestFactory = RequestFactoryInst<std::multimap<MPI_Request, RequestData*>>;
#endif

#ifdef HAVE_SESSION
#ifndef HANDLE_MAP
using SessionFactory = HandleFactory<MPI_Session, SessionData, MPI_Session>;
#else
using MapListSessionFactory = HandleFactory<MPI_Session, SessionData, std::multimap<MPI_Session, SessionData*>>;
#endif
#endif


#ifdef HANDLE_OP
extern OpFactory of;
#endif
#ifdef HANDLE_WIN
extern WinFactory wf;
#endif
#ifdef HANDLE_TYPE
extern TypeFactory tf;
#endif
#ifdef HANDLE_FILE
extern FileFactory ff;
#endif
#ifdef HANDLE_COMM
template <> bool AbstractHandleFactory<MPI_Comm, CommData>::isPredefined(MPI_Comm handle);

template <> void AbstractHandleFactory<MPI_Comm, CommData>::initPredefined();

extern CommFactory cf;
#endif
#ifdef HANDLE_GROUP
template <>
bool AbstractHandleFactory<MPI_Group, GroupData>::isPredefined(MPI_Group handle);

template <> void AbstractHandleFactory<MPI_Group, GroupData>::initPredefined();

extern GroupFactory gf;
#endif
#ifdef HANDLE_REQUEST
extern RequestFactory rf;
#endif
#if defined(HAVE_SESSION) && defined(HANDLE_SESSION)
extern SessionFactory sf;
#else
typedef int MPI_Session;
#endif

#endif
#endif
