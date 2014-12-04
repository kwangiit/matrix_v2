// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: meta.proto

#define INTERNAL_SUPPRESS_PROTOBUF_FIELD_DEPRECATION
#include "meta.pb.h"

#include <algorithm>

#include <google/protobuf/stubs/common.h>
#include <google/protobuf/stubs/once.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/wire_format_lite_inl.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>
// @@protoc_insertion_point(includes)

namespace {

const ::google::protobuf::Descriptor* Package_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  Package_reflection_ = NULL;

}  // namespace


void protobuf_AssignDesc_meta_2eproto() {
  protobuf_AddDesc_meta_2eproto();
  const ::google::protobuf::FileDescriptor* file =
    ::google::protobuf::DescriptorPool::generated_pool()->FindFileByName(
      "meta.proto");
  GOOGLE_CHECK(file != NULL);
  Package_descriptor_ = file->message_type(0);
  static const int Package_offsets_[5] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Package, virtualpath_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Package, realfullpath_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Package, isdir_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Package, replicanum_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Package, listitem_),
  };
  Package_reflection_ =
    ::google::protobuf::internal::GeneratedMessageReflection::NewGeneratedMessageReflection(
      Package_descriptor_,
      Package::default_instance_,
      Package_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Package, _has_bits_[0]),
      -1,
      -1,
      sizeof(Package),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(Package, _internal_metadata_),
      -1);
}

namespace {

GOOGLE_PROTOBUF_DECLARE_ONCE(protobuf_AssignDescriptors_once_);
inline void protobuf_AssignDescriptorsOnce() {
  ::google::protobuf::GoogleOnceInit(&protobuf_AssignDescriptors_once_,
                 &protobuf_AssignDesc_meta_2eproto);
}

void protobuf_RegisterTypes(const ::std::string&) {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
      Package_descriptor_, &Package::default_instance());
}

}  // namespace

void protobuf_ShutdownFile_meta_2eproto() {
  delete Package::default_instance_;
  delete Package_reflection_;
}

void protobuf_AddDesc_meta_2eproto() {
  static bool already_here = false;
  if (already_here) return;
  already_here = true;
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  ::google::protobuf::DescriptorPool::InternalAddGeneratedFile(
    "\n\nmeta.proto\"i\n\007Package\022\023\n\013virtualPath\030\001"
    " \001(\t\022\024\n\014realFullPath\030\002 \001(\t\022\r\n\005isDir\030\003 \001("
    "\010\022\022\n\nreplicanum\030\004 \001(\005\022\020\n\010listItem\030\005 \003(\t", 119);
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedFile(
    "meta.proto", &protobuf_RegisterTypes);
  Package::default_instance_ = new Package();
  Package::default_instance_->InitAsDefaultInstance();
  ::google::protobuf::internal::OnShutdown(&protobuf_ShutdownFile_meta_2eproto);
}

// Force AddDescriptors() to be called at static initialization time.
struct StaticDescriptorInitializer_meta_2eproto {
  StaticDescriptorInitializer_meta_2eproto() {
    protobuf_AddDesc_meta_2eproto();
  }
} static_descriptor_initializer_meta_2eproto_;

namespace {

static void MergeFromFail(int line) GOOGLE_ATTRIBUTE_COLD;
static void MergeFromFail(int line) {
  GOOGLE_CHECK(false) << __FILE__ << ":" << line;
}

}  // namespace


// ===================================================================

#ifndef _MSC_VER
const int Package::kVirtualPathFieldNumber;
const int Package::kRealFullPathFieldNumber;
const int Package::kIsDirFieldNumber;
const int Package::kReplicanumFieldNumber;
const int Package::kListItemFieldNumber;
#endif  // !_MSC_VER

Package::Package()
  : ::google::protobuf::Message() , _internal_metadata_(NULL)  {
  SharedCtor();
  // @@protoc_insertion_point(constructor:Package)
}

