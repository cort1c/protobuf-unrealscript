class CodedOutputStream extends Object;

// Class Vars
var array<byte> buffer;

function WriteFloat(int fieldNumber, float value)
{
	WriteTag(fieldNumber, class'WireFormat'.const.WIRE_TYPE_FIXED32);
	WriteRawLittleEndian32Float(value);
}

function WriteInt32(int fieldNumber, int value)
{
	WriteTag(fieldNumber, class'WireFormat'.const.WIRE_TYPE_VARINT);
	WriteRawVarint32(value);
}

function WriteUInt32(int fieldNumber, int value)
{
	WriteTag(fieldNumber, class'WireFormat'.const.WIRE_TYPE_VARINT);
	WriteRawVarint32(value);
}

function WriteSInt32(int fieldNumber, int value)
{
	WriteTag(fieldNumber, class'WireFormat'.const.WIRE_TYPE_VARINT);
	WriteRawVarint32(class'CodedUtil'.static.EncodeZigZag32(value));
}

function WriteFixed32(int fieldNumber, int value)
{
	WriteTag(fieldNumber, class'WireFormat'.const.WIRE_TYPE_FIXED32);
	WriteRawLittleEndian32(value);
}

function WriteSFixed32(int fieldNumber, int value)
{
	WriteTag(fieldNumber, class'WireFormat'.const.WIRE_TYPE_FIXED32);
	WriteRawLittleEndian32(value);
}

function WriteBool(int fieldNumber, bool value)
{
	WriteTag(fieldNumber, class'WireFormat'.const.WIRE_TYPE_VARINT);
	WriteRawByte(value ? 1 : 0);
}

/*
 * Only supports ASCII strings.
 */
function WriteString(int fieldNumber, string value)
{
	WriteTag(fieldNumber, class'WireFormat'.const.WIRE_TYPE_LENGTH_DELIMITED);
	WriteRawVarint32(Len(value));
	WriteRawString(value);
}

function WriteRawString(string value)
{
	local int idx;
	local string substring;

	for (idx = 0; idx < Len(value); idx++)
	{
		substring = Mid(value, idx, 1);

		WriteRawByte(Asc(substring));
	}
}

function WriteMessage(int fieldNumber, Message message)
{
	local CodedOutputStream stream;
	local int idx;

	stream = new class'CodedOutputStream';

	message.Serialize(stream);

	WriteTag(fieldNumber, class'WireFormat'.const.WIRE_TYPE_LENGTH_DELIMITED);
	WriteRawVarint32(stream.buffer.Length);

	for (idx = 0; idx < stream.buffer.Length; idx++)
	{
		WriteRawByte(stream.buffer[idx]);
	}
}

function WriteTag(int fieldNumber, int wireType)
{
	WriteRawVarint32(class'WireFormat'.static.MakeTag(fieldNumber, wireType));
}

function WriteRawVarint32(int value)
{
	while (true)
	{
		if ((value & ~0x7F) == 0)
		{
			WriteRawByte(value);
			
			return;
		}
		else
		{
			WriteRawByte((value & 0x7F) | 0x80);

			value = value >>> 7;
		}
	}
}

function WriteRawLittleEndian32(int value)
{
	WriteRawByte(value & 0xFF);
	WriteRawByte((value >> 8) & 0xFF);
	WriteRawByte((value >> 16) & 0xFF);
	WriteRawByte((value >> 24) & 0xFF);
}

function WriteRawLittleEndian32Float(float value)
{
	WriteRawByte(value & 0xFF);
	WriteRawByte((value >> 8) & 0xFF);
	WriteRawByte((value >> 16) & 0xFF);
	WriteRawByte((value >> 24) & 0xFF);
} 

function WriteRawByte(byte value)
{
	buffer.AddItem(value);
}