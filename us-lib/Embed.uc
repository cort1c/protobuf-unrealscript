class Embed extends Message;

// Class Constants
const TITLE_FIELD_NUMBER = 1;

// Class Variables
var string title;

// Class Functions
function Serialize(CodedOutputStream stream)
{
	stream.WriteString(TITLE_FIELD_NUMBER, title);
}

function Deserialize(CodedInputStream stream)
{
	local int tag, fieldNumber, wireType;

	tag = stream.ReadTag();

	while (tag > 0)
	{
		fieldNumber = class'WireFormat'.static.GetTagFieldNumber(tag);
		wireType = class'WireFormat'.static.GetTagWireType(tag);

		if (fieldNumber == TITLE_FIELD_NUMBER)
		{
			title = stream.ReadString();
		}

		`Log("title = '" $ title $ "'");

		tag = stream.ReadTag();
	}
}

function int GetSerializedSize()
{
	return class'CodedUtil'.static.ComputeStringSize(TITLE_FIELD_NUMBER, title);
}

defaultproperties
{
	id = "Embed";
}