void Package::InitAsDefaultInstance() {
}

Package::Package(const Package& from)
  : ::google::protobuf::Message(),
    _internal_metadata_(NULL) {
  SharedCtor();
  MergeFrom(from);
  // @@protoc_insertion_point(copy_constructor:Package)
}

void Package::SharedCtor() {
  ::google::protobuf::internal::GetEmptyString();
  _cached_size_ = 0;
  virtualpath_.UnsafeSetDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  realfullpath_.UnsafeSetDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  isdir_ = false;
  replicanum_ = 0;
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

Package::~Package() {
  // @@protoc_insertion_point(destructor:Package)
  SharedDtor();
}

void Package::SharedDtor() {
  virtualpath_.DestroyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  realfullpath_.DestroyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  if (this != default_instance_) {
  }
}

void Package::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* Package::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return Package_descriptor_;
}

const Package& Package::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_meta_2eproto();
  return *default_instance_;
}

Package* Package::default_instance_ = NULL;

Package* Package::New(::google::protobuf::Arena* arena) const {
  Package* n = new Package;
  if (arena != NULL) {
    arena->Own(n);
  }
  return n;
}

void Package::Clear() {
#define OFFSET_OF_FIELD_(f) (reinterpret_cast<char*>(      \
  &reinterpret_cast<Package*>(16)->f) - \
   reinterpret_cast<char*>(16))

#define ZR_(first, last) do {                              \
    size_t f = OFFSET_OF_FIELD_(first);                    \
    size_t n = OFFSET_OF_FIELD_(last) - f + sizeof(last);  \
    ::memset(&first, 0, n);                                \
  } while (0)

  if (_has_bits_[0 / 32] & 15) {
    ZR_(isdir_, replicanum_);
    if (has_virtualpath()) {
      virtualpath_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
    }
    if (has_realfullpath()) {
      realfullpath_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
    }
  }

#undef OFFSET_OF_FIELD_
#undef ZR_

  listitem_.Clear();
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  if (_internal_metadata_.have_unknown_fields()) {
    mutable_unknown_fields()->Clear();
  }
}

bool Package::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) goto failure
  ::google::protobuf::uint32 tag;
  // @@protoc_insertion_point(parse_start:Package)
  for (;;) {
    ::std::pair< ::google::protobuf::uint32, bool> p = input->ReadTagWithCutoff(127);
    tag = p.first;
    if (!p.second) goto handle_unusual;
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // optional string virtualPath = 1;
      case 1: {
        if (tag == 10) {
          DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                input, this->mutable_virtualpath()));
          ::google::protobuf::internal::WireFormat::VerifyUTF8StringNamedField(
            this->virtualpath().data(), this->virtualpath().length(),
            ::google::protobuf::internal::WireFormat::PARSE,
            "Package.virtualPath");
        } else {
          goto handle_unusual;
        }
        if (input->ExpectTag(18)) goto parse_realFullPath;
        break;
      }

      // optional string realFullPath = 2;
      case 2: {
        if (tag == 18) {
         parse_realFullPath:
          DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                input, this->mutable_realfullpath()));
          ::google::protobuf::internal::WireFormat::VerifyUTF8StringNamedField(
            this->realfullpath().data(), this->realfullpath().length(),
            ::google::protobuf::internal::WireFormat::PARSE,
            "Package.realFullPath");
        } else {
          goto handle_unusual;
        }
        if (input->ExpectTag(24)) goto parse_isDir;
        break;
      }

      // optional bool isDir = 3;
      case 3: {
        if (tag == 24) {
         parse_isDir:
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   bool, ::google::protobuf::internal::WireFormatLite::TYPE_BOOL>(
                 input, &isdir_)));
          set_has_isdir();
        } else {
          goto handle_unusual;
        }
        if (input->ExpectTag(32)) goto parse_replicanum;
        break;
      }

      // optional int32 replicanum = 4;
      case 4: {
        if (tag == 32) {
         parse_replicanum:
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 input, &replicanum_)));
          set_has_replicanum();
        } else {
          goto handle_unusual;
        }
        if (input->ExpectTag(42)) goto parse_listItem;
        break;
      }

      // repeated string listItem = 5;
      case 5: {
        if (tag == 42) {
         parse_listItem:
          DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                input, this->add_listitem()));
          ::google::protobuf::internal::WireFormat::VerifyUTF8StringNamedField(
            this->listitem(this->listitem_size() - 1).data(),
            this->listitem(this->listitem_size() - 1).length(),
            ::google::protobuf::internal::WireFormat::PARSE,
            "Package.listItem");
        } else {
          goto handle_unusual;
        }
        if (input->ExpectTag(42)) goto parse_listItem;
        if (input->ExpectAtEnd()) goto success;
        break;
      }

      default: {
      handle_unusual:
        if (tag == 0 ||
            ::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_END_GROUP) {
          goto success;
        }
        DO_(::google::protobuf::internal::WireFormat::SkipField(
              input, tag, mutable_unknown_fields()));
        break;
      }
    }
  }
