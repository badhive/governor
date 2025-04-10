// automatically generated by the FlatBuffers compiler, do not modify


#ifndef FLATBUFFERS_GENERATED_ALCA_SENSOR_H_
#define FLATBUFFERS_GENERATED_ALCA_SENSOR_H_

#include "flatbuffers/flatbuffers.h"

// Ensure the included flatbuffers.h is the same version as when this file was
// generated, otherwise it may not be compatible.
static_assert(FLATBUFFERS_VERSION_MAJOR == 25 &&
              FLATBUFFERS_VERSION_MINOR == 2 &&
              FLATBUFFERS_VERSION_REVISION == 10,
             "Non-compatible flatbuffers version included");

namespace Sensor {

struct FileEvent;
struct FileEventBuilder;

struct NetworkEvent;
struct NetworkEventBuilder;

struct WinProcess;
struct WinProcessBuilder;

struct ProcessEvent;
struct ProcessEventBuilder;

struct RegistryValue;
struct RegistryValueBuilder;

struct RegistryEvent;
struct RegistryEventBuilder;

enum FileAction : int8_t {
  FileAction_FileCreate = 0,
  FileAction_FileRename = 1,
  FileAction_FileDelete = 2,
  FileAction_FileModify = 3,
  FileAction_MIN = FileAction_FileCreate,
  FileAction_MAX = FileAction_FileModify
};

inline const FileAction (&EnumValuesFileAction())[4] {
  static const FileAction values[] = {
    FileAction_FileCreate,
    FileAction_FileRename,
    FileAction_FileDelete,
    FileAction_FileModify
  };
  return values;
}

inline const char * const *EnumNamesFileAction() {
  static const char * const names[5] = {
    "FileCreate",
    "FileRename",
    "FileDelete",
    "FileModify",
    nullptr
  };
  return names;
}

inline const char *EnumNameFileAction(FileAction e) {
  if (::flatbuffers::IsOutRange(e, FileAction_FileCreate, FileAction_FileModify)) return "";
  const size_t index = static_cast<size_t>(e);
  return EnumNamesFileAction()[index];
}

enum NetworkAction : int8_t {
  NetworkAction_NetAccept = 0,
  NetworkAction_NetConnect = 1,
  NetworkAction_NetDisconnect = 2,
  NetworkAction_NetSend = 3,
  NetworkAction_NetReceive = 4,
  NetworkAction_MIN = NetworkAction_NetAccept,
  NetworkAction_MAX = NetworkAction_NetReceive
};

inline const NetworkAction (&EnumValuesNetworkAction())[5] {
  static const NetworkAction values[] = {
    NetworkAction_NetAccept,
    NetworkAction_NetConnect,
    NetworkAction_NetDisconnect,
    NetworkAction_NetSend,
    NetworkAction_NetReceive
  };
  return values;
}

inline const char * const *EnumNamesNetworkAction() {
  static const char * const names[6] = {
    "NetAccept",
    "NetConnect",
    "NetDisconnect",
    "NetSend",
    "NetReceive",
    nullptr
  };
  return names;
}

inline const char *EnumNameNetworkAction(NetworkAction e) {
  if (::flatbuffers::IsOutRange(e, NetworkAction_NetAccept, NetworkAction_NetReceive)) return "";
  const size_t index = static_cast<size_t>(e);
  return EnumNamesNetworkAction()[index];
}

enum ProcessAction : int8_t {
  ProcessAction_ProcessStart = 0,
  ProcessAction_ProcessEnd = 1,
  ProcessAction_ThreadCreate = 2,
  ProcessAction_ThreadTerminate = 3,
  ProcessAction_ImageLoad = 4,
  ProcessAction_ImageUnload = 5,
  ProcessAction_AllocLocal = 6,
  ProcessAction_AllocRemote = 7,
  ProcessAction_MIN = ProcessAction_ProcessStart,
  ProcessAction_MAX = ProcessAction_AllocRemote
};

inline const ProcessAction (&EnumValuesProcessAction())[8] {
  static const ProcessAction values[] = {
    ProcessAction_ProcessStart,
    ProcessAction_ProcessEnd,
    ProcessAction_ThreadCreate,
    ProcessAction_ThreadTerminate,
    ProcessAction_ImageLoad,
    ProcessAction_ImageUnload,
    ProcessAction_AllocLocal,
    ProcessAction_AllocRemote
  };
  return values;
}

inline const char * const *EnumNamesProcessAction() {
  static const char * const names[9] = {
    "ProcessStart",
    "ProcessEnd",
    "ThreadCreate",
    "ThreadTerminate",
    "ImageLoad",
    "ImageUnload",
    "AllocLocal",
    "AllocRemote",
    nullptr
  };
  return names;
}

inline const char *EnumNameProcessAction(ProcessAction e) {
  if (::flatbuffers::IsOutRange(e, ProcessAction_ProcessStart, ProcessAction_AllocRemote)) return "";
  const size_t index = static_cast<size_t>(e);
  return EnumNamesProcessAction()[index];
}

enum RegistryAction : int8_t {
  RegistryAction_RegCreateKey = 0,
  RegistryAction_RegSetValue = 1,
  RegistryAction_RegDeleteKey = 2,
  RegistryAction_RegDeleteValue = 3,
  RegistryAction_MIN = RegistryAction_RegCreateKey,
  RegistryAction_MAX = RegistryAction_RegDeleteValue
};

inline const RegistryAction (&EnumValuesRegistryAction())[4] {
  static const RegistryAction values[] = {
    RegistryAction_RegCreateKey,
    RegistryAction_RegSetValue,
    RegistryAction_RegDeleteKey,
    RegistryAction_RegDeleteValue
  };
  return values;
}

inline const char * const *EnumNamesRegistryAction() {
  static const char * const names[5] = {
    "RegCreateKey",
    "RegSetValue",
    "RegDeleteKey",
    "RegDeleteValue",
    nullptr
  };
  return names;
}

inline const char *EnumNameRegistryAction(RegistryAction e) {
  if (::flatbuffers::IsOutRange(e, RegistryAction_RegCreateKey, RegistryAction_RegDeleteValue)) return "";
  const size_t index = static_cast<size_t>(e);
  return EnumNamesRegistryAction()[index];
}

enum RegistryValueType : int8_t {
  RegistryValueType_RegBinary = 0,
  RegistryValueType_RegDWord = 1,
  RegistryValueType_RegQWord = 2,
  RegistryValueType_RegString = 3,
  RegistryValueType_RegStringList = 4,
  RegistryValueType_RegLink = 5,
  RegistryValueType_MIN = RegistryValueType_RegBinary,
  RegistryValueType_MAX = RegistryValueType_RegLink
};

inline const RegistryValueType (&EnumValuesRegistryValueType())[6] {
  static const RegistryValueType values[] = {
    RegistryValueType_RegBinary,
    RegistryValueType_RegDWord,
    RegistryValueType_RegQWord,
    RegistryValueType_RegString,
    RegistryValueType_RegStringList,
    RegistryValueType_RegLink
  };
  return values;
}

inline const char * const *EnumNamesRegistryValueType() {
  static const char * const names[7] = {
    "RegBinary",
    "RegDWord",
    "RegQWord",
    "RegString",
    "RegStringList",
    "RegLink",
    nullptr
  };
  return names;
}

inline const char *EnumNameRegistryValueType(RegistryValueType e) {
  if (::flatbuffers::IsOutRange(e, RegistryValueType_RegBinary, RegistryValueType_RegLink)) return "";
  const size_t index = static_cast<size_t>(e);
  return EnumNamesRegistryValueType()[index];
}

struct FileEvent FLATBUFFERS_FINAL_CLASS : private ::flatbuffers::Table {
  typedef FileEventBuilder Builder;
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_ACTION = 4,
    VT_NAME = 6,
    VT_PATH = 8,
    VT_DIRECTORY = 10,
    VT_EXTENSION = 12,
    VT_NEW_NAME = 14,
    VT_IS_DIRECTORY = 16
  };
  Sensor::FileAction action() const {
    return static_cast<Sensor::FileAction>(GetField<int8_t>(VT_ACTION, 0));
  }
  const ::flatbuffers::String *name() const {
    return GetPointer<const ::flatbuffers::String *>(VT_NAME);
  }
  const ::flatbuffers::String *path() const {
    return GetPointer<const ::flatbuffers::String *>(VT_PATH);
  }
  const ::flatbuffers::String *directory() const {
    return GetPointer<const ::flatbuffers::String *>(VT_DIRECTORY);
  }
  const ::flatbuffers::String *extension() const {
    return GetPointer<const ::flatbuffers::String *>(VT_EXTENSION);
  }
  const ::flatbuffers::String *new_name() const {
    return GetPointer<const ::flatbuffers::String *>(VT_NEW_NAME);
  }
  bool is_directory() const {
    return GetField<uint8_t>(VT_IS_DIRECTORY, 0) != 0;
  }
  bool Verify(::flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<int8_t>(verifier, VT_ACTION, 1) &&
           VerifyOffset(verifier, VT_NAME) &&
           verifier.VerifyString(name()) &&
           VerifyOffset(verifier, VT_PATH) &&
           verifier.VerifyString(path()) &&
           VerifyOffset(verifier, VT_DIRECTORY) &&
           verifier.VerifyString(directory()) &&
           VerifyOffset(verifier, VT_EXTENSION) &&
           verifier.VerifyString(extension()) &&
           VerifyOffset(verifier, VT_NEW_NAME) &&
           verifier.VerifyString(new_name()) &&
           VerifyField<uint8_t>(verifier, VT_IS_DIRECTORY, 1) &&
           verifier.EndTable();
  }
};

