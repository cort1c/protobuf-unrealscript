class WireFormat extends Object;

// Wire Types
const WIRE_TYPE_VARINT = 0;
const WIRE_TYPE_FIXED64 = 1;
const WIRE_TYPE_LENGTH_DELIMITED = 2;
const WIRE_TYPE_START_GROUP = 3;
const WIRE_TYPE_END_GROUP = 4;
const WIRE_TYPE_FIXED32 = 5;

// Tag Types
const TAG_TYPE_BITS = 3;
const TAG_TYPE_MASK = 7;

// Message Sets
const MESSAGE_SET_ITEM = 1;
const MESSAGE_SET_TYPE_ID = 2;
const MESSAGE_SET_MESSAGE = 3;

//const MESSAGE_SET_ITEM_TAG = MakeTag(MESSAGE_SET_ITEM, WIRE_TYPE_START_GROUP);
//const MESSAGE_SET_ITEM_END_TAG = MakeTag(MESSAGE_SET_ITEM, WIRE_TYPE_END_GROUP);
//const MESSAGE_SET_TYPE_ID_TAG = MakeTag(MESSAGE_SET_TYPE_ID, WIRE_TYPE_VARINT);
//const MESSAGE_SET_MESSAGE_TAG = MakeTag(MESSAGE_SET_MESSAGE, WIRE_TYPE_LENGTH_DELIMITED);

static function int GetTagWireType(int tag)
{
	return tag & TAG_TYPE_MASK;
}

static function int GetTagFieldNumber(int tag)
{
	return tag >>> TAG_TYPE_BITS;
}

static function int MakeTag(int fieldNumber, int wireType)
{
	return (fieldNumber << TAG_TYPE_BITS) | wireType;
}