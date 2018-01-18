class Network extends TcpLink;

// Class Constants
const MESSAGE_NAME_LENGTH_SIZE = 1;

// Class Vars
var string serverAddress;
var int portNumber;

var array<byte> receiveBuffer;

// Class Delegates
delegate OnOpened();
delegate OnClosed();
delegate OnMessageReceived(Message message);

// Class Functions
function Start()
{
	`Log("Starting network.");
	`Log("Connecting to " $ serverAddress $ ":" $ portNumber);

	// Configure the link.
	ReceiveMode = RMODE_Event; // May need to go RMODE_Manual eventually.
	LinkMode = MODE_Binary;

	// This kicks off the event chain.
	Resolve(serverAddress);
}

function Stop()
{
	Close();
}

/*
 * This function is responsible for coordinating the 
 * message serialization and writing the resulting
 * bytes to the outgoing connection.
 */
function SendMessage(Message message)
{
	local CodedOutputStream header;
	local CodedOutputStream body;

	local int messageLength;

	`Log("Sending message: " $ message.id);
	
	body = new class'CodedOutputStream';

	message.Serialize(body);

	messageLength = body.buffer.Length 
		+ class'CodedUtil'.static.ComputeRawStringSize(message.id) 
		+ MESSAGE_NAME_LENGTH_SIZE;

	header = new class'CodedOutputStream';

	// Write the header.
	header.WriteRawVarint32(messageLength);
	header.WriteRawByte(Len(message.id));
	header.WriteRawString(message.id);

	SendBuffer(header.buffer); // Send header
	SendBuffer(body.buffer); // Send body
}

/*
 * A simple method for writing the contents of 
 * a byte array to the open connection.
 */
function SendBuffer(out array<byte> buffer)
{
	local byte bytes[255];
	local int total, count, idx;

	total = 0;

	while (total < buffer.Length)
	{
		count = Min(buffer.Length - total, 255);

		for (idx = 0; idx < count; idx++)
		{
			bytes[idx] = buffer[total + idx];
		}

		total += SendBinary(count, bytes);
	}
}

/*
 * Responsible for processing the buffer by
 * checking that a complete message has been 
 * received and then deserializing it.
 */
function ProcessBuffer()
{
	local byte temp;
	local int idx, cursor;

	local array<byte> tempBuffer;

	local int messageLength;
	local string messageName;
	local Message message;
	local class<Message> messageClazz;

	local CodedInputStream stream;

	cursor = 0;

	// Read Varint32 bytes for message length.
	for (idx = 0; idx < 5; idx++)
	{
		// If fifth iteration (i.e. wider than an int).
		if (idx == 4)
		{
			// TODO: Signal error here.

			return;
		}

		if (receiveBuffer.Length > cursor)
		{
			tempBuffer.AddItem(receiveBuffer[cursor++]);
			
			if (tempBuffer[idx] >= 0)
			{
				break;	
			}
		}
	}

	stream = new class'CodedInputStream';
	stream.buffer = tempBuffer;

	messageLength = stream.ReadRawVarint32();

	if (messageLength <= 0)
	{
		// TODO: Signal error here.

		return;
	}

	// Check that the entire message is available.
	if (receiveBuffer.Length < (cursor + messageLength))
	{
		return;
	}

	// Check that message name length byte is available.
	if (receiveBuffer.Length < (cursor + 1))
	{
		return;
	}

	// Get message name length.
	temp = receiveBuffer[cursor++];

	if (temp <= 0)
	{
		// TODO: Signal error here.

		return;
	}

	// Get message name.
	for (idx = 0; idx < temp; idx++)
	{
		messageName $= Chr(receiveBuffer[cursor++]);
	}

	`Log("Message name = '" $ messageName $ "'");

	// Copy message bytes into tempBuffer.
	tempBuffer.Remove(0, tempBuffer.Length);

	temp = cursor + (messageLength - temp - MESSAGE_NAME_LENGTH_SIZE);

	for (idx = cursor; idx < temp; idx++)
	{
		tempBuffer.AddItem(receiveBuffer[cursor++]);
	}

	// Dynamically load the message class.
	messageClazz = class<Message>(DynamicLoadObject("LastStand." $ messageName, class'Class'));

	if (messageClazz == none)
	{
		// TODO: Signal error here.

		return;
	}

	// Create and deserialize message.
	message = new messageClazz;

	stream = new class'CodedInputStream';
	stream.buffer = tempBuffer;

	message.Deserialize(stream);

	// Dispatch message.
	OnMessageReceived(message);

	// Clear message bytes from the receive buffer.
	receiveBuffer.Remove(0, cursor);
}

/*
 * Triggered when Resolve call succeeds.
 */
event Resolved(IpAddr address)
{
	`Log(serverAddress $ " resolved to " $ IpAddrToString(address));

	address.Port = portNumber;
	
	// UDK seems to require this.
	BindPort();

	// Open the connection!
	if (!Open(address))
	{
		// This does not appear to work as advertised.
		`Log("Unable to connect to specified address.");
	}
}

/*
 * Triggered when Resolve call fails.
 */
event ResolveFailed()
{
	`Log("Failed to resolve " $ serverAddress $ ".  Connection aborted.");
}

/*
 * Triggered when connection is opened.
 */
event Opened()
{
	`Log("The connection has been established.");

	OnOpened();
}

/*
 * Triggered when connection is closed.
 */
event Closed()
{
	`Log("The connection has been closed.");

	OnClosed();
}

/*
 * Triggered when some binary data is received.
 */
event ReceivedBinary(int count, byte buffer[255])
{
	local int idx;

	`Log("Received " $ count $ " bytes of data.");

	for (idx = 0; idx < count; idx++)
	{
		receiveBuffer.AddItem(buffer[idx]);
	}

	ProcessBuffer();
}