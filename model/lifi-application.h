/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#ifndef LIFI_APPLICATION_H
#define LIFI_APPLICATION_H
#include <fstream>
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
// ===========================================================================
//
//         node 0                 node 1
//   +----------------+    +----------------+
//   |    ns-3 TCP    |    |    ns-3 TCP    |
//   +----------------+    +----------------+
//   |    10.1.1.1    |    |    10.1.1.2    |
//   +----------------+    +----------------+
//   | point-to-point |    | point-to-point |
//   +----------------+    +----------------+
//           |                     |
//           +---------------------+
//                5 Mbps, 2 ms
//
//
// We want to look at changes in the ns-3 TCP congestion window.  We need
// to crank up a flow and hook the CongestionWindow attribute on the socket
// of the sender.  Normally one would use an on-off application to generate a
// flow, but this has a couple of problems.  First, the socket of the on-off 
// application is not created until Application Start time, so we wouldn't be 
// able to hook the socket (now) at configuration time.  Second, even if we 
// could arrange a call after start time, the socket is not public so we 
// couldn't get at it.
//
// So, we can cook up a simple version of the on-off application that does what
// we want.  On the plus side we don't need all of the complexity of the on-off
// application.  On the minus side, we don't have a helper, so we have to get
// a little more involved in the details, but this is trivial.
//
// So first, we create a socket and do the trace connect on it; then we pass 
// this socket into the constructor of our simple application which we then 
// install in the source node.
// ===========================================================================
//


#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include <iostream>
#include "ns3/core-module.h"
#include <vector>
#include <string>
#include <sstream>
 #include "ns3/flow-monitor-module.h"
namespace ns3 {
class LiFiApplication : public Application 
{
public:

  LiFiApplication ();
  virtual ~LiFiApplication();

  void Setup (Ptr<Socket> socket, Address address, uint32_t packetSize, uint32_t nPackets, DataRate dataRate);
  void Setup (Ptr<ns3::Node > node, Ptr<ns3::NetDevice > lifiNetDevice, Address address, uint32_t packetSize, uint32_t nPackets, DataRate dataRate);

  void CwndChange (uint32_t oldCwnd, uint32_t newCwnd);
  void RxDrop (Ptr<const Packet> p);

  ns3::Ptr<Socket> GetSocket (void);

  virtual void StartApplication (void);
  virtual void StopApplication (void);

  std::vector<ns3::Ptr<Packet> > GetSentPackets();
  std::vector<ns3::Ptr<Packet> > GetDroppedPackets();
  std::vector<double >           GetTotlaTransmissionTime();
  
private:
 
  void InstallFlowMonitor (void);
  void ScheduleTx (void);
  void SendPacket (void);

  Ptr<Socket>   m_socket;
  Ptr<Node>     m_node;

  Ptr<ns3::NetDevice > m_netdevice;

  std::vector<double> droppedPackets;
  std::vector<double> theTime;

  Ptr<FlowMonitor> m_flowMonitor;
  FlowMonitorHelper flowHelper;

  std::vector<ns3::Ptr<Packet> >   sentPacketsVector;
  std::vector<ns3::Ptr<Packet> >   droppedPacketsVector;
  std::vector<double >             transmissionTimeVector;

  Address         m_peer;
  uint32_t        m_packetSize;
  uint32_t        m_nPackets;
  DataRate        m_dataRate;
  EventId         m_sendEvent;
  bool            m_running;
  uint32_t        m_packetsSent;
};
}
#endif /* LIFI_CHANNEL_H */
