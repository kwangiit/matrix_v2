// Generated by the protocol buffer compiler.  DO NOT EDIT!

#define INTERNAL_SUPPRESS_PROTOBUF_FIELD_DEPRECATION
#include "metamatrix.pb.h"

#include <algorithm>

#include <google/protobuf/stubs/once.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/wire_format_lite_inl.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>
// @@protoc_insertion_point(includes)

namespace {

const ::google::protobuf::Descriptor* MatrixMsg_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  MatrixMsg_reflection_ = NULL;
const ::google::protobuf::Descriptor* MatrixMsg_TaskMsg_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  MatrixMsg_TaskMsg_reflection_ = NULL;

}  // namespace


void protobuf_AssignDesc_metamatrix_2eproto() {
  protobuf_AddDesc_metamatrix_2eproto();
  const ::google::protobuf::FileDescriptor* file =
    ::google::protobuf::DescriptorPool::generated_pool()->FindFileByName(
      "metamatrix.proto");
  GOOGLE_CHECK(file != NULL);
  MatrixMsg_descriptor_ = file->message_type(0);
  static const int MatrixMsg_offsets_[4] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(MatrixMsg, tasks_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(MatrixMsg, msgtype_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(MatrixMsg, count_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(MatrixMsg, extrainfo_),
  };
  MatrixMsg_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      MatrixMsg_descriptor_,
      MatrixMsg::default_instance_,
      MatrixMsg_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(MatrixMsg, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(MatrixMsg, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(MatrixMsg));
  MatrixMsg_TaskMsg_descriptor_ = MatrixMsg_descriptor_->nested_type(0);
  static const int MatrixMsg_TaskMsg_offsets_[5] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(MatrixMsg_TaskMsg, taskid_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(MatrixMsg_TaskMsg, user_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(MatrixMsg_TaskMsg, dir_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(MatrixMsg_TaskMsg, cmd_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(MatrixMsg_TaskMsg, datalength_),
  };
  MatrixMsg_TaskMsg_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      MatrixMsg_TaskMsg_descriptor_,
      MatrixMsg_TaskMsg::default_instance_,
      MatrixMsg_TaskMsg_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(MatrixMsg_TaskMsg, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(MatrixMsg_TaskMsg, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(MatrixMsg_TaskMsg));
}

namespace {

GOOGLE_PROTOBUF_DECLARE_ONCE(protobuf_AssignDescriptors_once_);
inline void protobuf_AssignDescriptorsOnce() {
  ::google::protobuf::GoogleOnceInit(&protobuf_AssignDescriptors_once_,
                 &protobuf_AssignDesc_metamatrix_2eproto);
}

void protobuf_RegisterTypes(const ::std::string&) {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    MatrixMsg_descriptor_, &MatrixMsg::default_instance());
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    MatrixMsg_TaskMsg_descriptor_, &MatrixMsg_TaskMsg::default_instance());
}

}  // namespace

void protobuf_ShutdownFile_metamatrix_2eproto() {
  delete MatrixMsg::default_instance_;
  delete MatrixMsg_reflection_;
  delete MatrixMsg_TaskMsg::default_instance_;
  delete MatrixMsg_TaskMsg_reflection_;
}

void protobuf_AddDesc_metamatrix_2eproto() {
  static bool already_here = false;
  if (already_here) return;
  already_here = true;
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  ::google::protobuf::DescriptorPool::InternalAddGeneratedFile(
    "\n\020metamatrix.proto\"\270\001\n\tMatrixMsg\022!\n\005task"
    "s\030\001 \003(\0132\022.MatrixMsg.TaskMsg\022\017\n\007msgType\030\002"
    " \002(\t\022\r\n\005count\030\003 \001(\003\022\021\n\textraInfo\030\004 \001(\t\032U"
    "\n\007TaskMsg\022\016\n\006taskId\030\001 \002(\t\022\014\n\004user\030\002 \002(\t\022"
    "\013\n\003dir\030\003 \002(\t\022\013\n\003cmd\030\004 \002(\t\022\022\n\ndataLength\030"
    "\005 \002(\003", 205);
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedFile(
    "metamatrix.proto", &protobuf_RegisterTypes);
  MatrixMsg::default_instance_ = new MatrixMsg();
  MatrixMsg_TaskMsg::default_instance_ = new MatrixMsg_TaskMsg();
  MatrixMsg::default_instance_->InitAsDefaultInstance();
  MatrixMsg_TaskMsg::default_instance_->InitAsDefaultInstance();
  ::google::protobuf::internal::OnShutdown(&protobuf_ShutdownFile_metamatrix_2eproto);
}

// Force AddDescriptors() to be called at static initialization time.
struct StaticDescriptorInitializer_metamatrix_2eproto {
  StaticDescriptorInitializer_metamatrix_2eproto() {
    protobuf_AddDesc_metamatrix_2eproto();
  }
} static_descriptor_initializer_metamatrix_2eproto_;


// ===================================================================

#ifndef _MSC_VER
const int MatrixMsg_TaskMsg::kTaskIdFieldNumber;
const int MatrixMsg_TaskMsg::kUserFieldNumber;
const int MatrixMsg_TaskMsg::kDirFieldNumber;
const int MatrixMsg_TaskMsg::kCmdFieldNumber;
const int MatrixMsg_TaskMsg::kDataLengthFieldNumber;
#endif  // !_MSC_VER

MatrixMsg_TaskMsg::MatrixMsg_TaskMsg()
  : ::google::protobuf::Message() {
  SharedCtor();
}

void MatrixMsg_TaskMsg::InitAsDefaultInstance() {
}

MatrixMsg_TaskMsg::MatrixMsg_TaskMsg(const MatrixMsg_TaskMsg& from)
  : ::google::protobuf::Message() {
  SharedCtor();
  MergeFrom(from);
}

void MatrixMsg_TaskMsg::SharedCtor() {
  _cached_size_ = 0;
  taskid_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
  user_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
  dir_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
  cmd_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
  datalength_ = GOOGLE_LONGLONG(0);
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

MatrixMsg_TaskMsg::~MatrixMsg_TaskMsg() {
  SharedDtor();
}

void MatrixMsg_TaskMsg::SharedDtor() {
  if (taskid_ != &::google::protobuf::internal::kEmptyString) {
    delete taskid_;
  }
  if (user_ != &::google::protobuf::internal::kEmptyString) {
    delete user_;
  }
  if (dir_ != &::google::protobuf::internal::kEmptyString) {
    delete dir_;
  }
  if (cmd_ != &::google::protobuf::internal::kEmptyString) {
    delete cmd_;
  }
  if (this != default_instance_) {
  }
}

void MatrixMsg_TaskMsg::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* MatrixMsg_TaskMsg::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return MatrixMsg_TaskMsg_descriptor_;
}

const MatrixMsg_TaskMsg& MatrixMsg_TaskMsg::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_metamatrix_2eproto();  return *default_instance_;
}

MatrixMsg_TaskMsg* MatrixMsg_TaskMsg::default_instance_ = NULL;

MatrixMsg_TaskMsg* MatrixMsg_TaskMsg::New() const {
  return new MatrixMsg_TaskMsg;
}

void MatrixMsg_TaskMsg::Clear() {
  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (has_taskid()) {
      if (taskid_ != &::google::protobuf::internal::kEmptyString) {
        taskid_->clear();
      }
    }
    if (has_user()) {
      if (user_ != &::google::protobuf::internal::kEmptyString) {
        user_->clear();
      }
    }
    if (has_dir()) {
      if (dir_ != &::google::protobuf::internal::kEmptyString) {
        dir_->clear();
      }
    }
    if (has_cmd()) {
      if (cmd_ != &::google::protobuf::internal::kEmptyString) {
        cmd_->clear();
      }
    }
    datalength_ = GOOGLE_LONGLONG(0);
  }
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  mutable_unknown_fields()->Clear();
}

bool MatrixMsg_TaskMsg::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) return false
  ::google::protobuf::uint32 tag;
  while ((tag = input->ReadTag()) != 0) {
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // required string taskId = 1;
      case 1: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
          DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                input, this->mutable_taskid()));
          ::google::protobuf::internal::WireFormat::VerifyUTF8String(
            this->taskid().data(), this->taskid().length(),
            ::google::protobuf::internal::WireFormat::PARSE);
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(18)) goto parse_user;
        break;
      }
      
      // required string user = 2;
      case 2: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
         parse_user:
          DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                input, this->mutable_user()));
          ::google::protobuf::internal::WireFormat::VerifyUTF8String(
            this->user().data(), this->user().length(),
            ::google::protobuf::internal::WireFormat::PARSE);
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(26)) goto parse_dir;
        break;
      }
      
      // required string dir = 3;
      case 3: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
         parse_dir:
          DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                input, this->mutable_dir()));
          ::google::protobuf::internal::WireFormat::VerifyUTF8String(
            this->dir().data(), this->dir().length(),
            ::google::protobuf::internal::WireFormat::PARSE);
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(34)) goto parse_cmd;
        break;
      }
      
      // required string cmd = 4;
      case 4: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
         parse_cmd:
          DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                input, this->mutable_cmd()));
          ::google::protobuf::internal::WireFormat::VerifyUTF8String(
            this->cmd().data(), this->cmd().length(),
            ::google::protobuf::internal::WireFormat::PARSE);
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(40)) goto parse_dataLength;
        break;
      }
      
      // required int64 dataLength = 5;
      case 5: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
         parse_dataLength:
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::int64, ::google::protobuf::internal::WireFormatLite::TYPE_INT64>(
                 input, &datalength_)));
          set_has_datalength();
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectAtEnd()) return true;
        break;
      }
      
      default: {
      handle_uninterpreted:
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_END_GROUP) {
          return true;
        }
        DO_(::google::protobuf::internal::WireFormat::SkipField(
              input, tag, mutable_unknown_fields()));
        break;
      }
    }
  }
  return true;