success:
  // @@protoc_insertion_point(parse_success:Package)
  return true;
failure:
  // @@protoc_insertion_point(parse_failure:Package)
  return false;
#undef DO_
}

void Package::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // @@protoc_insertion_point(serialize_start:Package)
  // optional string virtualPath = 1;
  if (has_virtualpath()) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8StringNamedField(
      this->virtualpath().data(), this->virtualpath().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE,
      "Package.virtualPath");
    ::google::protobuf::internal::WireFormatLite::WriteStringMaybeAliased(
      1, this->virtualpath(), output);
  }

  // optional string realFullPath = 2;
  if (has_realfullpath()) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8StringNamedField(
      this->realfullpath().data(), this->realfullpath().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE,
      "Package.realFullPath");
    ::google::protobuf::internal::WireFormatLite::WriteStringMaybeAliased(
      2, this->realfullpath(), output);
  }

  // optional bool isDir = 3;
  if (has_isdir()) {
    ::google::protobuf::internal::WireFormatLite::WriteBool(3, this->isdir(), output);
  }

  // optional int32 replicanum = 4;
  if (has_replicanum()) {
    ::google::protobuf::internal::WireFormatLite::WriteInt32(4, this->replicanum(), output);
  }

  // repeated string listItem = 5;
  for (int i = 0; i < this->listitem_size(); i++) {
  ::google::protobuf::internal::WireFormat::VerifyUTF8StringNamedField(
    this->listitem(i).data(), this->listitem(i).length(),
    ::google::protobuf::internal::WireFormat::SERIALIZE,
    "Package.listItem");
    ::google::protobuf::internal::WireFormatLite::WriteString(
      5, this->listitem(i), output);
  }

  if (_internal_metadata_.have_unknown_fields()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        unknown_fields(), output);
  }
  // @@protoc_insertion_point(serialize_end:Package)
}

::google::protobuf::uint8* Package::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const {
  // @@protoc_insertion_point(serialize_to_array_start:Package)
  // optional string virtualPath = 1;
  if (has_virtualpath()) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8StringNamedField(
      this->virtualpath().data(), this->virtualpath().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE,
      "Package.virtualPath");
    target =
      ::google::protobuf::internal::WireFormatLite::WriteStringToArray(
        1, this->virtualpath(), target);
  }

  // optional string realFullPath = 2;
  if (has_realfullpath()) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8StringNamedField(
      this->realfullpath().data(), this->realfullpath().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE,
      "Package.realFullPath");
    target =
      ::google::protobuf::internal::WireFormatLite::WriteStringToArray(
        2, this->realfullpath(), target);
  }

  // optional bool isDir = 3;
  if (has_isdir()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteBoolToArray(3, this->isdir(), target);
  }

  // optional int32 replicanum = 4;
  if (has_replicanum()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteInt32ToArray(4, this->replicanum(), target);
  }

  // repeated string listItem = 5;
  for (int i = 0; i < this->listitem_size(); i++) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8StringNamedField(
      this->listitem(i).data(), this->listitem(i).length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE,
      "Package.listItem");
    target = ::google::protobuf::internal::WireFormatLite::
      WriteStringToArray(5, this->listitem(i), target);
  }

  if (_internal_metadata_.have_unknown_fields()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        unknown_fields(), target);
  }
  // @@protoc_insertion_point(serialize_to_array_end:Package)
  return target;
}

