#ifndef PERFORMANCE_ANALYSIS_H
#define PERFORMANCE_ANALYSIS_H

#include "ns3/object.h"
#include <iostream>
#include <vector>
#include <string>
#include "ns3/applications-module.h"
#include "ns3/flow-monitor-module.h"

namespace ns3 {
  struct SinkStats
  {
         uint64_t lastTotalRx;
         uint64_t totalRecieved;
         Ipv4Address  ipAddress;
  };

  struct LiFiWiFiFlowStats
  {
    Ptr<PacketSink> sinkApp;
    uint32_t portAddres;
    Ipv4Address sourceAddress;
    Ipv4Address destinationAddress;

    Ipv4Address prevSourceAddress;
    Ipv4Address prevDestinationAddress;

    double totalLostBytes;

    double sunDelay;
    double sumJitters;

    double perPacketDelay;
    double perPacketJitter;
    
    double duration;
    
    double throughput;
    
    uint32_t txPackets;
    uint32_t rxPackets;

    uint32_t totalDroppedPackets;

    uint32_t totalRetransmission;

    uint64_t totalRxBytes;

    bool hasExperiencedHandover;
  };

class PerformanceAnalysis: public Object	// This class is a subclass of Object class
{
public:
  PerformanceAnalysis ();

  void SetLiFiSinkApplicationContainer (ApplicationContainer appContainer);
  void SetLiFiCsmaClientApplicationContainer (ApplicationContainer appContainer);
  void SetWiFiStaSinkApplicationContainer (ApplicationContainer appContainer);
  void SetWiFiCsmaClientsApplicationContainer (ApplicationContainer appContainer);

  ApplicationContainer CetLiFiSinkApplicationContainer ();
  ApplicationContainer GetLiFiCsmaClientApplicationContainer ();
  ApplicationContainer GetWiFiStaSinkApplicationContainer ();
  ApplicationContainer GetWiFiCsmaClientsApplicationContainer ();

  //std::vector<std::map<std::string, std::vector<LiFiWiFiFlowStats> > >  CheckThroughput (Ptr<FlowMonitor>  m_monitor, FlowMonitorHelper&  m_flowmon);
  std::vector<std::map<std::string, std::vector<LiFiWiFiFlowStats> > > CheckThroughput (Ptr<Ipv4FlowClassifier> classifier, FlowMonitor::FlowStatsContainer stats1);
  bool ShouldCaptureTrafficforPort (std::string keys);
  void  SetLiFiKeys (std::string);
  void  SetWiFiKeys (std::string);
  std::vector<std::string> GetWiFiKeys ();
  std::vector<std::string> GetLiFiKeys ();
  void SwitchUserFromLiFiToWifi (std::string wifi_key ,Ptr<PacketSink> sink);
Ptr<PacketSink> GetDestinationApplicationForIpAddress (Ipv4Address destAddress);

private:
  ApplicationContainer lifiStaSinkApps;
  ApplicationContainer lifiCsmaClientApps;
  ApplicationContainer wifiStaSinkApps;
  ApplicationContainer wifiCsmaClientApps;

  std::vector <ApplicationContainer> lifiAPsSinkContainer;

  std::map<std::string, SinkStats > lifiSinkStatMaps;
  std::map<std::string, SinkStats > wifiSinkStatMaps;

  std::vector<std::string> lifiAPKeys;
  std::vector<std::string> wifiAPKeys;
  Ptr<PacketSink> sink;                         /* Pointer to the packet sink application */
};
}//end ns3 namespace
#endif