#undef DO_
}

void MatrixMsg_TaskMsg::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // required string taskId = 1;
  if (has_taskid()) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->taskid().data(), this->taskid().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    ::google::protobuf::internal::WireFormatLite::WriteString(
      1, this->taskid(), output);
  }
  
  // required string user = 2;
  if (has_user()) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->user().data(), this->user().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    ::google::protobuf::internal::WireFormatLite::WriteString(
      2, this->user(), output);
  }
  
  // required string dir = 3;
  if (has_dir()) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->dir().data(), this->dir().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    ::google::protobuf::internal::WireFormatLite::WriteString(
      3, this->dir(), output);
  }
  
  // required string cmd = 4;
  if (has_cmd()) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->cmd().data(), this->cmd().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    ::google::protobuf::internal::WireFormatLite::WriteString(
      4, this->cmd(), output);
  }
  
  // required int64 dataLength = 5;
  if (has_datalength()) {
    ::google::protobuf::internal::WireFormatLite::WriteInt64(5, this->datalength(), output);
  }
  
  if (!unknown_fields().empty()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        unknown_fields(), output);
  }
}

::google::protobuf::uint8* MatrixMsg_TaskMsg::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const {
  // required string taskId = 1;
  if (has_taskid()) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->taskid().data(), this->taskid().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    target =
      ::google::protobuf::internal::WireFormatLite::WriteStringToArray(
        1, this->taskid(), target);
  }
  
  // required string user = 2;
  if (has_user()) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->user().data(), this->user().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    target =
      ::google::protobuf::internal::WireFormatLite::WriteStringToArray(
        2, this->user(), target);
  }
  
  // required string dir = 3;
  if (has_dir()) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->dir().data(), this->dir().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    target =
      ::google::protobuf::internal::WireFormatLite::WriteStringToArray(
        3, this->dir(), target);
  }
  
  // required string cmd = 4;
  if (has_cmd()) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->cmd().data(), this->cmd().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    target =
      ::google::protobuf::internal::WireFormatLite::WriteStringToArray(
        4, this->cmd(), target);
  }
  
  // required int64 dataLength = 5;
  if (has_datalength()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteInt64ToArray(5, this->datalength(), target);
  }
  
  if (!unknown_fields().empty()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        unknown_fields(), target);
  }
  return target;
}

