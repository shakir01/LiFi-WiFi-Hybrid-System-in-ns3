#include "ns3/assert.h"
#include "ns3/log.h"
#include "ns3/simulator.h"
#include "ns3/trace-source-accessor.h"
#include "ns3/wifi-mac-header.h"
#include "lifi-tx-mac.h"
#include "lifi-mac-low.h"
#include "ns3/lifi-regular-mac.h"
NS_LOG_COMPONENT_DEFINE ("LiFiTxMac");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (LiFiTxMac);

TypeId
LiFiTxMac::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::LiFiTxMac")
    .SetParent<Object> ()
    .AddConstructor<LiFiTxMac> ()
    .AddTraceSource ("MacTx",
                     "A packet has been received from higher layers and is being processed in preparation for "
                     "queueing for transmission.",
                     MakeTraceSourceAccessor (&LiFiTxMac::m_macTxTrace),
										 "ns3::LiFiTxMac::MacTxCallback")
    .AddTraceSource ("MacTxDrop",
                     "A packet has been dropped in the MAC layer before being queued for transmission.",
                     MakeTraceSourceAccessor (&LiFiTxMac::m_macTxDropTrace),
										 "ns3::LiFiTxMac::MacTxDropCallback");
  return tid;
}

LiFiTxMac::LiFiTxMac ()
{
  NS_LOG_FUNCTION (this);
  m_isTdmaRunning = false;
  //m_low = CreateObject<LiFiMacLow> ();
  m_queue = CreateObject<LiFiMacQueue> ();
  m_control_queue = CreateObject<LiFiMacQueue> ();
  m_bps = DataRate ("800000000b/s");

  m_control_queue->SetTdmaMacTxDropCallback (MakeCallback (&LiFiTxMac::NotifyTxDrop, this));
  m_queue->SetTdmaMacTxDropCallback (MakeCallback (&LiFiTxMac::NotifyTxDrop, this));
}

LiFiTxMac::~LiFiTxMac ()
{
  //m_low = 0;
  m_queue = 0;
  m_control_queue = 0;
  //m_controlMac = 0;
}

void
LiFiTxMac::DoDispose (void)
{
  //m_low->Dispose ();
  //m_low = 0;
  m_queue = 0;
  m_control_queue = 0;
  //m_controlMac = 0;
}

void LiFiTxMac::NotifyTx (Ptr<const Packet> packet)
{
  m_macTxTrace (packet);
}

void LiFiTxMac::NotifyTxDrop (Ptr<const Packet> packet)
{
  //int i=0;
 // NS_ASSERT(i);
 // std::cout<<"LiFiTxMac::NotifyTxDrop (Ptr<const Packet> packet) ID--> "<<m_low->GetPhy()->GetDevice()->GetNode()->GetId()<<std::endl;
  //WifiMacHeader wifiMacHeader;
 // packet->PeekHeader(wifiMacHeader);
 // if (m_low->GetPhy()->GetDevice()->GetNode()->GetId() == 8)
  //std::cout<<"LiFiTxMac::NotifyTxDrop AP MAC "<<this->GetAddress()<<" to "<<wifiMacHeader.GetAddr1()<<" from "<<wifiMacHeader.GetAddr2()<<" node id "<<m_low->GetPhy()->GetDevice()->GetNode()->GetId()<<std::endl;
  m_macTxDropTrace (packet);
}

void LiFiTxMac::SetMacLow(Ptr<LiFiMacLow> mac)
{
    m_low = mac;
}
  
Ptr<LiFiMacLow> LiFiTxMac::GetMacLow()
{
  return m_low;
}

void LiFiTxMac::SetControllingMac (Ptr<LiFiRegularMac> controller)
{
  m_controlMac = controller;
  //std::cout<<"void LiFiTxMac::SetLiFiApMac (Ptr<LiFiApMac> controller) "<<m_lifiApMac<<std::endl;
}

Ptr<LiFiRegularMac> LiFiTxMac::GetControllingMac (void) const
{
  return m_controlMac;
}

void LiFiTxMac::SetTxQueueStartCallback (Callback<bool,uint32_t> queueStart)
{
  NS_LOG_FUNCTION (this);
  m_queueStart = queueStart;
}

void LiFiTxMac::SetTxQueueStopCallback (Callback<bool,uint32_t> queueStop)
{
  NS_LOG_FUNCTION (this);
  m_queueStop = queueStop;
}

uint32_t LiFiTxMac::GetNQueues (void)
{
  //TDMA currently has only one queue
  return 2;
}

uint32_t LiFiTxMac::GetQueueState (uint32_t index)
{
  return 1;
}

void LiFiTxMac::SetLinkDownCallback (Callback<void> linkDown)
{
}

