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

#include <limits>
#include <vector>

#include <google/protobuf/compiler/us/us_helpers.h>
#include <google/protobuf/descriptor.pb.h>
#include <google/protobuf/stubs/strutil.h>
#include <google/protobuf/stubs/substitute.h>

namespace google {
namespace protobuf {
namespace compiler {
namespace us {

const char kThickSeparator[] =
  "// ===================================================================\n";
const char kThinSeparator[] =
  "// -------------------------------------------------------------------\n";

namespace {

const char* kDefaultPackage = "";

const string& FieldName(const FieldDescriptor* field) {
  // Groups are hacky:  The name of the field is just the lower-cased name
  // of the group type.  In Java, though, we would like to retain the original
  // capitalization of the type name.
  if (GetType(field) == FieldDescriptor::TYPE_GROUP) {
    return field->message_type()->name();
  } else {
    return field->name();
  }
}

string UnderscoresToCamelCaseImpl(const string& input, bool cap_next_letter) {
  string result;
  // Note:  I distrust ctype.h due to locales.
  for (int i = 0; i < input.size(); i++) {
    if ('a' <= input[i] && input[i] <= 'z') {
      if (cap_next_letter) {
        result += input[i] + ('A' - 'a');
      } else {
        result += input[i];
      }
      cap_next_letter = false;
    } else if ('A' <= input[i] && input[i] <= 'Z') {
      if (i == 0 && !cap_next_letter) {
        // Force first letter to lower-case unless explicitly told to
        // capitalize it.
        result += input[i] + ('a' - 'A');
      } else {
        // Capital letters after the first are left as-is.
        result += input[i];
      }
      cap_next_letter = false;
    } else if ('0' <= input[i] && input[i] <= '9') {
      result += input[i];
      cap_next_letter = true;
    } else {
      cap_next_letter = true;
    }
  }
  return result;
}

}  // namespace

string UnderscoresToCamelCase(const FieldDescriptor* field) {
  return UnderscoresToCamelCaseImpl(FieldName(field), false);
}

string UnderscoresToCapitalizedCamelCase(const FieldDescriptor* field) {
  return UnderscoresToCamelCaseImpl(FieldName(field), true);
}

string UnderscoresToCamelCase(const MethodDescriptor* method) {
  return UnderscoresToCamelCaseImpl(method->name(), false);
}

string StripProto(const string& filename) {
  if (HasSuffixString(filename, ".protodevel")) {
    return StripSuffixString(filename, ".protodevel");
  } else {
    return StripSuffixString(filename, ".proto");
  }
}

string FileClassName(const FileDescriptor* file) {
  if (file->options().has_java_outer_classname()) {
    return file->options().java_outer_classname();
  } else {
    string basename;
    string::size_type last_slash = file->name().find_last_of('/');
    if (last_slash == string::npos) {
      basename = file->name();
    } else {
      basename = file->name().substr(last_slash + 1);
    }
    return UnderscoresToCamelCaseImpl(StripProto(basename), true);
  }
}

string FileJavaPackage(const FileDescriptor* file) {
  string result;

  if (file->options().has_java_package()) {
    result = file->options().java_package();
  } else {
    result = kDefaultPackage;
    if (!file->package().empty()) {
      if (!result.empty()) result += '.';
      result += file->package();
    }
  }


  return result;
}

string JavaPackageToDir(string package_name) {
  string package_dir =
    StringReplace(package_name, ".", "/", true);
  if (!package_dir.empty()) package_dir += "/";
  return package_dir;
}

string ToJavaName(const string& full_name, const FileDescriptor* file) {
  string result;
  if (file->options().java_multiple_files()) {
    result = FileJavaPackage(file);
  } else {
    result = ClassName(file);
  }
  if (!result.empty()) {
    result += '.';
  }
  if (file->package().empty()) {
    result += full_name;
  } else {
    // Strip the proto package from full_name since we've replaced it with
    // the Java package.
    result += full_name.substr(file->package().size() + 1);
  }
  return result;
}

string ClassName(const FileDescriptor* descriptor) {
  string result = FileJavaPackage(descriptor);
  if (!result.empty()) result += '.';
  result += FileClassName(descriptor);
  return result;
}

string FieldConstantName(const FieldDescriptor *field) {
  string name = field->name() + "_FIELD_NUMBER";
  UpperString(&name);
  return name;
}

FieldDescriptor::Type GetType(const FieldDescriptor* field) {
  return field->type();
}

string ToUpperCase(string str) {
  for (int i = 0; i < str.length(); i++) {
    //str[i] = std::toupper(str[i]);
	  str[i] = toupper(str[i]);
  }
  return str;
}

string SafeFieldname(string str)
{
	if( str.compare("name") == 0 ) 
		str = "_name";
	return str;
}

UnrealScriptType GetUnrealScriptType(const FieldDescriptor* field) {
  switch (GetType(field)) {
    case FieldDescriptor::TYPE_INT32:
    case FieldDescriptor::TYPE_UINT32:
    case FieldDescriptor::TYPE_SINT32:
    case FieldDescriptor::TYPE_FIXED32:
    case FieldDescriptor::TYPE_SFIXED32:
      return UNREALSCRIPT_TYPE_INT;

    case FieldDescriptor::TYPE_STRING:
      return UNREALSCRIPT_TYPE_STRING;

    case FieldDescriptor::TYPE_BOOL:
      return UNREALSCRIPT_TYPE_BOOLEAN;

    case FieldDescriptor::TYPE_MESSAGE:
      return UNREALSCRIPT_TYPE_MESSAGE;

    case FieldDescriptor::TYPE_BYTES:
      return UNREALSCRIPT_TYPE_BYTES;
  }

  GOOGLE_LOG(FATAL) << "Unsupported UnrealScript Type!" << GetTypeLabel(field);

  return UNREALSCRIPT_TYPE_INT;
}

const char* GetTypeLabel(const FieldDescriptor* field)
{
	switch(GetType(field))
	{
		case FieldDescriptor::TYPE_DOUBLE: return "TYPE_DOUBLE";
		case FieldDescriptor::TYPE_FLOAT: return "TYPE_FLOAT";
		case FieldDescriptor::TYPE_INT64: return "TYPE_INT64";
		case FieldDescriptor::TYPE_UINT64: return "TYPE_UINT64";
		case FieldDescriptor::TYPE_INT32: return "TYPE_INT32";
		case FieldDescriptor::TYPE_FIXED64: return "TYPE_FIXED64";
		case FieldDescriptor::TYPE_FIXED32: return "TYPE_FIXED32";
		case FieldDescriptor::TYPE_BOOL: return "TYPE_BOOL";
		case FieldDescriptor::TYPE_STRING: return "TYPE_STRING";
		case FieldDescriptor::TYPE_GROUP: return "TYPE_GROUP";
		case FieldDescriptor::TYPE_MESSAGE: return "TYPE_MESSAGE";

		case FieldDescriptor::TYPE_BYTES: return "TYPE_BYTES";
		case FieldDescriptor::TYPE_UINT32: return "TYPE_UINT32";
		case FieldDescriptor::TYPE_ENUM: return "TYPE_ENUM";
		case FieldDescriptor::TYPE_SFIXED32: return "TYPE_SFIXED32";
		case FieldDescriptor::TYPE_SFIXED64: return "TYPE_SFIXED64";
		case FieldDescriptor::TYPE_SINT32: return "TYPE_SINT32";
		case FieldDescriptor::TYPE_SINT64: return "TYPE_SINT64";
	}
	return "NULL";
}

const char* GetDeserializeMethodName(const FieldDescriptor* field)
{
	switch (GetType(field))
	{
	    case FieldDescriptor::TYPE_INT32: return "ReadInt32";
	    case FieldDescriptor::TYPE_UINT32: return "ReadUInt32";
	    case FieldDescriptor::TYPE_SINT32: return "ReadSInt32";
	    case FieldDescriptor::TYPE_FIXED32: return "ReadFixed32";
	    case FieldDescriptor::TYPE_SFIXED32: return "ReadSFixed32";
	    case FieldDescriptor::TYPE_STRING: return "ReadString";
	    case FieldDescriptor::TYPE_BOOL: return "ReadBool";
		case FieldDescriptor::TYPE_MESSAGE: return "ReadMessage";
		case FieldDescriptor::TYPE_BYTES: return "ReadBytes";
			
	}

	GOOGLE_LOG(FATAL) << "Unsupported Deserialize Method Type!" << GetTypeLabel(field);

	return "NULL";
}

const char* GetSerializeMethodName(const FieldDescriptor* field) {
  switch (GetType(field)) {
    case FieldDescriptor::TYPE_INT32: return "WriteInt32";
    case FieldDescriptor::TYPE_UINT32: return "WriteUInt32";
    case FieldDescriptor::TYPE_SINT32: return "WriteSInt32";
    case FieldDescriptor::TYPE_FIXED32: return "WriteFixed32";
    case FieldDescriptor::TYPE_SFIXED32: return "WriteSFixed32";
    case FieldDescriptor::TYPE_STRING: return "WriteString";
    case FieldDescriptor::TYPE_BOOL: return "WriteBool";
    case FieldDescriptor::TYPE_MESSAGE: return "WriteMessage";
	case FieldDescriptor::TYPE_BYTES: return "WriteBytes";
		
  }

  GOOGLE_LOG(FATAL) << "Unsupported Serialize Method Type!" << GetTypeLabel(field);

  return "NULL";
}

const char* GetComputeSizeMethodName(const FieldDescriptor* field) {
  switch (GetType(field)) {
    case FieldDescriptor::TYPE_INT32: return "ComputeInt32Size";
    case FieldDescriptor::TYPE_UINT32: return "ComputeUInt32Size";
    case FieldDescriptor::TYPE_SINT32: return "ComputeSInt32Size";
    case FieldDescriptor::TYPE_FIXED32: return "ComputeFixed32Size";
	case FieldDescriptor::TYPE_SFIXED32: return "ComputeSFixed32Size";
    case FieldDescriptor::TYPE_STRING: return "ComputeStringSize";
    case FieldDescriptor::TYPE_BOOL: return "ComputeBoolSize";
    case FieldDescriptor::TYPE_MESSAGE: return "ComputeMessageSize";
	case FieldDescriptor::TYPE_BYTES: return "ComputeBytesSize";
  }

  GOOGLE_LOG(FATAL) << "Unsupported Size Type!" << GetTypeLabel(field);

  return "NULL";
}

const char* GetPrimitiveTypeName(UnrealScriptType type)
{
	switch (type)
	{
		case UNREALSCRIPT_TYPE_INT: return "int";
		case UNREALSCRIPT_TYPE_STRING: return "string";
		case UNREALSCRIPT_TYPE_BOOLEAN: return "bool";
		case UNREALSCRIPT_TYPE_MESSAGE: return "NULL";
		case UNREALSCRIPT_TYPE_BYTES: return "Array<byte>";
			
	}

	GOOGLE_LOG(FATAL) << "Unsupported Primitive Type!";
	return "NULL";
}

bool AllAscii(const string& text) {
  for (int i = 0; i < text.size(); i++) {
    if ((text[i] & 0x80) != 0) {
      return false;
    }
  }
  return true;
}

string DefaultValue(const FieldDescriptor* field) {
  // Switch on CppType since we need to know which default_value_* method
  // of FieldDescriptor to call.
  switch (field->cpp_type()) {
    case FieldDescriptor::CPPTYPE_INT32:
      return SimpleItoa(field->default_value_int32());
    case FieldDescriptor::CPPTYPE_UINT32:
      // Need to print as a signed int since Java has no unsigned.
      return SimpleItoa(static_cast<int32>(field->default_value_uint32()));
    case FieldDescriptor::CPPTYPE_INT64:
      return SimpleItoa(field->default_value_int64()) + "L";
    case FieldDescriptor::CPPTYPE_UINT64:
      return SimpleItoa(static_cast<int64>(field->default_value_uint64())) +
             "L";
    case FieldDescriptor::CPPTYPE_DOUBLE: {
      double value = field->default_value_double();
      if (value == numeric_limits<double>::infinity()) {
        return "Double.POSITIVE_INFINITY";
      } else if (value == -numeric_limits<double>::infinity()) {
        return "Double.NEGATIVE_INFINITY";
      } else if (value != value) {
        return "Double.NaN";
      } else {
        return SimpleDtoa(value) + "D";
      }
    }
    case FieldDescriptor::CPPTYPE_FLOAT: {
      float value = field->default_value_float();
      if (value == numeric_limits<float>::infinity()) {
        return "Float.POSITIVE_INFINITY";
      } else if (value == -numeric_limits<float>::infinity()) {
        return "Float.NEGATIVE_INFINITY";
      } else if (value != value) {
        return "Float.NaN";
      } else {
        return SimpleFtoa(value) + "F";
      }
    }
    case FieldDescriptor::CPPTYPE_BOOL:
      return field->default_value_bool() ? "true" : "false";
    case FieldDescriptor::CPPTYPE_STRING:
      if (GetType(field) == FieldDescriptor::TYPE_BYTES) {
        if (field->has_default_value()) {
          // See comments in Internal.java for gory details.
          return strings::Substitute(
            "com.google.protobuf.Internal.bytesDefaultValue(\"$0\")",
            CEscape(field->default_value_string()));
        } else {
          return "com.google.protobuf.ByteString.EMPTY";
        }
      } else {
        if (AllAscii(field->default_value_string())) {
          // All chars are ASCII.  In this case CEscape() works fine.
          return "\"" + CEscape(field->default_value_string()) + "\"";
        } else {
          // See comments in Internal.java for gory details.
          return strings::Substitute(
            "com.google.protobuf.Internal.stringDefaultValue(\"$0\")",
            CEscape(field->default_value_string()));
        }
      }

    case FieldDescriptor::CPPTYPE_ENUM:
      return ClassName(field->enum_type()) + "." +
             field->default_value_enum()->name();

    case FieldDescriptor::CPPTYPE_MESSAGE:
      return ClassName(field->message_type()) + ".getDefaultInstance()";

    // No default because we want the compiler to complain if any new
    // types are added.
  }

  GOOGLE_LOG(FATAL) << "Can't get here.";
  return "";
}

bool IsDefaultValueJavaDefault(const FieldDescriptor* field) {
  // Switch on CppType since we need to know which default_value_* method
  // of FieldDescriptor to call.
  switch (field->cpp_type()) {
    case FieldDescriptor::CPPTYPE_INT32:
      return field->default_value_int32() == 0;
    case FieldDescriptor::CPPTYPE_UINT32:
      return field->default_value_uint32() == 0;
    case FieldDescriptor::CPPTYPE_INT64:
      return field->default_value_int64() == 0L;
    case FieldDescriptor::CPPTYPE_UINT64:
      return field->default_value_uint64() == 0L;
    case FieldDescriptor::CPPTYPE_DOUBLE:
      return field->default_value_double() == 0.0;
    case FieldDescriptor::CPPTYPE_FLOAT:
      return field->default_value_float() == 0.0;
    case FieldDescriptor::CPPTYPE_BOOL:
      return field->default_value_bool() == false;

    case FieldDescriptor::CPPTYPE_STRING:
    case FieldDescriptor::CPPTYPE_ENUM:
    case FieldDescriptor::CPPTYPE_MESSAGE:
      return false;

    // No default because we want the compiler to complain if any new
    // types are added.
  }

  GOOGLE_LOG(FATAL) << "Can't get here.";
  return false;
}

}  // namespace us
}  // namespace compiler
}  // namespace protobuf
}  // namespace google