struct FileEventBuilder {
  typedef FileEvent Table;
  ::flatbuffers::FlatBufferBuilder &fbb_;
  ::flatbuffers::uoffset_t start_;
  void add_action(Sensor::FileAction action) {
    fbb_.AddElement<int8_t>(FileEvent::VT_ACTION, static_cast<int8_t>(action), 0);
  }
  void add_name(::flatbuffers::Offset<::flatbuffers::String> name) {
    fbb_.AddOffset(FileEvent::VT_NAME, name);
  }
  void add_path(::flatbuffers::Offset<::flatbuffers::String> path) {
    fbb_.AddOffset(FileEvent::VT_PATH, path);
  }
  void add_directory(::flatbuffers::Offset<::flatbuffers::String> directory) {
    fbb_.AddOffset(FileEvent::VT_DIRECTORY, directory);
  }
  void add_extension(::flatbuffers::Offset<::flatbuffers::String> extension) {
    fbb_.AddOffset(FileEvent::VT_EXTENSION, extension);
  }
  void add_new_name(::flatbuffers::Offset<::flatbuffers::String> new_name) {
    fbb_.AddOffset(FileEvent::VT_NEW_NAME, new_name);
  }
  void add_is_directory(bool is_directory) {
    fbb_.AddElement<uint8_t>(FileEvent::VT_IS_DIRECTORY, static_cast<uint8_t>(is_directory), 0);
  }
  explicit FileEventBuilder(::flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  ::flatbuffers::Offset<FileEvent> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = ::flatbuffers::Offset<FileEvent>(end);
    return o;
  }
};

