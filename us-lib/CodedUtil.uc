class CodedUtil extends Object;

// Class Consts
const LITTLE_ENDIAN_32_SIZE = 4;

// Class Functions
static function int ComputeFloatSize(int fieldNumber, float value)
{
	return ComputeTagSize(fieldNumber) + LITTLE_ENDIAN_32_SIZE;
}

static function int ComputeInt32Size(int fieldNumber, int value)
{
	if (value >= 0)
	{
		return ComputeTagSize(fieldNumber) + ComputeRawVarint32Size(value);
	}
	else
	{
		return ComputeTagSize(fieldNumber) + 10;
	}
}

static function int ComputeUInt32Size(int fieldNumber, int value)
{
	return ComputeTagSize(fieldNumber) + ComputeRawVarint32Size(value);
}

static function int ComputeSInt32Size(int fieldNumber, int value)
{
	return ComputeTagSize(fieldNumber) + ComputeRawVarint32Size(EncodeZigZag32(value));
}

static function int ComputeFixed32Size(int fieldNumber, int value)
{
	return ComputeTagSize(fieldNumber) + LITTLE_ENDIAN_32_SIZE;
}

static function int ComputeSFixed32Size(int fieldNumber, int value)
{
	return ComputeTagSize(fieldNumber) + LITTLE_ENDIAN_32_SIZE;
}

static function int ComputeBoolSize(int fieldNumber, bool value)
{
	return ComputeTagSize(fieldNumber) + 1;
}

/*
 * Only supports ASCII strings.
 */
static function int ComputeStringSize(int fieldNumber, string value)
{
	return ComputeTagSize(fieldNumber) + ComputeRawStringSize(value);
}

/*
 * Only supports ASCII strings.
 */
static function int ComputeRawStringSize(string value)
{
	return Len(value);
}

static function int ComputeMessageSize(int fieldNumber, Message message)
{
	return ComputeTagSize(fieldNumber) + message.GetSerializedSize();
}

static function int ComputeTagSize(int fieldNumber)
{
	return ComputeRawVarint32Size(class'WireFormat'.static.MakeTag(fieldNumber, 0));
}

static function int ComputeRawVarint32Size(int value)
{
	if ((value & (0xffffffff << 7)) == 0) return 1;
	if ((value & (0xffffffff << 14)) == 0) return 2;
	if ((value & (0xffffffff << 21)) == 0) return 3;
	if ((value & (0xffffffff << 28)) == 0) return 4;
	
	return 5;
}

static function int EncodeZigZag32(int value)
{
	return (value << 1) ^ (value >> 31);
}

static function int DecodeZigZag32(int value)
{
	return (value >>> 1) ^ -(value & 1);
}

static function PrintBytes(out array<byte> bytes)
{
	local int idx;

	`Log("bytes.Length = " $ bytes.Length);

	for (idx = 0; idx < bytes.Length; idx++)
	{
		`Log("bytes[" $ idx $ "] = " $ bytes[idx]);
	}
}