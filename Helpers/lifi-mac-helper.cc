#include "ns3/lifi-mac-helper.h"
#include "ns3/lifi-channel.h"
#include "ns3/channel.h"
#include "ns3/mobility-model.h"
#include "ns3/log.h"
#include "ns3/config.h"
#include "ns3/simulator.h"
#include "ns3/names.h"
#include "ns3/tdma-central-mac.h"

NS_LOG_COMPONENT_DEFINE ("ns3::LiFiMacHelper");

namespace ns3
{

LiFiMacHelper::LiFiMacHelper ()
{
}

LiFiMacHelper::~LiFiMacHelper ()
{
}

 Ptr<ns3::LiFiApMac> LiFiMacHelper::GetApMac (Ptr<Node> node)
{
  std::map <uint32_t, Ptr<ns3::LiFiApMac> >:: iterator it1;
  //std::map<std::string, std::vector<LiFiWiFiFlowStats> >;
  it1 = m_lifi_ap_netDevicesMap.find(node->GetId());
  if (it1 != m_lifi_ap_netDevicesMap.end())
  {
    	return it1->second;
  }

  return NULL;
}

 Ptr<ns3::LiFiStaMac> LiFiMacHelper::GetUserMac (Ptr<Node> node)
{
std::map <uint32_t, Ptr<ns3::LiFiStaMac> >:: iterator it1;
  //std::map<std::string, std::vector<LiFiWiFiFlowStats> >;
  it1 = m_lifi_sta_netDevicesMap.find(node->GetId());
  if (it1 != m_lifi_sta_netDevicesMap.end())
  {
    	return it1->second;
  }

  return NULL;
}


NetDeviceContainer LiFiMacHelper::CreateAPMac (Ptr<Node> node, ns3::Ptr<LiFiPhy> lifiPhy, Ptr<LiFiChannelHelper> channelHelper)
{
  NS_LOG_FUNCTION (this);
  Ptr<LiFiApMac> app_mac = CreateObject<LiFiApMac> ();
  NetDeviceContainer devices;
  NS_ASSERT (app_mac != 0);
  Ptr<LiFiChannel> channel = channelHelper->GetChannel(node->GetId());
  lifiPhy->SetChannel(channel);

  Ptr<TdmaCentralMac> mac = CreateObject<TdmaCentralMac> ();
  ns3::Ptr<LiFiNetDevice> device = CreateObject<LiFiNetDevice> ();
  mac->SetAddress (Mac48Address::Allocate ());
  lifiPhy->SetDevice(device);
  lifiPhy->SetMacLow(mac->GetTdmaMacLow());
  Ptr<TdmaMacLow> tdmaMacLow = mac->GetTdmaMacLow();
  tdmaMacLow->SetPhy(lifiPhy);
  device->SetMacHigh (app_mac);
  node->AddDevice (device);
  device->SetMac(mac);
  app_mac->SetMac(mac);
  mac->SetLiFiApMac(app_mac);
  m_lifi_ap_netDevicesMap.insert (std::pair<uint32_t,Ptr<LiFiApMac> >(node->GetId(), app_mac));
  return NetDeviceContainer(device);
}

NetDeviceContainer LiFiMacHelper::CreateUserDeviceMac (Ptr<Node> node, ns3::Ptr<LiFiPhy> lifiPhy)
{
  NS_LOG_FUNCTION (this);
  Ptr<LiFiStaMac> sta_mac = CreateObject<LiFiStaMac> ();
  NetDeviceContainer devices;
  ns3::Ptr<LiFiNetDevice> device = CreateObject<LiFiNetDevice> ();
  Ptr<TdmaCentralMac> mac = CreateObject<TdmaCentralMac> ();
  mac->SetAddress (Mac48Address::Allocate ());
  lifiPhy->SetMacLow(mac->GetTdmaMacLow());
 // device->SetAddress(mac->GetAddress());
  Ptr<TdmaMacLow> tdmaMacLow = mac->GetTdmaMacLow();
  tdmaMacLow->SetPhy(lifiPhy);
  device->SetMacHigh (sta_mac);
  lifiPhy->SetDevice(device);
  node->AddDevice (device);
  device->SetMac(mac);
  sta_mac->SetMac(mac);
  mac->SetLiFiApMac(sta_mac);

  m_lifi_sta_netDevicesMap.insert (std::pair<uint32_t,Ptr<LiFiStaMac> >(node->GetId(), sta_mac));
  return NetDeviceContainer(device);
}

}