inline ::flatbuffers::Offset<FileEvent> CreateFileEvent(
    ::flatbuffers::FlatBufferBuilder &_fbb,
    Sensor::FileAction action = Sensor::FileAction_FileCreate,
    ::flatbuffers::Offset<::flatbuffers::String> name = 0,
    ::flatbuffers::Offset<::flatbuffers::String> path = 0,
    ::flatbuffers::Offset<::flatbuffers::String> directory = 0,
    ::flatbuffers::Offset<::flatbuffers::String> extension = 0,
    ::flatbuffers::Offset<::flatbuffers::String> new_name = 0,
    bool is_directory = false) {
  FileEventBuilder builder_(_fbb);
  builder_.add_new_name(new_name);
  builder_.add_extension(extension);
  builder_.add_directory(directory);
  builder_.add_path(path);
  builder_.add_name(name);
  builder_.add_is_directory(is_directory);
  builder_.add_action(action);
  return builder_.Finish();
}

inline ::flatbuffers::Offset<FileEvent> CreateFileEventDirect(
    ::flatbuffers::FlatBufferBuilder &_fbb,
    Sensor::FileAction action = Sensor::FileAction_FileCreate,
    const char *name = nullptr,
    const char *path = nullptr,
    const char *directory = nullptr,
    const char *extension = nullptr,
    const char *new_name = nullptr,
    bool is_directory = false) {
  auto name__ = name ? _fbb.CreateString(name) : 0;
  auto path__ = path ? _fbb.CreateString(path) : 0;
  auto directory__ = directory ? _fbb.CreateString(directory) : 0;
  auto extension__ = extension ? _fbb.CreateString(extension) : 0;
  auto new_name__ = new_name ? _fbb.CreateString(new_name) : 0;
  return Sensor::CreateFileEvent(
      _fbb,
      action,
      name__,
      path__,
      directory__,
      extension__,
      new_name__,
      is_directory);
}

