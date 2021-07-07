#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/ipv4.h"
#include "ns3/applications-module.h"
#include "ns3/mobility-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/csma-module.h"
#include "ns3/config-store-module.h"
#include "ns3/net-device-container.h"
#include "ns3/wifi-module.h"
#include "ns3/internet-module.h"
#include "ns3/lifi-module.h"
#include "ns3/random-variable-stream.h"
#include "ns3/lifi-helper.h"
#include "ns3/flow-monitor-module.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <list>
#include <cmath>
#include <string>
#include "ns3/lifi-constant-velocity-mobility-model.h"
#include "ns3/lifi-random-waypoint-mobility-model.h"
#include "ns3/lifi-user-device-infos.h"
#include "ns3/PerformanceAnalysis.h"
#include <unordered_map>
#include "ns3/gnuplot.h"
#include "ns3/udp-socket-factory.h"
using namespace ns3;

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


      ns3::Ipv4Address              m_destinationAddress;
      ns3::Ipv4Address              sourceAddress;
      uint64_t                      m_lastTotalRxBytes;

      uint16_t                      source_port;
      uint16_t                      destination_port;

      Gnuplot2dDataset              m_output_instant_throughput;
      Gnuplot2dDataset              m_output_loss;
      Gnuplot2dDataset              m_output_snr;
      Gnuplot2dDataset              m_output_delay;
      Gnuplot2dDataset              m_output_latency;
      Gnuplot2dDataset              m_output_ber;

      bool                          isUserMobile;
  public:
       UserDeviceDataModel ();

       void SetUserId (uint32_t);
       uint32_t  GetUserId(void);

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

      Gnuplot2dDataset GetThroughputDatafile ();
      Gnuplot2dDataset GetPowerDatafile ();
      Gnuplot2dDataset GetSNRDatafile ();
      Gnuplot2dDataset GetBERDatafile ();
      Gnuplot2dDataset GetDelayDatafile ();
      Gnuplot2dDataset GetLatencyDatafile ();
      void ResetDataStructures ();
};

UserDeviceDataModel::UserDeviceDataModel ()
{
    this->rxBytesVector.push_back(0);
    m_lastTotalRxBytes = 0;   
    isUserMobile = false;
}

void UserDeviceDataModel::SetUserId (uint32_t id)
{
    this->u_id = id;
}

uint32_t  UserDeviceDataModel::GetUserId(void)
{
    return u_id;
}

void UserDeviceDataModel::AddSimulationTime (double simTime)
{
    simTimeVect.push_back(simTime);
}

std::vector <double>  UserDeviceDataModel::GetHistoricalSimTime(void)
{
    return simTimeVect;
}

void UserDeviceDataModel::AddSNR(double snr, double simTime)
{
    snrVect.push_back (snr);
    m_output_snr.Add (simTime, snr);
}

std::vector <double>  UserDeviceDataModel::GetHistoricalSNR(void)
{
    return snrVect;
}

void UserDeviceDataModel::AddErrorRate(double errorRate, double simTime)
{
    errRateVect.push_back (errorRate);
    m_output_ber.Add (simTime, errorRate);
}

std::vector <double>  UserDeviceDataModel::GetHistoricalErrorRate(void)
{
    return errRateVect;
}

void UserDeviceDataModel::AddPathLoss(double loss, double simTime)
{
    lossVect.push_back(loss);
    m_output_loss.Add (simTime, loss);
}

std::vector <double>  UserDeviceDataModel::GetHistoricalPathLoss(void)
{
    return lossVect;
}

void UserDeviceDataModel::AddInstantThroughput(double instantThroughput, double simTime)
{
    throughputVect.push_back(instantThroughput);
    m_output_instant_throughput.Add(simTime, instantThroughput);
}

std::vector <double>  UserDeviceDataModel::GetHistoricalInstantThroughput(void)
{
    return throughputVect;
}

void UserDeviceDataModel::AddLastRxBytes (uint64_t lastRxBytes)
{
  rxBytesVector.push_back(lastRxBytes);
}

void UserDeviceDataModel::SetLastTotalRxBytes (void)
{
    rxBytesVector.push_back(this->GetSinkApplication()->GetTotalRx ());
}

std::vector <uint64_t>  UserDeviceDataModel::GetHistoricalRxBytes (void)
{
    return rxBytesVector;
}

uint64_t  UserDeviceDataModel::GetLastRxBytes (void)
{
    return rxBytesVector[rxBytesVector.size() - 1];
}

void UserDeviceDataModel::SetSinkApplication (ns3::Ptr<PacketSink> app)
{
    m_sinkApplication = app;
}

ns3::Ptr<PacketSink> UserDeviceDataModel::GetSinkApplication (void)
{
    return m_sinkApplication;
}

void UserDeviceDataModel::SetSourceApplication (ns3::Ptr<OnOffApplication> onOffApplication)
{
    m_OnOffApplication = onOffApplication;
}

ns3::Ptr<OnOffApplication> UserDeviceDataModel::GetSourceApplication (void)
{
    return m_OnOffApplication;
}

void UserDeviceDataModel::SetDestinationAddress (Ipv4Address destination)
{
    m_destinationAddress =  destination;
}

ns3::Ipv4Address UserDeviceDataModel::GetDestinationAddress (void)
{
    return m_destinationAddress;
}

void UserDeviceDataModel::SetIsUserMbile (bool usrMobile)
{
    isUserMobile = usrMobile;
}

bool UserDeviceDataModel::IsUserMobile ()
{
    return isUserMobile;
}

void UserDeviceDataModel::SetSourcePort(uint16_t port)
{
    source_port = port;
}

uint16_t UserDeviceDataModel::GetSourcePort()
{
    return  source_port;
}

void UserDeviceDataModel::SetDestinationPort(uint16_t port)
{
    destination_port = port;
}

uint16_t UserDeviceDataModel::GetDestinationPort()
{
    return  destination_port ;
}

double UserDeviceDataModel::CalculateInstantThroughput (double time)
{
    uint64_t lastRx = this->GetLastRxBytes();
    uint64_t currentRxBytes = this->GetSinkApplication()->GetTotalRx ();
    double cur = ((currentRxBytes- lastRx) * (double) 8.0) / (1e3);     /* Convert Application RX Packets to MBits. */
    this->AddLastRxBytes(currentRxBytes);
    AddInstantThroughput(cur, time);
    return cur;
 }

 void UserDeviceDataModel::ResetDataStructures ()
{
    this->snrVect.clear();
    this->errRateVect.clear();
    this->lossVect.clear();
    this->throughputVect.clear();
 }

Gnuplot2dDataset UserDeviceDataModel::GetThroughputDatafile ()
{
    return m_output_instant_throughput;
}

Gnuplot2dDataset UserDeviceDataModel::GetPowerDatafile ()
{
     return m_output_loss;
}

Gnuplot2dDataset UserDeviceDataModel::GetSNRDatafile ()
{
    return m_output_snr;
}

Gnuplot2dDataset UserDeviceDataModel::GetBERDatafile ()
{
    return m_output_ber;
}

Gnuplot2dDataset UserDeviceDataModel::GetDelayDatafile ()
{
      return m_output_delay;
}

Gnuplot2dDataset UserDeviceDataModel::GetLatencyDatafile ()
{
      return m_output_latency;

}

double x_dest;
double y_dest;

uint16_t wifi_port = 9000;
// The number of bytes to send in this simulation.
static const uint32_t totalTxBytes = 90000;

bool isHandoverEnabled = false;
int iteration = 0;

uint8_t wifi_nodes_per_lifi_ap = 0;
double min_xx = 0; 
double max_xx = 0; 
double min_yy = 0; 
double max_yy = 0; 
double user_speed = 0;
      
int file_count = 0;

Ptr<UniformRandomVariable> rand_xdestination;
Ptr<UniformRandomVariable> rand_ydestination;

// Perform series of 1040 byte writes (this is a multiple of 26 since
// we want to detect data splicing in the output stream)
static const uint32_t writeSize = 1040;
NS_LOG_COMPONENT_DEFINE ("LiFiController");
bool isFirstIteration = true;
bool usingUL = true;
bool usingDL = true;

double simulationTime = 10;                   /* Simulation time in seconds. */

class LiFiController
{
public:
  LiFiController ();
  void RunSimulation (uint32_t nWifis, double totalTime, std::string rate);


private:
  // empty map container
  std::map<std::vector<Ptr<Node>>, uint16_t> lifi_ap_map;
  std::vector<std::vector<std::vector<double> > > lifiAPswithAssociatedUserPositions;
  std::vector<std::vector<double> > wifiUsersPositios;
  std::vector<std::vector<double> > wifiNetWorkNodes;
  std::vector<Ptr<LiFiHelper> > macHelperVector;

  std::vector<double> Received;
  std::vector<double> theTime;
  std::vector<std::pair<double,double> > appsPosition;
  std::vector<std::pair<double,double> > usersPosition;

  std::vector<ns3::NodeContainer> lifiNetworkNodesContainerList;
  std::vector<ns3::NodeContainer> otherNodesContainerList;
  NodeContainer internetServerNodes;
  NodeContainer wifiapNodes;
  NodeContainer wifiUserNodes;
  NodeContainer apNodes;
  NodeContainer lifiApNodes;
  NodeContainer allUserNodes;
  NodeContainer csmaClientNodes;

  ///Internet
  std::vector<Ipv4AddressHelper> m_ipv4AddressHelperVector;

 //////////////////////////////////////
 //Function to generate signals.
  std::map<double, double> m_transmitRangeMap;
  Ptr < Ipv4 > wifiAPRouter;
  Ptr < Ipv4 > internetServiceRouter;
  Ptr < Ipv4 > user0APRouter;
  Ptr < Ipv4 > user1APRouter;
  Ptr < Ipv4 > user2APRouter;
  Ptr < Ipv4 > user3APRouter;

  //devices
  std::vector<ns3::NetDeviceContainer> lifiNetworkNetDevicesContainerList;
  std::vector<std::vector<ns3::NetDeviceContainer> > otherNetworkNetDevicesContainerList;
  std::vector<NetDeviceContainer> wifiNodep2pNetDevicesVector;

  NetDeviceContainer internetServerNetDevices;
  NetDeviceContainer wifiNetDevices;
  NetDeviceContainer LiFiAPNodeDevices;
  NetDeviceContainer userLiFiDevices;//userLiFiDevices
  NetDeviceContainer userWiFiDevices;//userWiFiDevices
  NetDeviceContainer allNetDevices;
  NetDeviceContainer serverNetDevices;//serverNetDevices

  //IPV4 interfaces
  Ipv4InterfaceContainer lifiAPNetDevicesInterface;
  Ipv4InterfaceContainer serversIpv4interfaceContainer;
  Ipv4InterfaceContainer wifitocsmauserinterfaceContainer;

  std::vector <std::vector<Ptr<LiFiApplication> > > appsVectorsVector;
  std::vector <ns3::Ptr<LiFiChannel> > channelsVector;

  std::vector <NodeContainer> serverNodesContainerVector;//serverNodesContainerVector
  ///Performance Analysis
  ns3::Ptr <PerformanceAnalysis> performanceAnalysis;
  std::unordered_map <std::string, std::unordered_map<uint32_t, Ptr<UserDeviceDataModel> > > apUserDataModelMaps;
  std::unordered_map <std::string, std::unordered_map<uint32_t, Ptr<UserDeviceDataModel> > > wifi_UserDataModelMaps;
  typedef std::unordered_map <uint32_t, std::vector<ns3::ChannelStats> > ChannelStatsMap;

private:
  void CreateNodes ();
  void CreateCsmaNodes ();
  void InternetServerNetDevices ();
  void ConfigureLiFiNetwork ();
  void ConfigureWiFiNetwork ();
  void InstallInternetStack ();
  void InstallWiFiNodesApplications(std::string);
  void InstallLiFiNodesApplications(std::string);
  void ModifyLifiCache (void);
  void ConfigureUserDevicesCaches ();
  void SetupMobility ();
  void SetupMobility1();
  void AnalyzeData (FlowMonitorHelper* m_flowmon, Ptr<FlowMonitor>  m_monitor, std::string dataRate, uint16_t nn);
  void ReadUsersFromCSV(uint16_t);
  void LiFiDeviceCourseChangeU (Ptr<const MobilityModel>);
  void InstanteneousThroughput (uint16_t nodes, std::string dataRate, bool isFirstIteration);
  Ptr<UserDeviceDataModel> FindUserDeviceInMap( std::unordered_map <std::string, std::unordered_map<uint32_t, Ptr<UserDeviceDataModel> > > userDataModelMaps, uint32_t userId);
  void CheckUserPerformance();
  void KPI_Monitoring();
  ns3::ChannelStats AveragePerformanceMetrics (std::vector<ns3::ChannelStats> chStatsVect);
  void PerformanceMetrics ();
  bool IsHandoverRequiredForDevice(std::vector<double> vec, uint32_t id);
  void RegisterForHandoverForDevice(Ptr<UserDeviceDataModel> userDeviceDataModel);
  void CompleteHandover(Ptr<LiFiApMac>, std::vector <Ptr<Node> >);
  void PerformVerticalHandover(std::vector<Ptr<Node> >);
  void TransferApplicationLiFiToWiFi(Ptr<UserDeviceDataModel> userDeviceDataModel);
  void MonitorWiFiRxChannelStats(Ptr<YansWifiPhy> channel, uint32_t apId,std::unordered_map <uint32_t, std::vector<ns3::ChannelStats> > channelStats);
  void MonitorRxChannelStats(Ptr<LiFiPhy>, uint32_t,std::vector<ns3::ChannelStats> channelStatsVector);
  void WiFiCache (NodeContainer wifiapNodes, Ptr<Node> node);
  void TransferDataModelFromLiFiToWiFi (Ptr<UserDeviceDataModel> userDeviceDataModel);
};

