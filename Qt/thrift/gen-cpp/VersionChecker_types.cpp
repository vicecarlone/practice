/**
 * Autogenerated by Thrift Compiler (0.11.0)
 *
 * DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
 *  @generated
 */
#include "VersionChecker_types.h"

#include <algorithm>
#include <ostream>

#include <thrift/TToString.h>

namespace VersionChecker {


Version::~Version() throw() {
}


void Version::__set_version(const int32_t val) {
  this->version = val;
}

void Version::__set__major(const int32_t val) {
  this->_major = val;
}

void Version::__set__minor(const int32_t val) {
  this->_minor = val;
}

void Version::__set_additional(const int32_t val) {
  this->additional = val;
}
std::ostream& operator<<(std::ostream& out, const Version& obj)
{
  obj.printTo(out);
  return out;
}


uint32_t Version::read(::apache::thrift::protocol::TProtocol* iprot) {

  ::apache::thrift::protocol::TInputRecursionTracker tracker(*iprot);
  uint32_t xfer = 0;
  std::string fname;
  ::apache::thrift::protocol::TType ftype;
  int16_t fid;

  xfer += iprot->readStructBegin(fname);

  using ::apache::thrift::protocol::TProtocolException;


  while (true)
  {
    xfer += iprot->readFieldBegin(fname, ftype, fid);
    if (ftype == ::apache::thrift::protocol::T_STOP) {
      break;
    }
    switch (fid)
    {
      case 1:
        if (ftype == ::apache::thrift::protocol::T_I32) {
          xfer += iprot->readI32(this->version);
          this->__isset.version = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 2:
        if (ftype == ::apache::thrift::protocol::T_I32) {
          xfer += iprot->readI32(this->_major);
          this->__isset._major = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 3:
        if (ftype == ::apache::thrift::protocol::T_I32) {
          xfer += iprot->readI32(this->_minor);
          this->__isset._minor = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      case 4:
        if (ftype == ::apache::thrift::protocol::T_I32) {
          xfer += iprot->readI32(this->additional);
          this->__isset.additional = true;
        } else {
          xfer += iprot->skip(ftype);
        }
        break;
      default:
        xfer += iprot->skip(ftype);
        break;
    }
    xfer += iprot->readFieldEnd();
  }

  xfer += iprot->readStructEnd();

  return xfer;
}

uint32_t Version::write(::apache::thrift::protocol::TProtocol* oprot) const {
  uint32_t xfer = 0;
  ::apache::thrift::protocol::TOutputRecursionTracker tracker(*oprot);
  xfer += oprot->writeStructBegin("Version");

  xfer += oprot->writeFieldBegin("version", ::apache::thrift::protocol::T_I32, 1);
  xfer += oprot->writeI32(this->version);
  xfer += oprot->writeFieldEnd();

  xfer += oprot->writeFieldBegin("_major", ::apache::thrift::protocol::T_I32, 2);
  xfer += oprot->writeI32(this->_major);
  xfer += oprot->writeFieldEnd();

  xfer += oprot->writeFieldBegin("_minor", ::apache::thrift::protocol::T_I32, 3);
  xfer += oprot->writeI32(this->_minor);
  xfer += oprot->writeFieldEnd();

  xfer += oprot->writeFieldBegin("additional", ::apache::thrift::protocol::T_I32, 4);
  xfer += oprot->writeI32(this->additional);
  xfer += oprot->writeFieldEnd();

  xfer += oprot->writeFieldStop();
  xfer += oprot->writeStructEnd();
  return xfer;
}

void swap(Version &a, Version &b) {
  using ::std::swap;
  swap(a.version, b.version);
  swap(a._major, b._major);
  swap(a._minor, b._minor);
  swap(a.additional, b.additional);
  swap(a.__isset, b.__isset);
}

Version::Version(const Version& other0) {
  version = other0.version;
  _major = other0._major;
  _minor = other0._minor;
  additional = other0.additional;
  __isset = other0.__isset;
}
Version& Version::operator=(const Version& other1) {
  version = other1.version;
  _major = other1._major;
  _minor = other1._minor;
  additional = other1.additional;
  __isset = other1.__isset;
  return *this;
}
void Version::printTo(std::ostream& out) const {
  using ::apache::thrift::to_string;
  out << "Version(";
  out << "version=" << to_string(version);
  out << ", " << "_major=" << to_string(_major);
  out << ", " << "_minor=" << to_string(_minor);
  out << ", " << "additional=" << to_string(additional);
  out << ")";
}

} // namespace