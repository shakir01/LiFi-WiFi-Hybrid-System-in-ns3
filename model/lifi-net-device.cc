
#include "ns3/lifi-net-device.h"
#include "ns3/llc-snap-header.h"
#include "ns3/packet.h"
#include "ns3/uinteger.h"
#include "ns3/pointer.h"
#include "ns3/node.h"
#include "ns3/trace-source-accessor.h"
#include "ns3/log.h"
#include <iostream>
#include "lifi-regular-mac.h"

NS_LOG_COMPONENT_DEFINE ("LiFiNetDevice");

namespace ns3 {
// This value conforms to the 802.11 specification
  static const uint16_t MAX_MSDU_SIZE = 2304;

NS_OBJECT_ENSURE_REGISTERED (LiFiNetDevice);

TypeId
LiFiNetDevice::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::LiFiNetDevice")
    .SetParent<SimpleNetDevice> ()
    .AddConstructor<LiFiNetDevice> ()
    .AddAttribute ("Mtu", "The MAC-level Maximum Transmission Unit",
                   UintegerValue (MAX_MSDU_SIZE - LLC_SNAP_HEADER_LENGTH),
                   MakeUintegerAccessor (&LiFiNetDevice::SetMtu,
                                         &LiFiNetDevice::GetMtu),
                   MakeUintegerChecker<uint16_t> (1,MAX_MSDU_SIZE - LLC_SNAP_HEADER_LENGTH)).AddAttribute ("Channel", "The channel attached to this device",
                   PointerValue (),
                   MakePointerAccessor (&LiFiNetDevice::DoGetChannel,
                                        &LiFiNetDevice::SetChannel),
                   MakePointerChecker<LiFiChannel> ());
  return tid;
}

LiFiNetDevice::LiFiNetDevice ()
  : m_ifIndex (0),
		m_linkUp (false),
		m_mtu (0)
{
  NS_LOG_FUNCTION_NOARGS ();
m_configComplete = false;
        m_mobilityModel = ns3::CreateObject<LiFiMobilityModel>();
	m_mobilityModel->SetAzimuth(0);
	m_mobilityModel->SetElevation(0);
	m_mobilityModel->SetPosition(ns3::Vector(0.0, 0.0, 0.0));
//  LogComponentEnable ("TdmaNetDevice", LOG_LEVEL_DEBUG);
}

LiFiNetDevice::~LiFiNetDevice ()
{
  NS_LOG_FUNCTION_NOARGS ();
}

void LiFiNetDevice::CompleteConfig (void)
{
  NS_LOG_FUNCTION_NOARGS ();
  //  if (m_mac == 0 || m_node == 0 || m_channel == 0 || m_tdmaController == 0 || m_configComplete == true)
  if (m_mac == 0 || m_node == 0 || m_configComplete == true)
    {
      std::cout<<"void LiFiNetDevice::CompleteConfig (void) not completed "<<std::endl;
      return;
    }

  //setup mac
  //m_mac->SetTdmaController (m_tdmaController);
  m_mac->SetChannel (m_channel);
  // setup callbacks
  /*m_mac->SetLinkUpCallback (MakeCallback (&LiFiNetDevice::LinkUp, this));
  m_mac->SetLinkDownCallback (MakeCallback (&LiFiNetDevice::LinkDown, this));
  m_mac->SetTxQueueStartCallback (MakeCallback (&LiFiNetDevice::TxQueueStart, this));
  m_mac->SetTxQueueStopCallback (MakeCallback (&LiFiNetDevice::TxQueueStop, this));*/
  m_mac->SetForwardUpCallback (MakeCallback (&LiFiNetDevice::Receive, this));
  m_configComplete = true;
}

void
LiFiNetDevice::DoDispose (void)
{
  m_node = 0;
  m_mac->Dispose ();
  m_mac = 0;
  m_channel = 0;
   //chain up.
  SimpleNetDevice::DoDispose ();
}

/*void
LiFiNetDevice::Receive (Ptr<Packet> packet)
{
  return;
}*/