int main (int argc, char **argv)
{
  const double min_x = -2.0;
  const double max_x = 2.0;
  Ptr<UniformRandomVariable> uniform_x = CreateObject<UniformRandomVariable> ();
  uniform_x->SetAttribute ("Min", DoubleValue (min_x));
  uniform_x->SetAttribute ("Max", DoubleValue (max_x));

  const double min_y = -2.0;
  const double max_y = 2.6;
  Ptr<UniformRandomVariable> uniform_y = CreateObject<UniformRandomVariable> ();
  uniform_y->SetAttribute ("Min", DoubleValue (min_y));
  uniform_y->SetAttribute ("Max", DoubleValue (max_y));
  
  std::vector<std::pair<double, double> > x_vector;
  x_vector.push_back(std::make_pair(-11,11));
  x_vector.push_back(std::make_pair(-12,12));
 // x_vector.push_back(std::make_pair(-8,8));
 // x_vector.push_back(std::make_pair(-9,9));
  //x_vector.push_back(std::make_pair(-10,10));

  std::vector<std::pair<double, double> > y_vector;
  y_vector.push_back(std::make_pair(-11,11));
  y_vector.push_back(std::make_pair(-12,12));
 // y_vector.push_back(std::make_pair(-8,8));
 // y_vector.push_back(std::make_pair(-9,9));
  //y_vector.push_back(std::make_pair(-10,10));
 //uint16_t dIndex1 =1;
  //uint16_t dIndex2 = 1;
  //for (uint16_t dIndex3 = 0; dIndex3 < 2; dIndex3++)
  //{ 
      /*if (dIndex3 == 0)
      {
        isHandoverEnabled = true;
      }
      else 
      {
        isHandoverEnabled = false;
      }*/

      std::vector<double> speedVector = {1.2,1.6};
      rand_xdestination = CreateObject<UniformRandomVariable> ();
      rand_ydestination = CreateObject<UniformRandomVariable> ();

      const double min = -8;
      const double max = 8;

    
      rand_xdestination->SetAttribute ("Min", DoubleValue (min));
      rand_xdestination->SetAttribute ("Max", DoubleValue (max));

      rand_ydestination->SetAttribute ("Min", DoubleValue (min));
      rand_ydestination->SetAttribute ("Max", DoubleValue (max));
uint16_t dIndex2 = 0;
      //for (uint16_t dIndex2 = 0; dIndex2 < 2; dIndex2++)
      //{
          iteration = dIndex2;
          /*std::pair <double, double> y_pair = y_vector[dIndex2];
          std::pair <double, double> x_pair = x_vector[dIndex2];
          min_xx = y_pair.first; 
          max_xx = y_pair.second; 
          min_yy = x_pair.first; 
          max_yy = x_pair.second;
          user_speed =  speedVector[iteration];*/

          x_dest = rand_xdestination->GetValue();
          min_xx = dIndex2;
          min_yy = dIndex2;
          y_dest = rand_ydestination->GetValue();
          LiFiController test;//nWifis// nSinks  
          std::vector<double> simTimeVector;
          simTimeVector.push_back(10);
          simTimeVector.push_back(30);
          simTimeVector.push_back(50);
          simTimeVector.push_back(100);

          std::vector<std::string> dataRatesVector;
          dataRatesVector.push_back("20Mbps");
          dataRatesVector.push_back("3Mbps");
          dataRatesVector.push_back("4Mbps");
          dataRatesVector.push_back("5Mbps");
          dataRatesVector.push_back("6Mbps");
          dataRatesVector.push_back("7Mbps");
          uint16_t dIndex = 0;
          file_count = dIndex2;
          uint16_t nn = 4;
          uint16_t tt = 0;
          test = LiFiController ();
          test.RunSimulation (nn, simTimeVector[tt], dataRatesVector[dIndex]);
       //}
  //}
  return 0;
} 

LiFiController::LiFiController ()
{
}

void LiFiController::InstanteneousThroughput (uint16_t nodes, std::string dataRate, bool isFirstIteration)
 {
   //std::cout<<"void LiFiController::InstanteneousThroughput (uint16_t nodes, std::string dataRate, bool isFirstIteration)"<<std::endl;
   /* std::stringstream m_CSVfileNamestream;
    m_CSVfileNamestream<<"InstantThroughput/"<<"Instant_Throughput-"<<dataRate<<"-mbps-Nodes-"<<nodes<<"-Sim_Time-"<<simulationTime;
    //Time-10-Udp
    std::string m_CSVfileName = m_CSVfileNamestream.str();
    std::ofstream dat_file_out (m_CSVfileName.c_str (), std::ios::app);
    if (isFirstIteration)
    {
        dat_file_out << "nodes "<<"\t node_id  "<<"\t time " << "\t destination "<< "\t Throughput " << std::endl;
    }
    //std::unordered_map<std::string, std::vector <Ptr<InstantDataModel> > >:: iterator itr; 
    for (itr = instantDataModelMap.begin(); itr != instantDataModelMap.end(); itr++) 
    { 
       // cout << itr->first << "  " <<  << endl; 
        //std::vector <Ptr<InstantDataModel> >  vectModel =  itr->second;
        for (size_t i = 0; i < vectModel.size(); i++)
        {
          Time now = Simulator::Now ();                                         //Return the simulator's virtual time. 
         // Ptr<InstantDataModel> userDeviceDataModel = vectModel[i];
          //double cur = ((userDeviceDataModel->GetSinkApplication()->GetTotalRx () - userDeviceDataModel->GetLastTotalRxBytes()) * (double) 8) / (5e3);     //Convert Application RX Packets to MBits.
          //cur = cur/2;
          //userDeviceDataModel->SetLastTotalRxBytes(userDeviceDataModel->GetSinkApplication()->GetTotalRx ());
        }  
     }
     
    Simulator::Schedule (MilliSeconds (5), &LiFiController::InstanteneousThroughput, this, nodes, dataRate, true);
 */
 }