int MatrixMsg_TaskMsg::ByteSize() const {
  int total_size = 0;
  
  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    // required string taskId = 1;
    if (has_taskid()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::StringSize(
          this->taskid());
    }
    
    // required string user = 2;
    if (has_user()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::StringSize(
          this->user());
    }
    
    // required string dir = 3;
    if (has_dir()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::StringSize(
          this->dir());
    }
    
    // required string cmd = 4;
    if (has_cmd()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::StringSize(
          this->cmd());
    }
    
    // required int64 dataLength = 5;
    if (has_datalength()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::Int64Size(
          this->datalength());
    }
    
  }
  if (!unknown_fields().empty()) {
    total_size +=
      ::google::protobuf::internal::WireFormat::ComputeUnknownFieldsSize(
        unknown_fields());
  }
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = total_size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
  return total_size;
}

void MatrixMsg_TaskMsg::MergeFrom(const ::google::protobuf::Message& from) {
  GOOGLE_CHECK_NE(&from, this);
  const MatrixMsg_TaskMsg* source =
    ::google::protobuf::internal::dynamic_cast_if_available<const MatrixMsg_TaskMsg*>(
      &from);
  if (source == NULL) {
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
    MergeFrom(*source);
  }
}

void MatrixMsg_TaskMsg::MergeFrom(const MatrixMsg_TaskMsg& from) {
  GOOGLE_CHECK_NE(&from, this);
  if (from._has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (from.has_taskid()) {
      set_taskid(from.taskid());
    }
    if (from.has_user()) {
      set_user(from.user());
    }
    if (from.has_dir()) {
      set_dir(from.dir());
    }
    if (from.has_cmd()) {
      set_cmd(from.cmd());
    }
    if (from.has_datalength()) {
      set_datalength(from.datalength());
    }
  }
  mutable_unknown_fields()->MergeFrom(from.unknown_fields());
}

