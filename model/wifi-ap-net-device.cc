#include "ns3/wifi-ap-net-device.h"
#include "ns3/wifi-mac.h"
#include "ns3/wifi-phy.h"
#include "ns3/wifi-remote-station-manager.h"
#include "ns3/channel.h"
#include "ns3/llc-snap-header.h"
#include "ns3/packet.h"
#include "ns3/uinteger.h"
#include "ns3/pointer.h"
#include "ns3/node.h"
#include "ns3/trace-source-accessor.h"
#include "ns3/log.h"
#include "ns3/lifi-rx-net-device.h"
#include <iostream>
#include "ns3/sta-wifi-mac.h"
namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("WifiApNetDevice");

NS_OBJECT_ENSURE_REGISTERED (WifiApNetDevice);

TypeId
WifiApNetDevice::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::WifiApNetDevice").SetParent<WifiNetDevice> ().SetGroupName ("Wifi").AddConstructor<WifiApNetDevice> () ;
  return tid;
}

WifiApNetDevice::WifiApNetDevice ()
{
  WifiNetDevice();
  NS_LOG_FUNCTION_NOARGS ();
}


void
WifiApNetDevice::AddLinkChangeCallback (Callback<void> callback)
{
  WifiNetDevice::AddLinkChangeCallback (callback);
}

bool
WifiApNetDevice::NeedsArp (void) const
{
  return true;
}

void
WifiApNetDevice::SetReceiveCallback (NetDevice::ReceiveCallback cb)
{
  WifiNetDevice::SetReceiveCallback (cb);
}


void
WifiApNetDevice::SetAddress (Address address)
{
  WifiNetDevice::GetMac()->SetAddress (Mac48Address::ConvertFrom (address));
}

Address
WifiApNetDevice::GetAddress (void) const
{
  return WifiNetDevice::GetMac()->GetAddress ();
}

void WifiApNetDevice::SetPeerNetDevice(Ptr<ns3::NetDevice> netdevice)
{
    m_peerNetDevice = netdevice;
    if(netdevice == 0) return;
    ns3::Ptr < LiFiRxNetDevice > rxNetDevice = ns3::DynamicCast < LiFiRxNetDevice> (m_peerNetDevice);
    rxNetDevice->SetForwardingPeerNetDeviceCallback (MakeCallback (&WifiApNetDevice::ForwardDown, this));
}

Ptr<ns3::NetDevice> WifiApNetDevice::GetPeerNetDevice ()
{
   return m_peerNetDevice;
}

void WifiApNetDevice::SetGatewayMacAddress(Address address)
{
  m_GatewayAddress = address;
}

Address WifiApNetDevice::GetGatewayMacAddress( )
{
  return m_GatewayAddress;
}

void WifiApNetDevice::ForwardDown (Ptr<Packet> packet, const Address& dest, uint16_t protocolNumber)
{
Send (packet,dest,protocolNumber);
}

/*WifiApNetDevice::~WifiApNetDevice ()
{

  NS_LOG_FUNCTION_NOARGS ();
}*/

bool
WifiApNetDevice::Send (Ptr<Packet> packet, const Address& dest, uint16_t protocolNumber)
{
//std::cout<<"WifiApNetDevice::Send (Ptr<Packet> packet, const Address& dest, uint16_t protocolNumber) protocolNumber "<<protocolNumber<<" dest "<<dest<<" GetGatewayMacAddress() "<<GetGatewayMacAddress()<<std::endl;
Ptr<ns3::StaWifiMac> staMac = DynamicCast<ns3::StaWifiMac> (WifiNetDevice::GetMac());

/*if (GetNode()->GetId() == 22 || GetNode()->GetId() == 23 || GetNode()->GetId() == 24 )
{
  Mac48Address realTo = Mac48Address::ConvertFrom (GetGatewayMacAddress());
  std::cout<<"WifiApNetDevice::Send (Ptr<Packet> packet, const Address& dest, uint16_t protocolNumber) staMac "<<staMac<<" to add address "<<realTo<<std::endl;
}*/

if (staMac && GetPeerNetDevice())
{
        LlcSnapHeader llc;
        llc.SetType (protocolNumber);
        packet->AddHeader (llc);
        Mac48Address realTo = Mac48Address::ConvertFrom (GetGatewayMacAddress());
        WifiNetDevice::GetMac()->NotifyTx (packet);
        WifiNetDevice::GetMac()->Enqueue (packet, realTo);
}
else {

        /*LlcSnapHeader llc;
        llc.SetType (protocolNumber);

        packet->AddHeader (llc);
        Mac48Address realTo = Mac48Address::ConvertFrom (dest);

        //GetMac()->NotifyTx (packet);
        //WifiNetDevice::GetMac()->Enqueue (packet, realTo);*/
      //  std::cout<<"WifiApNetDevice::Send (Ptr<Packet> packet, const Address& dest, uint16_t protocolNumber) "<<dest<<" protocol number "<<protocolNumber<<std::endl;

        WifiNetDevice::Send (packet, dest, protocolNumber);
       /* if (GetNode()->GetId() == 7)
        {
              Mac48Address realTo = Mac48Address::ConvertFrom (dest);
              std::cout<<"STA Mac WifiApNetDevice::Send (Ptr<Packet> packet, const Address& dest, uint16_t protocolNumber) "<<staMac<<"  protocolNumber "<<protocolNumber<<" realTo "<<realTo<<std::endl;
        }*/
}

  return true;
}

void WifiApNetDevice::CreateIpv4Header (Ptr<Packet> packet, Ipv4Header& header,  Ipv4Address source,  Ipv4Address destination)
{
      header.SetDestination (destination);
      header.SetProtocol (UdpL4Protocol::PROT_NUMBER);
      header.SetSource (source);
      header.EnableChecksum ();
      header.SetTtl (64);
      header.SetPayloadSize (packet->GetSize());
}

void
WifiApNetDevice::DoDispose (void)
{

  WifiNetDevice::DoDispose ();
}

void
WifiApNetDevice::DoInitialize (void)
{
  WifiNetDevice::DoInitialize ();
}
} //namespace ns3