Ptr<UserDeviceDataModel> LiFiController::FindUserDeviceInMap( std::unordered_map <std::string, std::unordered_map<uint32_t, Ptr<UserDeviceDataModel> > > userDataModelMaps, uint32_t userId)
{
  for (auto& itr: userDataModelMaps) 
  {
      // Do stuff
      std::unordered_map<uint32_t, Ptr<UserDeviceDataModel> >  userDeviceMap = itr.second;
      std::unordered_map <uint32_t, Ptr<UserDeviceDataModel> >::  iterator itr1;
      itr1 = userDeviceMap.find(userId);
      if (itr1 != userDeviceMap.end())
      {
          Ptr<UserDeviceDataModel> userDeviceDataModel = itr1->second;
          return userDeviceDataModel;
      }
  }

  return 0;
}


 
void LiFiController::AnalyzeData (FlowMonitorHelper* m_flowmon,  Ptr<FlowMonitor>  m_monitor, std::string dataRate, uint16_t nn)
{
  //LiFiWiFiFlowStats stats=
  m_monitor->CheckForLostPackets ();
  Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (m_flowmon->GetClassifier ());
  FlowMonitor::FlowStatsContainer stats = m_monitor->GetFlowStats ();

	std::vector<std::map<std::string, std::vector<LiFiWiFiFlowStats> > >  flowStatsMapVect = performanceAnalysis->CheckThroughput (classifier, stats);
  std::map<std::string, std::vector<LiFiWiFiFlowStats> > lifi_handover_Map =  flowStatsMapVect[0];
  
  std::map<std::string, std::vector<LiFiWiFiFlowStats> > lifiMap =  flowStatsMapVect[1];
  std::vector<std::string> lifiKeysVect;
  std::vector<double> lifiSinksRx;

  std::stringstream m_CSVlififileNamestream;
  m_CSVlififileNamestream<<"LiFi-Network/"<<"LiFi-Only-Stas-"<<dataRate<<"-Nodes-"<<nn<<"-Sim_Time-"<<simulationTime;
  std::string m_CSVlififileName = m_CSVlififileNamestream.str();
  std::cout<<"m_CSVlififileName "<<m_CSVlififileName<<std::endl;
  std::ofstream dat_lifi_file_out (m_CSVlififileName.c_str (), std::ios::app);
  dat_lifi_file_out << "Node_Id "<<"\t time " << "\t source " << "\t destination "<<"\t duration " << "\t Sum Delay " << "\t Sum Jitters " << "\t Throughput " <<"\t Rx Packet "<<"\t Tx Packet "<< std::endl;
  
  uint16_t totalLiFiUsers = 0;
  for(std::map<std::string, std::vector<LiFiWiFiFlowStats> >::iterator it = lifiMap.begin(); it != lifiMap.end(); ++it)
  {
      lifiKeysVect.push_back(it->first);
      std::vector<LiFiWiFiFlowStats> statsVect = it->second;
      uint64_t lifiTotlRx = 0;
      double lifiThroughput = 0;
      for (size_t i = 0; i < statsVect.size(); i++)
      {
          std::cout<<"LiFi AP  "<< i<<" Data "<< "\n";

          LiFiWiFiFlowStats stats = statsVect[i];
          std::cout<<"stats.sinkApp "<<stats.sinkApp<<std::endl;
          double throughput    = stats.sinkApp->GetTotalRx()/(double)(simulationTime*1e6);
          Ptr <Node> node      = stats.sinkApp->GetNode();
          Ptr<Ipv4> ipv4A      = node->GetObject<Ipv4> ();
          Ipv4Address ipv4     = ipv4A->GetAddress(2,0).GetLocal();

          std::cout<<"LiFi Station IP Address : "<<ipv4<<" analyzer dest ip: "<<stats.destinationAddress<<" source address: "<<stats.sourceAddress<<std::endl;

          std::cout<<"Sink App Throughput: "<<throughput<<" Flow Monitor Throughput "<<stats.throughput<<std::endl;
          std::cout<<"totalLostBytes "<<stats.totalLostBytes<<std::endl;
          std::cout<<"sunDelay "<<stats.sunDelay<<std::endl;
          std::cout<<"sumJitters "<<stats.sumJitters<<std::endl;
          std::cout<<"perPacketDelay "<<stats.perPacketDelay<<std::endl;
          std::cout<<"perPacketJitter "<<stats.perPacketJitter<<std::endl;
          std::cout<<"totalDroppedPackets "<<stats.totalDroppedPackets<<std::endl;
          std::cout<<"totalRetransmission "<<stats.totalRetransmission<<std::endl;
          std::cout<<"Total Bytes Received  "<<stats.sinkApp->GetTotalRx()<<std::endl;
          double throughput1 = ((double)stats.sinkApp->GetTotalRx()*8.0)/((simulationTime-1.5)*1e6);
          std::cout<<"LiFi UD: "<<ipv4<<" Throughput "<<throughput1<<std::endl;
          lifiTotlRx += stats.sinkApp->GetTotalRx();
          lifiSinksRx.push_back(lifiTotlRx);
          std::cout<<"\n"<<std::endl;
          totalLiFiUsers += 1;
          //dat_file_out << nn <<", \t " << simulationTime << ", \t "<< stats.sourceAddress  << ", \t " << stats.destinationAddress<<", \t "<< stats.duration<< ", \t "<<stats.perPacketDelay<<", \t "<< stats.perPacketJitter<<", \t "<< throughput1<<", \t "
          //<<stats.txPackets<<", \t "<<stats.rxPackets<<", \t "<< std::endl;
          dat_lifi_file_out << node->GetId() <<", \t " << Simulator::Now().GetSeconds() << ", \t "<< stats.sourceAddress  << ", \t " << stats.destinationAddress<<", \t "<< stats.duration<< ", \t "<<stats.perPacketDelay<<", \t "<< stats.perPacketJitter<<", \t "<< throughput1<<", \t "
          <<stats.txPackets<<", \t "<<stats.rxPackets<<", \t "<< std::endl;
      }

      lifiThroughput = ((double)lifiTotlRx*8.0)/(double)(simulationTime*1e6);
      std::cout<<"Total LiFi Users "<<totalLiFiUsers<<" lifiTotlRx "<<lifiTotlRx<<"\n simulationTime: "<<simulationTime<<" Total LiFi Throughput: "<<lifiThroughput<<std::endl;
  }

  dat_lifi_file_out.close();

  std::cout<<"////////////////////////////////////////////////////////\n\n"<<std::endl;
  std::map<std::string, std::vector<LiFiWiFiFlowStats> > wifiMap =  flowStatsMapVect[2];
  std::vector<std::string> wifiKeysVect;
  double wifithroughput =0;
  uint64_t totalWiFiRx = 0;
  std::stringstream m_CSVwififileNamestream;
  m_CSVwififileNamestream<<"WiFi-Network/"<<"WiFi-Only-Stas-"<<dataRate<<"-Udp-WiFi- 4-Mobile-Nodes-"<<nn<<"-Sim_Time-"<<simulationTime;
  std::string m_CSVwififileName = m_CSVwififileNamestream.str();
  std::cout<<"m_CSVwififileName "<<m_CSVwififileName<<std::endl;
  std::ofstream dat_wifi_file_out (m_CSVwififileName.c_str (), std::ios::app);
  dat_wifi_file_out << "Node_Id "<<"\t time " << "\t source " << "\t destination "<<"\t duration " << "\t Sum Delay " << "\t Sum Jitters " << "\t Throughput " <<"\t Rx Packet "<<"\t Tx Packet "<< std::endl;
  
  for(std::map<std::string, std::vector<LiFiWiFiFlowStats> >::iterator it = wifiMap.begin(); it != wifiMap.end(); ++it)
  {
      wifiKeysVect.push_back(it->first);
      std::vector<LiFiWiFiFlowStats> statsVect = it->second;
      for (size_t i = 0; i < statsVect.size(); i++)
      {
          std::cout<<"WiFi Sink  "<< i<<" Data "<< "\n";

          LiFiWiFiFlowStats stats = statsVect[i];
          double throughput = stats.sinkApp->GetTotalRx()/(double)(simulationTime*1e6);
          Ptr <Node> node      = stats.sinkApp->GetNode();
          Ptr<Ipv4> ipv4A      = node->GetObject<Ipv4> ();
          Ipv4Address ipv4     = ipv4A->GetAddress(1,0).GetLocal();

          std::cout<<"WiFi Station IP Address : "<<ipv4<<" analyzer dest ip: "<<stats.destinationAddress<<" source address: "<<stats.sourceAddress<<std::endl;
          std::cout<<"Throughput: "<<throughput<<" Flow Monitor Throughput "<<stats.throughput<<std::endl;
          std::cout<<"totalLostBytes "<<stats.totalLostBytes<<std::endl;
          std::cout<<"sunDelay "<<stats.sunDelay<<std::endl;
          std::cout<<"sumJitters "<<stats.sumJitters<<std::endl;
          std::cout<<"perPacketDelay "<<stats.perPacketDelay<<std::endl;
          std::cout<<"perPacketJitter "<<stats.perPacketJitter<<std::endl;
          std::cout<<"totalDroppedPackets "<<stats.totalDroppedPackets<<std::endl;
          std::cout<<"totalRetransmission "<<stats.totalRetransmission<<std::endl;
          std::cout<<"Total Bytes Received  "<<stats.sinkApp->GetTotalRx()<<std::endl;
          double throughput2 = ((double)stats.sinkApp->GetTotalRx()*8.0)/((double)  stats.duration*1e6);
          std::cout<<"WiiFi UD: "<<ipv4<<" Throughput "<<throughput2<<std::endl;
          totalWiFiRx += stats.sinkApp->GetTotalRx();
          std::cout<<"\n"<<std::endl;
          //dat_file_out << nn <<", \t " << simulationTime << ", \t "<< stats.sourceAddress  << ", \t " << stats.destinationAddress<<", \t "<< stats.duration<< ", \t "<<stats.perPacketDelay<<", \t "<< stats.perPacketJitter<<", \t "<< throughput2<<", \t "
          //<<stats.txPackets<<", \t "<<stats.rxPackets<< std::endl;
          dat_wifi_file_out << node->GetId() <<", \t " << Simulator::Now().GetSeconds() << ", \t "<< stats.sourceAddress  << ", \t " << stats.destinationAddress<<", \t "<< stats.duration<< ", \t "<<stats.perPacketDelay<<", \t "<< stats.perPacketJitter<<", \t "<< throughput2<<", \t "
          <<stats.txPackets<<", \t "<<stats.rxPackets<< std::endl;
      }
  }

  dat_wifi_file_out.close();

  std::stringstream m_CSVlififileNamestream0;
  m_CSVlififileNamestream0<<"WiFi-Network/"<<"LiFi-Mobile-Stas-"<<dataRate<<"-Nodes-"<<nn<<"-Sim_Time-"<<simulationTime;
  std::string m_CSVlififileName0 = m_CSVlififileNamestream0.str();
  std::cout<<"m_CSVlififileName "<<m_CSVlififileName0<<std::endl;
  std::ofstream dat_lifi_file_mobile_stas_out (m_CSVlififileName0.c_str (), std::ios::app);
  dat_lifi_file_mobile_stas_out << "Node_Id "<<"\t time " << "\t source " << "\t destination "<<"\t duration " << "\t Sum Delay " << "\t Sum Jitters " << "\t Throughput " <<"\t Rx Packet "<<"\t Tx Packet "<< std::endl;
  
  for(std::map<std::string, std::vector<LiFiWiFiFlowStats> >::iterator it = lifi_handover_Map.begin(); it != lifi_handover_Map.end(); ++it)
  {
      std::vector<LiFiWiFiFlowStats> statsVect = it->second;
      uint64_t lifiTotlRx = 0;
      for (size_t i = 0; i < statsVect.size(); i++)
      {
          std::cout<<"LiFi AP  "<< i<<" Data "<< "\n";

          LiFiWiFiFlowStats stats = statsVect[i];
          double throughput                  = stats.totalRxBytes/(double)(simulationTime*1e6);
          Ptr<PacketSink> packetSink         = performanceAnalysis->GetDestinationApplicationForIpAddress (stats.destinationAddress);
          std::cout<<"packetSink "<<packetSink<<" stats.destinationAddress "<<stats.destinationAddress<<std::endl;
          Ptr <Node> node                    = packetSink->GetNode();
          Ptr<Ipv4> ipv4A                    = node->GetObject<Ipv4> ();
          Ipv4Address currentDestination     = ipv4A->GetAddress(2,0).GetLocal();

          std::cout<<"LiFi Station Current Destination IP Address  "<<stats.destinationAddress<<" Handovered with Previous IP "<<currentDestination<<" source address: "<<stats.sourceAddress<<std::endl;
          std::cout<<"Sink App Throughput: "<<throughput<<" Flow Monitor Throughput "<<stats.throughput<<std::endl;
          std::cout<<"totalLostBytes "<<stats.totalLostBytes<<std::endl;
          std::cout<<"sunDelay "<<stats.sunDelay<<std::endl;
          std::cout<<"sumJitters "<<stats.sumJitters<<std::endl;
          std::cout<<"perPacketDelay "<<stats.perPacketDelay<<std::endl;
          std::cout<<"perPacketJitter "<<stats.perPacketJitter<<std::endl;
          std::cout<<"totalDroppedPackets "<<stats.totalDroppedPackets<<std::endl;
          std::cout<<"totalRetransmission "<<stats.totalRetransmission<<std::endl;
          std::cout<<"Total Bytes Received  "<<stats.totalRxBytes<<std::endl;
          double throughput1 = ((stats.totalRxBytes*8.0))/((double)(simulationTime-1.5)*1e6);
          std::cout<<"LiFi UD: "<<stats.destinationAddress<<" Throughput "<<throughput1<<std::endl;
          lifiTotlRx += stats.totalRxBytes;
          std::cout<<"\n"<<std::endl;
          //dat_file_out << nn <<", \t " << simulationTime << ", \t "<< stats.sourceAddress  << ", \t " << stats.destinationAddress<<", \t "<< stats.duration<< ", \t "<<stats.perPacketDelay<<", \t "<< stats.perPacketJitter<<", \t "<< throughput1<<", \t "
          //<<stats.txPackets<<", \t "<<stats.rxPackets<<", \t "<< std::endl;
          dat_lifi_file_mobile_stas_out << node->GetId() <<", \t " << Simulator::Now().GetSeconds() << ", \t "<< stats.sourceAddress  << ", \t " << stats.destinationAddress<<", \t "<< stats.duration<< ", \t "<<stats.perPacketDelay<<", \t "<< stats.perPacketJitter<<", \t "<< throughput1<<", \t "
          <<stats.txPackets<<", \t "<<stats.rxPackets<<", \t "<< std::endl;
      }
  }
  dat_lifi_file_mobile_stas_out.close();
  wifithroughput = ((double)totalWiFiRx*8.0)/(double)(1e6*simulationTime);
  std::cout<<"totalWiFiRx "<<totalWiFiRx<<"\nTotal WiFi Throughput: "<<wifithroughput<<std::endl;
}

void LiFiController::PerformanceMetrics ()
{
    std::string outputFileName = "Mobile User Device Data/lifi_user_ ";
    for (auto& itr: apUserDataModelMaps) 
    {
        // Do stuff
        std::unordered_map<uint32_t, Ptr<UserDeviceDataModel> >  userDeviceMap = itr.second;
        for (auto& itr1: userDeviceMap) 
        {
            Ptr<UserDeviceDataModel> userDeviceDataModel = itr1.second;
            if (!userDeviceDataModel->IsUserMobile()) continue;

            std::stringstream nodeId;
            nodeId<<userDeviceDataModel->GetUserId();
            //Plots for AP0
            std::ofstream outfileTh0 ((outputFileName+ "instant_throughput-" + nodeId.str() + "-0.plt").c_str ());
            Gnuplot gnuplot = Gnuplot ((outputFileName+ "instant_throughput-" + "-0.eps").c_str (), "Throughput");
            gnuplot.SetTerminal ("post eps color enhanced");
            gnuplot.SetLegend ("Sampling Time", "Throughput (Mb/s)");
            gnuplot.SetTitle ("Instant Through Over 100 ms");
            gnuplot.AddDataset (userDeviceDataModel->GetThroughputDatafile ());
            gnuplot.GenerateOutput (outfileTh0);

            std::ofstream outfileSNR0 ((outputFileName+ "instant_snr-" + nodeId.str() + "-0.plt").c_str ());
            gnuplot = Gnuplot ((outputFileName+ "instant_snr-" + "-0.eps").c_str (), "SNR");
            gnuplot.SetTerminal ("post eps color enhanced");
            gnuplot.SetLegend ("Time (seconds)", "SNR (dBm)");
            gnuplot.SetTitle ("Instant SNR Over 100 ms");
            gnuplot.AddDataset (userDeviceDataModel->GetSNRDatafile ());
            gnuplot.GenerateOutput (outfileSNR0);

            std::ofstream outfileBER0 ((outputFileName+ "instant_ber-" + nodeId.str() + "-0.plt").c_str ());
            gnuplot = Gnuplot ((outputFileName+ "instant_ber-" + "-0.eps").c_str (), "BER");
            gnuplot.SetTerminal ("post eps color enhanced");
            gnuplot.SetLegend ("Time (seconds)", "BER");
            gnuplot.SetTitle ("Instant BER Over 100 ms");
            gnuplot.AddDataset (userDeviceDataModel->GetBERDatafile ());
            gnuplot.GenerateOutput (outfileBER0);
        }       
    }
}

bool LiFiController::IsHandoverRequiredForDevice(std::vector<double> vec, uint32_t id)
{
  double sum = 0;
  if (vec.size() < 15)
  {
    return false;
  }
 
// std::cout<<"IsHandoverRequiredForDevice(std::vector<double> vec, uint32_t id) Time "<<Simulator::Now().GetSeconds()<<" node "<<id<<std::endl;
  for (size_t i = vec.size()-1; i >= vec.size()-12; i--)
  {
    //  std::cout<<(10*log(vec[i]))<<"  ";
     // if (vec[i] <= 0.1)
   //   {
        sum += 10*log(vec[i]);
    //  }
    //  else      
  }
  
  //std::cout<<"\naverage "<<(sum/15.0)<<"\n\n"<<std::endl;
  if ((sum/15.0) <= 16.50)
  {
    return true;
  }
 
  return false;
}

