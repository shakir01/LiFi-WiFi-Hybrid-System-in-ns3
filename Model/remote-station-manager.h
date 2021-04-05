#ifndef REMOTE_STATION_MANAGER_H
#define REMOTE_STATION_MANAGER_H

#include <stdint.h>
#include "ns3/traced-callback.h"
#include "ns3/packet.h"
#include "ns3/ptr.h"
#include "ns3/mac48-address.h"
#include "ns3/ssid.h"
#include "ns3/node.h"
#include "tdma-controller.h"
#include "lifi-user-device-infos.h"
#include "ns3/tdma-mac.h"
#include "ns3/lifi-regular-mac.h"

namespace ns3 
{
class LiFiChannel;
class LiFiNetDevice;
class TdmaController;
class ReceiverModel;
class TdmaMac;
class LiFiRegularMac;

class RemoteStationManager: public Object
{
    public:
        static TypeId GetTypeId (void);
    void AddMac(Ptr<LiFiRegularMac> mac, Mac48Address macAddress);
        ns3::Ptr<LiFiRegularMac> GetMac(Mac48Address macAddress);
    private:
        std::vector <TdmaMac> macsVector;
        std::map<Mac48Address, Ptr<LiFiRegularMac> > regularMacMap;
};

}
#endif
