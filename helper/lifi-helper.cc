#include "ns3/lifi-helper.h"
#include "ns3/lifi-channel.h"
#include "ns3/channel.h"
#include "ns3/mobility-model.h"
#include "ns3/log.h"
#include "ns3/config.h"
#include "ns3/simulator.h"
#include "ns3/names.h"
#include "ns3/lifi-phy-helper.h"

NS_LOG_COMPONENT_DEFINE ("LiFiHelper");

namespace ns3 {
LiFiHelper::LiFiHelper ()
{
}

LiFiHelper::~LiFiHelper ()
{
}

bool LiFiHelper::HasThisNodeAttached(Ptr<Node> node)
{
  std::unordered_map<uint32_t, ns3::Ptr<Node> >::  iterator itr1;
  itr1 = ueNodesMap.find(node->GetId());
  if (itr1 != ueNodesMap.end())
  {
     // Ptr<Node> userDeviceDataModel = itr1->second;
      return true;
  }
return false;
}

void LiFiHelper::MarkNodeForHandoverNodes(Ptr<Node> node)
{
  m_controller->MarkNodeForHandoverNodes(node);
}

Ptr<LiFiApMac> LiFiHelper::GetAPMacController ()
{
   return m_controller;
}

NetDeviceContainer LiFiHelper::AssociateUserDevicesWithAP (NodeContainer c, ns3::Ptr<Node> appNode, Ptr<LiFiChannelHelper> channelHelper,Ptr<LiFiPhyHelper> phyHelper, Ptr<LiFiMacHelper> macHelper )
{
  NS_LOG_FUNCTION (this);
  std::vector<Ptr<LiFiUserDeviceInfos> > recvVector;
  m_controller = macHelper->GetApMac(appNode);
  NetDeviceContainer netDeviceContainer; 
  //NS_ASSERT (m_controller != 0);
  Ptr<LiFiNetDevice> appDevice = ns3::DynamicCast < LiFiNetDevice> (appNode->GetDevice(1));
  int ii=0;
  for (NodeContainer::Iterator i = c.Begin (); i != c.End (); ++i)
    {
      Ptr<Node> node = *i;
      Ptr<LiFiChannel> channel = channelHelper->GetChannel(appNode->GetId());
      Ptr<LiFiPhy> phy = phyHelper->GetPhy(node->GetId());
      channel->AddPhy(phy);
      ueNodesMap.insert(std::make_pair(node->GetId(), node));
      ns3::Ptr < LiFiNetDevice > device = ns3::DynamicCast < LiFiNetDevice> (node->GetDevice(0));
      netDeviceContainer.Add(device);
      Ptr<LiFiUserDeviceInfos> receiveModel = CreateObject <LiFiUserDeviceInfos> ();
      //Ptr<TdmaMac> mac = device->GetMac();
      //receiveModel->SetMac (mac);
      receiveModel->SetDeviceID (device->GetNode()->GetId());
      m_controller->AddUserDeviceInfos(receiveModel);
      recvVector.push_back(receiveModel);
      appDevice->AddReceiverModel(receiveModel);
      //std::cout<<"ns3::Ptr < LiFiRxNetDevice > "<<device<<" ns3::Ptr<LiFiTxNetDevice> "<<appDevice<<" recvVector "<<recvVector.size()<<"  mac addresses "<<device->GetAddress()<<" NodeContainer "<<c.GetN()<<" node->GetId () "<<node->GetId ()<<std::endl;
      ii++;
    }
  //appDevice->CompleteConfig ();
  return netDeviceContainer;
}

void LiFiHelper::ModifyLifiAPCache (ns3::NodeContainer nodeContainer)
{
  //for (std::list<ns3::NodeContainer>::const_iterator i = lifiNetworkNodesContainerList.begin(); i != lifiNetworkNodesContainerList.end(); ++i)
  //{
	 //ns3::NodeContainer nodeContainer = *i;
   //if (nodeContainer.GetN() == 1) continue;

	 Ptr <Node> node                        = nodeContainer.Get(0);
	 Ptr<Ipv4> ipv4A                        = node->GetObject<Ipv4> ();//node->GetDevice(2)
	 Ptr<LiFiTxNetDevice> txNetDev = ns3::DynamicCast <LiFiTxNetDevice> (node->GetDevice(1));

	 int32_t interfaceIndex1                 = ipv4A->GetInterfaceForDevice (txNetDev);
	 Ptr<Ipv4L3Protocol> nodeIpv4L3Protocol = node->GetObject<Ipv4L3Protocol> ();
	 Ptr<Ipv4Interface> ipv4Interface       = nodeIpv4L3Protocol->GetInterface (interfaceIndex1);
	 Ptr<ArpCache> lifiApArpCache           = ipv4Interface->GetArpCache ();
	 lifiApArpCache->Flush();

	 for (uint32_t j = 1; j < (nodeContainer.GetN()); j++)
	 {
    // std::cout<<"create ap device cache "<<nodeContainer.GetN()<<std::endl;
		 ns3::Ptr<Node> udNodes = nodeContainer.Get(j);
		 Ptr<LiFiRxNetDevice> rxNetDev = ns3::DynamicCast <LiFiRxNetDevice> (udNodes->GetDevice(0));

 		 Ptr<Ipv4> ud_ipv4A        = udNodes->GetObject<Ipv4> ();
	 	 int32_t interfaceIndex = ud_ipv4A->GetInterfaceForDevice (rxNetDev);
		 Ipv4InterfaceAddress 	interfaceAddress = ud_ipv4A->GetAddress (interfaceIndex,0);

		 ArpCache::Entry *  cacheEntry = lifiApArpCache->Add (interfaceAddress.GetLocal());
		 cacheEntry->SetMacAddress(rxNetDev->GetAddress());
     cacheEntry->MarkPermanent();
	 }
   std::cout<<"\n";
	 ipv4Interface->SetArpCache (lifiApArpCache);
 // }
}

void LiFiHelper::ConfigureUserDevicesCaches (ns3::NodeContainer nodeContainer, NodeContainer wifiapNodes,   Ipv4InterfaceContainer wifitocsmauserinterfaceContainer)
{
  //for (std::list<ns3::NodeContainer>::const_iterator i = lifiNetworkNodesContainerList.begin(); i != lifiNetworkNodesContainerList.end(); ++i)
  //{
	//ns3::NodeContainer        	 = *i;
  //if (nodeContainer.GetN() <= 1) continue;

	ns3::Ptr<Node>     apNode 	       	     = nodeContainer.Get(0);
	Ptr<LiFiTxNetDevice> aptxNetDev          = ns3::DynamicCast <LiFiTxNetDevice> (apNode->GetDevice(1));

	int32_t apNetDeviceInterfaceIndex        = apNode->GetObject<Ipv4> ()->GetInterfaceForDevice (aptxNetDev);
	Ptr<Ipv4L3Protocol> apnodeIpv4L3Protocol = apNode->GetObject<Ipv4L3Protocol> ();
	Ptr<Ipv4Interface> apipv4Interface       = apnodeIpv4L3Protocol->GetInterface (apNetDeviceInterfaceIndex);
//  std::cout<<"create user device cache "<<nodeContainer.GetN()<<std::endl;

	for (uint32_t j = 1; j <nodeContainer.GetN(); ++j)
	{
      Ptr <Node> node                        = nodeContainer.Get(j);
      Ptr<Ipv4> ipv4A                        = node->GetObject<Ipv4> ();
      Ptr<LiFiRxNetDevice> rxNetDev          = ns3::DynamicCast <LiFiRxNetDevice> (node->GetDevice(0));
      int32_t interfaceIndex                 = ipv4A->GetInterfaceForDevice (rxNetDev);
      Ptr<Ipv4L3Protocol> nodeIpv4L3Protocol = node->GetObject<Ipv4L3Protocol> ();
      Ptr<Ipv4Interface> ipv4Interface       = nodeIpv4L3Protocol->GetInterface (interfaceIndex);
      Ptr<ArpCache> userDeviceArpCache       = ipv4Interface->GetArpCache ();
      userDeviceArpCache->Flush();
      ArpCache::Entry *  cacheEntry          = userDeviceArpCache->Add (wifitocsmauserinterfaceContainer.GetAddress(0,0));
      Ptr<WifiNetDevice> wifiApNetDevice;
      for (uint32_t i = 0; i <(wifiapNodes.Get(0)->GetNDevices()); i++) {
          wifiApNetDevice =  ns3::DynamicCast < WifiNetDevice> (wifiapNodes.Get(0)->GetDevice(i));
          if (wifiApNetDevice != 0) break;
      }

      cacheEntry->SetMacAddress(wifiApNetDevice->GetAddress());
      Ptr<WifiApNetDevice> wifiStaNetDev      = ns3::DynamicCast <WifiApNetDevice> (node->GetDevice(1));
      wifiStaNetDev->SetGatewayMacAddress (wifiApNetDevice->GetAddress());
      wifiStaNetDev->SetPeerNetDevice (rxNetDev);
      ArpCache::Entry *  cacheEntry1          = userDeviceArpCache->Add (apipv4Interface->GetAddress(0).GetLocal());
      cacheEntry1->SetMacAddress (aptxNetDev->GetAddress());
      ipv4Interface->SetArpCache (userDeviceArpCache);
  }
    std::cout<<"\n";
}

void LiFiHelper::UpdateUserDeviceCache (Ptr<Node> node)
{
  std::cout<<"At time "<<Simulator::Now().GetSeconds()<<" void LiFiHelper::UpdateUserDeviceCache (Ptr<Node> node "<<node->GetId()<<std::endl;
  Ptr<Ipv4> ipv4A                        = node->GetObject<Ipv4> ();
  //Ptr<LiFiRxNetDevice> rxNetDev          = ns3::DynamicCast <LiFiRxNetDevice> (node->GetDevice(0));
  //int32_t interfaceIndex                 = ipv4A->GetInterfaceForDevice (rxNetDev);
  Ptr<Ipv4L3Protocol> nodeIpv4L3Protocol = node->GetObject<Ipv4L3Protocol> ();
  Ptr<Ipv4Interface> ipv4Interface       = nodeIpv4L3Protocol->GetInterface (1);
  Ptr<ArpCache> userDeviceArpCache       = ipv4Interface->GetArpCache ();
  userDeviceArpCache->Flush();
  Ptr<WifiApNetDevice> wifiApNetDevice          = ns3::DynamicCast <WifiApNetDevice> (node->GetDevice(1));
  wifiApNetDevice->SetPeerNetDevice(0);
  Ptr<Ipv4Interface> ipv4Interface1       = nodeIpv4L3Protocol->GetInterface (2);
  Ptr<ArpCache> wifiuserDeviceArpCache    = ipv4Interface1->GetArpCache ();
  wifiuserDeviceArpCache->Flush();
}
 
void LiFiHelper::UpdateAPCache (Ptr<LiFiTxNetDevice> apNetDevice)
{

}

void LiFiHelper::EnableLogComponents (void)
{
  LogComponentEnable ("LiFiHelper", LOG_LEVEL_ALL);
  LogComponentEnable ("TdmaControllerHelper", LOG_LEVEL_ALL);
  LogComponentEnable ("TdmaCentralMac", LOG_LEVEL_ALL);
  LogComponentEnable ("TdmaMacLow", LOG_LEVEL_ALL);
  LogComponentEnable ("TdmaController", LOG_LEVEL_ALL);
  LogComponentEnable ("TdmaMacQueue", LOG_LEVEL_ALL);
  LogComponentEnable ("LiFiNetDevice", LOG_LEVEL_ALL);
  LogComponentEnable ("SimpleWirelessChannel", LOG_LEVEL_ALL);
}

} // namespace ns3
