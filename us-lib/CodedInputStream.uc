class CodedInputStream extends Object;

// Class Vars
var array<byte> buffer;
var int cursor;

// Class Functions
function float ReadFloat()
{
	return ReadRawLittleEndian32Float();
}

function int ReadInt32()
{
	return ReadRawVarint32();
}

function int ReadUInt32()
{
	return ReadRawVarint32();
}

function int ReadFixed32()
{
	return ReadRawLittleEndian32();
}

function int ReadSFixed32()
{
	return ReadRawLittleEndian32();
}

function int ReadSInt32()
{
	return class'CodedUtil'.static.DecodeZigZag32(ReadRawVarint32());
}

function bool ReadBool()
{
	return ReadRawVarint32() != 0;
}

function string ReadString()
{
	local int size, idx;
	local string result;

	size = ReadRawVarint32();

	if (size > 0)
	{
		for (idx = 0; idx < size; idx++)
		{
			result $= Chr(ReadRawByte());
		}
	}

	return result;
}

function Message ReadMessage(class<Message> messageClazz)
{
	local int size, idx;
	local Message message;
	local array<byte> tempBuffer;

	local CodedInputStream stream;

	size = ReadRawVarint32();

	for (idx = 0; idx < size; idx++)
	{
		tempBuffer.AddItem(ReadRawByte());
	}

	stream = new class'CodedInputStream';
	stream.buffer = tempBuffer;

	message = new messageClazz;
	
	message.Deserialize(stream);

	return message;
}

function int ReadTag()
{
	return ReadRawVarint32();
}

function int ReadRawVarint32()
{
	local int temp;
	local int result, idx;

	if (buffer.Length <= cursor)
	{
		return 0;
	}

	temp = ReadRawSignedByte();

	if (temp >= 0)
	{
		result = temp;
	}
	else
	{
		result = temp & 0x7F;

		temp = ReadRawSignedByte();

		if (temp >= 0)
		{
			result = result | (temp << 7);
		}
		else
		{
			result = result | ((temp & 0x7F) << 7);

			temp = ReadRawSignedByte();

			if (temp >= 0)
			{
				result = result | (temp << 14);
			}
			else
			{
				result = result | ((temp & 0x7F) << 14);

				temp = ReadRawSignedByte();

				if (temp >= 0)
				{
					result = result | (temp << 21);
				}
				else
				{
					result = result | ((temp & 0x7F) << 21);

					temp = ReadRawSignedByte();

					result = result | ((temp & 0x7F) << 28);

					if (temp < 0)
					{
						for (idx = 0; idx < 5; idx++)
						{
							if (ReadRawSignedByte() >= 0)
							{
								return result;
							}
						}

						// TODO: Signal error condition.
					}
				}
			}
		}
	}

	return result;
}

function int ReadRawLittleEndian32()
{
	local int result;

	result = ReadRawByte();
	result = result | (ReadRawByte() << 8);
	result = result | (ReadRawByte() << 16);
	result = result | (ReadRawByte() << 24);

	return result;
}

function float ReadRawLittleEndian32Float()
{
	local float result;

	result = ReadRawByte();
	result = result | (ReadRawByte() << 8);
	result = result | (ReadRawByte() << 16);
	result = result | (ReadRawByte() << 24);

	return result;
}

function int ReadRawSignedByte()
{
	local byte temp;
	local int result;

	temp = ReadRawByte();

	if ((temp & 0x80) == 0x80)
	{
		temp = temp & 0x7F;

		result = temp;

		result = result | 0x80000000;
	}
	else
	{
		result = temp;
	}

	return result;
}

function byte ReadRawByte()
{
	return buffer[cursor++];
}

function SkipBytes(int count)
{
	cursor += count;
}