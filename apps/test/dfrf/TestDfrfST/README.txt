TestDfrfST

This application tests the DFRF protocoll with Spanning Tree Policy. A dedicated
node (programmed with node ID 0) sends a message to the network with broadcast
policy (with pseudorandom data), to build the spanning tree. If any node receives
this message, it will set up it's rank in the spanning tree policy and send it
back with it's nodeid (in this case, the rank is the parent of the node) with SpanningTreePolicy. 
Every node sends the receied dfrf messages to the serial port.

Usage:

- Program some node with the TestDfrfST application, one with node ID 0, this
will be the sender.
- Connect the nodes you want to monitor to a PC
- Use the net.tinyos.tools.Listen application to monitor them

Message format:
<0x00><2B destination><2B source><1B length><1B group ID><0x77><2B data><2B original source>
The destination and source are the destination and source of the message, and the
original source is the node which sent the message first.
The group ID is 0 if the message received with SpanningTreePolicy and 1 if it's
received with BroadCastPolicy
The data is random if the message received with BroadcastPolicy and the rank, if
it's received with SpanningTreePolicy