void MatrixMsg_TaskMsg::CopyFrom(const ::google::protobuf::Message& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void MatrixMsg_TaskMsg::CopyFrom(const MatrixMsg_TaskMsg& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool MatrixMsg_TaskMsg::IsInitialized() const {
  if ((_has_bits_[0] & 0x0000001f) != 0x0000001f) return false;
  
  return true;
}

void MatrixMsg_TaskMsg::Swap(MatrixMsg_TaskMsg* other) {
  if (other != this) {
    std::swap(taskid_, other->taskid_);
    std::swap(user_, other->user_);
    std::swap(dir_, other->dir_);
    std::swap(cmd_, other->cmd_);
    std::swap(datalength_, other->datalength_);
    std::swap(_has_bits_[0], other->_has_bits_[0]);
    _unknown_fields_.Swap(&other->_unknown_fields_);
    std::swap(_cached_size_, other->_cached_size_);
  }
}

::google::protobuf::Metadata MatrixMsg_TaskMsg::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = MatrixMsg_TaskMsg_descriptor_;
  metadata.reflection = MatrixMsg_TaskMsg_reflection_;
  return metadata;
}


// -------------------------------------------------------------------

#ifndef _MSC_VER
const int MatrixMsg::kTasksFieldNumber;
const int MatrixMsg::kMsgTypeFieldNumber;
const int MatrixMsg::kCountFieldNumber;
const int MatrixMsg::kExtraInfoFieldNumber;
#endif  // !_MSC_VER

MatrixMsg::MatrixMsg()
  : ::google::protobuf::Message() {
  SharedCtor();
}

void MatrixMsg::InitAsDefaultInstance() {
}

MatrixMsg::MatrixMsg(const MatrixMsg& from)
  : ::google::protobuf::Message() {
  SharedCtor();
  MergeFrom(from);
}

void MatrixMsg::SharedCtor() {
  _cached_size_ = 0;
  msgtype_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
  count_ = GOOGLE_LONGLONG(0);
  extrainfo_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

MatrixMsg::~MatrixMsg() {
  SharedDtor();
}

void MatrixMsg::SharedDtor() {
  if (msgtype_ != &::google::protobuf::internal::kEmptyString) {
    delete msgtype_;
  }
  if (extrainfo_ != &::google::protobuf::internal::kEmptyString) {
    delete extrainfo_;
  }
  if (this != default_instance_) {
  }
}

void MatrixMsg::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* MatrixMsg::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return MatrixMsg_descriptor_;
}

const MatrixMsg& MatrixMsg::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_metamatrix_2eproto();  return *default_instance_;
}

