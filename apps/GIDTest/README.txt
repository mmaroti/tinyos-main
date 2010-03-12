README for GIDTest
Author/Contact: veresskrisztian@gmail.com

Description: 

This application is intended to test the radio drivers whether they properly set the 
AM group id. To execute the test, one should program 3 motes with TOS_NODE_ID 1, 2, and 3.

This way Mote 1 will send packages every sec with a count value from 0 to 7 and the same group id.
Mote 2 should display the count value on it's leds and Mote 3 should display the group id of the 
received message on it's leds.

If Mote 2 and Mote 3 displays the same value, then the Group ID is properly set.
