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

#include <algorithm>
#include <google/protobuf/stubs/hash.h>
#include <google/protobuf/compiler/us/us_message.h>
#include <google/protobuf/compiler/us/us_helpers.h>
#include <google/protobuf/stubs/strutil.h>
#include <google/protobuf/io/printer.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/wire_format.h>
#include <google/protobuf/descriptor.pb.h>

namespace google {
namespace protobuf {
namespace compiler {
namespace us {

using internal::WireFormat;
using internal::WireFormatLite;

namespace {

struct FieldOrderingByNumber {
  inline bool operator()(const FieldDescriptor* a,
                         const FieldDescriptor* b) const {
    return a->number() < b->number();
  }
};

struct ExtensionRangeOrdering {
  bool operator()(const Descriptor::ExtensionRange* a,
                  const Descriptor::ExtensionRange* b) const {
    return a->start < b->start;
  }
};

// Sort the fields of the given Descriptor by number into a new[]'d array
// and return it.
const FieldDescriptor** SortFieldsByNumber(const Descriptor* descriptor) {
  const FieldDescriptor** fields =
    new const FieldDescriptor*[descriptor->field_count()];
  for (int i = 0; i < descriptor->field_count(); i++) {
    fields[i] = descriptor->field(i);
  }
  sort(fields, fields + descriptor->field_count(),
       FieldOrderingByNumber());
  return fields;
}

}  // namespace

// ===================================================================

MessageGenerator::MessageGenerator(const Descriptor* descriptor)
  : descriptor_(descriptor) {
}

MessageGenerator::~MessageGenerator() {}

// ===================================================================

bool MessageGenerator::HasRepeatedField()
{
	for (int i = 0; i < descriptor_->field_count(); i++)
		if (descriptor_->field(i)->is_repeated())
			return true;
	return false;
}

void MessageGenerator::Generate(io::Printer* printer) {
  // Print class declaration
  printer->Print("class $classname$ extends Message;\n\n", "classname", "Message" + descriptor_->name());

  // Print class constants
  printer->Print("// Class constants\n");

  for (int i = 0; i < descriptor_->field_count(); i++) {
    printer->Print("const $fieldname$_FIELD_NUMBER = $fieldnumber$;\n",
      "fieldname", ToUpperCase(descriptor_->field(i)->name()), 
	  "fieldnumber", SimpleItoa(descriptor_->field(i)->number())); 
  }

  // Print class variables
  printer->Print("\n// Class variables\n");

  for (int i = 0; i < descriptor_->field_count(); i++) {
    if (descriptor_->field(i)->is_repeated()) {
      printer->Print("var array<$fieldtype$> $fieldname$;\n",
        "fieldtype",
          (descriptor_->field(i)->type() == FieldDescriptor::TYPE_MESSAGE) ? 
            "Message" + descriptor_->field(i)->message_type()->name() : 
            GetPrimitiveTypeName(GetUnrealScriptType(descriptor_->field(i))),
        "fieldname", SafeFieldname(descriptor_->field(i)->name()));
    } else {
      printer->Print("var $fieldtype$ $fieldname$;\n",
        "fieldtype",
          (descriptor_->field(i)->type() == FieldDescriptor::TYPE_MESSAGE) ? 
            "Message" + descriptor_->field(i)->message_type()->name() : 
            GetPrimitiveTypeName(GetUnrealScriptType(descriptor_->field(i))),
        "fieldname", SafeFieldname(descriptor_->field(i)->name()));
    }
  }

  printer->Print("\n// Class functions\n");

  // Print Serialize method
  printer->Print("function Serialize(CodedOutputStream stream)\n{\n");
  printer->Indent();
  printer->Indent();

  // Print this only if there are repeated fields
  if( HasRepeatedField() )
	printer->Print("local int idx;\n\n");

  for (int i = 0; i < descriptor_->field_count(); i++) {
    if (descriptor_->field(i)->is_repeated()) {
      printer->Print("\nfor (idx = 0; idx < $fieldname$.Length; idx++)\n{\n",
        "fieldname", SafeFieldname(descriptor_->field(i)->name()));
      printer->Indent();
      printer->Indent();
      printer->Print("stream.$methodname$($constname$_FIELD_NUMBER, $fieldname$[idx]);\n",
        "methodname", GetSerializeMethodName(descriptor_->field(i)),
        "constname", ToUpperCase(descriptor_->field(i)->name()),
        "fieldname", SafeFieldname(descriptor_->field(i)->name()));
      printer->Outdent();
      printer->Outdent();
      printer->Print("}\n");
    } else {
      printer->Print("stream.$methodname$($constname$_FIELD_NUMBER, $fieldname$);\n",
        "methodname", GetSerializeMethodName(descriptor_->field(i)),
        "constname", ToUpperCase(descriptor_->field(i)->name()),
        "fieldname", SafeFieldname(descriptor_->field(i)->name()));
    }
  }

  printer->Outdent();
  printer->Outdent();
  printer->Print("}\n");
  
  // Print Deserialize method
  printer->Print("\nfunction Deserialize(CodedInputStream stream)\n{\n");
  printer->Indent();
  printer->Indent();
  //printer->Print("local int tag, fieldNumber;\n\n");

  // Only print the field number if there are fields to iterate on
  if( descriptor_->field_count() > 0 )
	printer->Print("local int tag, fieldNumber;\n\n");
  else
	printer->Print("local int tag;\n\n");

  printer->Print("tag = stream.ReadTag();\n\n");
  printer->Print("while (tag > 0)\n{\n");
  printer->Indent();
  printer->Indent();

  if( descriptor_->field_count() > 0 )
	printer->Print("fieldNumber = class'WireFormat'.static.GetTagFieldNumber(tag);\n");
  else
	printer->Print("class'WireFormat'.static.GetTagFieldNumber(tag);\n");
  

  for (int i = 0; i < descriptor_->field_count(); i++) {
    printer->Print("$if$ (fieldNumber == $constname$_FIELD_NUMBER)\n{\n",
      "if", (i == 0) ? "if" : "else if", "constname", ToUpperCase(descriptor_->field(i)->name()));
    printer->Indent();
    printer->Indent();

    if (descriptor_->field(i)->is_repeated())
	{
		// Check for a the type of message, so we can pass in the proper class
		if( descriptor_->field(i)->type() == FieldDescriptor::TYPE_MESSAGE )
		{
			printer->Print("$fieldname$.AddItem( $fieldtype$(stream.$methodname$(class'$fieldtype$')) );\n",
				"fieldname", SafeFieldname(descriptor_->field(i)->name()), 
				"methodname", GetDeserializeMethodName(descriptor_->field(i)),
				"fieldtype","Message" + descriptor_->field(i)->message_type()->name() );
		}
		else
		{
			printer->Print("$fieldname$.AddItem(stream.$methodname$());\n",
				"fieldname", SafeFieldname(descriptor_->field(i)->name()), 
				"methodname", GetDeserializeMethodName(descriptor_->field(i))
			);
		}
    }
	else
	{
		// Check for a the type of message, so we can pass in the proper class
		if( descriptor_->field(i)->type() == FieldDescriptor::TYPE_MESSAGE )
		{
			printer->Print("$fieldname$ = $fieldtype$(stream.$methodname$(class'$fieldtype$'));\n", 
				"fieldname", SafeFieldname(descriptor_->field(i)->name()), 
				"methodname", GetDeserializeMethodName(descriptor_->field(i)),
				"fieldtype","Message" + descriptor_->field(i)->message_type()->name()
			);
		}
		else
		{
			printer->Print("$fieldname$ = stream.$methodname$();\n", 
				"fieldname", SafeFieldname(descriptor_->field(i)->name()), 
				"methodname", GetDeserializeMethodName(descriptor_->field(i))
			);
		}

    }

    printer->Outdent();
    printer->Outdent();
    printer->Print("}\n");
  }

  printer->Print("\ntag = stream.ReadTag();\n");
  printer->Outdent();
  printer->Outdent();
  printer->Print("}\n");
  printer->Outdent();
  printer->Outdent();
  printer->Print("}\n");

  // Print GetSerializedSize method
  printer->Print("\nfunction int GetSerializedSize()\n{\n");
  printer->Indent();
  printer->Indent();
  //printer->Print("local int size, idx;\n\nsize = 0;\n\n");
  printer->Print("local int _size;\n");

  if( HasRepeatedField() )
	printer->Print("local int idx;\n");

  printer->Print("_size = 0;\n\n");
  
  for (int i = 0; i < descriptor_->field_count(); i++)
  {
	if (descriptor_->field(i)->is_repeated())
	{
		printer->Print("\nfor (idx = 0; idx < $fieldname$.Length; idx++)\n{\n", "fieldname", descriptor_->field(i)->name());
		printer->Indent();
		printer->Indent();

		printer->Print("_size += class'CodedUtil'.static.$methodname$($constname$_FIELD_NUMBER, $fieldname$[idx]);\n",
		  "methodname", GetComputeSizeMethodName(descriptor_->field(i)),
		  "constname", ToUpperCase(descriptor_->field(i)->name()),
		  "fieldname", SafeFieldname(descriptor_->field(i)->name()));
    }
	else
	{
		printer->Print("_size += class'CodedUtil'.static.$methodname$($constname$_FIELD_NUMBER, $fieldname$);\n",
		  "methodname", GetComputeSizeMethodName(descriptor_->field(i)),
		  "constname", ToUpperCase(descriptor_->field(i)->name()),
		  "fieldname", SafeFieldname(descriptor_->field(i)->name()));
	}


    if (descriptor_->field(i)->is_repeated()) {
      printer->Outdent();
      printer->Outdent();
      printer->Print("}\n");
    }
  }

  printer->Print("\nreturn _size;\n");
  printer->Outdent();
  printer->Outdent();
  printer->Print("}\n");

  // Print defaultproperties block
  printer->Print("\ndefaultproperties\n{\n");
  printer->Indent();
  printer->Indent();
  printer->Print("_id = \"$classname$\";\n", "classname", descriptor_->name());
  printer->Outdent();
  printer->Outdent();
  printer->Print("}\n");
}

}  // namespace us
}  // namespace compiler
}  // namespace protobuf
}  // namespace google