void LiFiNetDevice::SetMacHigh (Ptr<LiFiRegularMac> regularMac)
{
 
}

void LiFiNetDevice::SetTrafficType(TrafficType trafficType)
{
  m_trafficType = trafficType;    
}

TrafficType LiFiNetDevice::GetTrafficType()
{
  return m_trafficType;
}

void
LiFiNetDevice::DoInitialize (void)
{
//std::cout<<"LiFiNetDevice::DoInitialize (void) m_mac "<<m_mac<<std::endl;
  m_mac->Initialize ();
  SimpleNetDevice::DoInitialize ();
}

Ptr<Channel> LiFiNetDevice::GetChannel (void) const
{
  return m_channel;
}

Ptr<LiFiChannel> LiFiNetDevice::DoGetChannel (void) const
{
  return m_channel;
}

void LiFiNetDevice::SetDeviceName (std::string name) {
        this->deviceName = name;
}

Ptr<Node>
LiFiNetDevice::GetNode (void) const
{
  return m_node;
}

void
LiFiNetDevice::SetNode (Ptr<Node> node)
{
  m_node = node;
}

void LiFiNetDevice::SetChannel (Ptr<LiFiChannel> channel)
{
  if (channel != 0)
    {
      m_channel = channel;
      CompleteConfig ();
    }
}


void
LiFiNetDevice::SetIfIndex (const uint32_t index)
{

  m_ifIndex = index;
}

uint32_t
LiFiNetDevice::GetIfIndex (void) const
{
  return m_ifIndex;
}

void LiFiNetDevice::SetAddress (Address address)
{
  //m_mac->SetAddress (Mac48Address::ConvertFrom (address));
}

Address LiFiNetDevice::GetAddress (void) const
{
  return m_mac->GetAddress ();
}

bool LiFiNetDevice::SetMtu (const uint16_t mtu)
{
  if (mtu > MAX_MSDU_SIZE - LLC_SNAP_HEADER_LENGTH)
    {
      return false;
    }
  m_mtu = mtu;
  return true;
}

uint16_t LiFiNetDevice::GetMtu (void) const
{
  return m_mtu;
}

bool LiFiNetDevice::IsLinkUp (void) const
{
  return m_linkUp;
}
void
LiFiNetDevice::SetQueueStateChangeCallback (Callback<void,uint32_t> callback)
{
  m_queueStateChanges.ConnectWithoutContext (callback);
}

bool
LiFiNetDevice::TxQueueStart (uint32_t index)
{
  m_queueStateChanges (index);
  return true;
}

bool
LiFiNetDevice::TxQueueStop (uint32_t index)
{
  m_queueStateChanges (index);
  return true;
}

uint32_t
LiFiNetDevice::GetQueueState (uint32_t index)
{
  return m_mac->GetQueueState (index);
}

uint32_t
LiFiNetDevice::GetNQueues (void)
{
  return m_mac->GetNQueues ();
}

void LiFiNetDevice::SetMac (Ptr<LiFiRegularMac> mac)
{
  m_mac = mac;
  //std::cout<<"void LiFiNetDevice::SetMac (Ptr<LiFiRegularMac> mac) "<<mac<<std::endl;
  //m_mac->SetDevice (this);
  CompleteConfig ();
}

Ptr<LiFiRegularMac> LiFiNetDevice::GetMac (void) const
{
  //std::cout<<"Ptr<LiFiRegularMac> LiFiNetDevice::GetMac (void) const "<<m_mac<<std::endl;
  return m_mac;
}

void LiFiNetDevice::AddLinkChangeCallback (Callback<void> callback)
{
  m_linkChanges.ConnectWithoutContext (callback);
}

bool LiFiNetDevice::IsBroadcast (void) const
{
  return true;
}

Address
LiFiNetDevice::GetBroadcast (void) const
{
  return Mac48Address::GetBroadcast ();
}

bool
LiFiNetDevice::IsMulticast (void) const
{
  return true;
}

