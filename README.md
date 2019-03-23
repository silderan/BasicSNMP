# BasicSNMP

This is a library for basic SNMPv1 and SNMPv2 protocol for a manager (not the agent).

Sadly, this won't include any MIB parsing.
Anyway, the advantatjes using a MIB are not many, because this library is not for a universal NMB program able to control many unrelated agents. This is for a program to control a snmp-agent.

The main goal for this project is to include this library in any simple program that needs to talk to specific agents, but you must know the OIDs and the type of the values to send/get.

All the protocol encoding/decoding is written in standard c++11.
But the GUI API testing, communication and "UnitTests" classes/funcions are based on the fantastic Qt5 framework.

That means that if anyone likes to use this library into their non-qt project, can use all files that are in the /src/lib/ folder. Anyone can include code directly. There is no need to search for precompiled libraries for specific compiler-platform.

Obviously, you can compile it easily with your prefered compiler to create your own suitable libraries.
For example, the project QBasicSNMPCommLibrary is a Qt5 project that creates a .a static library for include in any Qt5 project.

## General view
Regardless the way you like to use this library, the header you want to include in your code is lib/snmplib.h

The structure of the data retreived from the SNMP agent is stored in the class SNMP::Encoder like that:

    SNMP::ENCODER - version
                  - comunity_name
                  - request-id
                  - error-code
                  - error-object
                  - request-type
                  - varbindlist - varbind - asn1variable
                                          - OID
                                          - raw-data

This could be the more important information you must know in order to use this library.

Remember that the base library (the one in the /src/lib/ folder) does not include any network feature. You must code your own or use the QBasicSNMPCommLibrary. But this needs the Qt framework to compile.

## Examples.
### Simplest code
The simplest code I can imagine is to encode a GetRequest or GetNextRequest datagram.
```
#include "lib/snmplib.h"

void main()
{
  SNMP::Encoder snmp;
  snmp.setupGetRequest(1, "public", 2);
  // or snmp.setupGetNextRequest(1, "public", 2);
  StdByteVector bytes = snmp.encodeRequest();
}
```

Next step is to send it throw any UDP socket to the agent and wait for the reply.
Then you can do something like that:
```
void onDataReceived(const StdByteVector bytes)
{
  SNMP::Encoder snmp;
  if( !snmp.decodeAll(bytes, true) )    // second parameter 'true' to keep raw data. Usefull for debugging.
    std::cerr << "Error " << snmp.errorCode() << " in object " << snmp.errorObjectIndex() << std::endl;
  else
  {
    for( const auto &asn1Varbind : snmp.varbindList() )
    {
      switch( asn1Varbind.type() )
      {
      case ASN1TYPE_NULL:
        std::cout << "<null" << std::endl;
        break;
      case ASN1TYPE_OCTETSTRING:
      case ASN1TYPE_IA5String:
      case ASN1TYPE_VideoString:
  //		case ASN1TYPE_xxxxString:
        // Beware. Never call octetString() to use data as a string because it doesn't includes the last /0 byte.
        std::cout << "OctetString " <<  asn1Varbind.toStdString() << std::endl;
        break;
      case ASN1TYPE_INTEGER:
      case ASN1TYPE_Integer64:
      case ASN1TYPE_Unsigned64:
      case ASN1TYPE_Counter64:
      case ASN1TYPE_Gauge:
  //		case ASN1TYPE_numbers:
        std::cout << "Number " << asn1Varbind.toUnsigned64() << std::endl;
        break;
      default:
        std::cout << "Unkown type: " << Utils::printableByte(asn1Varbind.type())
              << "Raw data: " << Utils::printableBytes(asn1Varbind.rawValue()) << std::endl;
      }
    }
  }
}
```
Well, that's enough for now. Later I'll continue.
