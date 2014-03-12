// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: metazht.proto

#ifndef PROTOBUF_metazht_2eproto__INCLUDED
#define PROTOBUF_metazht_2eproto__INCLUDED

#include <string>

#include <google/protobuf/stubs/common.h>

#if GOOGLE_PROTOBUF_VERSION < 2004000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please update
#error your headers.
#endif
#if 2004001 < GOOGLE_PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/repeated_field.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/generated_message_reflection.h>
// @@protoc_insertion_point(includes)

// Internal implementation detail -- do not call these.
void  protobuf_AddDesc_metazht_2eproto();
void protobuf_AssignDesc_metazht_2eproto();
void protobuf_ShutdownFile_metazht_2eproto();

class Value;

// ===================================================================

class Value : public ::google::protobuf::Message {
 public:
  Value();
  virtual ~Value();
  
  Value(const Value& from);
  
  inline Value& operator=(const Value& from) {
    CopyFrom(from);
    return *this;
  }
  
  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _unknown_fields_;
  }
  
  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return &_unknown_fields_;
  }
  
  static const ::google::protobuf::Descriptor* descriptor();
  static const Value& default_instance();
  
  void Swap(Value* other);
  
  // implements Message ----------------------------------------------
  
  Value* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const Value& from);
  void MergeFrom(const Value& from);
  void Clear();
  bool IsInitialized() const;
  
  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const;
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  public:
  
  ::google::protobuf::Metadata GetMetadata() const;
  
  // nested types ----------------------------------------------------
  
  // accessors -------------------------------------------------------
  
  // required string id = 1;
  inline bool has_id() const;
  inline void clear_id();
  static const int kIdFieldNumber = 1;
  inline const ::std::string& id() const;
  inline void set_id(const ::std::string& value);
  inline void set_id(const char* value);
  inline void set_id(const char* value, size_t size);
  inline ::std::string* mutable_id();
  inline ::std::string* release_id();
  
  // optional int64 indegree = 2;
  inline bool has_indegree() const;
  inline void clear_indegree();
  static const int kIndegreeFieldNumber = 2;
  inline ::google::protobuf::int64 indegree() const;
  inline void set_indegree(::google::protobuf::int64 value);
  
  // optional int64 allDataSize = 3;
  inline bool has_alldatasize() const;
  inline void clear_alldatasize();
  static const int kAllDataSizeFieldNumber = 3;
  inline ::google::protobuf::int64 alldatasize() const;
  inline void set_alldatasize(::google::protobuf::int64 value);
  
  // optional int32 numMove = 4;
  inline bool has_nummove() const;
  inline void clear_nummove();
  static const int kNumMoveFieldNumber = 4;
  inline ::google::protobuf::int32 nummove() const;
  inline void set_nummove(::google::protobuf::int32 value);
  
  // optional string history = 5;
  inline bool has_history() const;
  inline void clear_history();
  static const int kHistoryFieldNumber = 5;
  inline const ::std::string& history() const;
  inline void set_history(const ::std::string& value);
  inline void set_history(const char* value);
  inline void set_history(const char* value, size_t size);
  inline ::std::string* mutable_history();
  inline ::std::string* release_history();
  
  // optional double submitTime = 6;
  inline bool has_submittime() const;
  inline void clear_submittime();
  static const int kSubmitTimeFieldNumber = 6;
  inline double submittime() const;
  inline void set_submittime(double value);
  
  // optional double arriveTime = 7;
  inline bool has_arrivetime() const;
  inline void clear_arrivetime();
  static const int kArriveTimeFieldNumber = 7;
  inline double arrivetime() const;
  inline void set_arrivetime(double value);
  
  // optional double rqueuedTime = 8;
  inline bool has_rqueuedtime() const;
  inline void clear_rqueuedtime();
  static const int kRqueuedTimeFieldNumber = 8;
  inline double rqueuedtime() const;
  inline void set_rqueuedtime(double value);
  
  // optional double exeTime = 9;
  inline bool has_exetime() const;
  inline void clear_exetime();
  static const int kExeTimeFieldNumber = 9;
  inline double exetime() const;
  inline void set_exetime(double value);
  
  // optional double finTime = 10;
  inline bool has_fintime() const;
  inline void clear_fintime();
  static const int kFinTimeFieldNumber = 10;
  inline double fintime() const;
  inline void set_fintime(double value);
  
  // optional int64 numTaskFin = 11;
  inline bool has_numtaskfin() const;
  inline void clear_numtaskfin();
  static const int kNumTaskFinFieldNumber = 11;
  inline ::google::protobuf::int64 numtaskfin() const;
  inline void set_numtaskfin(::google::protobuf::int64 value);
  
  // optional int32 numTaskWait = 12;
  inline bool has_numtaskwait() const;
  inline void clear_numtaskwait();
  static const int kNumTaskWaitFieldNumber = 12;
  inline ::google::protobuf::int32 numtaskwait() const;
  inline void set_numtaskwait(::google::protobuf::int32 value);
  
  // optional int32 numTaskReady = 13;
  inline bool has_numtaskready() const;
  inline void clear_numtaskready();
  static const int kNumTaskReadyFieldNumber = 13;
  inline ::google::protobuf::int32 numtaskready() const;
  inline void set_numtaskready(::google::protobuf::int32 value);
  
  // optional int32 numCoreAvilable = 14;
  inline bool has_numcoreavilable() const;
  inline void clear_numcoreavilable();
  static const int kNumCoreAvilableFieldNumber = 14;
  inline ::google::protobuf::int32 numcoreavilable() const;
  inline void set_numcoreavilable(::google::protobuf::int32 value);
  
  // optional int32 numAllCore = 15;
  inline bool has_numallcore() const;
  inline void clear_numallcore();
  static const int kNumAllCoreFieldNumber = 15;
  inline ::google::protobuf::int32 numallcore() const;
  inline void set_numallcore(::google::protobuf::int32 value);
  
  // optional int64 numWorkSteal = 16;
  inline bool has_numworksteal() const;
  inline void clear_numworksteal();
  static const int kNumWorkStealFieldNumber = 16;
  inline ::google::protobuf::int64 numworksteal() const;
  inline void set_numworksteal(::google::protobuf::int64 value);
  
  // optional int64 numWorkStealFail = 17;
  inline bool has_numworkstealfail() const;
  inline void clear_numworkstealfail();
  static const int kNumWorkStealFailFieldNumber = 17;
  inline ::google::protobuf::int64 numworkstealfail() const;
  inline void set_numworkstealfail(::google::protobuf::int64 value);
  
  // repeated string parents = 18;
  inline int parents_size() const;
  inline void clear_parents();
  static const int kParentsFieldNumber = 18;
  inline const ::std::string& parents(int index) const;
  inline ::std::string* mutable_parents(int index);
  inline void set_parents(int index, const ::std::string& value);
  inline void set_parents(int index, const char* value);
  inline void set_parents(int index, const char* value, size_t size);
  inline ::std::string* add_parents();
  inline void add_parents(const ::std::string& value);
  inline void add_parents(const char* value);
  inline void add_parents(const char* value, size_t size);
  inline const ::google::protobuf::RepeatedPtrField< ::std::string>& parents() const;
  inline ::google::protobuf::RepeatedPtrField< ::std::string>* mutable_parents();
  
  // repeated string children = 19;
  inline int children_size() const;
  inline void clear_children();
  static const int kChildrenFieldNumber = 19;
  inline const ::std::string& children(int index) const;
  inline ::std::string* mutable_children(int index);
  inline void set_children(int index, const ::std::string& value);
  inline void set_children(int index, const char* value);
  inline void set_children(int index, const char* value, size_t size);
  inline ::std::string* add_children();
  inline void add_children(const ::std::string& value);
  inline void add_children(const char* value);
  inline void add_children(const char* value, size_t size);
  inline const ::google::protobuf::RepeatedPtrField< ::std::string>& children() const;
  inline ::google::protobuf::RepeatedPtrField< ::std::string>* mutable_children();
  
  // repeated string dataNameList = 20;
  inline int datanamelist_size() const;
  inline void clear_datanamelist();
  static const int kDataNameListFieldNumber = 20;
  inline const ::std::string& datanamelist(int index) const;
  inline ::std::string* mutable_datanamelist(int index);
  inline void set_datanamelist(int index, const ::std::string& value);
  inline void set_datanamelist(int index, const char* value);
  inline void set_datanamelist(int index, const char* value, size_t size);
  inline ::std::string* add_datanamelist();
  inline void add_datanamelist(const ::std::string& value);
  inline void add_datanamelist(const char* value);
  inline void add_datanamelist(const char* value, size_t size);
  inline const ::google::protobuf::RepeatedPtrField< ::std::string>& datanamelist() const;
  inline ::google::protobuf::RepeatedPtrField< ::std::string>* mutable_datanamelist();
  
  // repeated int64 dataSize = 21;
  inline int datasize_size() const;
  inline void clear_datasize();
  static const int kDataSizeFieldNumber = 21;
  inline ::google::protobuf::int64 datasize(int index) const;
  inline void set_datasize(int index, ::google::protobuf::int64 value);
  inline void add_datasize(::google::protobuf::int64 value);
  inline const ::google::protobuf::RepeatedField< ::google::protobuf::int64 >&
      datasize() const;
  inline ::google::protobuf::RepeatedField< ::google::protobuf::int64 >*
      mutable_datasize();
  
  // @@protoc_insertion_point(class_scope:Value)
 private:
  inline void set_has_id();
  inline void clear_has_id();
  inline void set_has_indegree();
  inline void clear_has_indegree();
  inline void set_has_alldatasize();
  inline void clear_has_alldatasize();
  inline void set_has_nummove();
  inline void clear_has_nummove();
  inline void set_has_history();
  inline void clear_has_history();
  inline void set_has_submittime();
  inline void clear_has_submittime();
  inline void set_has_arrivetime();
  inline void clear_has_arrivetime();
  inline void set_has_rqueuedtime();
  inline void clear_has_rqueuedtime();
  inline void set_has_exetime();
  inline void clear_has_exetime();
  inline void set_has_fintime();
  inline void clear_has_fintime();
  inline void set_has_numtaskfin();
  inline void clear_has_numtaskfin();
  inline void set_has_numtaskwait();
  inline void clear_has_numtaskwait();
  inline void set_has_numtaskready();
  inline void clear_has_numtaskready();
  inline void set_has_numcoreavilable();
  inline void clear_has_numcoreavilable();
  inline void set_has_numallcore();
  inline void clear_has_numallcore();
  inline void set_has_numworksteal();
  inline void clear_has_numworksteal();
  inline void set_has_numworkstealfail();
  inline void clear_has_numworkstealfail();
  
  ::google::protobuf::UnknownFieldSet _unknown_fields_;
  
  ::std::string* id_;
  ::google::protobuf::int64 indegree_;
  ::google::protobuf::int64 alldatasize_;
  ::std::string* history_;
  double submittime_;
  double arrivetime_;
  double rqueuedtime_;
  ::google::protobuf::int32 nummove_;
  ::google::protobuf::int32 numtaskwait_;
  double exetime_;
  double fintime_;
  ::google::protobuf::int64 numtaskfin_;
  ::google::protobuf::int32 numtaskready_;
  ::google::protobuf::int32 numcoreavilable_;
  ::google::protobuf::int64 numworksteal_;
  ::google::protobuf::int64 numworkstealfail_;
  ::google::protobuf::RepeatedPtrField< ::std::string> parents_;
  ::google::protobuf::RepeatedPtrField< ::std::string> children_;
  ::google::protobuf::RepeatedPtrField< ::std::string> datanamelist_;
  ::google::protobuf::RepeatedField< ::google::protobuf::int64 > datasize_;
  ::google::protobuf::int32 numallcore_;
  
  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(21 + 31) / 32];
  
  friend void  protobuf_AddDesc_metazht_2eproto();
  friend void protobuf_AssignDesc_metazht_2eproto();
  friend void protobuf_ShutdownFile_metazht_2eproto();
  
  void InitAsDefaultInstance();
  static Value* default_instance_;
};
// ===================================================================