struct NetworkEvent FLATBUFFERS_FINAL_CLASS : private ::flatbuffers::Table {
  typedef NetworkEventBuilder Builder;
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_ACTION = 4,
    VT_TCP = 6,
    VT_UDP = 8,
    VT_LOCAL_ADDR = 10,
    VT_LOCAL_PORT = 12,
    VT_REMOTE_ADDR = 14,
    VT_REMOTE_PORT = 16,
    VT_PACKET_SIZE = 18
  };
  Sensor::NetworkAction action() const {
    return static_cast<Sensor::NetworkAction>(GetField<int8_t>(VT_ACTION, 0));
  }
  bool tcp() const {
    return GetField<uint8_t>(VT_TCP, 0) != 0;
  }
  bool udp() const {
    return GetField<uint8_t>(VT_UDP, 0) != 0;
  }
  const ::flatbuffers::String *local_addr() const {
    return GetPointer<const ::flatbuffers::String *>(VT_LOCAL_ADDR);
  }
  int16_t local_port() const {
    return GetField<int16_t>(VT_LOCAL_PORT, 0);
  }
  const ::flatbuffers::String *remote_addr() const {
    return GetPointer<const ::flatbuffers::String *>(VT_REMOTE_ADDR);
  }
  int16_t remote_port() const {
    return GetField<int16_t>(VT_REMOTE_PORT, 0);
  }
  int32_t packet_size() const {
    return GetField<int32_t>(VT_PACKET_SIZE, 0);
  }
  bool Verify(::flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<int8_t>(verifier, VT_ACTION, 1) &&
           VerifyField<uint8_t>(verifier, VT_TCP, 1) &&
           VerifyField<uint8_t>(verifier, VT_UDP, 1) &&
           VerifyOffset(verifier, VT_LOCAL_ADDR) &&
           verifier.VerifyString(local_addr()) &&
           VerifyField<int16_t>(verifier, VT_LOCAL_PORT, 2) &&
           VerifyOffset(verifier, VT_REMOTE_ADDR) &&
           verifier.VerifyString(remote_addr()) &&
           VerifyField<int16_t>(verifier, VT_REMOTE_PORT, 2) &&
           VerifyField<int32_t>(verifier, VT_PACKET_SIZE, 4) &&
           verifier.EndTable();
  }
};

struct NetworkEventBuilder {
  typedef NetworkEvent Table;
  ::flatbuffers::FlatBufferBuilder &fbb_;
  ::flatbuffers::uoffset_t start_;
  void add_action(Sensor::NetworkAction action) {
    fbb_.AddElement<int8_t>(NetworkEvent::VT_ACTION, static_cast<int8_t>(action), 0);
  }
  void add_tcp(bool tcp) {
    fbb_.AddElement<uint8_t>(NetworkEvent::VT_TCP, static_cast<uint8_t>(tcp), 0);
  }
  void add_udp(bool udp) {
    fbb_.AddElement<uint8_t>(NetworkEvent::VT_UDP, static_cast<uint8_t>(udp), 0);
  }
  void add_local_addr(::flatbuffers::Offset<::flatbuffers::String> local_addr) {
    fbb_.AddOffset(NetworkEvent::VT_LOCAL_ADDR, local_addr);
  }
  void add_local_port(int16_t local_port) {
    fbb_.AddElement<int16_t>(NetworkEvent::VT_LOCAL_PORT, local_port, 0);
  }
  void add_remote_addr(::flatbuffers::Offset<::flatbuffers::String> remote_addr) {
    fbb_.AddOffset(NetworkEvent::VT_REMOTE_ADDR, remote_addr);
  }
  void add_remote_port(int16_t remote_port) {
    fbb_.AddElement<int16_t>(NetworkEvent::VT_REMOTE_PORT, remote_port, 0);
  }
  void add_packet_size(int32_t packet_size) {
    fbb_.AddElement<int32_t>(NetworkEvent::VT_PACKET_SIZE, packet_size, 0);
  }
  explicit NetworkEventBuilder(::flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  ::flatbuffers::Offset<NetworkEvent> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = ::flatbuffers::Offset<NetworkEvent>(end);
    return o;
  }
};

inline ::flatbuffers::Offset<NetworkEvent> CreateNetworkEvent(
    ::flatbuffers::FlatBufferBuilder &_fbb,
    Sensor::NetworkAction action = Sensor::NetworkAction_NetAccept,
    bool tcp = false,
    bool udp = false,
    ::flatbuffers::Offset<::flatbuffers::String> local_addr = 0,
    int16_t local_port = 0,
    ::flatbuffers::Offset<::flatbuffers::String> remote_addr = 0,
    int16_t remote_port = 0,
    int32_t packet_size = 0) {
  NetworkEventBuilder builder_(_fbb);
  builder_.add_packet_size(packet_size);
  builder_.add_remote_addr(remote_addr);
  builder_.add_local_addr(local_addr);
  builder_.add_remote_port(remote_port);
  builder_.add_local_port(local_port);
  builder_.add_udp(udp);
  builder_.add_tcp(tcp);
  builder_.add_action(action);
  return builder_.Finish();
}

