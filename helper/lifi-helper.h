#ifndef LIFI_HELPER_H
#define LIFI_HELPER_H

#include <string>
#include <stdarg.h>
#include "ns3/attribute.h"
#include "ns3/object-factory.h"
#include "ns3/node-container.h"
#include "ns3/lifi-net-device.h"
#include "ns3/lifi-tx-net-device.h"
#include "ns3/lifi-rx-net-device.h"
#include "ns3/trace-helper.h"
#include "ns3/internet-module.h"
#include <iostream>
#include "ns3/wifi-module.h"
#include "ns3/lifi-module.h"
#include "ns3/lifi-ap-mac.h"
#include "ns3/lifi-sta-mac.h"
#include "ns3/lifi-regular-mac.h"
#include "ns3/lifi-tx-mac.h"
#include "ns3/lifi-rx-mac.h"
#include <unordered_map> 

namespace ns3 {

class TdmaMac;
class LiFiNetDevice;
class Node;
class TdmaController;
class LiFiPhyHelper;

/**
 * \brief create MAC objects
 *
 * This base class must be implemented by new MAC implementation which wish to integrate
 * with the \ref ns3::TdmaHelper class.
 */
/**
 * \brief helps to create TdmaNetDevice objects and ensures creation of a centralized
 * TdmaController which takes care of the slot scheduling
 */
class LiFiHelper : public Object
{
public:
  LiFiHelper ();
  ~LiFiHelper ();
  static void EnableLogComponents (void);
  Ptr<LiFiChannel>  GetChannel () const;
  NetDeviceContainer AssociateUserDevicesWithAP (NodeContainer c, ns3::Ptr<Node> appNode, Ptr<LiFiChannelHelper> channelHelper,Ptr<LiFiPhyHelper> phyHelper, Ptr<LiFiMacHelper> macHelper );
  void ModifyLifiAPCache (ns3::NodeContainer nodeContainer);
  void ConfigureUserDevicesCaches (ns3::NodeContainer nodeContainer, NodeContainer wifiapNodes,   Ipv4InterfaceContainer wifitocsmauserinterfaceContainer);
  void MarkNodeForHandoverNodes(Ptr<Node> node);
  bool HasThisNodeAttached(Ptr<Node>);
  Ptr<LiFiApMac> GetAPMacController ();
  void UpdateUserDeviceCache (Ptr<Node> node);
  void UpdateAPCache (Ptr<LiFiTxNetDevice>);
private:
  std::string m_filename;
  std::map<std::string, ns3::Ptr<LiFiTxNetDevice> > m_TXDevices;
  std::map<std::string, ns3::Ptr<LiFiRxNetDevice> > m_RXDevices;
  std::unordered_map<uint32_t, ns3::Ptr<Node> >    ueNodesMap;
  Ptr<LiFiApMac> m_controller;
};

} // namespace ns3

#endif /* AERO_HELPER_H */
