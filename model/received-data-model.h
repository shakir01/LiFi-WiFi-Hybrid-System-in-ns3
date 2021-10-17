#ifndef RECEIVED_DATA_MODEL_H
#define RECEIVED_DATA_MODEL_H

#include "ns3/object.h"
#include <string>
#include "ns3/address.h"
#include <iostream>
#include "ns3/log.h"
#include "ns3/packet.h"

namespace ns3 {

class ReceivedDataModel: public Object
{
private:
     Ptr<ns3::Packet> receivedPacket;
     double snr;
     double ber;
     double receivedPower;
     double timeReceived;
     bool   isCorrupted;

public:
    // Public methods
     static TypeId GetTypeId(void); // returns meta-information about PAMErrorModel class 

     ReceivedDataModel ();
     ~ReceivedDataModel ();
     void SetReceivedPacket (Ptr<ns3::Packet>);
     void SetSNR (double);
     void SetBER (double);
     void SetReceivedPower (double);
     void SetTimeReceived (double);
     void SetIsCorrupted (bool);

     Ptr<ns3::Packet> GetReceivedPacket ();
     bool GetSNR ();
     double GetBER ();
     double GetReceivedPower();
     double GetTimeReceived ();
     bool GetIsCorrupted ();
};

}//End namespace

#endif