void LiFiController::KPI_Monitoring()
{
 for (auto& itr: apUserDataModelMaps) 
 {
      std::unordered_map<uint32_t, Ptr<UserDeviceDataModel> >  userDeviceMap = itr.second;   
      for(std::unordered_map <uint32_t, Ptr<UserDeviceDataModel> >::iterator it = userDeviceMap.begin(); it != userDeviceMap.end(); ++it)
      {

         Ptr<UserDeviceDataModel> userDeviceDataModel = it->second; 
         if (!userDeviceDataModel->IsUserMobile()) continue;
         
         std::vector<double> vec = userDeviceDataModel->GetHistoricalSNR();
         //std::cout<<"Sim Time "<<Simulator::Now().GetSeconds()<<" Historical Samples:"<<vec.size()<<" userDeviceDataModel id "<<userDeviceDataModel->GetSinkApplication()->GetNode()->GetId()<<"\n";
         //std::cout<<"At Sim Time "<<Simulator::Now().GetSeconds()<<" U_ID "<<userDeviceDataModel->GetUserId()<<" Historical Throughput Data: "<<std::endl;
         if (this->IsHandoverRequiredForDevice(vec,userDeviceDataModel->GetSinkApplication()->GetNode()->GetId()))
         {
           // std::cout<<"Initiate Handover for Device "<<userDeviceDataModel->GetUserId() <<" at time "<<Simulator::Now().GetSeconds()<<std::endl;
             //RegisterForHandoverForDevice(userDeviceDataModel);
        //  Simulator::Schedule (MilliSeconds (200), &LiFiController::RegisterForHandoverForDevice,this,userDeviceDataModel);
         }         
      }
  }

  Simulator::Schedule (MilliSeconds (200), &LiFiController::KPI_Monitoring,this); 
}

void LiFiController::CompleteHandover(Ptr<LiFiApMac> controller, std::vector<Ptr<Node> > devicesVector)
{
    for (size_t i = 0; i < devicesVector.size(); i++)
    {
        ns3::Ptr<LiFiChannel> channel = controller->GetChannel();
        ns3::Ptr<LiFiTxNetDevice> txDevice = ns3::DynamicCast < LiFiTxNetDevice> (channel->GetTxDevice());
        channel->RemoveUserDevice(devicesVector[i]->GetId());
        txDevice->RemoveRxDevice(devicesVector[i]->GetId());
    }
    PerformVerticalHandover (devicesVector);
}

void LiFiController::PerformVerticalHandover (std::vector<Ptr<Node> > devicesVector)
{
    for (size_t i = 0; i < devicesVector.size(); i++)
    {
         Ptr<UserDeviceDataModel> userDeviceDataModel = FindUserDeviceInMap(apUserDataModelMaps, devicesVector[i]->GetId());
         this->TransferApplicationLiFiToWiFi(userDeviceDataModel);
    }
}

void LiFiController::RegisterForHandoverForDevice(Ptr<UserDeviceDataModel> userDeviceDataModel)
{
  /*bool isNodeAttach = false;
  static int updateCache = 0;
  Ptr<LiFiNetDeviceHelper> helper;
  for (size_t i = 0; i < macHelperVector.size(); i++)
  {
    helper = macHelperVector[i];
    isNodeAttach = helper->HasThisNodeAttached(userDeviceDataModel->GetSinkApplication()->GetNode());
    if (isNodeAttach) break;
  }

  if (isNodeAttach) //clean up 
  {
    helper->MarkNodeForHandoverNodes(userDeviceDataModel->GetSinkApplication()->GetNode());
    if (updateCache == 0)
    {
         // helper->UpdateUserDeviceCache(userDeviceDataModel->GetSinkApplication()->GetNode());
          updateCache++;
    } 
  }*/
  //std::cout<<"isNodeAttached "<<isNodeAttach<<std::endl;
  //std::cout<<"PerformHandoverForDevice(Ptr<UserDeviceDataModel> userDeviceDataModel)   lifiNetworkNodesContainerList "<<lifiNetworkNodesContainerList.size()<<std::endl;
}

void LiFiController::RunSimulation (uint32_t nWifis, double totalTime, std::string rate)
{
  performanceAnalysis = CreateObject <PerformanceAnalysis> ();
  simulationTime = totalTime;

  Received.push_back(0);
  theTime.push_back(0);

  ReadUsersFromCSV(nWifis);
  CreateNodes ();
  CreateCsmaNodes();
  InternetServerNetDevices ();
  ConfigureLiFiNetwork ();
  ConfigureWiFiNetwork ();
  InstallInternetStack ();
 //	ModifyLifiCache ();
  //ConfigureUserDevicesCaches ();
  InstallLiFiNodesApplications(rate);
  //InstallWiFiNodesApplications(rate);
  FlowMonitorHelper m_flowmon;
  Ptr<FlowMonitor>  m_monitor;
  m_monitor = m_flowmon.InstallAll ();
  SetupMobility ();

  //Simulator::Schedule (MilliSeconds (2000), &LiFiController::KPI_Monitoring,this);
 // void InstanteneousThroughput (uint16_t nodes, std::string dataRate, bool isFirstIteration);
 //Simulator::Schedule (MilliSeconds (100), &LiFiController::InstanteneousThroughput,this,nWifis,rate,true);
  Simulator::Stop (Seconds (simulationTime+1));
  Simulator::Run ();

  AnalyzeData(&m_flowmon, m_monitor,rate,nWifis);
  
  PerformanceMetrics ();
  Simulator::Destroy ();
}

void LiFiController::CreateNodes ()
{
  internetServerNodes.Create (1);
  wifiapNodes.Create (1);
}

void LiFiController::CreateCsmaNodes ()
{
    NodeContainer WifiCsmaNodes;
    uint16_t j = 0;
    for (std::pair< std::vector<ns3::NodeContainer>::iterator, std::vector<std::vector<std::vector<double> > >::iterator> i(lifiNetworkNodesContainerList.begin(), lifiAPswithAssociatedUserPositions.begin() ); i.first != lifiNetworkNodesContainerList.end(); ++i.first, ++i.second)
    {
        ns3::NodeContainer nodeContainer = *i.first;
        NodeContainer lifiAP1CsmaNodes;

        for (size_t ii = 1; ii < (nodeContainer.GetN()); ii++)
        {
           lifiAP1CsmaNodes.Add(csmaClientNodes.Get(j));
           j++;
        }
        serverNodesContainerVector.push_back (lifiAP1CsmaNodes);
    }

    for (uint32_t i = 0; i < wifiUserNodes.GetN(); i++)
    {
      WifiCsmaNodes.Add(csmaClientNodes.Get(j));
      j++;
    }
    if (WifiCsmaNodes.GetN() > 0)
    serverNodesContainerVector.push_back(WifiCsmaNodes);
}

void LiFiController::LiFiDeviceCourseChangeU (Ptr<const MobilityModel> mob)
{
}

void LiFiController::SetupMobility ()
{
    MobilityHelper mobility;
    Ptr <ListPositionAllocator > m_listPosition1 = CreateObject<
    ListPositionAllocator>();
    m_listPosition1->Add(Vector(0.0, 0.0, 0.0));
    mobility.SetPositionAllocator(m_listPosition1);
    mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
    mobility.Install (wifiapNodes);
    if (wifiUserNodes.GetN() > 0)
    {
        mobility.Install (wifiUserNodes); 
    }

    //Mark Mobility type for each user in node container. Mobility type choices are constant velocity, randomwaypoint mobility model, and constant mobility model
    for (size_t i = 0; i < lifiNetworkNodesContainerList.size() ; i++)
    {
        ns3::NodeContainer nodeContainer =  lifiNetworkNodesContainerList[i];
        Ptr<LiFiNetDevice> apNetDevice = 0;
        for (size_t j = 0; j < nodeContainer.Get(0)->GetNDevices(); j++)
        {
            apNetDevice = ns3::DynamicCast < LiFiNetDevice>  (nodeContainer.Get(0)->GetDevice(j));
            if (apNetDevice)
            {
                break;
            }
        }
        
        std::vector<std::vector<double> > apPositionVector   = lifiAPswithAssociatedUserPositions[i];
        std::vector<double> apPosition = apPositionVector[0];
        Vector txPosition = Vector(apPosition[0], apPosition [1], apPosition[2]);
        //usrDevice->SetIsUserMbile (false);
        mobility.SetMobilityModel ("ns3::LiFiMobilityModel");
        mobility.Install(nodeContainer.Get(0));
        Ptr<ns3::LiFiMobilityModel>  lifiApMobilityModel  = ns3::DynamicCast < ns3::LiFiMobilityModel>  (nodeContainer.Get(0)->GetObject<MobilityModel>());
        lifiApMobilityModel->SetPosition(txPosition);        
        for (uint16_t j = 1; j < nodeContainer.GetN(); j++)
        {   
            Ptr<LiFiNetDevice> rxNetDev = ns3::DynamicCast < LiFiNetDevice>  (nodeContainer.Get(j)->GetDevice(0));
            std::vector<double> rxPositionVector = apPositionVector [j];
            if(j== 12232) 
            {
                Ptr<UserDeviceDataModel> usrDevice = FindUserDeviceInMap(apUserDataModelMaps, nodeContainer.Get(j)->GetId());
                usrDevice->SetIsUserMbile (true);
                mobility.SetMobilityModel ("ns3::LiFiConstantVelocityMobilityModel");
                mobility.Install(NodeContainer(rxNetDev->GetNode()));
                Ptr<ns3::LiFiConstantVelocityMobilityModel>  lifiConstantVelModel  = ns3::DynamicCast < ns3::LiFiConstantVelocityMobilityModel>  (rxNetDev->GetNode()->GetObject<MobilityModel>());
                Vector v = Vector(1.5,0.9,0.0);
                lifiConstantVelModel->SetVelocity(v);
                lifiConstantVelModel->SetPosition(Vector(rxPositionVector[0],rxPositionVector[1],rxPositionVector[2]));
            } 
            else if(j >=1  &&  j<=4)
            {
                Ptr<UserDeviceDataModel> usrDevice = FindUserDeviceInMap(apUserDataModelMaps,nodeContainer.Get(j)->GetId());
                //std::cout<<"usrDevice usrDevice "<<usrDevice<<std::endl;
                usrDevice->SetIsUserMbile (true);
                std::ostringstream xx_str;
                std::ostringstream yy_str;
                xx_str<<"ns3::UniformRandomVariable[Min="<<min_xx<<"|"<<"Max="<<max_xx<<"]";
                yy_str<<"ns3::UniformRandomVariable[Min="<<min_yy<<"|"<<"Max="<<max_yy<<"]";
                //pos.Set ("X", StringValue (-200.0|Max=200.0]"));
                //pos.Set ("Y", StringValue ("ns3::UniformRandomVariable[Min=-200.0|Max=200.0]"));
                bool usingBoxPositionAllocator = true;
                if (usingBoxPositionAllocator)
                {
                    ObjectFactory pos;
                    pos.SetTypeId ("ns3::GridPositionAllocator");
                    pos.Set ("GridWidth", UintegerValue (40));
                    pos.Set ("MinX", DoubleValue (x_dest));
                    pos.Set ("MinY", DoubleValue (y_dest));
                    pos.Set ("DeltaX", DoubleValue (0.9));
                    pos.Set ("DeltaY", DoubleValue (-0.8));
                    pos.Set ("LayoutType", EnumValue (GridPositionAllocator::ROW_FIRST));
                    /*ObjectFactory pos;
                    pos.SetTypeId ("ns3::RandomRectanglePositionAllocator");   
                    pos.Set ("X", StringValue (xx_str.str()));
                    pos.Set ("Y", StringValue (yy_str.str()));
                    pos.Set ("Z", DoubleValue (0.8));*/
                    std::cout<<"usingBoxPositionAllocator usingBoxPositionAllocator usingBoxPositionAllocator"<<std::endl;
                    Ptr<PositionAllocator> taPositionAlloc = pos.Create ()->GetObject<PositionAllocator> ();                
                    mobility.SetMobilityModel ("ns3::LiFiRandomWaypointMobilityModel",
                    "Pause", StringValue ("ns3::UniformRandomVariable[Min=1.0|Max=2.0]"),
                    "PositionAllocator", PointerValue (taPositionAlloc));
                    mobility.Install(nodeContainer.Get(j));
                    ns3::Ptr<LiFiRandomWaypointMobilityModel> lifiRandomWaypointMobilityModel =ns3::DynamicCast < ns3::LiFiRandomWaypointMobilityModel>  (rxNetDev->GetNode()->GetObject<MobilityModel>());
                    lifiRandomWaypointMobilityModel->SetPosition(Vector(rxPositionVector[0],rxPositionVector[1],rxPositionVector[2]));
                }
                else 
                {
                    ObjectFactory pos;
                    pos.SetTypeId ("ns3::RandomRectanglePositionAllocator");   
                    pos.Set ("X", StringValue (xx_str.str()));
                    pos.Set ("Y", StringValue (yy_str.str()));
                    pos.Set ("Z", DoubleValue (0.8));
                    /* ObjectFactory pos;
                    pos.SetTypeId ("ns3::GridPositionAllocator");
                    pos.Set ("GridWidth", UintegerValue (40));
                    pos.Set ("MinX", DoubleValue (vect1[0] + 4.5));
                    pos.Set ("MinY", DoubleValue (vect1[2] + 4.7));
                    pos.Set ("DeltaX", DoubleValue (0.9));
                    pos.Set ("DeltaY", DoubleValue (-0.8));
                    pos.Set ("LayoutType", EnumValue (GridPositionAllocator::ROW_FIRST));*/
                    Ptr<PositionAllocator> taPositionAlloc = pos.Create ()->GetObject<PositionAllocator> ();
                    mobility.SetMobilityModel ("ns3::LiFiRandomWaypointMobilityModel",
                    // "StartTime", DoubleValue (3.7),
                    "PositionAllocator", PointerValue (taPositionAlloc));
                    mobility.Install(nodeContainer.Get(j));
                    ns3::Ptr<LiFiRandomWaypointMobilityModel> lifiRandomWaypointMobilityModel =ns3::DynamicCast < ns3::LiFiRandomWaypointMobilityModel>  (rxNetDev->GetNode()->GetObject<MobilityModel>());
                    lifiRandomWaypointMobilityModel->SetPosition(Vector(rxPositionVector[0],rxPositionVector[1],rxPositionVector[2]));
                }
            }
            else 
            {
                //usrDevice->SetIsUserMbile (false);
                mobility.SetMobilityModel ("ns3::LiFiMobilityModel");
                mobility.Install(rxNetDev->GetNode());
                Ptr<ns3::LiFiMobilityModel>  lifiMobilityModel  = ns3::DynamicCast < ns3::LiFiMobilityModel>  (rxNetDev->GetNode()->GetObject<MobilityModel>());
                lifiMobilityModel->SetPosition(Vector(rxPositionVector[0], rxPositionVector [1], rxPositionVector[2]));
                //lificonstantMobilityNodes.Add(nodeContainer.Get(j));
            }
        }
    }
}

