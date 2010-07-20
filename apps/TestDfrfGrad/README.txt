TestDfrfGrad

This application tests the DFRF protocoll with Gradient Policy. A dedicated node
(programmed with node ID 0) sends a message to the network with broadcast policy
(with pseudorandom data), to build the gradient field. If any node receives this
message, it will set up it's rank in the gradient policy and send it back with it's
nodeid (in this case, the rank is the location from the sender in hops) with 
Gradient Policy. 
Every node sends the received dfrf messages to the serial port.

Usage:

- Program some node with the TestDfrfGrad application, one with node ID 0, this
will be the sender.
- Connect the nodes you want to monitor to a PC
- Use the net.tinyos.tools.Listen application to monitor them

Message format:
<0x00><2B destination><2B source><1B length><1B group ID><0x77><2B data><2B original source>
The destination and source are the destination and source of the message, and the
original source is the node which sent the message first.
The group ID is 0 if the message received with GradientPolicy and 1 if it's
received with BroadCastPolicy
The data is random if the message received with BroadcastPolicy and the rank, if
it's received with GradientPolicy