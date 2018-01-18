// Protocol Buffers - Google's data interchange format
// Copyright 2008 Google Inc.  All rights reserved.
// http://code.google.com/p/protobuf/
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//     * Neither the name of Google Inc. nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

// Author: kenton@google.com (Kenton Varda)
//  Based on original Protocol Buffers design by
//  Sanjay Ghemawat, Jeff Dean, and others.

#ifndef GOOGLE_PROTOBUF_COMPILER_US_HELPERS_H__
#define GOOGLE_PROTOBUF_COMPILER_US_HELPERS_H__

#if _MSC_VER < 1600
	#include<ctype.h>
#endif

#include <string>
#include <google/protobuf/descriptor.pb.h>
#include <google/protobuf/descriptor.h>

namespace google {
namespace protobuf {
namespace compiler {
namespace us {

// Commonly-used separator comments.  Thick is a line of '=', thin is a line
// of '-'.
extern const char kThickSeparator[];
extern const char kThinSeparator[];

// Converts the field's name to camel-case, e.g. "foo_bar_baz" becomes
// "fooBarBaz" or "FooBarBaz", respectively.
string UnderscoresToCamelCase(const FieldDescriptor* field);
string UnderscoresToCapitalizedCamelCase(const FieldDescriptor* field);

// Similar, but for method names.  (Typically, this merely has the effect
// of lower-casing the first letter of the name.)
string UnderscoresToCamelCase(const MethodDescriptor* method);

// Strips ".proto" or ".protodevel" from the end of a filename.
string StripProto(const string& filename);

// Gets the unqualified class name for the file.  Each .proto file becomes a
// single Java class, with all its contents nested in that class.
string FileClassName(const FileDescriptor* file);

// Returns the file's Java package name.
string FileJavaPackage(const FileDescriptor* file);

// Returns output directory for the given package name.
string JavaPackageToDir(string package_name);

// Converts the given fully-qualified name in the proto namespace to its
// fully-qualified name in the Java namespace, given that it is in the given
// file.
string ToJavaName(const string& full_name, const FileDescriptor* file);

// These return the fully-qualified class name corresponding to the given
// descriptor.
inline string ClassName(const Descriptor* descriptor) {
  return ToJavaName(descriptor->full_name(), descriptor->file());
}
inline string ClassName(const EnumDescriptor* descriptor) {
  return ToJavaName(descriptor->full_name(), descriptor->file());
}
inline string ClassName(const ServiceDescriptor* descriptor) {
  return ToJavaName(descriptor->full_name(), descriptor->file());
}
inline string ExtensionIdentifierName(const FieldDescriptor* descriptor) {
  return ToJavaName(descriptor->full_name(), descriptor->file());
}
string ClassName(const FileDescriptor* descriptor);

// Get the unqualified name that should be used for a field's field
// number constant.
string FieldConstantName(const FieldDescriptor *field);

// Returns the type of the FieldDescriptor.
// This does nothing interesting for the open source release, but is used for
// hacks that improve compatability with version 1 protocol buffers at Google.
FieldDescriptor::Type GetType(const FieldDescriptor* field);

enum UnrealScriptType {
  UNREALSCRIPT_TYPE_INT,
  UNREALSCRIPT_TYPE_STRING,
  UNREALSCRIPT_TYPE_BOOLEAN,
  UNREALSCRIPT_TYPE_MESSAGE,
  UNREALSCRIPT_TYPE_BYTES
};

UnrealScriptType GetUnrealScriptType(const FieldDescriptor* field);

const char* GetTypeLabel(const FieldDescriptor* field);
const char* GetDeserializeMethodName(const FieldDescriptor* field);
const char* GetSerializeMethodName(const FieldDescriptor* field);
const char* GetComputeSizeMethodName(const FieldDescriptor* field);
const char* GetPrimitiveTypeName(UnrealScriptType type);

string ToUpperCase(string str);
string SafeFieldname(string str);

string DefaultValue(const FieldDescriptor* field);
bool IsDefaultValueJavaDefault(const FieldDescriptor* field);

// Does this message class keep track of unknown fields?
inline bool HasUnknownFields(const Descriptor* descriptor) {
  return descriptor->file()->options().optimize_for() !=
           FileOptions::LITE_RUNTIME;
}

// Does this message class have generated parsing, serialization, and other
// standard methods for which reflection-based fallback implementations exist?
inline bool HasGeneratedMethods(const Descriptor* descriptor) {
  return descriptor->file()->options().optimize_for() !=
           FileOptions::CODE_SIZE;
}

// Does this message have specialized equals() and hashCode() methods?
inline bool HasEqualsAndHashCode(const Descriptor* descriptor) {
  return descriptor->file()->options().java_generate_equals_and_hash();
}

// Does this message class have descriptor and reflection methods?
inline bool HasDescriptorMethods(const Descriptor* descriptor) {
  return descriptor->file()->options().optimize_for() !=
           FileOptions::LITE_RUNTIME;
}
inline bool HasDescriptorMethods(const EnumDescriptor* descriptor) {
  return descriptor->file()->options().optimize_for() !=
           FileOptions::LITE_RUNTIME;
}
inline bool HasDescriptorMethods(const FileDescriptor* descriptor) {
  return descriptor->options().optimize_for() !=
           FileOptions::LITE_RUNTIME;
}

inline bool HasNestedBuilders(const Descriptor* descriptor) {
  // The proto-lite version doesn't support nested builders.
  return descriptor->file()->options().optimize_for() !=
           FileOptions::LITE_RUNTIME;
}

// Should we generate generic services for this file?
inline bool HasGenericServices(const FileDescriptor *file) {
  return file->service_count() > 0 &&
         file->options().optimize_for() != FileOptions::LITE_RUNTIME &&
         file->options().java_generic_services();
}

}  // namespace us
}  // namespace compiler
}  // namespace protobuf

}  // namespace google
#endif  // GOOGLE_PROTOBUF_COMPILER_US_HELPERS_H__
