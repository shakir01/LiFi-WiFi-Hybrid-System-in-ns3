#ifndef REMOTE_STATION_MANAGER_H
#define REMOTE_STATION_MANAGER_H

#include <stdint.h>
#include "ns3/traced-callback.h"
#include "ns3/packet.h"
#include "ns3/ptr.h"
#include "ns3/mac48-address.h"
#include "ns3/ssid.h"
#include "ns3/node.h"
#include "lifi-user-device-infos.h"
#include "ns3/lifi-regular-mac.h"
#include "ns3/lifi-tx-mac.h"
#include "ns3/lifi-rx-mac.h"

namespace ns3 
{
class LiFiChannel;
class LiFiNetDevice;
class ReceiverModel;
class LiFiRegularMac;

class RemoteStationManager: public Object
{
    public:
        static TypeId GetTypeId (void);
        void AddStaMac(Ptr<LiFiRegularMac> mac, Mac48Address macAddress);
        ns3::Ptr<LiFiRegularMac> GetStaMac(Mac48Address macAddress);
        void AddMac(ns3::Ptr<LiFiRegularMac> mac, Mac48Address macAddress);
        ns3::Ptr<LiFiRegularMac> GetMac(Mac48Address macAddress);
        
    private:
        std::map<Mac48Address, Ptr<LiFiRegularMac> > regularMacMap;
        std::map<Mac48Address, ns3::Ptr<LiFiRegularMac> > tdmaMacMap;

};

}
#endif
