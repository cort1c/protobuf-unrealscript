class Error extends Message;

// Class Constants
const CODE_FIELD_NUMBER = 1;
const MESSAGE_FIELD_NUMBER = 2;

// Class Vars
var int code;
var string message;

// Class Functions
function Serialize(CodedOutputStream stream)
{
	stream.WriteInt32(CODE_FIELD_NUMBER, code);
	stream.WriteString(MESSAGE_FIELD_NUMBER, message);
}

function Deserialize(CodedInputStream stream)
{
	local int tag, fieldNumber, wireType;

	tag = stream.ReadTag();

	while (tag > 0)
	{
		`Log("tag = " $ tag);

		fieldNumber = class'WireFormat'.static.GetTagFieldNumber(tag);
		wireType = class'WireFormat'.static.GetTagWireType(tag);

		`Log("fieldNumber = " $ fieldNumber $ ", wireType = " $ wireType);

		if (fieldNumber == CODE_FIELD_NUMBER)
		{
			code = stream.ReadInt32();
		}
		else if (fieldNumber == MESSAGE_FIELD_NUMBER)
		{
			message = stream.ReadString();
		}

		tag = stream.ReadTag();
	}

	`Log("code = " $ code $ ", message = '" $ message $ "'");
}

function int GetSerializedSize()
{
	return class'CodedUtil'.static.ComputeInt32Size(CODE_FIELD_NUMBER, code)
		+ class'CodedUtil'.static.ComputeStringSize(MESSAGE_FIELD_NUMBER, message);
}

defaultproperties
{
	id = "Error";
}