int Package::ByteSize() const {
  int total_size = 0;

  if (_has_bits_[0 / 32] & 15) {
    // optional string virtualPath = 1;
    if (has_virtualpath()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::StringSize(
          this->virtualpath());
    }

    // optional string realFullPath = 2;
    if (has_realfullpath()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::StringSize(
          this->realfullpath());
    }

    // optional bool isDir = 3;
    if (has_isdir()) {
      total_size += 1 + 1;
    }

    // optional int32 replicanum = 4;
    if (has_replicanum()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::Int32Size(
          this->replicanum());
    }

  }
  // repeated string listItem = 5;
  total_size += 1 * this->listitem_size();
  for (int i = 0; i < this->listitem_size(); i++) {
    total_size += ::google::protobuf::internal::WireFormatLite::StringSize(
      this->listitem(i));
  }

  if (_internal_metadata_.have_unknown_fields()) {
    total_size +=
      ::google::protobuf::internal::WireFormat::ComputeUnknownFieldsSize(
        unknown_fields());
  }
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = total_size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
  return total_size;
}

void Package::MergeFrom(const ::google::protobuf::Message& from) {
  if (GOOGLE_PREDICT_FALSE(&from == this)) MergeFromFail(__LINE__);
  const Package* source =
    ::google::protobuf::internal::dynamic_cast_if_available<const Package*>(
      &from);
  if (source == NULL) {
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
    MergeFrom(*source);
  }
}

void Package::MergeFrom(const Package& from) {
  if (GOOGLE_PREDICT_FALSE(&from == this)) MergeFromFail(__LINE__);
  listitem_.MergeFrom(from.listitem_);
  if (from._has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (from.has_virtualpath()) {
      set_has_virtualpath();
      virtualpath_.AssignWithDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), from.virtualpath_);
    }
    if (from.has_realfullpath()) {
      set_has_realfullpath();
      realfullpath_.AssignWithDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), from.realfullpath_);
    }
    if (from.has_isdir()) {
      set_isdir(from.isdir());
    }
    if (from.has_replicanum()) {
      set_replicanum(from.replicanum());
    }
  }
  if (from._internal_metadata_.have_unknown_fields()) {
    mutable_unknown_fields()->MergeFrom(from.unknown_fields());
  }
}

void Package::CopyFrom(const ::google::protobuf::Message& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void Package::CopyFrom(const Package& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool Package::IsInitialized() const {

  return true;
}

void Package::Swap(Package* other) {
  if (other == this) return;
  InternalSwap(other);
}
void Package::InternalSwap(Package* other) {
  virtualpath_.Swap(&other->virtualpath_);
  realfullpath_.Swap(&other->realfullpath_);
  std::swap(isdir_, other->isdir_);
  std::swap(replicanum_, other->replicanum_);
  listitem_.UnsafeArenaSwap(&other->listitem_);
  std::swap(_has_bits_[0], other->_has_bits_[0]);
  _internal_metadata_.Swap(&other->_internal_metadata_);
  std::swap(_cached_size_, other->_cached_size_);
}

::google::protobuf::Metadata Package::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = Package_descriptor_;
  metadata.reflection = Package_reflection_;
  return metadata;
}


// @@protoc_insertion_point(namespace_scope)

// @@protoc_insertion_point(global_scope)
