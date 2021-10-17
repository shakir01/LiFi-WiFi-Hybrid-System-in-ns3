#ifndef CENTRALIZED_CONTROLLER_H
#define CENTRALIZED_CONTROLLER_H

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/ipv4.h"
#include "ns3/applications-module.h"
#include "ns3/mobility-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/config-store-module.h"
#include "ns3/net-device-container.h"
#include "ns3/wifi-module.h"
#include "ns3/internet-module.h"

#include "ns3/flow-monitor-module.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <list>
#include <cmath>
#include <string>
#include <unordered_map>
#include "ns3/gnuplot.h"
#include "ns3/udp-socket-factory.h"
#include "ns3/net-device-container.h"
#include "ns3/wifi-net-device.h"
#include "ns3/point-to-point-net-device.h"
#include "measurement-report.h"
#include "ns3/lifi-net-device.h"

namespace ns3 {
class CentralizedController;
class UserDeviceDataModel : public Object
{
  private:
      uint32_t 		                  u_id;
      std::string                   trafficType;// TCP or UDP

      std::vector <double>          simTimeVect;
      std::vector <double>          snrVect;
      std::vector <double>          errRateVect;
      std::vector <double>          lossVect;
      std::vector <double>          throughputVect;
      std::vector <uint64_t>        rxBytesVector;

      ns3::Ptr<PacketSink>          m_sinkApplication;
      ns3::Ptr<OnOffApplication>    m_OnOffApplication;

      Ptr<CentralizedController>    m_CentralizedController;

      std::vector<uint32_t >  m_lifiApNetDevicesVector;

      Ptr<LiFiPhy>                m_lifiphy;


      ns3::Ipv4Address              m_destinationAddress;
      ns3::Ipv4Address              sourceAddress;
      uint64_t                      m_lastTotalRxBytes;

      uint16_t                      source_port;
      uint16_t                      destination_port;
      std::pair<uint32_t,uint32_t>  m_lagPair;
      uint32_t                      m_lagAfterTTT;

      double                        m_handoverDelay;
      double                        m_kpi_monetoringTime;

      Gnuplot2dDataset              m_output_instant_throughput;
      Gnuplot2dDataset              m_output_loss;
      Gnuplot2dDataset              m_output_snr;
      Gnuplot2dDataset              m_output_delay;
      Gnuplot2dDataset              m_output_latency;
      Gnuplot2dDataset              m_output_ber;

      bool                          isUserMobile;
      bool                          isHandoverInProgress;
      double                        m_time_to_trigger;
  public:
    UserDeviceDataModel ();

    void SetUserId (uint32_t);
    uint32_t  GetUserId(void);

    void SetLagValue (std::pair<uint32_t,uint32_t>);
    std::pair<uint32_t,uint32_t> GetLagValue ();

    void SetSchedularCheckTime(double);
    double GetSchedularCheckTime();

    void SetForwardLagValue (uint32_t);
    uint32_t GetForwardLagValue ();

    void InitiateHandover();

    void AddSimulationTime (double);
    std::vector <double>  GetHistoricalSimTime(void);

    void AddSNR(double,double);
    std::vector <double>  GetHistoricalSNR(void);

    void AddErrorRate(double,double);
    std::vector <double>  GetHistoricalErrorRate(void);

    void AddPathLoss(double,double);
    std::vector <double>  GetHistoricalPathLoss(void);

    void AddInstantThroughput(double,double);
    std::vector <double>  GetHistoricalInstantThroughput(void);
    double CalculateInstantThroughput (double);

    void AddLastRxBytes (uint64_t);
    std::vector <uint64_t>  GetHistoricalRxBytes (void);
    void SetLastTotalRxBytes (void);
    uint64_t  GetLastRxBytes (void);

    void SetSinkApplication (ns3::Ptr<PacketSink>);
    ns3::Ptr<PacketSink> GetSinkApplication (void);

    void SetSourceApplication (ns3::Ptr<OnOffApplication>);
    ns3::Ptr<OnOffApplication> GetSourceApplication (void);

    void SetDestinationAddress (Ipv4Address);
    ns3::Ipv4Address GetDestinationAddress ();

