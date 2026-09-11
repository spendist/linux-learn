Question: Why my raspberry PI has 
My Raspberry Pi B+ is running Raspbian (Linux version 3.18.11+) updated, etc.
It's connected to the network by ethernet. I want a static IP, and have assigned it one in /etc/network/interfaces (shown below). However, every boot it requests an address via DHCP and is assigned one! Both the static and dynamic addresses work to access the machine on the network.
Where can I stop the DHCP address from being requested and assigned??

From the command prompt, type:
nano /etc/network/interfaces and press enter