void LiFiController::InternetServerNetDevices ()
{
    //CSMA Helpers
    CsmaHelper csmaHelper;
    csmaHelper.SetChannelAttribute ("DataRate", StringValue ("400Mbps"));
    csmaHelper.SetChannelAttribute ("Delay", TimeValue (MicroSeconds (5)));
    csmaHelper.SetDeviceAttribute ("EncapsulationMode", StringValue ("Llc"));
    csmaHelper.SetDeviceAttribute ("Mtu", UintegerValue (1500));
    NodeContainer cont;
    cont.Add(wifiapNodes.Get(0));
    cont.Add(csmaClientNodes);
    serverNetDevices  = csmaHelper.Install (cont);
    wifiNodep2pNetDevicesVector.push_back(serverNetDevices.Get(0));
}

void
LiFiController::ConfigureLiFiNetwork ()
{
     for (std::vector<ns3::NodeContainer>::const_iterator i = lifiNetworkNodesContainerList.begin(); i != lifiNetworkNodesContainerList.end(); ++i)
     {
            ns3::NodeContainer nodeContainer = *i;
            PointToPointHelper pointToPoint;
            pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("400Mbps"));
            pointToPoint.SetChannelAttribute ("Delay", StringValue ("30us"));

            NetDeviceContainer p2pDevices;
            p2pDevices = pointToPoint.Install (NodeContainer(wifiapNodes,nodeContainer.Get(0)));

            wifiNodep2pNetDevicesVector.push_back(p2pDevices);
            struct LifiPhyParameters lifiPhyParameters;
            lifiPhyParameters.semiAngle = 60;
            lifiPhyParameters.photoDetectorArea = 1.0e-4;
            lifiPhyParameters.filterGain = 1.0;
            lifiPhyParameters.FOVAngle = 90.0;
            lifiPhyParameters.refractiveIndex = 1.5;
            lifiPhyParameters.dataRateInMBPS = 5.0;
            lifiPhyParameters.beta = 1.0;
            lifiPhyParameters.isVPPM = false;
            lifiPhyParameters.modulation = LiFiErrorModel::OOK;

 
            ns3::Ptr<ns3::LiFiPhyHelper> phyHelper = CreateObject <ns3::LiFiPhyHelper> ();
            phyHelper->Create(nodeContainer,lifiPhyParameters); 
            ns3::Ptr<ns3::LiFiChannelHelper> chHelper = CreateObject <ns3::LiFiChannelHelper> ();
            chHelper->Create(nodeContainer.Get(0));
            Ptr<LiFiPhy> aplifiPhy = phyHelper->GetPhy(nodeContainer.Get(0)->GetId());
            aplifiPhy->SetDeviceType("AP");
            chHelper->AddPhy(nodeContainer.Get(0),aplifiPhy);

            Ptr<LiFiMacHelper> lifiMacHelper = CreateObject <ns3::LiFiMacHelper> (); //::CreateAPMac (Ptr<Node> node, ns3::Ptr<LiFiPhy> lifiPhy)
            NetDeviceContainer apNetdevices = lifiMacHelper->CreateAPMac(nodeContainer.Get(0),phyHelper->GetPhy(nodeContainer.Get(0)->GetId()),chHelper);    
           
            NodeContainer lifiUserNodes;
            NetDeviceContainer userNetdevices;
            for (uint32_t j = 1; j < (nodeContainer.GetN()); j++)
            {
              lifiUserNodes.Add(nodeContainer.Get(j));
              Ptr<LiFiPhy> lifiPhy = phyHelper->GetPhy(nodeContainer.Get(j)->GetId());
              lifiPhy->TraceConnectWithoutContext("ChannelData", MakeCallback(&LiFiController::MonitorRxChannelStats,this));
              lifiPhy->SetDeviceType("UD");
              Ptr<LiFiChannel> LiFiChannel= chHelper->GetChannel(nodeContainer.Get(0)->GetId());
              lifiPhy->SetChannel(LiFiChannel);
             // LiFiChannel->AddPhy(lifiPhy);
              userNetdevices.Add(lifiMacHelper->CreateUserDeviceMac(nodeContainer.Get(j), lifiPhy));
            }  

            Ptr<LiFiHelper> lifiHelper = CreateObject <LiFiHelper>();
            lifiHelper->AssociateUserDevicesWithAP(lifiUserNodes,nodeContainer.Get(0),chHelper,phyHelper,lifiMacHelper);
            NetDeviceContainer lifiNetworkContainer;
            lifiNetworkContainer.Add(apNetdevices);
            lifiNetworkContainer.Add(userNetdevices);
            lifiNetworkNetDevicesContainerList.push_back(lifiNetworkContainer);
    }
}

void LiFiController::ConfigureWiFiNetwork ()
{
    std::string staManager = "ns3::MinstrelHtWifiManager";
    std::string apManager = "ns3::MinstrelHtWifiManager";
    uint32_t rtsThreshold = 65535;
    double maxPower = 59;
    double minPower = 50;
    uint32_t powerLevels = 10;
    Config::Set ("/NodeList/*/DeviceList/*/$ns3::WifiNetDevice/Phy/ChannelWidth", UintegerValue (80));
    Config::SetDefault ("ns3::WifiRemoteStationManager::FragmentationThreshold", StringValue ("5000"));
    Config::SetDefault ("ns3::WifiRemoteStationManager::RtsCtsThreshold", StringValue ("16000"));
    std::string phyRate = "VhtMcs6";                    /* Physical layer bitrate. */
    WifiApHelper wifi;
    wifi.SetStandard (WIFI_PHY_STANDARD_80211ac);
    /* Set up Legacy Channel */
    YansWifiChannelHelper channel;
    //Ptr<YansWifiChannel> wifiChannel = 0;
    channel.SetPropagationDelay ("ns3::ConstantSpeedPropagationDelayModel");
    channel.AddPropagationLoss ("ns3::FriisPropagationLossModel", "Frequency", DoubleValue (5e9));
    //wifiChannel = channel->GetChannel();
    /* Setup Physical Layer */       
    YansWifiPhyHelper phy = YansWifiPhyHelper::Default ();
    phy.Set ("ShortGuardEnabled", BooleanValue (1));
    // Set MIMO capabilities
    uint8_t nStreams = 1 + (9 / 8); //number of MIMO streams
    phy.Set ("Antennas", UintegerValue (nStreams));
    phy.Set ("MaxSupportedTxSpatialStreams", UintegerValue (nStreams));
    phy.Set ("MaxSupportedRxSpatialStreams", UintegerValue (nStreams));
    phy.SetChannel (channel.Create ());
    // phy.Set ("TxPowerLevels", UintegerValue (1));
    phy.Set ("TxGain", DoubleValue (0));
    phy.Set ("RxGain", DoubleValue (0));
    phy.Set ("RxNoiseFigure", DoubleValue (10));
    phy.Set ("EnergyDetectionThreshold", DoubleValue (-79 + 3));
    phy.SetErrorRateModel ("ns3::YansErrorRateModel");        
    Ssid ssid = Ssid ("IEEE802ac");
    WifiMacHelper mac;
    NetDeviceContainer staDevices;
    NetDeviceContainer appDevices;
    bool hybrid = true;//devices equiped with Lifi and WiFi devices
    NetDeviceContainer  wifionlstaDevices;
    if (hybrid)
    {
        ns3::NodeContainer hybridNodes;
        for (size_t i = 0; i < lifiNetworkNodesContainerList.size(); i++)
        {  
            ns3::NodeContainer nodeContainer = lifiNetworkNodesContainerList[i];
            for (uint32_t j = 1;j < nodeContainer.GetN(); j++)
            {
                hybridNodes.Add(nodeContainer.Get(j));
            }
        }

        wifi.SetRemoteStationManager (staManager, "RtsCtsThreshold", UintegerValue (rtsThreshold));
        phy.Set ("TxPowerStart", DoubleValue (maxPower));
        phy.Set ("TxPowerEnd", DoubleValue (maxPower));
        mac.SetType ("ns3::StaWifiMac", "ActiveProbing", BooleanValue (false),"Ssid", SsidValue (ssid));
        wifionlstaDevices = wifi.Install (phy, mac, wifiUserNodes);
        staDevices = wifi.Install (phy, mac, hybridNodes);
        wifi.SetRemoteStationManager (apManager, "RtsCtsThreshold", UintegerValue (rtsThreshold));
        phy.Set ("TxPowerStart", DoubleValue (minPower));
        phy.Set ("TxPowerEnd", DoubleValue (maxPower));
        phy.Set ("TxPowerLevels", UintegerValue (powerLevels));
        mac.SetType ("ns3::ApWifiMac", "Ssid", SsidValue (ssid));
        appDevices = wifi.Install (phy, mac, wifiapNodes.Get(0));
        wifiNetDevices.Add(appDevices.Get(0));
        wifiNetDevices.Add(wifionlstaDevices);
        wifiNetDevices.Add(staDevices);
        //wifiChannel->SetWifiNetDevive(appDevices.Get(0));
        //wifiChannel->TraceConnectWithoutContext("ChannelData", MakeCallback(&LiFiController::MonitorWifiRxDataCallback,this));
        for (uint32_t z = 0; z < staDevices.GetN(); z++)
        {
            Ptr<ns3::WifiApNetDevice> wifiApDev = DynamicCast <ns3::WifiApNetDevice> (staDevices.Get(z));
            wifiApDev->SetGatewayMacAddress (appDevices.Get(0)->GetAddress());
            wifiNetDevices.Add(wifiApDev);
        }

        for (size_t i = 0; i < wifiNetDevices.GetN(); i++)
        {
            ns3::Ptr<YansWifiPhy> phyPtr;
            ns3::Ptr<WifiNetDevice> netDev = ns3::DynamicCast <WifiNetDevice> (wifiNetDevices.Get(i));
            if (netDev != 0)
            {
                phyPtr = ns3::DynamicCast<YansWifiPhy> (netDev->GetPhy());
                phyPtr->TraceConnectWithoutContext("ChannelData", MakeCallback(&LiFiController::MonitorWiFiRxChannelStats,this));
            }
            else 
            {
                Ptr<ns3::WifiApNetDevice> wifiApDev = DynamicCast <ns3::WifiApNetDevice> (wifiNetDevices.Get(i));
                phyPtr = ns3::DynamicCast<YansWifiPhy> (wifiApDev->GetPhy());
                phyPtr->TraceConnectWithoutContext("ChannelData", MakeCallback(&LiFiController::MonitorWiFiRxChannelStats,this));
            }           
        }
    }
    else
    {
        mac.SetType ("ns3::StaWifiMac","ActiveProbing", BooleanValue (true),"Ssid", SsidValue (ssid));
        staDevices = wifi.Install (phy, mac, allUserNodes);
        mac.SetType ("ns3::ApWifiMac", "Ssid", SsidValue (ssid));
        appDevices = wifi.Install (phy, mac, wifiapNodes.Get(0));
        wifiNetDevices.Add(appDevices.Get(0));
    }
}