inline ::flatbuffers::Offset<NetworkEvent> CreateNetworkEventDirect(
    ::flatbuffers::FlatBufferBuilder &_fbb,
    Sensor::NetworkAction action = Sensor::NetworkAction_NetAccept,
    bool tcp = false,
    bool udp = false,
    const char *local_addr = nullptr,
    int16_t local_port = 0,
    const char *remote_addr = nullptr,
    int16_t remote_port = 0,
    int32_t packet_size = 0) {
  auto local_addr__ = local_addr ? _fbb.CreateString(local_addr) : 0;
  auto remote_addr__ = remote_addr ? _fbb.CreateString(remote_addr) : 0;
  return Sensor::CreateNetworkEvent(
      _fbb,
      action,
      tcp,
      udp,
      local_addr__,
      local_port,
      remote_addr__,
      remote_port,
      packet_size);
}

struct WinProcess FLATBUFFERS_FINAL_CLASS : private ::flatbuffers::Table {
  typedef WinProcessBuilder Builder;
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_IMAGE_NAME = 4,
    VT_IMAGE_SIZE = 6,
    VT_ALLOC_SIZE = 8,
    VT_ALLOC_DATA = 10
  };
  const ::flatbuffers::String *image_name() const {
    return GetPointer<const ::flatbuffers::String *>(VT_IMAGE_NAME);
  }
  int32_t image_size() const {
    return GetField<int32_t>(VT_IMAGE_SIZE, 0);
  }
  int64_t alloc_size() const {
    return GetField<int64_t>(VT_ALLOC_SIZE, 0);
  }
  const ::flatbuffers::Vector<uint8_t> *alloc_data() const {
    return GetPointer<const ::flatbuffers::Vector<uint8_t> *>(VT_ALLOC_DATA);
  }
  bool Verify(::flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyOffset(verifier, VT_IMAGE_NAME) &&
           verifier.VerifyString(image_name()) &&
           VerifyField<int32_t>(verifier, VT_IMAGE_SIZE, 4) &&
           VerifyField<int64_t>(verifier, VT_ALLOC_SIZE, 8) &&
           VerifyOffset(verifier, VT_ALLOC_DATA) &&
           verifier.VerifyVector(alloc_data()) &&
           verifier.EndTable();
  }
};

struct WinProcessBuilder {
  typedef WinProcess Table;
  ::flatbuffers::FlatBufferBuilder &fbb_;
  ::flatbuffers::uoffset_t start_;
  void add_image_name(::flatbuffers::Offset<::flatbuffers::String> image_name) {
    fbb_.AddOffset(WinProcess::VT_IMAGE_NAME, image_name);
  }
  void add_image_size(int32_t image_size) {
    fbb_.AddElement<int32_t>(WinProcess::VT_IMAGE_SIZE, image_size, 0);
  }
  void add_alloc_size(int64_t alloc_size) {
    fbb_.AddElement<int64_t>(WinProcess::VT_ALLOC_SIZE, alloc_size, 0);
  }
  void add_alloc_data(::flatbuffers::Offset<::flatbuffers::Vector<uint8_t>> alloc_data) {
    fbb_.AddOffset(WinProcess::VT_ALLOC_DATA, alloc_data);
  }
  explicit WinProcessBuilder(::flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  ::flatbuffers::Offset<WinProcess> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = ::flatbuffers::Offset<WinProcess>(end);
    return o;
  }
};

inline ::flatbuffers::Offset<WinProcess> CreateWinProcess(
    ::flatbuffers::FlatBufferBuilder &_fbb,
    ::flatbuffers::Offset<::flatbuffers::String> image_name = 0,
    int32_t image_size = 0,
    int64_t alloc_size = 0,
    ::flatbuffers::Offset<::flatbuffers::Vector<uint8_t>> alloc_data = 0) {
  WinProcessBuilder builder_(_fbb);
  builder_.add_alloc_size(alloc_size);
  builder_.add_alloc_data(alloc_data);
  builder_.add_image_size(image_size);
  builder_.add_image_name(image_name);
  return builder_.Finish();
}

inline ::flatbuffers::Offset<WinProcess> CreateWinProcessDirect(
    ::flatbuffers::FlatBufferBuilder &_fbb,
    const char *image_name = nullptr,
    int32_t image_size = 0,
    int64_t alloc_size = 0,
    const std::vector<uint8_t> *alloc_data = nullptr) {
  auto image_name__ = image_name ? _fbb.CreateString(image_name) : 0;
  auto alloc_data__ = alloc_data ? _fbb.CreateVector<uint8_t>(*alloc_data) : 0;
  return Sensor::CreateWinProcess(
      _fbb,
      image_name__,
      image_size,
      alloc_size,
      alloc_data__);
}

