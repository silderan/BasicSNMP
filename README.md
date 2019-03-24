# BasicSNMP

This is a library for basic SNMPv1 and SNMPv2c protocol for a manager (not the agent).

Sadly, this won't include any MIB parsing.
Anyway, the advantages using a MIB are not many in a library like this one, because this library is not for a universal NMS program able to control many unrelated agents. This is for a program to control a specific snmp-agent.

The main goal for this project is to include this library in any simple program that needs to talk to specific agents, but you must know the OIDs and the type of the values to send/get.

All the protocol encoding/decoding is written in standard c++11.
But the GUI API testing, communication and "UnitTests" classes/funcions are based on the fantastic Qt5 framework.
If you look at the examples below you'll see some non-standard classes like StdString or StdByteVector. All this are subclass of the standards std::xxx basic classes. So, you can static_cast<>() to their base clases without any problem.
The only exceptions are the ones like StdxxxxList that could be StdList or StdDeque. Maybe I'll change it in future revisions.

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
### Simplest code: GetRequest and GetNextRequest
The simplest code I can imagine is to encode a GetRequest or GetNextRequest datagram.
```
#include "lib/snmplib.h"

// void Encoder::setupGetRequest(int version, const StdString &comunity, int requestID, const OID &oid)
void main()
{
  SNMP::Encoder snmp;
  snmp.setupGetNextRequest( 1, "public", 2, SNMP::OID("1.3.1.1.") );
  // or snmp.setupGetRequest( 1, "public", 2, SNMP::OID("....") ); if you know the exact OID to request.
  StdByteVector bytes = snmp.encodeRequest();
}
```

### Simpe code: decode data
Next step is to send ```bytes``` through any UDP socket to the agent and wait for the reply.
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
        std::cout << "<null>" << std::endl;
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
        std::cout << "Unkown type: " << SNMP::Utils::printableByte(asn1Varbind.type())
              << "Raw data: " << SNMP::Utils::printableBytes(asn1Varbind.rawValue()) << std::endl;
      }
    }
  }
}
```
As you can see, the only "verbose" code is the one you'll need to print the variable data.
Usually, you just need to check if incoming data is what you need and use it.
And, also usually and the easiest is that you ask for one data at time because of the limit at 1500bytes per UDP datagram.
```
void onDataReceived(const StdByteVector bytes)
{
  SNMP::Encoder snmp;
  if( !snmp.decodeAll(bytes, true) )    // second parameter 'true' to keep raw data. Usefull for debugging.
    std::cerr << "Error " << snmp.errorCode() << " in object " << snmp.errorObjectIndex() << std::endl;
  else
  {
    if( snmp.varbindList().first().type() == ASN1TYPE_OCTETSTRING )
        std::cout << snmp.varbindList().first()->toStdString() << std::endl;    // Remember to use toStdString and no toOctetString.
  }
}
```

### Simple code: SetRequest
Setting request is also simple.
There are two convenient funcions to encode SetRequest:
```
void Encoder::setupSetRequest(int version, const StdString &comunity, int requestID, const PDUVarbindList &varbindList);
void Encoder::setupSetRequest(int version, const StdString &comunity, int requestID, const OID &oid, const ASN1Variable &asn1Var);
```
The second one is just an overload of the first one for the most use of this feature.
See the code:
```
StdByteVector setRemoteOctetString( const SNMP::OID &oid, const StdString &str)
{
    SNMP::Encoder snmp;
    // 1 is the Version
    // "private" is the comunity. Usually for setting values must be private and not public.
    // 2 is the requestID. Will be reported back to identify the request.
    // oid... must is the EXACT oid
    // str is the name to set.
    snmp.setupSetRequest(1, "private", 2, oid, str);
    return snmp.encodeRequest();
}
void main()
{
    StdByteVector bytes = setRemoteOctetString( SNMP::OID("1.3.1.1.1.1.1", "HelloSNMP" );
    // now, send bytes to the UDP socket.
}
```
You'll wait for the agent responce to the SetRequest.
In my experience, agents reports an error or the data they set remotely. Meaning, reports back the same as you send exept the requestType (requestType is the "command" part of the request, we'll see in next section.

Will continue...
