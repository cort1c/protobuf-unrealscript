class Test extends Message;

// Class Constants
const LEVEL_FIELD_NUMBER = 1;
const USERNAME_FIELD_NUMBER = 2;
const AGE_FIELD_NUMBER = 3;
const YEAR_FIELD_NUMBER = 4;
const RANGE_FIELD_NUMBER = 5;
const SPEED_FIELD_NUMBER = 6;
const BANNED_FIELD_NUMBER = 7;
const VELOCITY_FIELD_NUMBER = 8;
const EMBED_FIELD_NUMBER = 9;
const ROLES_FIELD_NUMBER = 10;

// Class Variables
var int level;
var string username;
var int age;
var int year;
var int range;
var int speed;
var bool banned;
var float velocity;
var Embed embed;
var array<string> roles;

// Class Functions
function Serialize(CodedOutputStream stream)
{
	local int idx;

	stream.WriteFixed32(LEVEL_FIELD_NUMBER, level);
	stream.WriteString(USERNAME_FIELD_NUMBER, username);
	stream.WriteInt32(AGE_FIELD_NUMBER, age);
	stream.WriteUInt32(YEAR_FIELD_NUMBER, year);
	stream.WriteSInt32(RANGE_FIELD_NUMBER, range);
	stream.WriteSFixed32(SPEED_FIELD_NUMBER, speed);
	stream.WriteBool(BANNED_FIELD_NUMBER, banned);
	stream.WriteFloat(VELOCITY_FIELD_NUMBER, velocity);
	stream.WriteMessage(EMBED_FIELD_NUMBER, embed);

	for (idx = 0; idx < roles.Length; idx++)
	{
		stream.WriteString(ROLES_FIELD_NUMBER, roles[idx]);
	}
}

function Deserialize(CodedInputStream stream)
{
	local int tag, fieldNumber, wireType;

	tag = stream.ReadTag();

	while (tag > 0)
	{
		fieldNumber = class'WireFormat'.static.GetTagFieldNumber(tag);
		wireType = class'WireFormat'.static.GetTagWireType(tag);

		if (fieldNumber == LEVEL_FIELD_NUMBER)
		{
			level = stream.ReadFixed32();
		}
		else if (fieldNumber == USERNAME_FIELD_NUMBER)
		{
			username = stream.ReadString();

			`Log("username = '" $ username $ "'");
		}
		else if (fieldNumber == AGE_FIELD_NUMBER)
		{
			age = stream.ReadInt32();

			`Log("age = " $ age);
		}
		else if (fieldNumber == YEAR_FIELD_NUMBER)
		{
			year = stream.ReadUInt32();

			`Log("year = " $ year);
		}
		else if (fieldNumber == RANGE_FIELD_NUMBER)
		{
			range = stream.ReadSInt32();

			`Log("range = " $ range);
		}
		else if (fieldNumber == SPEED_FIELD_NUMBER)
		{
			speed = stream.ReadSFixed32();

			`Log("speed = " $ speed);
		}
		else if (fieldNumber == BANNED_FIELD_NUMBER)
		{
			banned = stream.ReadBool();

			`Log("banned = " $ banned);
		}
		else if (fieldNumber == VELOCITY_FIELD_NUMBER)
		{
			velocity = stream.ReadFloat();

			`Log("velocity = " $ velocity);
		}
		else if (fieldNumber == EMBED_FIELD_NUMBER)
		{
			embed = Embed(stream.ReadMessage(class'Embed'));

			`Log("embed.title = '" $ embed.title $ "'");
		}
		else if (fieldNumber == ROLES_FIELD_NUMBER)
		{
			roles.AddItem(stream.ReadString());

			`Log("role = '" $ roles[roles.Length - 1] $ "'");
		}

		tag = stream.ReadTag();
	}
}

function int GetSerializedSize()
{
	local int size, idx;

	size = 0;

	size += class'CodedUtil'.static.ComputeFixed32Size(LEVEL_FIELD_NUMBER, level);
	size += class'CodedUtil'.static.ComputeStringSize(USERNAME_FIELD_NUMBER, username);
	size += class'CodedUtil'.static.ComputeInt32Size(AGE_FIELD_NUMBER, age);
	size += class'CodedUtil'.static.ComputeUInt32Size(YEAR_FIELD_NUMBER, year);
	size += class'CodedUtil'.static.ComputeSInt32Size(RANGE_FIELD_NUMBER, range);
	size += class'CodedUtil'.static.ComputeSFixed32Size(SPEED_FIELD_NUMBER, speed);
	size += class'CodedUtil'.static.ComputeBoolSize(BANNED_FIELD_NUMBER, banned);
	size += class'CodedUtil'.static.ComputeFloatSize(VELOCITY_FIELD_NUMBER, velocity);
	size += class'CodedUtil'.static.ComputeMessageSize(EMBED_FIELD_NUMBER, embed);

	for (idx = 0; idx < roles.Length; idx++)
	{
		size += class'CodedUtil'.static.ComputeStringSize(ROLES_FIELD_NUMBER, roles[idx]);
	}

	return size;
}

defaultproperties
{
	id = "Test";
}