struct ProcessEvent FLATBUFFERS_FINAL_CLASS : private ::flatbuffers::Table {
  typedef ProcessEventBuilder Builder;
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_ACTION = 4,
    VT_PID = 6,
    VT_PPID = 8,
    VT_SELF_PID = 10,
    VT_NAME = 12,
    VT_COMMAND_LINE = 14,
    VT_WIN = 16
  };
  Sensor::ProcessAction action() const {
    return static_cast<Sensor::ProcessAction>(GetField<int8_t>(VT_ACTION, 0));
  }
  int32_t pid() const {
    return GetField<int32_t>(VT_PID, 0);
  }
  int32_t ppid() const {
    return GetField<int32_t>(VT_PPID, 0);
  }
  int32_t self_pid() const {
    return GetField<int32_t>(VT_SELF_PID, 0);
  }
  const ::flatbuffers::String *name() const {
    return GetPointer<const ::flatbuffers::String *>(VT_NAME);
  }
  const ::flatbuffers::String *command_line() const {
    return GetPointer<const ::flatbuffers::String *>(VT_COMMAND_LINE);
  }
  const Sensor::WinProcess *win() const {
    return GetPointer<const Sensor::WinProcess *>(VT_WIN);
  }
  bool Verify(::flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<int8_t>(verifier, VT_ACTION, 1) &&
           VerifyField<int32_t>(verifier, VT_PID, 4) &&
           VerifyField<int32_t>(verifier, VT_PPID, 4) &&
           VerifyField<int32_t>(verifier, VT_SELF_PID, 4) &&
           VerifyOffset(verifier, VT_NAME) &&
           verifier.VerifyString(name()) &&
           VerifyOffset(verifier, VT_COMMAND_LINE) &&
           verifier.VerifyString(command_line()) &&
           VerifyOffset(verifier, VT_WIN) &&
           verifier.VerifyTable(win()) &&
           verifier.EndTable();
  }
};

struct ProcessEventBuilder {
  typedef ProcessEvent Table;
  ::flatbuffers::FlatBufferBuilder &fbb_;
  ::flatbuffers::uoffset_t start_;
  void add_action(Sensor::ProcessAction action) {
    fbb_.AddElement<int8_t>(ProcessEvent::VT_ACTION, static_cast<int8_t>(action), 0);
  }
  void add_pid(int32_t pid) {
    fbb_.AddElement<int32_t>(ProcessEvent::VT_PID, pid, 0);
  }
  void add_ppid(int32_t ppid) {
    fbb_.AddElement<int32_t>(ProcessEvent::VT_PPID, ppid, 0);
  }
  void add_self_pid(int32_t self_pid) {
    fbb_.AddElement<int32_t>(ProcessEvent::VT_SELF_PID, self_pid, 0);
  }
  void add_name(::flatbuffers::Offset<::flatbuffers::String> name) {
    fbb_.AddOffset(ProcessEvent::VT_NAME, name);
  }
  void add_command_line(::flatbuffers::Offset<::flatbuffers::String> command_line) {
    fbb_.AddOffset(ProcessEvent::VT_COMMAND_LINE, command_line);
  }
  void add_win(::flatbuffers::Offset<Sensor::WinProcess> win) {
    fbb_.AddOffset(ProcessEvent::VT_WIN, win);
  }
  explicit ProcessEventBuilder(::flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  ::flatbuffers::Offset<ProcessEvent> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = ::flatbuffers::Offset<ProcessEvent>(end);
    return o;
  }
};

inline ::flatbuffers::Offset<ProcessEvent> CreateProcessEvent(
    ::flatbuffers::FlatBufferBuilder &_fbb,
    Sensor::ProcessAction action = Sensor::ProcessAction_ProcessStart,
    int32_t pid = 0,
    int32_t ppid = 0,
    int32_t self_pid = 0,
    ::flatbuffers::Offset<::flatbuffers::String> name = 0,
    ::flatbuffers::Offset<::flatbuffers::String> command_line = 0,
    ::flatbuffers::Offset<Sensor::WinProcess> win = 0) {
  ProcessEventBuilder builder_(_fbb);
  builder_.add_win(win);
  builder_.add_command_line(command_line);
  builder_.add_name(name);
  builder_.add_self_pid(self_pid);
  builder_.add_ppid(ppid);
  builder_.add_pid(pid);
  builder_.add_action(action);
  return builder_.Finish();
}

inline ::flatbuffers::Offset<ProcessEvent> CreateProcessEventDirect(
    ::flatbuffers::FlatBufferBuilder &_fbb,
    Sensor::ProcessAction action = Sensor::ProcessAction_ProcessStart,
    int32_t pid = 0,
    int32_t ppid = 0,
    int32_t self_pid = 0,
    const char *name = nullptr,
    const char *command_line = nullptr,
    ::flatbuffers::Offset<Sensor::WinProcess> win = 0) {
  auto name__ = name ? _fbb.CreateString(name) : 0;
  auto command_line__ = command_line ? _fbb.CreateString(command_line) : 0;
  return Sensor::CreateProcessEvent(
      _fbb,
      action,
      pid,
      ppid,
      self_pid,
      name__,
      command_line__,
      win);
}