void LiFiController::ReadUsersFromCSV (uint16_t num_nodes)
{

    uint16_t num_lifi_aps = 1;
  
//load APs
  //  uint16_t num_nodes = 1;
    csmaClientNodes.Create(num_nodes);

    std::vector <std::string> filesStringVect;
    filesStringVect.push_back("NetworkPositions/lifi-ap1.txt");
    //filesStringVect.push_back("NetworkPositions/lifi-ap2.txt");
    //filesStringVect.push_back("UsersAndAPs/LifiAP3andUsersPoistions.txt");
    //filesStringVect.push_back("UsersAndAPs/LifiAP4andUsersPoistions.txt");
    //filesStringVect.push_back("UsersAndAPs/LifiAP5andUsersPoistions.txt");
    filesStringVect.push_back("NetworkPositions/wifi-ap.txt");

    //open pointers to file containing positions for different access points..
    std::vector <std::fstream*> filesStreamVector;
    for (uint32_t i = 0; i < filesStringVect.size();i++ )
    {
         std::fstream* myFile  = new std::fstream(filesStringVect[i].c_str());
         filesStreamVector.push_back(myFile);
    }

    //Parse Positions from pointers to lists
    std::vector<std::vector<std::vector<double> > > vect;
    for (uint32_t i = 0; i < filesStreamVector.size();++i)
    {
        std::string line;
        std::fstream* myFile = (filesStreamVector [i]);
        std::vector<std::vector<double> > position;
        while ( getline(*myFile , line ) )
        {
            std::istringstream is( line );
            position.push_back(std::vector<double>( std::istream_iterator<double>(is),std::istream_iterator<double>() ) );
        }
        vect.push_back(position);
 	    myFile->close();
     }
      
    //WiFi users
    uint64_t lifiUsers;
    uint64_t total_wifi_users;
    uint64_t numUserPerAp;
    uint64_t residuals;
    bool usingWiFiAsAPrimaryNetwork = false;
    if (usingWiFiAsAPrimaryNetwork)
    {
        lifiUsers = floor(num_nodes/5.0);
        total_wifi_users =  num_nodes - lifiUsers;
        numUserPerAp = lifiUsers/num_lifi_aps;
    }
    else 
    {
        //LiFi Users
        total_wifi_users = floor(num_nodes/5.0);
        lifiUsers = num_nodes - total_wifi_users;
        numUserPerAp = lifiUsers/num_lifi_aps;
    }

    std::vector<std::vector<std::vector<double> > >  lifiAps;
    for (std::vector<std::vector<std::vector<double> > >::const_iterator i = vect.begin(); i != vect.end(); ++i)
    {
        std::vector<std::vector<double> > apPosition = *i;
        std::vector<std::vector<double> > apVect;
        for (std::vector<std::vector<double> >::const_iterator ii = apPosition.begin(); ii != apPosition.end(); ++ii)
        {
                std::vector <double>  posVector = *ii;
                apVect.push_back(posVector);
        }
        lifiAps.push_back(apVect);
        //std::cout<<"apVect apVect "<<apVect.size()<<std::endl;
    }

    std::cout<<"Number of LiFi  "<<lifiAps.size()<<std::endl;
    if (lifiUsers <= (lifiAps.size()-1)) 
        residuals = 0;
    else
        residuals  = lifiUsers%(lifiAps.size()-1);

     // std::cout<<lifiAps.size()<<" total_users "<<num_nodes<<" total_wifi_users "<<total_wifi_users<<" lifiUsers "<<lifiUsers<<" numUserPerAp "<<numUserPerAp<<" residuals "<<residuals<<std::endl;
    if ((lifiUsers) <= (filesStringVect.size()-1))
    {
        for (uint64_t i = 0; i < (lifiUsers); i++)
        {
          std::vector<std::vector<double> > posVectList;
          std::vector<std::vector<double> >  vct = lifiAps[i];
          posVectList.push_back(vct[0]);
          posVectList.push_back(vct[1]);
          lifiAPswithAssociatedUserPositions.push_back (posVectList);
          NodeContainer lifinodesContainer;
          lifinodesContainer.Create (2);
          lifiNetworkNodesContainerList.push_back(lifinodesContainer);
        }
    }
      else
      {
        for (size_t i = 0; i < (lifiAps.size()-1); i++)
        {
          std::vector<std::vector<double> > posVectList;
          std::vector<std::vector<double> >  vect = lifiAps [i];
          uint16_t j = 0;
          for (; j < (numUserPerAp+1); j++)
          {
            std::vector<double> vv = vect[j];
        //   std::cout<<"vect "<<vect.size()<<" lifi pos vect "<<vv[0]<<" "<<vv[1]<<" "<<vv[2]<<std::endl;
              posVectList.push_back(vect[j]);
          }

          if (residuals > 0)
          {
            std::vector<double> vect1 = vect[j];
            //std::cout<<"residuals "<<vect1[0]<<" "<<vect1[1]<<" "<<vect1[2]<<std::endl;
            posVectList.push_back(vect1);
            residuals--;
          }

          lifiAPswithAssociatedUserPositions.push_back (posVectList);
          NodeContainer lifinodesContainer;
          lifinodesContainer.Create (posVectList.size());
          lifiNetworkNodesContainerList.push_back(lifinodesContainer);
        }
      }

      std::vector<std::vector<double> > wifiusrs = lifiAps[lifiAps.size()-1];
      if (total_wifi_users > 0)
      {
          for (uint16_t i = 0; i < (total_wifi_users); i++)
          {
              std::vector<double> v = wifiusrs [i];
              //std::cout<<"wifi users "<<v[0]<<" "<<v[1]<<" "<<v[2]<<std::endl;
              wifiUsersPositios.push_back(v);
              NodeContainer lifinodesContainer;
              lifinodesContainer.Create (1);
              wifiUserNodes.Add(lifinodesContainer);
          }
        }
}

void
LiFiController::InstallInternetStack ()
{
   InternetStackHelper stack;
   stack.InstallAll ();
   Ipv4StaticRoutingHelper ipv4RoutingHelper;
   Ipv4AddressHelper ipv4;
   ipv4.SetBase("10.1.0.0", "255.255.255.0"); //Server Nodes ----> wifi node
   serversIpv4interfaceContainer = ipv4.Assign(serverNetDevices);
    //configure default route in server nodes
   for (uint32_t z = 1; z < serversIpv4interfaceContainer.GetN();z++)
   {
     Ptr < Ipv4StaticRouting > staticRoutingHandleUserDevice =  ipv4RoutingHelper.GetStaticRouting(serverNetDevices.Get(z)->GetNode()->GetObject<Ipv4>());
     staticRoutingHandleUserDevice->SetDefaultRoute(serversIpv4interfaceContainer.GetAddress(0,0), 1,1); //This block is for sending from WIFI AP to the MT Ipv4Address
   }

   ipv4.SetBase("10.1.1.0", "255.255.255.0"); //Set up WiFi Network 
   wifitocsmauserinterfaceContainer = ipv4.Assign(wifiNetDevices);

   Ptr < Ipv4StaticRouting > staticRoutingWifiAP   = ipv4RoutingHelper.GetStaticRouting(wifiapNodes.Get(0)->GetObject<Ipv4>());
   
  std::string ipFirstTwoByteString = "10.1.";
  std::string ipLastByteString = ".0";

  int networkOctate = 2;
  int lifiApIndex 			=  1;//j
  Ptr <Node> wifiApNode                 =  wifiapNodes.Get(0);
  Ptr<Ipv4> wifiApIpObject              =  wifiApNode->GetObject<Ipv4> (); //ipv4A
  uint32_t p2pLinkIn = 1;
  for (size_t i = 0; i < lifiNetworkNetDevicesContainerList.size(); i++)
  {
      ns3::NetDeviceContainer netDevicesContainerList = lifiNetworkNetDevicesContainerList[i];
      
    ns3::NetDeviceContainer lifiAPtoWiFinetContainer  = wifiNodep2pNetDevicesVector[p2pLinkIn];
      //The following sets up address bases for output net devices on the nodes so as to identify them on a routing table as we do
      //Configure IP address for WiFi to LiFi AP (i.e. poi)
      std::ostringstream strg;
      strg<< networkOctate;
      std::string ipString = ipFirstTwoByteString+strg.str()+ipLastByteString;
      networkOctate++;
      ipv4.SetBase(ipString.c_str(), "255.255.255.0");//WiFi AP Node -----> LiFi AP Node
      //Setup Network Between WiFi AP and Each LiFi AP using Point-to-Point Link
      //NetDeviceContainer wifitolifinetwork                   = NetDeviceContainer (wifiNodep2pNetDevices.Get(lifiApIndex),lifiAPnetContainer.Get(0));
      Ipv4InterfaceContainer wifitolifinetinterfaceContainer = ipv4.Assign(lifiAPtoWiFinetContainer);//WiFi AP Node -----> LiFi AP assigned IP addresses container

      //Configure Net Devices Container for LiFi Networks
      //NetDeviceContainer lifitoTDMANetDevices  = NetDeviceContainer(lifiApNetContainer, lifiUsernetContainer);
      //Configure IP address for LiFi Networks
      std::ostringstream strg1;
      strg1<< networkOctate;;
      ipString = ipFirstTwoByteString+strg1.str()+ipLastByteString;
      networkOctate++;
      ipv4.SetBase(ipString.c_str(), "255.255.255.0");
      ///LiFi AP Node -----> User Nodes for TDMA
      //std::cout<<"lifitoTDMANetDevices size "<<netDevicesContainerList.GetN()<<" lifiAPtoWiFinetContainer size "<<lifiAPtoWiFinetContainer.GetN()<<std::endl;
      lifiAPNetDevicesInterface = ipv4.Assign(netDevicesContainerList);
      Ptr<ns3::PointToPointNetDevice> wifiNetDevice = ns3::DynamicCast < ns3::PointToPointNetDevice> (lifiAPtoWiFinetContainer.Get(0));
      int32_t interfaceIndex                  = wifiApIpObject->GetInterfaceForDevice (wifiNetDevice);
      Ptr<Ipv4L3Protocol> nodeIpv4L3Protocol  = wifiApNode->GetObject<Ipv4L3Protocol> ();
      Ptr<Ipv4Interface> ipv4Interface        = nodeIpv4L3Protocol->GetInterface (interfaceIndex);
      //std::cout<<"ipString "<<ipString<<" lifiUsernetContainer "<<netDevicesContainerList.GetN()<<" wifitolifinetinterfaceContainer.GetAddress(1,0) "<<wifitolifinetinterfaceContainer.GetAddress(1,0)<<" interfaceIndex "<<interfaceIndex<<" lifiAPtoWiFinetContainer "<<lifiAPtoWiFinetContainer.GetN()<<"wifiapNodes "<<wifiNetDevice->GetNode()->GetId() <<std::endl;
      staticRoutingWifiAP->AddNetworkRouteTo (Ipv4Address (ipString.c_str()),Ipv4Mask("/24"),wifitolifinetinterfaceContainer.GetAddress(1,0),interfaceIndex,1);
      //Configure Static Routes on User Devices
      for (uint32_t z = 1; z < netDevicesContainerList.GetN();z++)
      {
          Ptr < Ipv4StaticRouting > staticRoutingHandleUserDevice =  ipv4RoutingHelper.GetStaticRouting(netDevicesContainerList.Get(z)->GetNode()->GetObject<Ipv4>());
          staticRoutingHandleUserDevice->SetDefaultRoute(lifiAPNetDevicesInterface.GetAddress(0,0), 1,1); //This block is for sending from WIFI AP to the MT Ipv4Address
      }
      lifiApIndex++;
      p2pLinkIn++;
    }
}

void LiFiController::ModifyLifiCache (void)
{
  int index = 0;
  for (std::vector<ns3::NodeContainer>::const_iterator i = lifiNetworkNodesContainerList.begin(); i != lifiNetworkNodesContainerList.end(); ++i)
  {
	 ns3::NodeContainer nodeContainer = *i;
   if (nodeContainer.GetN() == 1) continue;

    //Ptr<LiFiNetDeviceHelper> tdma = macHelperVector[index];
    //tdma->ModifyLifiAPCache(nodeContainer);
    index++;
  }
}

void LiFiController::ConfigureUserDevicesCaches (void)
{
  int index = 0;
  for (std::vector<ns3::NodeContainer>::const_iterator i = lifiNetworkNodesContainerList.begin(); i != lifiNetworkNodesContainerList.end(); ++i)
  {
	ns3::NodeContainer nodeContainer       	 = *i;
  if (nodeContainer.GetN() <= 1) continue;

  //Ptr<LiFiNetDeviceHelper> tdma = macHelperVector[index];
  //tdma->ConfigureUserDevicesCaches(nodeContainer, wifiapNodes,wifitocsmauserinterfaceContainer);
  index++;
  }
}

