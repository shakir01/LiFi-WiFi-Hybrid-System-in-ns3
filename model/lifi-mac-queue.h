#ifndef TDMA_MAC_QUEUE_H
#define TDMA_MAC_QUEUE_H

#include <list>
#include "ns3/packet.h"
#include "ns3/nstime.h"
#include "ns3/object.h"
#include "ns3/wifi-mac-header.h"
#include "ns3/lifi-user-device-infos.h"


namespace ns3 {
class LiFiMacQueue : public Object
{
public:
  typedef Callback<void, Ptr<const Packet> > TdmaMacTxDropCallback;

  static TypeId GetTypeId (void);
  LiFiMacQueue ();
  ~LiFiMacQueue ();

  void SetReceiverModelPtr (Ptr<LiFiUserDeviceInfos> rxPtr);
  void SetMaxSize (uint32_t maxSize);
  void SetMaxDelay (Time delay);
  uint32_t GetMaxSize (void) const;
  Time GetMaxDelay (void) const;
  bool Enqueue (Ptr<const Packet> packet, const WifiMacHeader &hdr);
  Ptr<const Packet> Dequeue (WifiMacHeader *hdr);
  Ptr<const Packet> Peek (WifiMacHeader *hdr);
  bool Remove (Ptr<const Packet> packet);
  void SetTdmaMacTxDropCallback (Callback<void,Ptr<const Packet> > callback);
  void Flush (void);
  bool IsEmpty (void);
  uint32_t GetSize (void);
   uint32_t GetDroppedPackets();

private:
  struct Item;

  typedef std::list<struct Item> PacketQueue;
  typedef std::list<struct Item>::reverse_iterator PacketQueueRI;
  typedef std::list<struct Item>::iterator PacketQueueI;

  void Cleanup (void);
  Mac48Address GetAddressForPacket (enum WifiMacHeader::AddressType type, PacketQueueI);

  struct Item
  {
    Item (Ptr<const Packet> packet,
          const WifiMacHeader &hdr,
          Time tstamp);
    Ptr<const Packet> packet;
    WifiMacHeader hdr;
    Time tstamp;
  };

  PacketQueue m_queue;
  uint32_t m_size;
  uint32_t m_maxSize;
  Time m_maxDelay;
  uint32_t m_count;
  Ptr<LiFiUserDeviceInfos> m_receiverModel;
  TdmaMacTxDropCallback m_txDropCallback;
  mutable uint32_t m_dropped_packets;
};

} // namespace ns3

#endif /* AERO_MAC_QUEUE_H */