struct RegistryValue FLATBUFFERS_FINAL_CLASS : private ::flatbuffers::Table {
  typedef RegistryValueBuilder Builder;
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_INT_ = 4,
    VT_STR_ = 6,
    VT_STRLIST = 8,
    VT_BINARY = 10
  };
  int64_t int_() const {
    return GetField<int64_t>(VT_INT_, 0);
  }
  const ::flatbuffers::String *str_() const {
    return GetPointer<const ::flatbuffers::String *>(VT_STR_);
  }
  const ::flatbuffers::Vector<::flatbuffers::Offset<::flatbuffers::String>> *strlist() const {
    return GetPointer<const ::flatbuffers::Vector<::flatbuffers::Offset<::flatbuffers::String>> *>(VT_STRLIST);
  }
  const ::flatbuffers::Vector<uint8_t> *binary() const {
    return GetPointer<const ::flatbuffers::Vector<uint8_t> *>(VT_BINARY);
  }
  bool Verify(::flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<int64_t>(verifier, VT_INT_, 8) &&
           VerifyOffset(verifier, VT_STR_) &&
           verifier.VerifyString(str_()) &&
           VerifyOffset(verifier, VT_STRLIST) &&
           verifier.VerifyVector(strlist()) &&
           verifier.VerifyVectorOfStrings(strlist()) &&
           VerifyOffset(verifier, VT_BINARY) &&
           verifier.VerifyVector(binary()) &&
           verifier.EndTable();
  }
};

struct RegistryValueBuilder {
  typedef RegistryValue Table;
  ::flatbuffers::FlatBufferBuilder &fbb_;
  ::flatbuffers::uoffset_t start_;
  void add_int_(int64_t int_) {
    fbb_.AddElement<int64_t>(RegistryValue::VT_INT_, int_, 0);
  }
  void add_str_(::flatbuffers::Offset<::flatbuffers::String> str_) {
    fbb_.AddOffset(RegistryValue::VT_STR_, str_);
  }
  void add_strlist(::flatbuffers::Offset<::flatbuffers::Vector<::flatbuffers::Offset<::flatbuffers::String>>> strlist) {
    fbb_.AddOffset(RegistryValue::VT_STRLIST, strlist);
  }
  void add_binary(::flatbuffers::Offset<::flatbuffers::Vector<uint8_t>> binary) {
    fbb_.AddOffset(RegistryValue::VT_BINARY, binary);
  }
  explicit RegistryValueBuilder(::flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  ::flatbuffers::Offset<RegistryValue> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = ::flatbuffers::Offset<RegistryValue>(end);
    return o;
  }
};

inline ::flatbuffers::Offset<RegistryValue> CreateRegistryValue(
    ::flatbuffers::FlatBufferBuilder &_fbb,
    int64_t int_ = 0,
    ::flatbuffers::Offset<::flatbuffers::String> str_ = 0,
    ::flatbuffers::Offset<::flatbuffers::Vector<::flatbuffers::Offset<::flatbuffers::String>>> strlist = 0,
    ::flatbuffers::Offset<::flatbuffers::Vector<uint8_t>> binary = 0) {
  RegistryValueBuilder builder_(_fbb);
  builder_.add_int_(int_);
  builder_.add_binary(binary);
  builder_.add_strlist(strlist);
  builder_.add_str_(str_);
  return builder_.Finish();
}

inline ::flatbuffers::Offset<RegistryValue> CreateRegistryValueDirect(
    ::flatbuffers::FlatBufferBuilder &_fbb,
    int64_t int_ = 0,
    const char *str_ = nullptr,
    const std::vector<::flatbuffers::Offset<::flatbuffers::String>> *strlist = nullptr,
    const std::vector<uint8_t> *binary = nullptr) {
  auto str___ = str_ ? _fbb.CreateString(str_) : 0;
  auto strlist__ = strlist ? _fbb.CreateVector<::flatbuffers::Offset<::flatbuffers::String>>(*strlist) : 0;
  auto binary__ = binary ? _fbb.CreateVector<uint8_t>(*binary) : 0;
  return Sensor::CreateRegistryValue(
      _fbb,
      int_,
      str___,
      strlist__,
      binary__);
}