// ===================================================================

// Value

// required string id = 1;
inline bool Value::has_id() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void Value::set_has_id() {
  _has_bits_[0] |= 0x00000001u;
}
inline void Value::clear_has_id() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void Value::clear_id() {
  if (id_ != &::google::protobuf::internal::kEmptyString) {
    id_->clear();
  }
  clear_has_id();
}
inline const ::std::string& Value::id() const {
  return *id_;
}
inline void Value::set_id(const ::std::string& value) {
  set_has_id();
  if (id_ == &::google::protobuf::internal::kEmptyString) {
    id_ = new ::std::string;
  }
  id_->assign(value);
}
inline void Value::set_id(const char* value) {
  set_has_id();
  if (id_ == &::google::protobuf::internal::kEmptyString) {
    id_ = new ::std::string;
  }
  id_->assign(value);
}
inline void Value::set_id(const char* value, size_t size) {
  set_has_id();
  if (id_ == &::google::protobuf::internal::kEmptyString) {
    id_ = new ::std::string;
  }
  id_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* Value::mutable_id() {
  set_has_id();
  if (id_ == &::google::protobuf::internal::kEmptyString) {
    id_ = new ::std::string;
  }
  return id_;
}
inline ::std::string* Value::release_id() {
  clear_has_id();
  if (id_ == &::google::protobuf::internal::kEmptyString) {
    return NULL;
  } else {
    ::std::string* temp = id_;
    id_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
    return temp;
  }
}

// optional int64 indegree = 2;
inline bool Value::has_indegree() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void Value::set_has_indegree() {
  _has_bits_[0] |= 0x00000002u;
}
inline void Value::clear_has_indegree() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void Value::clear_indegree() {
  indegree_ = GOOGLE_LONGLONG(0);
  clear_has_indegree();
}
inline ::google::protobuf::int64 Value::indegree() const {
  return indegree_;
}
inline void Value::set_indegree(::google::protobuf::int64 value) {
  set_has_indegree();
  indegree_ = value;
}

// optional int64 allDataSize = 3;
inline bool Value::has_alldatasize() const {
  return (_has_bits_[0] & 0x00000004u) != 0;
}
inline void Value::set_has_alldatasize() {
  _has_bits_[0] |= 0x00000004u;
}
inline void Value::clear_has_alldatasize() {
  _has_bits_[0] &= ~0x00000004u;
}
inline void Value::clear_alldatasize() {
  alldatasize_ = GOOGLE_LONGLONG(0);
  clear_has_alldatasize();
}
inline ::google::protobuf::int64 Value::alldatasize() const {
  return alldatasize_;
}
inline void Value::set_alldatasize(::google::protobuf::int64 value) {
  set_has_alldatasize();
  alldatasize_ = value;
}

// optional int32 numMove = 4;
inline bool Value::has_nummove() const {
  return (_has_bits_[0] & 0x00000008u) != 0;
}
inline void Value::set_has_nummove() {
  _has_bits_[0] |= 0x00000008u;
}
inline void Value::clear_has_nummove() {
  _has_bits_[0] &= ~0x00000008u;
}
inline void Value::clear_nummove() {
  nummove_ = 0;
  clear_has_nummove();
}
inline ::google::protobuf::int32 Value::nummove() const {
  return nummove_;
}
inline void Value::set_nummove(::google::protobuf::int32 value) {
  set_has_nummove();
  nummove_ = value;
}

// optional string history = 5;
inline bool Value::has_history() const {
  return (_has_bits_[0] & 0x00000010u) != 0;
}
inline void Value::set_has_history() {
  _has_bits_[0] |= 0x00000010u;
}
inline void Value::clear_has_history() {
  _has_bits_[0] &= ~0x00000010u;
}
inline void Value::clear_history() {
  if (history_ != &::google::protobuf::internal::kEmptyString) {
    history_->clear();
  }
  clear_has_history();
}
inline const ::std::string& Value::history() const {
  return *history_;
}
inline void Value::set_history(const ::std::string& value) {
  set_has_history();
  if (history_ == &::google::protobuf::internal::kEmptyString) {
    history_ = new ::std::string;
  }
  history_->assign(value);
}
inline void Value::set_history(const char* value) {
  set_has_history();
  if (history_ == &::google::protobuf::internal::kEmptyString) {
    history_ = new ::std::string;
  }
  history_->assign(value);
}
inline void Value::set_history(const char* value, size_t size) {
  set_has_history();
  if (history_ == &::google::protobuf::internal::kEmptyString) {
    history_ = new ::std::string;
  }
  history_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* Value::mutable_history() {
  set_has_history();
  if (history_ == &::google::protobuf::internal::kEmptyString) {
    history_ = new ::std::string;
  }
  return history_;
}
inline ::std::string* Value::release_history() {
  clear_has_history();
  if (history_ == &::google::protobuf::internal::kEmptyString) {
    return NULL;
  } else {
    ::std::string* temp = history_;
    history_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
    return temp;
  }
}

// optional double submitTime = 6;
inline bool Value::has_submittime() const {
  return (_has_bits_[0] & 0x00000020u) != 0;
}
inline void Value::set_has_submittime() {
  _has_bits_[0] |= 0x00000020u;
}
inline void Value::clear_has_submittime() {
  _has_bits_[0] &= ~0x00000020u;
}
inline void Value::clear_submittime() {
  submittime_ = 0;
  clear_has_submittime();
}
inline double Value::submittime() const {
  return submittime_;
}
inline void Value::set_submittime(double value) {
  set_has_submittime();
  submittime_ = value;
}

// optional double arriveTime = 7;
inline bool Value::has_arrivetime() const {
  return (_has_bits_[0] & 0x00000040u) != 0;
}
inline void Value::set_has_arrivetime() {
  _has_bits_[0] |= 0x00000040u;
}
inline void Value::clear_has_arrivetime() {
  _has_bits_[0] &= ~0x00000040u;
}
inline void Value::clear_arrivetime() {
  arrivetime_ = 0;
  clear_has_arrivetime();
}
inline double Value::arrivetime() const {
  return arrivetime_;
}
inline void Value::set_arrivetime(double value) {
  set_has_arrivetime();
  arrivetime_ = value;
}

// optional double rqueuedTime = 8;
inline bool Value::has_rqueuedtime() const {
  return (_has_bits_[0] & 0x00000080u) != 0;
}
inline void Value::set_has_rqueuedtime() {
  _has_bits_[0] |= 0x00000080u;
}
inline void Value::clear_has_rqueuedtime() {
  _has_bits_[0] &= ~0x00000080u;
}
inline void Value::clear_rqueuedtime() {
  rqueuedtime_ = 0;
  clear_has_rqueuedtime();
}
inline double Value::rqueuedtime() const {
  return rqueuedtime_;
}
inline void Value::set_rqueuedtime(double value) {
  set_has_rqueuedtime();
  rqueuedtime_ = value;
}

// optional double exeTime = 9;
inline bool Value::has_exetime() const {
  return (_has_bits_[0] & 0x00000100u) != 0;
}
inline void Value::set_has_exetime() {
  _has_bits_[0] |= 0x00000100u;
}
inline void Value::clear_has_exetime() {
  _has_bits_[0] &= ~0x00000100u;
}
inline void Value::clear_exetime() {
  exetime_ = 0;
  clear_has_exetime();
}
inline double Value::exetime() const {
  return exetime_;
}
inline void Value::set_exetime(double value) {
  set_has_exetime();
  exetime_ = value;
}

// optional double finTime = 10;
inline bool Value::has_fintime() const {
  return (_has_bits_[0] & 0x00000200u) != 0;
}
inline void Value::set_has_fintime() {
  _has_bits_[0] |= 0x00000200u;
}
inline void Value::clear_has_fintime() {
  _has_bits_[0] &= ~0x00000200u;
}
inline void Value::clear_fintime() {
  fintime_ = 0;
  clear_has_fintime();
}
inline double Value::fintime() const {
  return fintime_;
}
inline void Value::set_fintime(double value) {
  set_has_fintime();
  fintime_ = value;
}

// optional int64 numTaskFin = 11;
inline bool Value::has_numtaskfin() const {
  return (_has_bits_[0] & 0x00000400u) != 0;
}
inline void Value::set_has_numtaskfin() {
  _has_bits_[0] |= 0x00000400u;
}
inline void Value::clear_has_numtaskfin() {
  _has_bits_[0] &= ~0x00000400u;
}
inline void Value::clear_numtaskfin() {
  numtaskfin_ = GOOGLE_LONGLONG(0);
  clear_has_numtaskfin();
}
inline ::google::protobuf::int64 Value::numtaskfin() const {
  return numtaskfin_;
}
inline void Value::set_numtaskfin(::google::protobuf::int64 value) {
  set_has_numtaskfin();
  numtaskfin_ = value;
}

// optional int32 numTaskWait = 12;
inline bool Value::has_numtaskwait() const {
  return (_has_bits_[0] & 0x00000800u) != 0;
}
inline void Value::set_has_numtaskwait() {
  _has_bits_[0] |= 0x00000800u;
}
inline void Value::clear_has_numtaskwait() {
  _has_bits_[0] &= ~0x00000800u;
}
inline void Value::clear_numtaskwait() {
  numtaskwait_ = 0;
  clear_has_numtaskwait();
}
inline ::google::protobuf::int32 Value::numtaskwait() const {
  return numtaskwait_;
}
inline void Value::set_numtaskwait(::google::protobuf::int32 value) {
  set_has_numtaskwait();
  numtaskwait_ = value;
}

// optional int32 numTaskReady = 13;
inline bool Value::has_numtaskready() const {
  return (_has_bits_[0] & 0x00001000u) != 0;
}
inline void Value::set_has_numtaskready() {
  _has_bits_[0] |= 0x00001000u;
}
inline void Value::clear_has_numtaskready() {
  _has_bits_[0] &= ~0x00001000u;
}
inline void Value::clear_numtaskready() {
  numtaskready_ = 0;
  clear_has_numtaskready();
}
inline ::google::protobuf::int32 Value::numtaskready() const {
  return numtaskready_;
}
inline void Value::set_numtaskready(::google::protobuf::int32 value) {
  set_has_numtaskready();
  numtaskready_ = value;
}

// optional int32 numCoreAvilable = 14;
inline bool Value::has_numcoreavilable() const {
  return (_has_bits_[0] & 0x00002000u) != 0;
}
inline void Value::set_has_numcoreavilable() {
  _has_bits_[0] |= 0x00002000u;
}
inline void Value::clear_has_numcoreavilable() {
  _has_bits_[0] &= ~0x00002000u;
}
inline void Value::clear_numcoreavilable() {
  numcoreavilable_ = 0;
  clear_has_numcoreavilable();
}
inline ::google::protobuf::int32 Value::numcoreavilable() const {
  return numcoreavilable_;
}
inline void Value::set_numcoreavilable(::google::protobuf::int32 value) {
  set_has_numcoreavilable();
  numcoreavilable_ = value;
}

// optional int32 numAllCore = 15;
inline bool Value::has_numallcore() const {
  return (_has_bits_[0] & 0x00004000u) != 0;
}
inline void Value::set_has_numallcore() {
  _has_bits_[0] |= 0x00004000u;
}
inline void Value::clear_has_numallcore() {
  _has_bits_[0] &= ~0x00004000u;
}
inline void Value::clear_numallcore() {
  numallcore_ = 0;
  clear_has_numallcore();
}
inline ::google::protobuf::int32 Value::numallcore() const {
  return numallcore_;
}
inline void Value::set_numallcore(::google::protobuf::int32 value) {
  set_has_numallcore();
  numallcore_ = value;
}

// optional int64 numWorkSteal = 16;
inline bool Value::has_numworksteal() const {
  return (_has_bits_[0] & 0x00008000u) != 0;
}
inline void Value::set_has_numworksteal() {
  _has_bits_[0] |= 0x00008000u;
}
inline void Value::clear_has_numworksteal() {
  _has_bits_[0] &= ~0x00008000u;
}
inline void Value::clear_numworksteal() {
  numworksteal_ = GOOGLE_LONGLONG(0);
  clear_has_numworksteal();
}
inline ::google::protobuf::int64 Value::numworksteal() const {
  return numworksteal_;
}
inline void Value::set_numworksteal(::google::protobuf::int64 value) {
  set_has_numworksteal();
  numworksteal_ = value;
}

// optional int64 numWorkStealFail = 17;
inline bool Value::has_numworkstealfail() const {
  return (_has_bits_[0] & 0x00010000u) != 0;
}
inline void Value::set_has_numworkstealfail() {
  _has_bits_[0] |= 0x00010000u;
}
inline void Value::clear_has_numworkstealfail() {
  _has_bits_[0] &= ~0x00010000u;
}
inline void Value::clear_numworkstealfail() {
  numworkstealfail_ = GOOGLE_LONGLONG(0);
  clear_has_numworkstealfail();
}
inline ::google::protobuf::int64 Value::numworkstealfail() const {
  return numworkstealfail_;
}
inline void Value::set_numworkstealfail(::google::protobuf::int64 value) {
  set_has_numworkstealfail();
  numworkstealfail_ = value;
}

// repeated string parents = 18;
inline int Value::parents_size() const {
  return parents_.size();
}
inline void Value::clear_parents() {
  parents_.Clear();
}
inline const ::std::string& Value::parents(int index) const {
  return parents_.Get(index);
}
inline ::std::string* Value::mutable_parents(int index) {
  return parents_.Mutable(index);
}
inline void Value::set_parents(int index, const ::std::string& value) {
  parents_.Mutable(index)->assign(value);
}
inline void Value::set_parents(int index, const char* value) {
  parents_.Mutable(index)->assign(value);
}
inline void Value::set_parents(int index, const char* value, size_t size) {
  parents_.Mutable(index)->assign(
    reinterpret_cast<const char*>(value), size);
}
inline ::std::string* Value::add_parents() {
  return parents_.Add();
}
inline void Value::add_parents(const ::std::string& value) {
  parents_.Add()->assign(value);
}
inline void Value::add_parents(const char* value) {
  parents_.Add()->assign(value);
}
inline void Value::add_parents(const char* value, size_t size) {
  parents_.Add()->assign(reinterpret_cast<const char*>(value), size);
}
inline const ::google::protobuf::RepeatedPtrField< ::std::string>&
Value::parents() const {
  return parents_;
}
inline ::google::protobuf::RepeatedPtrField< ::std::string>*
Value::mutable_parents() {
  return &parents_;
}

// repeated string children = 19;
inline int Value::children_size() const {
  return children_.size();
}
inline void Value::clear_children() {
  children_.Clear();
}
inline const ::std::string& Value::children(int index) const {
  return children_.Get(index);
}
inline ::std::string* Value::mutable_children(int index) {
  return children_.Mutable(index);
}
inline void Value::set_children(int index, const ::std::string& value) {
  children_.Mutable(index)->assign(value);
}
inline void Value::set_children(int index, const char* value) {
  children_.Mutable(index)->assign(value);
}
inline void Value::set_children(int index, const char* value, size_t size) {
  children_.Mutable(index)->assign(
    reinterpret_cast<const char*>(value), size);
}
inline ::std::string* Value::add_children() {
  return children_.Add();
}
inline void Value::add_children(const ::std::string& value) {
  children_.Add()->assign(value);
}
inline void Value::add_children(const char* value) {
  children_.Add()->assign(value);
}
inline void Value::add_children(const char* value, size_t size) {
  children_.Add()->assign(reinterpret_cast<const char*>(value), size);
}
inline const ::google::protobuf::RepeatedPtrField< ::std::string>&
Value::children() const {
  return children_;
}
inline ::google::protobuf::RepeatedPtrField< ::std::string>*
Value::mutable_children() {
  return &children_;
}

// repeated string dataNameList = 20;
inline int Value::datanamelist_size() const {
  return datanamelist_.size();
}
inline void Value::clear_datanamelist() {
  datanamelist_.Clear();
}
inline const ::std::string& Value::datanamelist(int index) const {
  return datanamelist_.Get(index);
}
inline ::std::string* Value::mutable_datanamelist(int index) {
  return datanamelist_.Mutable(index);
}
inline void Value::set_datanamelist(int index, const ::std::string& value) {
  datanamelist_.Mutable(index)->assign(value);
}
inline void Value::set_datanamelist(int index, const char* value) {
  datanamelist_.Mutable(index)->assign(value);
}
inline void Value::set_datanamelist(int index, const char* value, size_t size) {
  datanamelist_.Mutable(index)->assign(
    reinterpret_cast<const char*>(value), size);
}
inline ::std::string* Value::add_datanamelist() {
  return datanamelist_.Add();
}
inline void Value::add_datanamelist(const ::std::string& value) {
  datanamelist_.Add()->assign(value);
}
inline void Value::add_datanamelist(const char* value) {
  datanamelist_.Add()->assign(value);
}
inline void Value::add_datanamelist(const char* value, size_t size) {
  datanamelist_.Add()->assign(reinterpret_cast<const char*>(value), size);
}
inline const ::google::protobuf::RepeatedPtrField< ::std::string>&
Value::datanamelist() const {
  return datanamelist_;
}
inline ::google::protobuf::RepeatedPtrField< ::std::string>*
Value::mutable_datanamelist() {
  return &datanamelist_;
}

// repeated int64 dataSize = 21;
inline int Value::datasize_size() const {
  return datasize_.size();
}
inline void Value::clear_datasize() {
  datasize_.Clear();
}
inline ::google::protobuf::int64 Value::datasize(int index) const {
  return datasize_.Get(index);
}
inline void Value::set_datasize(int index, ::google::protobuf::int64 value) {
  datasize_.Set(index, value);
}
inline void Value::add_datasize(::google::protobuf::int64 value) {
  datasize_.Add(value);
}
inline const ::google::protobuf::RepeatedField< ::google::protobuf::int64 >&
Value::datasize() const {
  return datasize_;
}
inline ::google::protobuf::RepeatedField< ::google::protobuf::int64 >*
Value::mutable_datasize() {
  return &datasize_;
}


// @@protoc_insertion_point(namespace_scope)

#ifndef SWIG
namespace google {
namespace protobuf {


}  // namespace google
}  // namespace protobuf
#endif  // SWIG

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_metazht_2eproto__INCLUDED
