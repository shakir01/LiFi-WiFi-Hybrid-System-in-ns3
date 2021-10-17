#include "ns3/simulator.h"
#include "ns3/uinteger.h"
#include "ns3/log.h"
#include "lifi-mac-queue.h"

using namespace std;
NS_LOG_COMPONENT_DEFINE ("LiFiMacQueue");
namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (LiFiMacQueue);

LiFiMacQueue::Item::Item (Ptr<const Packet> packet,
                          const WifiMacHeader &hdr,
                          Time tstamp)
  : packet (packet),
    hdr (hdr),
    tstamp (tstamp)
{
}

TypeId
LiFiMacQueue::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::LiFiMacQueue")
    .SetParent<Object> ()
    .AddConstructor<LiFiMacQueue> ()
    .AddAttribute ("MaxPacketNumber", "If a packet arrives when there are already this number of packets, it is dropped.",
                   UintegerValue (400),
                   MakeUintegerAccessor (&LiFiMacQueue::m_maxSize),
                   MakeUintegerChecker<uint32_t> ())
    .AddAttribute ("MaxDelay", "If a packet stays longer than this delay in the queue, it is dropped.",
                   TimeValue (MilliSeconds (500.0)),
                   MakeTimeAccessor (&LiFiMacQueue::m_maxDelay),
                   MakeTimeChecker ())
  ;
  return tid;
}

LiFiMacQueue::LiFiMacQueue ()
  : m_size (0),
		m_maxSize (0),
    m_count (0),m_receiverModel(0)
{
  NS_LOG_FUNCTION_NOARGS ();
  m_dropped_packets = 0;
//  LogComponentEnable ("LiFiMacQueue", LOG_LEVEL_DEBUG);
}

LiFiMacQueue::~LiFiMacQueue ()
{
  Flush ();
}

void
LiFiMacQueue::SetMaxSize (uint32_t maxSize)
{
  m_maxSize = maxSize;
}

void LiFiMacQueue::SetReceiverModelPtr (Ptr<LiFiUserDeviceInfos> rxPtr)
{
 m_receiverModel = rxPtr;
}

void
LiFiMacQueue::SetMaxDelay (Time delay)
{

  m_maxDelay = delay;
}

void
LiFiMacQueue::SetTdmaMacTxDropCallback (Callback<void,Ptr<const Packet> > callback)
{
  m_txDropCallback = callback;
}

uint32_t
LiFiMacQueue::GetMaxSize (void) const
{
  return m_maxSize;
}

Time
LiFiMacQueue::GetMaxDelay (void) const
{
  return m_maxDelay;
}

bool
LiFiMacQueue::Enqueue (Ptr<const Packet> packet, const WifiMacHeader &hdr)
{
  NS_LOG_DEBUG ("Queue Size: " << GetSize () << " Max Size: " << GetMaxSize ());
//std::cout<<"Now "<<Simulator::Now().GetSeconds()<<"Queue Size: " << GetSize () << " Max Size: " << GetMaxSize ()<<std::endl;
  Cleanup ();
  if (m_size == m_maxSize)
    {
      return false;
    }
  Time now = Simulator::Now ();
  m_queue.push_back (Item (packet, hdr, now));
  m_size++;
  NS_LOG_DEBUG ("Inserted packet of size: " << packet->GetSize ()
                                            << " uid: " << packet->GetUid ());
  return true;
}

void
LiFiMacQueue::Cleanup (void)
{
  NS_LOG_FUNCTION_NOARGS ();
  if (m_queue.empty ())
    {
      return;
    }
  Time now = Simulator::Now ();
  uint32_t n = 0;
  for (PacketQueueI i = m_queue.begin (); i != m_queue.end (); )
    {
      if (i->tstamp + m_maxDelay > now)
        {
          i++;
        }
      else
        {
          m_count++;
                    m_dropped_packets = m_dropped_packets + 1;

          NS_LOG_DEBUG (Simulator::Now ().GetSeconds () << "s Dropping this packet as its exceeded queue time, pid: " << i->packet->GetUid ()
                                                        << " queueSize: " << m_queue.size ()
                                                        << " count:" << m_count);
//std::cout<<"LiFiMacQueue::Cleanup (void) "<<Simulator::Now ().GetSeconds () << "s Dropping this packet as its exceeded queue time, pid: " << i->packet->GetUid ()
  //                                                      << " queueSize: " << m_queue.size ()
    //                                                    << " count:" << m_count<<" m_dropped_packets "<<m_dropped_packets<<std::endl;
          m_txDropCallback (i->packet);
          i = m_queue.erase (i);
          n++;
        }
    }
  m_size -= n;
}

uint32_t LiFiMacQueue::GetDroppedPackets()
{
  std::cout<<"uint32_t LiFiMacQueue::GetDroppedPackets() "<<m_dropped_packets<<std::endl;
  return m_dropped_packets;
}

Ptr<const Packet>
LiFiMacQueue::Dequeue (WifiMacHeader *hdr)
{
  NS_LOG_FUNCTION_NOARGS ();
  Cleanup ();
  if (!m_queue.empty ())
    {
      Item i = m_queue.front ();
      m_queue.pop_front ();
      m_size--;
      *hdr = i.hdr;
      NS_LOG_DEBUG ("Dequeued packet of size: " << i.packet->GetSize ());
      return i.packet;
    }
  return 0;
}

Ptr<const Packet>
LiFiMacQueue::Peek (WifiMacHeader *hdr)
{
  NS_LOG_FUNCTION_NOARGS ();
  Cleanup ();
  if (!m_queue.empty ())
    {
      Item i = m_queue.front ();
      *hdr = i.hdr;
      return i.packet;
    }
  return 0;
}

bool
LiFiMacQueue::IsEmpty (void)
{
  Cleanup ();
  return m_queue.empty ();
}

uint32_t
LiFiMacQueue::GetSize (void)
{
  return m_size;
}

void
LiFiMacQueue::Flush (void)
{
  m_queue.erase (m_queue.begin (), m_queue.end ());
  m_size = 0;
}

Mac48Address
LiFiMacQueue::GetAddressForPacket (enum WifiMacHeader::AddressType type, PacketQueueI it)
{
  if (type == WifiMacHeader::ADDR1)
    {
      return it->hdr.GetAddr1 ();
    }
  if (type == WifiMacHeader::ADDR2)
    {
      return it->hdr.GetAddr2 ();
    }
  if (type == WifiMacHeader::ADDR3)
    {
      return it->hdr.GetAddr3 ();
    }
  return 0;
}

bool
LiFiMacQueue::Remove (Ptr<const Packet> packet)
{
  PacketQueueI it = m_queue.begin ();
  for (; it != m_queue.end (); it++)
    {
      if (it->packet == packet)
        {
          m_queue.erase (it);
          m_size--;
          return true;
        }
    }
  return false;
}
} // namespace ns3
