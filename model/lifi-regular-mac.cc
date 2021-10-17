#include "ns3/assert.h"
#include "ns3/enum.h"
#include "ns3/log.h"
#include "ns3/simulator.h"
#include "lifi-regular-mac.h"
#include "lifi-mac-low.h"
#include "ns3/abort.h"

NS_LOG_COMPONENT_DEFINE ("LiFiRegularMac");

namespace ns3 {
NS_OBJECT_ENSURE_REGISTERED (LiFiRegularMac);

TypeId
LiFiRegularMac::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3:LiFiRegularMac")
    .SetParent<Object> ()
    .AddConstructor<LiFiRegularMac> ();
    return tid;
}

 LiFiRegularMac::LiFiRegularMac ()
 {
    m_rxMac = CreateObject<LiFiRxMac> ();
    m_txMac = CreateObject<LiFiTxMac> ();
    m_rxMac->SetForwardUpCallback (MakeCallback (&LiFiRegularMac::Receive, this));
    m_low = CreateObject<LiFiMacLow> ();
    m_low->SetRxCallback (MakeCallback (&LiFiRxMac::Receive, m_rxMac));
    m_low->SetMac(m_rxMac);
    m_txMac->SetMacLow (m_low);
    //m_txMac->SetTxOkCallback (MakeCallback (&LiFiRegularMac::TxOk, this));
    //m_txMac->SetTxFailedCallback (MakeCallback (&LiFiRegularMac::TxFailed, this));
    //m_txMac->SetTxDroppedCallback (MakeCallback (&LiFiRegularMac::NotifyTxDrop, this));
}
 
LiFiRegularMac::~LiFiRegularMac ()
{

}

ns3::Ptr<LiFiTxMac> LiFiRegularMac::GetLiFiTxMac()
{
  return m_txMac;
}

void LiFiRegularMac::Initialize()
{

}

void
LiFiRegularMac::TxOk (const WifiMacHeader &hdr)
{
  NS_LOG_FUNCTION (this << hdr);
}

void LiFiRegularMac::StartTransmission (uint64_t transmissionTime,bool isControlFrame)
{

}

void LiFiRegularMac::StopTransmission (bool)
{

}

uint32_t LiFiRegularMac::GetQueueState (uint32_t index)
{
  return 0;
}

uint32_t LiFiRegularMac::GetNQueues (void)
{
  return 0;
}

void LiFiRegularMac::TxFailed (const WifiMacHeader &hdr)
{
  NS_LOG_FUNCTION (this << hdr);
}

void LiFiRegularMac::NotifyTx (Ptr<const Packet> packet)
{

}

void LiFiRegularMac::NotifyTxDrop (Ptr<const Packet> packet)
{
}

void LiFiRegularMac::NotifyRxDrop (Ptr<const Packet> packet) 
{

}

void LiFiRegularMac::NotifyPromiscRx (Ptr<const Packet> packet)
{

}

void LiFiRegularMac::NotifyRx (Ptr<const Packet> packet)
{

}

void LiFiRegularMac::Start (void)
{
  
}

  void LiFiRegularMac::SetTxQueueStartCallback (Callback<bool,uint32_t> queueStart) 
  {

  }

  void LiFiRegularMac::SetTxQueueStopCallback (Callback<bool,uint32_t> queueStop) 
  {

  }

 void LiFiRegularMac::SetChannel (Ptr<LiFiChannel> c)
 {

 }

Ptr<LiFiChannel> LiFiRegularMac::GetChannel (void)
{
  return 0;
}

void LiFiRegularMac::Receive (Ptr<Packet> packet,WifiMacHeader *hdr)
{

}

void LiFiRegularMac::SetNode(Ptr<Node> node)
{
  m_node = node;
}

Ptr<Node> LiFiRegularMac::GetNode(void)
{
  return m_node;
}

Mac48Address LiFiRegularMac::GetAddress (void) const
{
  return m_address;
}

void LiFiRegularMac::SetSsid (Ssid ssid)
{

}

Ssid LiFiRegularMac::GetSsid (void) const
{
  return m_ssid;
}

void LiFiRegularMac::SetAddress (Mac48Address address)
{
  m_address = address;
  m_low->SetAddress(m_address);
}

void LiFiRegularMac::Enqueue (Ptr<Packet> packet, Mac48Address to, Mac48Address from)
{

}

void LiFiRegularMac::Enqueue (Ptr<Packet> packet, Mac48Address to)
{
}

bool LiFiRegularMac::SupportsSendFrom (void) const
{
  return false;
}

/*void LiFiRegularMac::SetDevice (Ptr<LiFiNetDevice> device)
{
  m_device = device;
}*/

void
LiFiRegularMac::SetForwardUpCallback (Callback<void,Ptr<Packet>,uint16_t,Mac48Address, Mac48Address>  upCallback)
{
  NS_LOG_FUNCTION (this);
  m_forwardUp = upCallback;
}

void
LiFiRegularMac::SetLinkUpCallback (Callback<void> linkUp)
{
  NS_LOG_FUNCTION (this);
  m_linkUp = linkUp;
}

void
LiFiRegularMac::SetLinkDownCallback (Callback<void> linkDown)
{
  NS_LOG_FUNCTION (this);
  m_linkDown = linkDown;
}

void LiFiRegularMac::DoInitialize ()
{

}

void LiFiRegularMac::DoDispose ()
{

}

void
LiFiRegularMac::SetQosSupported (bool enable)
{
  NS_LOG_FUNCTION (this << enable);
  m_qosSupported = enable;
}

bool
LiFiRegularMac::GetQosSupported () const
{
  return m_qosSupported;
}

void LiFiRegularMac::SetMacLow(ns3::Ptr<ns3::LiFiMacLow> low)
{
  m_low = low;
}

ns3::Ptr<LiFiMacLow> LiFiRegularMac::GetMacLow()
{
  return m_low;
}

Time LiFiRegularMac::CalculateTxTime (Ptr<const Packet> packet)
{
  NS_LOG_FUNCTION (packet);
//std::cout<<"packet->GetSize () "<<packet->GetSize ()<<std::endl;
  NS_ASSERT_MSG (packet->GetSize () < 1500,"PacketSize must be less than 1500B, it is: " << packet->GetSize ());

  return Time(Seconds(0));
}

}