void LiFiTxMac::SetQueueSize (uint32_t size)
{
  NS_LOG_FUNCTION (this << size);
  m_queue->SetMaxSize (size);
}

void LiFiTxMac::SetMaximumDelay (Time delay)
{
  NS_LOG_FUNCTION (this << delay);
  m_queue->SetMaxDelay (delay);
}

void
LiFiTxMac::Queue (Ptr<const Packet> packet, const WifiMacHeader &hdr)
{
  NS_LOG_FUNCTION (this << packet << &hdr);
  //std::cout<<"LiFiTxMac::Queue (Ptr<const Packet> packet, const WifiMacHeader &hdr) m_queue "<<m_queue<<" packet size "<<packet->GetSize()<<std::endl;
  if (!m_queue->Enqueue (packet, hdr))
    {
      NotifyTxDrop (packet);
    }
}

void
LiFiTxMac::StartTransmission (uint64_t transmissionTimeUs)
{
  NS_LOG_DEBUG (transmissionTimeUs << " usec");
  Time totalTransmissionSlot = MicroSeconds (transmissionTimeUs);
  if (m_queue->IsEmpty ())
  {

    NS_LOG_DEBUG ("queue empty");  
    return;
  }

 // if (m_low->GetPhy()->GetDevice()->GetNode()->GetId() != 8)  
  WifiMacHeader header;
  Ptr<const Packet> 
  peekPacket = m_queue->Peek (&header); 
  //Time packetTransmissionTime = m_tdmaController->CalculateTxTime (peekPacket);
  Time packetTransmissionTime = CalculateTxTime (peekPacket);
  NS_LOG_DEBUG ("Packet TransmissionTime(microSeconds): " << packetTransmissionTime.GetMicroSeconds () << "usec");
    //std::cout<<"LiFiTxMac::StartTransmission (Time remainingTime) time "<< packetTransmissionTime.GetMicroSeconds () << "usec"<<peekPacket<<std::endl;

  if (packetTransmissionTime < totalTransmissionSlot)
    {
      //std::cout<<"Packet takes more time to transmit than the slot allotted. Will send in next slot packetTransmissionTime "<<packetTransmissionTime<<" totalTransmissionSlot "<<totalTransmissionSlot<<std::endl;
      totalTransmissionSlot -= packetTransmissionTime;
      Simulator::Schedule (packetTransmissionTime, &LiFiTxMac::SendPacketDown, this,totalTransmissionSlot);
    }
  else
    {

       NS_LOG_DEBUG ("Packet takes more time to transmit than the slot allotted. Will send in next slot");
    }
}

void LiFiTxMac::StopTransmission (bool stop)
{
m_isTdmaRunning  = stop;
}

void
LiFiTxMac::SendPacketDown (Time remainingTime)
{

  WifiMacHeader header;
  Ptr<const Packet>  packet = m_queue->Dequeue (&header);
  m_low->StartTransmission (packet, &header);
  TxQueueStart (0);
  NotifyTx (packet);
  TxQueueStart (0);
  //if (m_low->GetPhy()->GetDevice()->GetNode()->GetId() != 8)
//std::cout<<"LiFiTxMac::SendPacketDown (Time remainingTime)"<<std::endl;
  StartTransmission (remainingTime.GetMicroSeconds ());
}

void
LiFiTxMac::TxOk (const WifiMacHeader &hdr)
{
}
void
LiFiTxMac::TxFailed (const WifiMacHeader &hdr)
{
}

Ptr<LiFiMacQueue> LiFiTxMac::GetMacQueue()
{
  return m_queue;
}

void
LiFiTxMac::TxQueueStart (uint32_t index)
{
  NS_ASSERT (index < GetNQueues ());
 // m_queueStart (index);
}
void
LiFiTxMac::TxQueueStop (uint32_t index)
{
  NS_ASSERT (index < GetNQueues ());
  m_queueStop (index);
}

void LiFiTxMac::SetDataRate (DataRate bps)
{
  NS_LOG_FUNCTION (this << bps);
  m_bps = bps;
}

DataRate LiFiTxMac::GetDataRate (void) const
{
  return m_bps;
}

Time LiFiTxMac::CalculateTxTime (Ptr<const Packet> packet)
{
  NS_LOG_FUNCTION (packet);
  NS_ASSERT_MSG (packet->GetSize () < 1500,"PacketSize must be less than 1500B, it is: " << packet->GetSize ());
 // std::cout<<"Time LiFiTxMac::CalculateTxTime (Ptr<const Packet> packet) "<<m_bps<<" Packet Size "<<packet->GetSize ()<<std::endl;
  return m_bps.CalculateBytesTxTime (packet->GetSize ());
}

void
LiFiTxMac::Initialize ()
{
  NS_LOG_FUNCTION_NOARGS ();
  m_isTdmaRunning = true;

}

} // namespace ns3