Address
LiFiNetDevice::GetMulticast (Ipv4Address multicastGroup) const
{
  return Mac48Address::GetMulticast (multicastGroup);
}

Address LiFiNetDevice::GetMulticast (Ipv6Address addr) const
{
  return Mac48Address::GetMulticast (addr);
}

bool LiFiNetDevice::IsPointToPoint (void) const
{
  return false;
}

bool LiFiNetDevice::IsBridge (void) const
{
  return false;
}

//Marked for Tx
bool
LiFiNetDevice::Send (Ptr<Packet> packet, const Address& dest, uint16_t protocolNumber)
{
  NS_LOG_FUNCTION (packet << " Dest:" << dest << " ProtocolNo:" << protocolNumber);
  NS_ASSERT (Mac48Address::IsMatchingType (dest));
//Mac48Address realFrom = Mac48Address::ConvertFrom (GetAddress ());
  //if (m_node->GetId() != 2)
    //std::cout<<"LiFiNetDevice::Send (Ptr<Packet> packet, const Address& dest, uint16_t protocolNumber)  realFrom "<<realFrom<<" dest "<<dest<<std::endl;
  
  Mac48Address realTo = Mac48Address::ConvertFrom (dest);
  
  LlcSnapHeader llc;
  llc.SetType (protocolNumber);
  packet->AddHeader (llc);
  //std::cout<<"bool LiFiTxNetDevice::Send (Ptr<Packet> packet, const Address& dest, uint16_t protocolNumber) receiverModel "<<std::endl;
  //if (m_node->GetId() != 8)  
  //std::cout<<"bool LiFiTxNetDevice::Send (Ptr<Packet> packet, const Address& dest, uint16_t protocolNumber) receiverModel "<<std::endl;
  m_mac->Enqueue (packet, realTo);
  //std::cout<<"m_mac m_mac m_mac m_mac "<<m_mac<<std::endl;
  return true;
}

void LiFiNetDevice::SetQoSCategory (uint8_t ac)
{
	m_QoS_category  = ac;
}

uint8_t LiFiNetDevice::GetQoSCategory (void)
{
	return m_QoS_category;
}

Ptr<LiFiUserDeviceInfos> LiFiNetDevice::SearchForReceiverModel (Address address)
{
      return 0;
}

void LiFiNetDevice::AddReceiverModel (Ptr<LiFiUserDeviceInfos> recieverModel)
{
  m_ReceiverModelList.push_back(recieverModel);
}

void LiFiNetDevice::RemoveRxDevice (uint32_t index)
{
 uint32_t itemIndex=-1;
  for (uint32_t i = 0; i < m_ReceiverModelList.size(); i++)
  {
    if (m_ReceiverModelList[i]->GetDeviceID() == index)
    {
      itemIndex = i;
      break;
    }
  }

  if (itemIndex >= 0)
  {
    m_ReceiverModelList.erase(m_ReceiverModelList.begin() + itemIndex);
  }
}

//Marked for Tx
bool
LiFiNetDevice::SendFrom (Ptr<Packet> packet, const Address& source, const Address& dest, uint16_t protocolNumber)
{
  NS_LOG_FUNCTION_NOARGS ();
  NS_ASSERT (Mac48Address::IsMatchingType (dest));
  NS_ASSERT (Mac48Address::IsMatchingType (source));
  Mac48Address realTo = Mac48Address::ConvertFrom (dest);
  Mac48Address realFrom = Mac48Address::ConvertFrom (source);
  LlcSnapHeader llc;
  llc.SetType (protocolNumber);
  packet->AddHeader (llc);
//  Ptr<ReceiverModel> receiverModel = SearchForReceiverModel (dest);
  m_mac->Enqueue (packet, realTo,realFrom);
  return true;
}