void LiFiController::InstallLiFiNodesApplications (std::string rate)
{
  std::string dataRate = rate;                  /* Application layer datarate. */
  uint32_t payloadSize = 1448;
  std::map <std::string, std::vector<ApplicationContainer> > applicationMap;
  std::vector<uint16_t> wifiSinkPortsVector;
  wifiSinkPortsVector.push_back (900);

  std::vector<uint16_t> lifiSinkPortsVector;
  lifiSinkPortsVector.push_back(100);
  lifiSinkPortsVector.push_back(200);
  lifiSinkPortsVector.push_back(300);

  const double min = 0.3;
  const double max = 0.7;
  Ptr<UniformRandomVariable> uniform = CreateObject<UniformRandomVariable> ();
  uniform->SetAttribute ("Min", DoubleValue (min));
  uniform->SetAttribute ("Max", DoubleValue (max));

  uint16_t pi = 0;
  
  for (size_t i = 0; i < lifiNetworkNodesContainerList.size(); i++)
    {
  	 ns3::NodeContainer nodeContainer = lifiNetworkNodesContainerList[i];
     ns3::NodeContainer lifiNodesContainer;
  	 for (uint32_t j = 1; j < (nodeContainer.GetN()); j++)
  	 {
  		   lifiNodesContainer.Add(nodeContainer.Get(j));
  	 }

    NodeContainer server_Nodes =  serverNodesContainerVector[pi];
    if (server_Nodes.GetN() == 0 || lifiNodesContainer.GetN() == 0) continue;

    std::string protocolTypeStr = "ns3::UdpSocketFactory";
    uint16_t port = lifiSinkPortsVector[pi];

    PacketSinkHelper staSinkAppHelper (protocolTypeStr, Address (InetSocketAddress (Ipv4Address::GetAny (), port)));
    ApplicationContainer lifiSinkApps  = staSinkAppHelper.Install (lifiNodesContainer);
    lifiSinkApps.Start (Seconds (0.1));
    lifiSinkApps.Stop (Seconds (simulationTime+1));
    performanceAnalysis->SetLiFiSinkApplicationContainer(lifiSinkApps);
    std::ostringstream stream;
    stream<<port;
    std::string portStr = stream.str();
    performanceAnalysis->SetLiFiKeys(portStr);
    std::unordered_map<uint32_t, Ptr<UserDeviceDataModel> > userDataModelMap;
    for (uint32_t i = 0; i < server_Nodes.GetN(); i++)
    {
       Ptr <Node> node                        = lifiNodesContainer.Get(i);
       Ptr<Ipv4> ipv4A                        = node->GetObject<Ipv4> ();
       Ipv4Address ipv4                       = ipv4A->GetAddress(2,0).GetLocal();

       OnOffHelper sourceAppHelper (protocolTypeStr, (InetSocketAddress (ipv4,port)));
       sourceAppHelper.SetAttribute ("PacketSize", UintegerValue (payloadSize));
       sourceAppHelper.SetAttribute ("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1]"));//onNormRandVariable"));//StringValue ("ns3::ExponentialRandomVariable[Mean=0.8]"));
       sourceAppHelper.SetAttribute ("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));//offNormRandVariable);_//StringValue ("ns3::ExponentialRandomVariable[Mean=0.5]"));
       sourceAppHelper.SetAttribute ("DataRate", DataRateValue (DataRate (dataRate)));
       Ptr <Node> node1                        = server_Nodes.Get(i);
       Ptr<Ipv4> ipv4A1                        = node1->GetObject<Ipv4> ();
       ApplicationContainer serverApps = sourceAppHelper.Install (server_Nodes.Get(i));
    
       serverApps.Get(0)->SetStartTime (Seconds (uniform->GetValue()));
       serverApps.Stop (Seconds (simulationTime+0.5));

       ns3::Ptr<UserDeviceDataModel > userDeviceDataModel = CreateObject<UserDeviceDataModel>();
       userDeviceDataModel->SetDestinationAddress (ipv4);
       userDeviceDataModel->SetSinkApplication (DynamicCast<PacketSink>(lifiSinkApps.Get(i)));
       userDeviceDataModel->SetSourceApplication(DynamicCast<ns3::OnOffApplication>(serverApps.Get(0)));
       userDeviceDataModel->AddLastRxBytes (0);
       userDeviceDataModel->SetDestinationPort(port);
       userDataModelMap.insert(std::make_pair(node->GetId(), userDeviceDataModel));
    }
    apUserDataModelMaps.insert(std::make_pair(portStr, userDataModelMap));
    std::cout<<std::endl;
    pi++;
   }
}

void LiFiController::InstallWiFiNodesApplications (std::string rate)
{
  std::string dataRate = rate; /* Application layer datarate. */
  uint32_t payloadSize = 1448;
  Ptr<UniformRandomVariable> uniform = CreateObject<UniformRandomVariable> ();
  const double min = 0.7;
  const double max = 1.0;
  uniform->SetAttribute ("Min", DoubleValue (min));
  uniform->SetAttribute ("Max", DoubleValue (max));
  // uint16_t port = 9000;
  NodeContainer wifiNodes;
  wifiNodes.Add(wifiUserNodes);
  if (wifiNodes.GetN() == 0) return;
  uint32_t ind = serverNodesContainerVector.size();
  NodeContainer server_Nodes =  serverNodesContainerVector[ind-1];
  ApplicationContainer sinkApps;
  uint32_t i = 0;
  std::unordered_map<uint32_t, Ptr<UserDeviceDataModel> > userDataModelMap;
   /*for (; i < server_Nodes.GetN()/2; i++)
      {
        PacketSinkHelper staSinkAppHelper ("ns3::UdpSocketFactory", Address (InetSocketAddress (Ipv4Address::GetAny (), port)));

        //std::cout<<"wifi server_Nodes server_Nodes "<<server_Nodes.GetN()<<" wifiNodes "<<wifiNodes.GetN()<<std::endl;
        ApplicationContainer csmaSinks  = staSinkAppHelper.Install (server_Nodes.Get(i));
        sinkApps.Add(csmaSinks);
        Ptr <Node> node                        = server_Nodes.Get(i);
        Ptr<Ipv4> ipv4A                        = node->GetObject<Ipv4> ();
        Ipv4Address ipv4                       = ipv4A->GetAddress(1,0).GetLocal();
        std::cout<<"index "<<i<<" Node ID "<<node->GetId()<<" wifi stations as ipv4 destination "<<ipv4<<std::endl;
        OnOffHelper sourceAppHelper ("ns3::UdpSocketFactory", (InetSocketAddress (ipv4,port)));
        sourceAppHelper.SetAttribute ("PacketSize", UintegerValue (payloadSize));
        sourceAppHelper.SetAttribute ("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1]"));//RandomVariable(NormalVariable(0.8,0.045, 0.90)));//StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
        sourceAppHelper.SetAttribute ("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));//RandomVariable(NormalVariable(0.6,0.095, 0.70)));//StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
        sourceAppHelper.SetAttribute ("DataRate", DataRateValue (DataRate (dataRate)));

        
        ApplicationContainer serverApps = sourceAppHelper.Install (wifiNodes.Get(i));
        serverApps.Start (Seconds (uniform->GetValue()));
        serverApps.Stop (Seconds (simulationTime+1));
      }*/

      for (; i<server_Nodes.GetN(); i++)
      {
        PacketSinkHelper staSinkAppHelper ("ns3::UdpSocketFactory", Address (InetSocketAddress (Ipv4Address::GetAny (), wifi_port)));
        ApplicationContainer wifiSinks  = staSinkAppHelper.Install (wifiNodes.Get(i));
        sinkApps.Add(wifiSinks);
        Ptr <Node> node                        = wifiNodes.Get(i);
        Ptr<Ipv4> ipv4A                        = node->GetObject<Ipv4> ();
        Ipv4Address ipv4                       = ipv4A->GetAddress(1,0).GetLocal();
        OnOffHelper sourceAppHelper ("ns3::UdpSocketFactory", (InetSocketAddress (ipv4,wifi_port)));
       // wifi_port = port;
        sourceAppHelper.SetAttribute ("PacketSize", UintegerValue (payloadSize));
        sourceAppHelper.SetAttribute ("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1]"));//RandomVariable(NormalVariable(0.8,0.045, 0.90)));//StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
        sourceAppHelper.SetAttribute ("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));//RandomVariable(NormalVariable(0.6,0.095, 0.70)));//StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
        sourceAppHelper.SetAttribute ("DataRate", DataRateValue (DataRate (dataRate)));
       
        ApplicationContainer serverApps = sourceAppHelper.Install (server_Nodes.Get(i));
        ns3::Ptr<UserDeviceDataModel > userDeviceDataModel = CreateObject<UserDeviceDataModel>();
        userDeviceDataModel->SetDestinationAddress (ipv4);
        userDeviceDataModel->SetSinkApplication (DynamicCast<PacketSink>(wifiSinks.Get(0)));
        userDeviceDataModel->SetSourceApplication(DynamicCast<ns3::OnOffApplication>(serverApps.Get(0)));
        userDeviceDataModel->AddLastRxBytes (0);
        userDeviceDataModel->SetDestinationPort(wifi_port);
        userDataModelMap.insert(std::make_pair(node->GetId(), userDeviceDataModel));
        serverApps.Start (Seconds (uniform->GetValue()));
        serverApps.Stop (Seconds (simulationTime+1));
      }

      sinkApps.Start (Seconds (0.3));
      //sinkApps.Stop (Seconds (simulationTime+1));
      performanceAnalysis->SetWiFiStaSinkApplicationContainer(sinkApps);
      std::ostringstream stream;
      stream<<wifi_port;
      std::string portStr = stream.str();
      wifi_UserDataModelMaps.insert(std::make_pair(portStr, userDataModelMap));
      performanceAnalysis->SetWiFiKeys(portStr);
}

void LiFiController::TransferApplicationLiFiToWiFi(Ptr<UserDeviceDataModel> userDeviceDataModel)
{
  std::cout<<"void LiFiController::TransferApplicationLiFiToWiFi(Ptr<UserDeviceDataModel> userDeviceDataModel) "<<std::endl;
  //std::string dataRate = "4mbps"; /* Application layer datarate. */
  //uint32_t payloadSize = 1448;
  Ptr<OnOffApplication> onOffApplication = userDeviceDataModel->GetSourceApplication();
  Ptr<PacketSink> sinkApplication        = userDeviceDataModel->GetSinkApplication();
  //sinkApplication->SetStopTime(Seconds (Simulator::Now().GetSeconds() + 0.05));
  onOffApplication->ForceStop (Time(Seconds(0.1)));
  sinkApplication->ForceStop(Time(Seconds(0.3)));

  Ptr <Node> sink_node                   = sinkApplication->GetNode() ;
  Ptr<Ipv4> sink_ipv4A                   = sink_node->GetObject<Ipv4> ();
  Ipv4Address sink_ipv4                  = sink_ipv4A->GetAddress(1,0).GetLocal();
  userDeviceDataModel->SetDestinationPort(wifi_port);
  Address sinkaddr(InetSocketAddress (sink_ipv4, userDeviceDataModel->GetDestinationPort()));
  sinkApplication->SetAttribute ("Local", AddressValue(sinkaddr));
    
  /*Ptr <Node> source_node                 = onOffApplication->GetNode() ;
  Ptr<Ipv4> source_ipv4A                 = source_node->GetObject<Ipv4> ();
  Ipv4Address source_ipv4                = source_ipv4A->GetAddress(1,0).GetLocal();
*/
  Address sourceAddr(InetSocketAddress (sink_ipv4, userDeviceDataModel->GetDestinationPort()));

  //WiFiCache (wifiapNodes, sink_node);
  onOffApplication->SetAttribute ("Remote", AddressValue(sourceAddr));
  double stopTime = simulationTime - Simulator::Now().GetSeconds();
  sinkApplication->UpdateStartAndStop (Time(Seconds(0.6)), Time(stopTime + 1.0));
  onOffApplication->UpdateStartAndStop (Time(Seconds(0.7)), Time(stopTime + 0.8));
  //std::cout<<"Simulation Time "<<Simulator::Now().GetSeconds()<<" source_node id "<<source_node->GetId()<<" Sink ID "<<sinkApplication->GetNode()->GetId()<<"  WiFi AP Node ID "<<wifiapNodes.Get(0)->GetId()<<" onOffApplication Node "<<onOffApplication->GetNode()->GetId()<<" TransferApplicationLiFiToWiFi sink_ipv4 "<<sink_ipv4<<" source_ipv4 "<<source_ipv4<<" Node ID "<<sinkApplication->GetNode()->GetId()<<std::endl;

  Ptr<UserDeviceDataModel> userDeviceDataModel1  = FindUserDeviceInMap(apUserDataModelMaps, sink_node->GetId());
  TransferDataModelFromLiFiToWiFi(userDeviceDataModel1);
  std::ostringstream stream;
  stream<<userDeviceDataModel->GetDestinationPort();
  std::string keyStr =  stream.str();
 
  performanceAnalysis->SwitchUserFromLiFiToWifi (keyStr,sinkApplication);
}

