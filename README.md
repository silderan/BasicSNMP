# BasicSNMP

This is a library for basic SNMPv1 and SNMPv2 protocol for a manager (not the agent).

Sadly, this won't include any MIB parsing.
Anyway, the advantatjes using a MIB are not many, because this library is not for a universal NMB program able to control many unrelated agents. This is for a program to control a snmp-agent.

The main goal for this project is to include this library in any simple program that needs to talk to specific agents, but you must know the OIDs and the type of the values to send/get.

All the protocol and encoding/decoding is written in standard c++11 in, but the API testing, communication and "UnitTests" classes/funcions are based on the fantastic Qt5 framework.

That means that if anyone likes to use this library into their non-qt project, can use all files except the ones starting with 'q'
Furthermore, you can include code directly. There is no need to search for precompiled libraries for specific compiler-platform.
Or, obviously, you can compile it easily with your prefered compiler, to create your suitable libraries.