    void SetSourcePort(uint16_t port);
    uint16_t GetSourcePort();

    void SetDestinationPort(uint16_t port);
    uint16_t GetDestinationPort();

    void SetIsUserMbile (bool);
    bool IsUserMobile ();

    void SetCentralizedController (Ptr<CentralizedController>);

    bool IsHandoverHandoverRequired ();
    bool IsHandoverCriteriaSatisfied(uint32_t reprots_count);
    void SetTimeToTrigger (double ttt);
    double GetTimeToTrigger ();

    void CheckHandoverMetricsAfterTTTExpirey ();

    void AddAPID(uint32_t);
    uint32_t GetCurrentAPID();

    void CompleteHandover();

    void SetHandoverDelay(double db);
    double GetHandoverDelay();

    void SetUDPhy (Ptr<LiFiPhy> phy);
    Ptr<LiFiPhy> GetUdPhy ();

    Gnuplot2dDataset GetThroughputDatafile ();
    Gnuplot2dDataset GetPowerDatafile ();
    Gnuplot2dDataset GetSNRDatafile ();
    Gnuplot2dDataset GetBERDatafile ();
    Gnuplot2dDataset GetDelayDatafile ();
    Gnuplot2dDataset GetLatencyDatafile ();
    void ResetDataStructures ();
};

class CentralizedController : public Object
{
  private:
    uint32_t 		                cc_u_id;
    NetDeviceContainer          m_lifiNetDevicesContainer;
    NetDeviceContainer          m_wifiNetDevicesContainer;
    bool                        m_isHandoverEnabled;
    bool                        m_isFirstIteration;

    std::unordered_map <std::string, std::unordered_map<uint32_t, Ptr<UserDeviceDataModel> > > m_userDataModelMaps;

    Ptr<Node> m_controller_node;
  public:
    CentralizedController ();
    ~CentralizedController ();

    
    void SetCCNode (Ptr<Node> controllernode);
    Ptr<Node> GetCCNode(void);

    void SetLiFiAPsNetDeviceInterface(Ptr<ns3::PointToPointNetDevice> netdevice);
    void SetWiFiAPsNetDeviceInterface(Ptr<ns3::WifiNetDevice> netdevice);

    bool LiFiAPMeasurementReports(Ptr<NetDevice> netDevice,Ptr<const Packet> packet,uint16_t protocolId, const Address & address);
    bool WiFiAPMeasurementReports(Ptr<NetDevice> netDevice,Ptr<const Packet> packet,uint16_t protocolId, const Address & address);
    void PrintDroppedPackets(Ptr<UserDeviceDataModel>);


    Ptr<UserDeviceDataModel> FindUserWithID(uint32_t);
    void RegisterUserDevice (std::unordered_map<uint32_t, Ptr<UserDeviceDataModel>>,std::string);
    void MonitorRxChannelStats();
    void TransferApplicationLiFiToWiFi(Ptr<UserDeviceDataModel> userDeviceDataModel);
    ns3::ChannelStats AveragePerformanceMetrics (std::vector<ns3::ChannelStats> chStatsVect);
    void MonitorWiFiRxChannelStats(Ptr<YansWifiPhy> channel, uint32_t user_id,std::unordered_map <uint32_t, std::vector<ns3::ChannelStats> > channelStats);
    void KPI_Monitoring();

    //void CompleteHandover(Ptr<LiFiApMac> controller, std::vector<Ptr<Node> > devicesVector);
    void PerformVerticalHandover (Ptr<UserDeviceDataModel> userDeviceDataModel);
    void RegisterForHandoverForDevice(Ptr<UserDeviceDataModel> userDeviceDataModel);
    Ptr<Ipv4StaticRouting> GetStaticRouting (Ptr<Ipv4> ipv4) const;
    std::pair<int,uint32_t>  GetWiFiNetDeviceInterfaceIndex (Ptr<Node> node);

    void SetUserDataModelMaps(std::unordered_map <std::string, std::unordered_map<uint32_t, Ptr<UserDeviceDataModel> > >);
};
} 
#endif 