struct RegistryEvent FLATBUFFERS_FINAL_CLASS : private ::flatbuffers::Table {
  typedef RegistryEventBuilder Builder;
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_ACTION = 4,
    VT_KEY_PATH = 6,
    VT_VALUE_NAME = 8,
    VT_VALUE_TYPE = 10,
    VT_VALUE = 12
  };
  Sensor::RegistryAction action() const {
    return static_cast<Sensor::RegistryAction>(GetField<int8_t>(VT_ACTION, 0));
  }
  const ::flatbuffers::String *key_path() const {
    return GetPointer<const ::flatbuffers::String *>(VT_KEY_PATH);
  }
  const ::flatbuffers::String *value_name() const {
    return GetPointer<const ::flatbuffers::String *>(VT_VALUE_NAME);
  }
  Sensor::RegistryValueType value_type() const {
    return static_cast<Sensor::RegistryValueType>(GetField<int8_t>(VT_VALUE_TYPE, 0));
  }
  const Sensor::RegistryValue *value() const {
    return GetPointer<const Sensor::RegistryValue *>(VT_VALUE);
  }
  bool Verify(::flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<int8_t>(verifier, VT_ACTION, 1) &&
           VerifyOffset(verifier, VT_KEY_PATH) &&
           verifier.VerifyString(key_path()) &&
           VerifyOffset(verifier, VT_VALUE_NAME) &&
           verifier.VerifyString(value_name()) &&
           VerifyField<int8_t>(verifier, VT_VALUE_TYPE, 1) &&
           VerifyOffset(verifier, VT_VALUE) &&
           verifier.VerifyTable(value()) &&
           verifier.EndTable();
  }
};

struct RegistryEventBuilder {
  typedef RegistryEvent Table;
  ::flatbuffers::FlatBufferBuilder &fbb_;
  ::flatbuffers::uoffset_t start_;
  void add_action(Sensor::RegistryAction action) {
    fbb_.AddElement<int8_t>(RegistryEvent::VT_ACTION, static_cast<int8_t>(action), 0);
  }
  void add_key_path(::flatbuffers::Offset<::flatbuffers::String> key_path) {
    fbb_.AddOffset(RegistryEvent::VT_KEY_PATH, key_path);
  }
  void add_value_name(::flatbuffers::Offset<::flatbuffers::String> value_name) {
    fbb_.AddOffset(RegistryEvent::VT_VALUE_NAME, value_name);
  }
  void add_value_type(Sensor::RegistryValueType value_type) {
    fbb_.AddElement<int8_t>(RegistryEvent::VT_VALUE_TYPE, static_cast<int8_t>(value_type), 0);
  }
  void add_value(::flatbuffers::Offset<Sensor::RegistryValue> value) {
    fbb_.AddOffset(RegistryEvent::VT_VALUE, value);
  }
  explicit RegistryEventBuilder(::flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  ::flatbuffers::Offset<RegistryEvent> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = ::flatbuffers::Offset<RegistryEvent>(end);
    return o;
  }
};

inline ::flatbuffers::Offset<RegistryEvent> CreateRegistryEvent(
    ::flatbuffers::FlatBufferBuilder &_fbb,
    Sensor::RegistryAction action = Sensor::RegistryAction_RegCreateKey,
    ::flatbuffers::Offset<::flatbuffers::String> key_path = 0,
    ::flatbuffers::Offset<::flatbuffers::String> value_name = 0,
    Sensor::RegistryValueType value_type = Sensor::RegistryValueType_RegBinary,
    ::flatbuffers::Offset<Sensor::RegistryValue> value = 0) {
  RegistryEventBuilder builder_(_fbb);
  builder_.add_value(value);
  builder_.add_value_name(value_name);
  builder_.add_key_path(key_path);
  builder_.add_value_type(value_type);
  builder_.add_action(action);
  return builder_.Finish();
}

inline ::flatbuffers::Offset<RegistryEvent> CreateRegistryEventDirect(
    ::flatbuffers::FlatBufferBuilder &_fbb,
    Sensor::RegistryAction action = Sensor::RegistryAction_RegCreateKey,
    const char *key_path = nullptr,
    const char *value_name = nullptr,
    Sensor::RegistryValueType value_type = Sensor::RegistryValueType_RegBinary,
    ::flatbuffers::Offset<Sensor::RegistryValue> value = 0) {
  auto key_path__ = key_path ? _fbb.CreateString(key_path) : 0;
  auto value_name__ = value_name ? _fbb.CreateString(value_name) : 0;
  return Sensor::CreateRegistryEvent(
      _fbb,
      action,
      key_path__,
      value_name__,
      value_type,
      value);
}

}  // namespace Sensor

#endif  // FLATBUFFERS_GENERATED_ALCA_SENSOR_H_
