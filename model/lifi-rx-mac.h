#ifndef LIFI_RX_MAC_H
#define LIFI_RX_MAC_H  
#include "ns3/data-rate.h"
#include "ns3/nstime.h"
#include "ns3/llc-snap-header.h"
#include "ns3/simulator.h"
#include "ns3/data-rate.h"
#include "ns3/nstime.h"
#include "ns3/packet.h"
#include "ns3/traced-callback.h"
#include "ns3/wifi-mac-header.h"
//#include "tdma-mac-queue.h"

namespace ns3
{    


class WifiMacHeader;
class OriginatorRxStatus;
//class Packet;
//class Mac48Address;


class LiFiRxMac : public Object
{
    public:
        void Receive (Ptr<Packet> packet);
        void ForwardUp (Ptr<Packet> packet, WifiMacHeader *hdr);
        virtual void DoDispose (void);
        virtual void SetForwardUpCallback (Callback<void,Ptr<Packet>,WifiMacHeader*> upCallback);
        virtual void NotifyRx (Ptr<const Packet> packet);
        virtual void NotifyPromiscRx (Ptr<const Packet> packet);
        virtual void NotifyRxDrop (Ptr<const Packet> packet);


  private:
        /**
     * The trace source fired for packets successfully received by the device
     * immediately before being forwarded up to higher layers (at the L2/L3
     * transition).  This is a promiscuous trace.
     *
     * \see class CallBackTraceSource
     */
    ns3::TracedCallback<Ptr<const Packet> > m_macPromiscRxTrace;

    /**
     * The trace source fired for packets successfully received by the device
     * immediately before being forwarded up to higher layers (at the L2/L3
     * transition).  This is a non- promiscuous trace.
     *
     * \see class CallBackTraceSource
     */
    ns3::TracedCallback<Ptr<const Packet> > m_macRxTrace;

    /**
     * The trace source fired when packets coming into the "top" of the device
     * are dropped at the MAC layer during reception.
     *
     * \see class CallBackTraceSource
     */
    ns3::TracedCallback<Ptr<const Packet> > m_macRxDropTrace;
    Callback<void, Ptr<Packet>,WifiMacHeader*> m_upCallback;

};
}
#endif /* TDMA_MAC_LOW_H */
  
  