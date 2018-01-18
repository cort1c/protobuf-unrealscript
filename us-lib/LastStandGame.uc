class LastStandGame extends FrameworkGame;

var Network network;

/*
 * Called once a successful connection has 
 * been established.
 */
function OnOpened()
{
	`Log("The connection has been opened!  Yay!");

	SendTestData();
}

/*
 * Called when the connection is closed either 
 * by the client or server.
 */
function OnClosed()
{
	`Log("The connection has been closed.  Oh noes!");
}

/*
 * Called when a message is received and 
 * successfully deserialized.
 */
function OnMessageReceived(Message message)
{
	`Log("Received message: id = " $ message.id);
}

function SendTestData()
{
	local Test test;
	local Embed embed;

	test = new class'Test';
	test.level = 1234567890;
	test.username = "justin";
	test.age = 31;
	test.year = 1234567890;
	test.range = -1234567890;
	test.speed = -1234567890;
	test.banned = true;
	test.velocity = 7.0; // Yeah, this doesn't work right now.
	test.roles.AddItem("hello");
	test.roles.AddItem("right");

	embed = new class'Embed';
	embed.title = "programmer";

	test.embed = embed;

	network.SendMessage(test);
}

event PostBeginPlay()
{
	`Log("Hello from LastStandGame!");
	
	// Create the network and set host and port.
	network = Spawn(class'Network');
	network.serverAddress = "192.168.1.136";
	network.portNumber = 5770;

	// Set relevent delegates.
	network.OnOpened = OnOpened;
	network.OnClosed = OnClosed;
	network.OnMessageReceived = OnMessageReceived;

	// Start the connection.
	network.Start();
}