void
LiFiNetDevice::Receive (Ptr<Packet> packet, uint16_t protocol, Mac48Address to, Mac48Address from)
{
  //if (m_node->GetId() == 8)
  //if (m_node->GetId() == 2)
    //  std::cout<<"LiFiNetDevice::Receive (Ptr<Packet> packet, uint16_t protocol, Mac48Address to, Mac48Address from) protocol "<<protocol<<std::endl;
  NS_LOG_FUNCTION (this << packet << protocol << to << from);
  NetDevice::PacketType packetType;
  if (to == GetAddress ())
    {

      packetType = NetDevice::PACKET_HOST;

    }
  else if (to.IsBroadcast ())
    {
      packetType = NetDevice::PACKET_BROADCAST;
    }
  else if (to.IsGroup ())
    {
      packetType = NetDevice::PACKET_MULTICAST;
    }
  else
    {
      packetType = NetDevice::PACKET_OTHERHOST;

    }

  if (packetType != NetDevice::PACKET_OTHERHOST)
    {
     // std::cout<<"LiFiNetDevice::Receive (Ptr<Packet> packet, uint16_t protocol, Mac48Address to, Mac48Address from) "<<m_node->GetId()<<" protocol "<<protocol<<std::endl;
      LiFiNetDevice::GetMac()->NotifyPromiscRx (packet);
      m_forwardUp (this, packet, protocol, from);
    }

  if (!m_promiscRx.IsNull ())
    {
      m_promiscRx (this, packet, protocol, from, to, packetType);
    }
}

void LiFiNetDevice::ForwardUp (Ptr<Packet> packet)
{
  NS_LOG_DEBUG (*packet);
  
  WifiMacHeader hdr;
  packet->RemoveHeader (hdr);
 
 if (hdr.IsData () || hdr.IsMgt ())
    {
     // NS_LOG_DEBUG ("rx group from=" << hdr.GetAddr2 ());
   
      LlcSnapHeader llc;
      packet->RemoveHeader (llc);
    
     // std::cout<<"LiFiNetDevice::ForwardUp (Ptr<Packet> packet) hdr.GetAddr1 () "<<hdr.GetAddr1 ()<<" hdr.GetAddr3 () "<<hdr.GetAddr3 ()<<" llc.GetType () "<<llc.GetType ()<<std::endl;
      Receive (packet,llc.GetType (), hdr.GetAddr1 (), hdr.GetAddr3 ());
     // m_rxCallback (packet, &hdr);
    }
  else
    {
      NS_LOG_DEBUG ("Need to drop frame as it is not either data or management frame");
    }

  return;
}

bool LiFiNetDevice::NeedsArp (void) const
{
  return true;
}

void LiFiNetDevice::SetReceiveCallback (SimpleNetDevice::ReceiveCallback cb)
{
  m_forwardUp = cb;
}

void
LiFiNetDevice::SetPromiscReceiveCallback (PromiscReceiveCallback cb)
{
  m_promiscRx = cb;
}

void LiFiNetDevice::LinkUp (void)
{
  m_linkUp = true;
  m_linkChanges ();
}

void
LiFiNetDevice::LinkDown (void)
{
  m_linkUp = false;
  m_linkChanges ();
}

bool
LiFiNetDevice::SupportsSendFrom (void) const
{
  return false;
}

double LiFiNetDevice::GetAzmuth()
{
  return this->m_mobilityModel->GetAzimuth();
}

void LiFiNetDevice::SetAzmuth(double az)
{
  this->m_mobilityModel->SetAzimuth(az);
}

ns3::Vector LiFiNetDevice::GetPosition()
{
  return this->m_mobilityModel->GetPosition();
}

void LiFiNetDevice::SetPosition(ns3::Vector position)
{
  m_mobilityModel->SetPosition(position);
}

double LiFiNetDevice::GetElevation()
{
  return m_mobilityModel->GetElevation();
}

void LiFiNetDevice::SetElevation(double elevation)
{
  m_mobilityModel->SetElevation(elevation);
}

ns3::Ptr<LiFiMobilityModel> LiFiNetDevice::GetMobilityModel() {
	return m_mobilityModel;
}
void LiFiNetDevice::SetMobilityModel(ns3::Ptr<LiFiMobilityModel> model) {
	m_mobilityModel = model;
}

} // namespace ns3