MatrixMsg* MatrixMsg::default_instance_ = NULL;

MatrixMsg* MatrixMsg::New() const {
  return new MatrixMsg;
}

void MatrixMsg::Clear() {
  if (_has_bits_[1 / 32] & (0xffu << (1 % 32))) {
    if (has_msgtype()) {
      if (msgtype_ != &::google::protobuf::internal::kEmptyString) {
        msgtype_->clear();
      }
    }
    count_ = GOOGLE_LONGLONG(0);
    if (has_extrainfo()) {
      if (extrainfo_ != &::google::protobuf::internal::kEmptyString) {
        extrainfo_->clear();
      }
    }
  }
  tasks_.Clear();
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  mutable_unknown_fields()->Clear();
}

bool MatrixMsg::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) return false
  ::google::protobuf::uint32 tag;
  while ((tag = input->ReadTag()) != 0) {
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // repeated .MatrixMsg.TaskMsg tasks = 1;
      case 1: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
         parse_tasks:
          DO_(::google::protobuf::internal::WireFormatLite::ReadMessageNoVirtual(
                input, add_tasks()));
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(10)) goto parse_tasks;
        if (input->ExpectTag(18)) goto parse_msgType;
        break;
      }
      
      // required string msgType = 2;
      case 2: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
         parse_msgType:
          DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                input, this->mutable_msgtype()));
          ::google::protobuf::internal::WireFormat::VerifyUTF8String(
            this->msgtype().data(), this->msgtype().length(),
            ::google::protobuf::internal::WireFormat::PARSE);
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(24)) goto parse_count;
        break;
      }
      
      // optional int64 count = 3;
      case 3: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
         parse_count:
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::int64, ::google::protobuf::internal::WireFormatLite::TYPE_INT64>(
                 input, &count_)));
          set_has_count();
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(34)) goto parse_extraInfo;
        break;
      }
      
      // optional string extraInfo = 4;
      case 4: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
         parse_extraInfo:
          DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                input, this->mutable_extrainfo()));
          ::google::protobuf::internal::WireFormat::VerifyUTF8String(
            this->extrainfo().data(), this->extrainfo().length(),
            ::google::protobuf::internal::WireFormat::PARSE);
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectAtEnd()) return true;
        break;
      }
      
      default: {
      handle_uninterpreted:
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_END_GROUP) {
          return true;
        }
        DO_(::google::protobuf::internal::WireFormat::SkipField(
              input, tag, mutable_unknown_fields()));
        break;
      }
    }
  }
  return true;
#undef DO_
}

void MatrixMsg::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // repeated .MatrixMsg.TaskMsg tasks = 1;
  for (int i = 0; i < this->tasks_size(); i++) {
    ::google::protobuf::internal::WireFormatLite::WriteMessageMaybeToArray(
      1, this->tasks(i), output);
  }
  
  // required string msgType = 2;
  if (has_msgtype()) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->msgtype().data(), this->msgtype().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    ::google::protobuf::internal::WireFormatLite::WriteString(
      2, this->msgtype(), output);
  }
  
  // optional int64 count = 3;
  if (has_count()) {
    ::google::protobuf::internal::WireFormatLite::WriteInt64(3, this->count(), output);
  }
  
  // optional string extraInfo = 4;
  if (has_extrainfo()) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->extrainfo().data(), this->extrainfo().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    ::google::protobuf::internal::WireFormatLite::WriteString(
      4, this->extrainfo(), output);
  }
  
  if (!unknown_fields().empty()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        unknown_fields(), output);
  }
}