void LiFiController::WiFiCache (NodeContainer wifiapNodes, Ptr<Node> node)
{
  Ptr<WifiNetDevice> wifiApNetDevice;
  for (uint32_t i = 0; i <(wifiapNodes.Get(0)->GetNDevices()); i++) {
      wifiApNetDevice =  ns3::DynamicCast < WifiNetDevice> (wifiapNodes.Get(0)->GetDevice(i));
      if (wifiApNetDevice != 0) break;
  }

  Ptr<Ipv4L3Protocol> nodeIpv4L3Protocol = wifiapNodes.Get(0)->GetObject<Ipv4L3Protocol> ();
  Ptr<Ipv4> ipv4A                        = wifiapNodes.Get(0)->GetObject<Ipv4> ();

  int32_t interfaceIndex                 = ipv4A->GetInterfaceForDevice (wifiApNetDevice);
  Ptr<Ipv4Interface> ipv4Interface       = nodeIpv4L3Protocol->GetInterface (interfaceIndex);
  Ptr<ArpCache> wifiuserDeviceArpCache    = ipv4Interface->GetArpCache ();
wifiuserDeviceArpCache->Flush();
  
  Ptr <Node> sink_node                   = node;
  Ptr<Ipv4> sink_ipv4A                   = sink_node->GetObject<Ipv4> ();
  Ipv4Address sink_ipv4                  = sink_ipv4A->GetAddress(1,0).GetLocal();
  ArpCache::Entry *  cacheEntry          = wifiuserDeviceArpCache->Add (sink_ipv4);

  Ptr<WifiApNetDevice> wifiuserNetDevice;
  for (uint32_t i = 0; i <(node->GetNDevices()); i++) {
      wifiuserNetDevice =  ns3::DynamicCast < WifiApNetDevice> (node->GetDevice(i));
      if (wifiuserNetDevice != 0) break;
  }
  cacheEntry->SetMacAddress (wifiuserNetDevice->GetAddress());
 // std::cout<<"void LiFiController::WiFiCache (NodeContainer wifiapNodes, Ptr<Node> node) Mac "<<wifiuserNetDevice->GetAddress()<<" sink_ipv4 "<<sink_ipv4<<std::endl;
}


ns3::ChannelStats LiFiController::AveragePerformanceMetrics (std::vector<ns3::ChannelStats> chStatsVect)
{
  ns3::ChannelStats agg_chStats;
  std::stringstream m_CSVfileNamestream;
  m_CSVfileNamestream<<"LiFi-Network/LiFi-Channel_Stats_For AP_1";
  std::string cSVfileNamestream = m_CSVfileNamestream.str();
  std::ofstream dat_file_out (cSVfileNamestream.c_str (), std::ios::app);
 /* static bool isFirstIteration1 = true;
  if (isFirstIteration1)
  {
       // dat_file_out << "nodes "<<"\t node_id  "<<"\t time " << "\t destination "<< "\t Throughput " << std::endl;
        dat_file_out<<"nodes\t "<<"ID \t " << "Time \t " << " SNR \t "<< " BER\t "<<" Loss\t "<< std::endl;
        isFirstIteration1 = false;
    }*/

  
   for (size_t i = 0; i < chStatsVect.size(); i++)
        {
            Time now = Simulator::Now ();                                         // Return the simulator's virtual time. 
            ns3::ChannelStats chStats =  chStatsVect[i];//chStats.throughput
            if (i == 0)
            {
              agg_chStats.u_id = chStats.u_id;
            }     
       // dat_file_out << chStats.u_id <<", \t " << simulationTime << ", \t "<< chStats.snr  << ", \t " << chStats.errRate<<", \t "<< chStats.loss<<"\t "<< std::endl;   
            agg_chStats.snr += chStats.snr;
            agg_chStats.loss += chStats.loss;
            agg_chStats.errRate += chStats.errRate;
        }

        agg_chStats.snr = agg_chStats.snr/chStatsVect.size();
        agg_chStats.loss = agg_chStats.loss/chStatsVect.size();
        agg_chStats.errRate = agg_chStats.errRate/chStatsVect.size();

        return agg_chStats;
}

void LiFiController::MonitorRxChannelStats(Ptr<LiFiPhy> channel, uint32_t ap_id ,std::vector<ns3::ChannelStats> channelStatsVector)
{
    //std::cout<<"Simulation Time "<<Simulator::Now().GetSeconds()<<"  MonitorRxChannelStats(Ptr<LiFiChannel> channel, uint32_t apId,std::unordered_map <uint32_t, std::vector<ns3::ChannelStats> > channelStats)"<<std::endl;
        isHandoverEnabled = true;
        std::stringstream m_CSVfileNamestream;
        if (isHandoverEnabled)
        {
            m_CSVfileNamestream<<"LiFi-Network/"<<"Lifi_Stations_Instant_Data_handover_active "<<min_xx<<"_"<<iteration<<".csv";

        }
        else 
        {
            m_CSVfileNamestream<<"LiFi-Network/"<<"Lifi_Instant_Data_no_handover_measurement_time 1ms "<<min_xx<<"_"<<max_xx<<"_"<<iteration<<".csv";
        }
        
        std::string cSVfileNamestream = m_CSVfileNamestream.str();
        std::ofstream dat_file_out (cSVfileNamestream.c_str (), std::ios::app);
        
        if (isFirstIteration)
        {
        // dat_file_out << "nodes "<<"\t node_id  "<<"\t time " << "\t destination "<< "\t Throughput " << std::endl;
            dat_file_out<<"nodes\t "<<"ID \t " << "Time \t " << " SNR \t "<< " BER\t "<<" Loss\t "<<" Throughput\t "<< std::endl;
            isFirstIteration = false;
        }

        double Samp_time = Simulator::Now().GetSeconds();
        ChannelStats chStats;
        chStats =  AveragePerformanceMetrics(channelStatsVector);
        Ptr<UserDeviceDataModel> userDeviceDataModel = FindUserDeviceInMap(apUserDataModelMaps, chStats.u_id);
        userDeviceDataModel->SetUserId(chStats.u_id);
        userDeviceDataModel->AddSimulationTime(Samp_time);
        userDeviceDataModel->AddSNR(10*log(chStats.snr), Samp_time);
        userDeviceDataModel->AddErrorRate(chStats.errRate,Samp_time);
        userDeviceDataModel->AddPathLoss(chStats.loss,Samp_time);
        double throughput =  userDeviceDataModel->CalculateInstantThroughput(Samp_time);
        std::vector<double> vec = userDeviceDataModel->GetHistoricalSNR();
        if (isHandoverEnabled && this->IsHandoverRequiredForDevice(vec,userDeviceDataModel->GetSinkApplication()->GetNode()->GetId()))
        {
           //std::cout<<"Initiate Handover for Device "<<userDeviceDataModel->GetUserId() <<" at time "<<Simulator::Now().GetSeconds()<<std::endl;
           RegisterForHandoverForDevice(userDeviceDataModel);
        }  
        dat_file_out << chStats.u_id <<", \t " << Samp_time << ", \t "<< 10*log(chStats.snr)  << ", \t " << chStats.errRate<<", \t "<< chStats.loss<< ", \t "<<throughput<< std::endl;
    
 }

 void LiFiController::MonitorWiFiRxChannelStats(Ptr<YansWifiPhy> channel, uint32_t user_id,std::unordered_map <uint32_t, std::vector<ns3::ChannelStats> > channelStats)
{
  
    std::stringstream m_CSVfileNamestream;
    m_CSVfileNamestream<<"WiFi-Network/"<<"Wifi_HandedOver_Stations_Instant_Data "<<min_xx<<"_"<<max_xx;
    std::string cSVfileNamestream = m_CSVfileNamestream.str();
    std::ofstream dat_file_out (cSVfileNamestream.c_str (), std::ios::app);
    static bool isFirstIteration1 = true;
    if (isFirstIteration1)
    {
       // dat_file_out << "nodes "<<"\t node_id  "<<"\t time " << "\t destination "<< "\t Throughput " << std::endl;
        dat_file_out<<"ID \t " << "Samp_time \t " << " SNR \t "<< " BER\t "<<" Loss\t "<<" Throughput\t "<< std::endl;
        isFirstIteration1 = false;
    }

    std::unordered_map <uint32_t, std::vector<ns3::ChannelStats> >:: iterator itr;
    for (itr = channelStats.begin(); itr != channelStats.end(); itr++) 
    { 
        double Samp_time = Simulator::Now().GetSeconds();
        std::vector<ns3::ChannelStats> chStatsVect    = itr->second;
        ns3::ChannelStats chStats                     = AveragePerformanceMetrics (chStatsVect);
        Ptr<UserDeviceDataModel> userDeviceDataModel  = FindUserDeviceInMap(wifi_UserDataModelMaps, chStats.u_id);
        //std::cout<<"userDeviceDataModel "<<userDeviceDataModel<<" user_id "<<user_id<<" chStats.u_id "<<chStats.u_id<<" wifi_UserDataModelMaps "<<wifi_UserDataModelMaps.size()<<std::endl;
        if (userDeviceDataModel == 0) continue;
         
        userDeviceDataModel->SetUserId(chStats.u_id);
        userDeviceDataModel->AddSimulationTime(Samp_time);
        userDeviceDataModel->AddSNR(10*log(chStats.snr), Samp_time);
        userDeviceDataModel->AddErrorRate(chStats.errRate,Samp_time);
        userDeviceDataModel->AddPathLoss(chStats.loss,Samp_time);
        double throughput = userDeviceDataModel->CalculateInstantThroughput(Samp_time);
        dat_file_out << chStats.u_id <<", \t " << Samp_time << ", \t "<< chStats.snr  << ", \t " << chStats.errRate<<", \t "<< chStats.loss<< ", \t "<<throughput<< std::endl;
      }
  }

  void LiFiController::TransferDataModelFromLiFiToWiFi (Ptr<UserDeviceDataModel> userDeviceDataModel)
{
  for (std::unordered_map <std::string, std::unordered_map<uint32_t, Ptr<UserDeviceDataModel> > >::iterator itr = apUserDataModelMaps.begin(); itr != apUserDataModelMaps.end();) 
  {
        // Do stuff
        std::unordered_map<uint32_t, Ptr<UserDeviceDataModel> >  userDeviceMap = itr->second;
        std::unordered_map <uint32_t, Ptr<UserDeviceDataModel> >::  iterator itr1;
        std::string keyStr = itr->first;
        itr1 = userDeviceMap.find(userDeviceDataModel->GetSinkApplication()->GetNode()->GetId());
        bool found = false;
        if (itr1 != userDeviceMap.end())
        {
            userDeviceMap.erase (itr1);
            found = true;
        }

        if (found)
        {  
          apUserDataModelMaps.erase(itr++);
          if (userDeviceMap.size())
          {
              apUserDataModelMaps.insert(std::make_pair(keyStr, userDeviceMap));
          }
        }
        else {
        itr++;
        }
  }

  std::ostringstream stream;
  stream<<wifi_port;
  std::string portStr = stream.str();
  std::unordered_map <std::string, std::unordered_map<uint32_t, Ptr<UserDeviceDataModel> > >::iterator itr;
  itr = wifi_UserDataModelMaps.find(portStr);
  std::unordered_map<uint32_t, Ptr<UserDeviceDataModel> >  userDeviceMap;
  if (itr != wifi_UserDataModelMaps.end())
  {
      userDeviceMap = itr->second;
      userDeviceMap.insert(std::make_pair(userDeviceDataModel->GetSinkApplication()->GetNode()->GetId(),userDeviceDataModel));      
  }
  else 
  {
      userDeviceMap.insert(std::make_pair(userDeviceDataModel->GetSinkApplication()->GetNode()->GetId(),userDeviceDataModel));
  }

  wifi_UserDataModelMaps.erase(portStr);
  wifi_UserDataModelMaps.insert(std::make_pair(portStr,userDeviceMap));

/*for (std::unordered_map <std::string, std::unordered_map<uint32_t, Ptr<UserDeviceDataModel> > >::iterator itr = wifi_UserDataModelMaps.begin(); itr != wifi_UserDataModelMaps.end();itr++) 
{
 std::unordered_map<uint32_t, Ptr<UserDeviceDataModel> >  userDeviceMap = itr->second;
 for (std::unordered_map<uint32_t, Ptr<UserDeviceDataModel> >::iterator itr1 = userDeviceMap.begin(); itr1 != userDeviceMap.end();itr1++) 
{
  std::cout<<"Node ID "<<itr1->first<<std::endl;
}
std::cout<<std::endl;
}*/

}