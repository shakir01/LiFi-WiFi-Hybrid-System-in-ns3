#ifndef LIFI_MAC_HELPER_H
#define LIFI_MAC_HELPER_H
#include <string>
#include <stdarg.h>
#include "ns3/attribute.h"
#include "ns3/lifi-ap-mac.h"
#include "ns3/lifi-mac-helper.h"
#include "ns3/lifi-sta-mac.h"
#include "ns3/object-factory.h"
#include "ns3/node-container.h"
#include "ns3/lifi-net-device.h"
#include "ns3/internet-module.h"
#include "ns3/object.h"
#include <iostream>
#include "ns3/commons.h"
#include <map>
#include "ns3/object-factory.h"
#include "ns3/lifi-phy-helper.h"
#include "ns3/remote-station-manager.h"

namespace ns3
{
class LiFiPhyHelper;
class LiFiChannelHelper;
class RemoteStationManager;
class LiFiMacHelper : public Object
{
 public:
    virtual ~LiFiMacHelper ();
    LiFiMacHelper ();
   // TypeId GetTypeId (void);
  Ptr<ns3::LiFiApMac> GetApMac (Ptr<Node> node);
    Ptr<ns3::LiFiStaMac> GetUserMac (Ptr<Node> node);


NetDeviceContainer CreateAPMac (Ptr<Node> node, ns3::Ptr<LiFiPhy> lifiPhy, Ptr<LiFiChannelHelper> channelHelper,Ptr<NetDevice> ccnetdevice);    
NetDeviceContainer CreateUserDeviceMac (Ptr<Node> node, ns3::Ptr<LiFiPhy> lifiPhy);
  protected:
    ObjectFactory m_mac; ///< MAC object factory
  private:
    std::map <uint32_t, Ptr<LiFiApMac> > m_lifi_ap_netDevicesMap;
    std::map <uint32_t, Ptr<LiFiStaMac> > m_lifi_sta_netDevicesMap;
    ns3::Ptr<RemoteStationManager> m_remoteStationManager;


};
}
#endif

