class Message extends Object abstract;

/*
 * Must be set by subclasses in defaultproperties
 * block.
 */
var string id;

/*
 * Takes a CodedOutputStream and writes all 
 * data to dematerialize this message.
 * 
 * Must be overriden by subclasses.
 */
function Serialize(CodedOutputStream stream)
{
	// Intentionally empty.
}

/*
 * Takes a CodedInputStream and reads data to
 * materialize this message.
 * 
 * Must be overriden by subclasses.
 */
function Deserialize(CodedInputStream stream)
{
	// Intentionally empty.
}

/*
 * Returns the serialized size of this 
 * message.
 * 
 * Must be overriden by subclasses.
 */
function int GetSerializedSize()
{
	return -1;
}