::google::protobuf::uint8* MatrixMsg::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const {
  // repeated .MatrixMsg.TaskMsg tasks = 1;
  for (int i = 0; i < this->tasks_size(); i++) {
    target = ::google::protobuf::internal::WireFormatLite::
      WriteMessageNoVirtualToArray(
        1, this->tasks(i), target);
  }
  
  // required string msgType = 2;
  if (has_msgtype()) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->msgtype().data(), this->msgtype().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    target =
      ::google::protobuf::internal::WireFormatLite::WriteStringToArray(
        2, this->msgtype(), target);
  }
  
  // optional int64 count = 3;
  if (has_count()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteInt64ToArray(3, this->count(), target);
  }
  
  // optional string extraInfo = 4;
  if (has_extrainfo()) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->extrainfo().data(), this->extrainfo().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    target =
      ::google::protobuf::internal::WireFormatLite::WriteStringToArray(
        4, this->extrainfo(), target);
  }
  
  if (!unknown_fields().empty()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        unknown_fields(), target);
  }
  return target;
}

int MatrixMsg::ByteSize() const {
  int total_size = 0;
  
  if (_has_bits_[1 / 32] & (0xffu << (1 % 32))) {
    // required string msgType = 2;
    if (has_msgtype()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::StringSize(
          this->msgtype());
    }
    
    // optional int64 count = 3;
    if (has_count()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::Int64Size(
          this->count());
    }
    
    // optional string extraInfo = 4;
    if (has_extrainfo()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::StringSize(
          this->extrainfo());
    }
    
  }
  // repeated .MatrixMsg.TaskMsg tasks = 1;
  total_size += 1 * this->tasks_size();
  for (int i = 0; i < this->tasks_size(); i++) {
    total_size +=
      ::google::protobuf::internal::WireFormatLite::MessageSizeNoVirtual(
        this->tasks(i));
  }
  
  if (!unknown_fields().empty()) {
    total_size +=
      ::google::protobuf::internal::WireFormat::ComputeUnknownFieldsSize(
        unknown_fields());
  }
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = total_size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
  return total_size;
}

void MatrixMsg::MergeFrom(const ::google::protobuf::Message& from) {
  GOOGLE_CHECK_NE(&from, this);
  const MatrixMsg* source =
    ::google::protobuf::internal::dynamic_cast_if_available<const MatrixMsg*>(
      &from);
  if (source == NULL) {
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
    MergeFrom(*source);
  }
}

void MatrixMsg::MergeFrom(const MatrixMsg& from) {
  GOOGLE_CHECK_NE(&from, this);
  tasks_.MergeFrom(from.tasks_);
  if (from._has_bits_[1 / 32] & (0xffu << (1 % 32))) {
    if (from.has_msgtype()) {
      set_msgtype(from.msgtype());
    }
    if (from.has_count()) {
      set_count(from.count());
    }
    if (from.has_extrainfo()) {
      set_extrainfo(from.extrainfo());
    }
  }
  mutable_unknown_fields()->MergeFrom(from.unknown_fields());
}

void MatrixMsg::CopyFrom(const ::google::protobuf::Message& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void MatrixMsg::CopyFrom(const MatrixMsg& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool MatrixMsg::IsInitialized() const {
  if ((_has_bits_[0] & 0x00000002) != 0x00000002) return false;
  
  for (int i = 0; i < tasks_size(); i++) {
    if (!this->tasks(i).IsInitialized()) return false;
  }
  return true;
}

void MatrixMsg::Swap(MatrixMsg* other) {
  if (other != this) {
    tasks_.Swap(&other->tasks_);
    std::swap(msgtype_, other->msgtype_);
    std::swap(count_, other->count_);
    std::swap(extrainfo_, other->extrainfo_);
    std::swap(_has_bits_[0], other->_has_bits_[0]);
    _unknown_fields_.Swap(&other->_unknown_fields_);
    std::swap(_cached_size_, other->_cached_size_);
  }
}

::google::protobuf::Metadata MatrixMsg::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = MatrixMsg_descriptor_;
  metadata.reflection = MatrixMsg_reflection_;
  return metadata;
}


// @@protoc_insertion_point(namespace_scope)

// @@protoc_insertion_point(global_scope)
