#include "ns3/lifi-mac-helper.h"
#include "ns3/lifi-channel.h"
#include "ns3/channel.h"
#include "ns3/mobility-model.h"
#include "ns3/log.h"
#include "ns3/config.h"
#include "ns3/simulator.h"
#include "ns3/names.h"

NS_LOG_COMPONENT_DEFINE ("ns3::LiFiMacHelper");

namespace ns3 
{
/*NS_OBJECT_ENSURE_REGISTERED (LiFiMacHelper);
TypeId LiFiMacHelper::GetTypeId (void)
{
    static TypeId tid = TypeId ("ns3::LiFiMacHelper").SetParent<Object> ().
    AddConstructor<LiFiMacHelper> ();
    return tid;
}*/

LiFiMacHelper::LiFiMacHelper ()
{
  m_remoteStationManager = CreateObject <RemoteStationManager>();
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


/*NetDeviceContainer LiFiMacHelper::CreateAPMac (Ptr<Node> node, ns3::Ptr<LiFiPhy> lifiPhy, Ptr<LiFiChannelHelper> channelHelper)
{
    NS_LOG_FUNCTION (this);
    Ptr<LiFiApMac> app_mac = CreateObject<LiFiApMac> ();
    NetDeviceContainer devices;
    Ptr<LiFiChannel> channel = channelHelper->GetChannel(node->GetId());
    lifiPhy->SetChannel(channel);
    Ptr<TdmaCentralMac> mac = CreateObject<TdmaCentralMac> ();
    ns3::Ptr<LiFiNetDevice> device = CreateObject<LiFiNetDevice> ();
    mac->SetAddress (Mac48Address::Allocate ());
    lifiPhy->SetDevice(device);
    lifiPhy->SetMacLow(mac->GetTdmaMacLow());
    Ptr<LiFiMacLow> LiFiMacLow = mac->GetTdmaMacLow();
    LiFiMacLow->SetPhy(lifiPhy);
    device->SetMacHigh (app_mac);
    node->AddDevice (device);
    device->SetMac(mac);
    app_mac->SetMac(mac);
    //app_mac->SetDevice(device);
    app_mac->SetMacLow(LiFiMacLow);
    mac->SetLiFiApMac(app_mac);
    m_remoteStationManager->AddMac(mac,mac->GetAddress());
    app_mac->SetRemoteStationManager(m_remoteStationManager);
    m_lifi_ap_netDevicesMap.insert (std::pair<uint32_t,Ptr<LiFiApMac> >(node->GetId(), app_mac)); 
    return NetDeviceContainer(device);
}*/

NetDeviceContainer LiFiMacHelper::CreateAPMac (Ptr<Node> node, ns3::Ptr<LiFiPhy> lifiPhy, Ptr<LiFiChannelHelper> channelHelper)
{
    NS_LOG_FUNCTION (this);
    Ptr<LiFiApMac> app_mac = CreateObject<LiFiApMac> ();
    NetDeviceContainer devices;
    Ptr<LiFiChannel> channel = channelHelper->GetChannel(node->GetId());
    lifiPhy->SetChannel(channel);
    //Ptr<TdmaCentralMac> mac = CreateObject<TdmaCentralMac> ();
    ns3::Ptr<LiFiNetDevice> device = CreateObject<LiFiNetDevice> ();
    app_mac->SetAddress (Mac48Address::Allocate ());
    lifiPhy->SetDevice(device);
    Ptr<LiFiMacLow> LiFiMacLow = app_mac->GetMacLow();
    lifiPhy->SetMacLow(LiFiMacLow);
    LiFiMacLow->SetPhy(lifiPhy);
    //device->SetMacHigh (app_mac);
    node->AddDevice (device);
    device->SetMac(app_mac);
    //app_mac->SetMac(mac);
    //app_mac->SetDevice(device);
    //app_mac->SetMacLow(LiFiMacLow);
    //mac->SetLiFiApMac(app_mac);
    m_remoteStationManager->AddMac(app_mac,app_mac->GetAddress());
    app_mac->SetRemoteStationManager(m_remoteStationManager);
    m_lifi_ap_netDevicesMap.insert (std::pair<uint32_t,Ptr<LiFiApMac> >(node->GetId(), app_mac)); 
    return NetDeviceContainer(device);
}

NetDeviceContainer LiFiMacHelper::CreateUserDeviceMac (Ptr<Node> node, ns3::Ptr<LiFiPhy> lifiPhy)
{
        //std::cout<<"NetDeviceContainer LiFiMacHelper::CreateUserDeviceMac node "<<node->GetId()<<std::endl;

    NS_LOG_FUNCTION (this);
    Ptr<LiFiStaMac> sta_mac = CreateObject<LiFiStaMac> ();
    NetDeviceContainer devices;
    ns3::Ptr<LiFiNetDevice> device = CreateObject<LiFiNetDevice> ();
    //Ptr<TdmaCentralMac> mac = CreateObject<TdmaCentralMac> ();
    sta_mac->SetAddress (Mac48Address::Allocate ());
    Ptr<LiFiMacLow> LiFiMacLow = sta_mac->GetMacLow();
    lifiPhy->SetMacLow(LiFiMacLow);
  // device->SetAddress(mac->GetAddress());
    LiFiMacLow->SetPhy(lifiPhy);
    device->SetMacHigh (sta_mac);
    lifiPhy->SetDevice(device);
    node->AddDevice (device);
    device->SetMac(sta_mac);
    //sta_mac->SetMac(mac);
    sta_mac->SetDevice(device);
    //sta_mac->SetMacLow(LiFiMacLow);
    //mac->SetLiFiApMac(sta_mac);
    m_remoteStationManager->AddMac(sta_mac,sta_mac->GetAddress());
    m_lifi_sta_netDevicesMap.insert (std::pair<uint32_t,Ptr<LiFiStaMac> >(node->GetId(), sta_mac)); 
    return NetDeviceContainer(device);
}
/*NetDeviceContainer LiFiMacHelper::CreateUserDeviceMac (Ptr<Node> node, ns3::Ptr<LiFiPhy> lifiPhy)
{
    NS_LOG_FUNCTION (this);
    Ptr<LiFiStaMac> sta_mac = CreateObject<LiFiStaMac> ();
    NetDeviceContainer devices;
    ns3::Ptr<LiFiNetDevice> device = CreateObject<LiFiNetDevice> ();
    Ptr<TdmaCentralMac> mac = CreateObject<TdmaCentralMac> ();
    mac->SetAddress (Mac48Address::Allocate ());
    lifiPhy->SetMacLow(mac->GetTdmaMacLow());
  // device->SetAddress(mac->GetAddress());
    Ptr<LiFiMacLow> LiFiMacLow = mac->GetTdmaMacLow();
    LiFiMacLow->SetPhy(lifiPhy);
    device->SetMacHigh (sta_mac);
    lifiPhy->SetDevice(device);
    node->AddDevice (device);
    device->SetMac(mac);
    sta_mac->SetMac(mac);
    sta_mac->SetDevice(device);
    sta_mac->SetMacLow(LiFiMacLow);
    mac->SetLiFiApMac(sta_mac);
    m_remoteStationManager->AddMac(mac,mac->GetAddress());
    m_lifi_sta_netDevicesMap.insert (std::pair<uint32_t,Ptr<LiFiStaMac> >(node->GetId(), sta_mac)); 
    return NetDeviceContainer(device);
}*/

}

