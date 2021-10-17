
#include "centralized-controller.h"
#include "ns3/lifi-ap-mac.h"
#include "ns3/assert.h"
#include "ns3/enum.h"
#include "ns3/log.h"
#include "ns3/simulator.h"
#include "ns3/abort.h"
#include "ns3/wifi-ap-net-device.h"
namespace ns3 
{

UserDeviceDataModel::UserDeviceDataModel ()
{
    this->rxBytesVector.push_back(0);
    m_lastTotalRxBytes = 0;   
    isUserMobile = false;
    isHandoverInProgress = false;
    m_time_to_trigger = 0;
    m_handoverDelay   = 0;
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

void UserDeviceDataModel::SetForwardLagValue (uint32_t frdLag)
{
    m_lagAfterTTT = frdLag;
}

uint32_t UserDeviceDataModel::GetForwardLagValue ()
{
    return m_lagAfterTTT;
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

 void UserDeviceDataModel::AddAPID(uint32_t id)
 {
if (m_lifiApNetDevicesVector.size() == 0)
    {
        m_lifiApNetDevicesVector.push_back(id);
    }
    else if (m_lifiApNetDevicesVector.size() > 0 && m_lifiApNetDevicesVector[m_lifiApNetDevicesVector.size()-1] == id)
    {
        m_lifiApNetDevicesVector.push_back(id);

    }
 }

uint32_t UserDeviceDataModel::GetCurrentAPID()
{
    return m_lifiApNetDevicesVector[m_lifiApNetDevicesVector.size()-1];

}

void UserDeviceDataModel::SetHandoverDelay(double db)
{
    m_handoverDelay = db;
}

double UserDeviceDataModel::GetHandoverDelay()
{
    return m_handoverDelay;
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

bool UserDeviceDataModel::IsHandoverCriteriaSatisfied(uint32_t reprots_count)
{
  std::vector<double> snrVect = GetHistoricalSNR();
  //uint32_t id = this->GetSinkApplication()->GetNode()->GetId();
  double sum = 0;
  if (snrVect.size() < 41)
  {
    return false;
  }
 
// std::cout<<"IsHandoverRequiredForDevice(std::vector<double> vec, uint32_t id) Time "<<Simulator::Now().GetSeconds()<<" node "<<id<<std::endl;
  for (size_t i = snrVect.size()-1; i >= snrVect.size()-reprots_count; i--)
  {
    sum += snrVect[i];
       
  }
  
  //std::cout<<"\naverage "<<(sum/15.0)<<"\n\n"<<std::endl;
  if ((sum/reprots_count) <= 20.50)
  {
    return true;
  }
 
  return false;
}

bool UserDeviceDataModel::IsHandoverHandoverRequired ()
{
    if (!isHandoverInProgress && IsHandoverCriteriaSatisfied(m_lagPair.first))
    {
        return true;
    }

    return false;
}

void UserDeviceDataModel::SetUDPhy (Ptr<LiFiPhy> phy)
{
    m_lifiphy = phy;
}

Ptr<LiFiPhy> UserDeviceDataModel::GetUdPhy ()
{
    return m_lifiphy;
}

void UserDeviceDataModel::SetLagValue (std::pair<uint32_t,uint32_t> lag)
{
    m_lagPair = lag;
}

std::pair<uint32_t,uint32_t> UserDeviceDataModel::GetLagValue ()
{
    return m_lagPair;
}

double UserDeviceDataModel::GetTimeToTrigger ()
{
    return m_time_to_trigger;
}

void UserDeviceDataModel::SetSchedularCheckTime(double db)
{
    m_kpi_monetoringTime = db;
}

double UserDeviceDataModel::GetSchedularCheckTime()
{
   return m_kpi_monetoringTime;
}

void UserDeviceDataModel::SetTimeToTrigger (double ttt)
{
    m_time_to_trigger = ttt;
    //m_lifiphy->SetHandoverTriggered(m_time_to_trigger,m_lag);
}

void UserDeviceDataModel::InitiateHandover()
{
    isHandoverInProgress = true;
    Simulator::Schedule (MilliSeconds (m_time_to_trigger), &UserDeviceDataModel::CheckHandoverMetricsAfterTTTExpirey,this);
}

void UserDeviceDataModel::SetCentralizedController (Ptr<CentralizedController> centralizedController)
{
    m_CentralizedController = centralizedController;
}

void UserDeviceDataModel::CheckHandoverMetricsAfterTTTExpirey ()
{
    if (IsHandoverCriteriaSatisfied((m_lagPair.second)))
    {
        Simulator::Schedule (MilliSeconds (m_handoverDelay), &UserDeviceDataModel::CompleteHandover,this);
    }
    else
    {
        m_time_to_trigger = 0;
        isHandoverInProgress = false;
    }
}

void UserDeviceDataModel::CompleteHandover()
{
    isHandoverInProgress = false;
    m_CentralizedController->PerformVerticalHandover(this);
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

CentralizedController::CentralizedController ()
{
    m_isHandoverEnabled = true;
    m_isFirstIteration  = true;
    Simulator::Schedule (Seconds (1), &CentralizedController::KPI_Monitoring,this); 


}

CentralizedController::~CentralizedController ()
{

}

void CentralizedController::SetCCNode (Ptr<Node> controllernode)
{
    m_controller_node = controllernode;
}

Ptr<Node> CentralizedController::GetCCNode(void)
{
    return m_controller_node;
}

Ptr<UserDeviceDataModel> CentralizedController::FindUserWithID(uint32_t userId)
{
  for (auto& itr: m_userDataModelMaps) 
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

void CentralizedController::SetUserDataModelMaps(std::unordered_map <std::string, std::unordered_map<uint32_t, Ptr<UserDeviceDataModel> > > userDataModelMaps)
{
    m_userDataModelMaps = userDataModelMaps;
}

void CentralizedController::RegisterUserDevice (std::unordered_map<uint32_t, Ptr<UserDeviceDataModel>> map, std::string str)
{
    m_userDataModelMaps.insert(std::make_pair(str, map));
}


void CentralizedController::SetLiFiAPsNetDeviceInterface(Ptr<ns3::PointToPointNetDevice> netdevice)
{
    netdevice->SetReceiveCallback(MakeCallback (&CentralizedController::LiFiAPMeasurementReports, this));
    m_lifiNetDevicesContainer.Add(netdevice);
}

void CentralizedController::SetWiFiAPsNetDeviceInterface(Ptr<ns3::WifiNetDevice> netdevice)
{
    netdevice->SetReceiveCallback(MakeCallback (&CentralizedController::WiFiAPMeasurementReports, this));
    m_wifiNetDevicesContainer.Add(netdevice);
}

//<bool,Ptr<NetDevice>,Ptr<const Packet>,uint16_t,const Address &>
bool CentralizedController::LiFiAPMeasurementReports(Ptr<NetDevice> netDevice,Ptr<const Packet> packet,uint16_t protocolId, const Address & address)
{
    m_isHandoverEnabled = true;
    std::stringstream m_CSVfileNamestream;
    if (m_isHandoverEnabled)
    {
        m_CSVfileNamestream<<"LiFi-Network/"<<"Lifi_Stations_Instant_Data_handover_active_apid "<<netDevice->GetNode()->GetId()<<".csv";

    }
    else 
    {
        m_CSVfileNamestream<<"LiFi-Network/"<<"Lifi_Instant_Data_no_handover_measurement_time _apid "<<netDevice->GetNode()->GetId()<<".csv";
    }
        
    std::string cSVfileNamestream = m_CSVfileNamestream.str();
    std::ofstream dat_file_out (cSVfileNamestream.c_str (), std::ios::app);
        
    if (m_isFirstIteration)
    {
        dat_file_out<<"nodes\t "<<"ID \t " << "Time \t " << " SNR \t "<< " BER\t "<<" Loss\t "<<" Throughput\t "<< std::endl;
        m_isFirstIteration = false;
    }

    

    double record_time = Simulator::Now().GetSeconds();
    Ptr<Packet> cpPacket = packet->Copy();
    ns3::MeasurementReport measurementReport;
    cpPacket->RemovePacketTag(measurementReport);
    double snr = measurementReport.GetSNR()[0];
    Ptr<UserDeviceDataModel> userDeviceDataModel = FindUserWithID(measurementReport.GetDeviceID());
   // userDeviceDataModel->AddAPNetDevice(DynamicCast<LiFiNetDevice>(netDevice));
     
    userDeviceDataModel->AddSimulationTime(record_time);
    userDeviceDataModel->AddSNR(snr, record_time);
 // std::cout<<"LiFiAPMeasurementReports(Ptr<NetDev node id "<<netDevice->GetNode()->GetId()<<" time: "<<Simulator::Now().GetSeconds()<<" snr "<<snr<<std::endl;

    userDeviceDataModel->AddErrorRate(0,record_time);
    userDeviceDataModel->AddPathLoss(0,record_time);
   // if (snr> 10 && snr<90)
   // std::cout<<"Time: "<<Simulator::Now().GetSeconds()<<" SNR: "<<snr<<std::endl;

    /*
    if (snr> 10 && snr<50)
	{
		std::cout<<"m_error->IsCorrupt (packet) time "<<ns3::Simulator::Now().GetSeconds()<<" "<<m_error->IsCorrupt(packet)<<" snr "<<snr<<std::endl;

	}
    ChannelStats chStats;
    chStats =  AveragePerformanceMetrics(channelStatsVector);
    Ptr<UserDeviceDataModel> userDeviceDataModel = FindUserWithID(chStats.u_id);
    userDeviceDataModel->SetUserId(chStats.u_id);
    userDeviceDataModel->AddSimulationTime(Samp_time);
    userDeviceDataModel->AddSNR(10*log(chStats.snr), Samp_time);
    userDeviceDataModel->AddErrorRate(chStats.errRate,Samp_time);
    userDeviceDataModel->AddPathLoss(chStats.loss,Samp_time);
    double throughput =  userDeviceDataModel->CalculateInstantThroughput(Samp_time);
    std::vector<double> vec = userDeviceDataModel->GetHistoricalSNR();
    if (m_isHandoverEnabled && this->IsHandoverRequiredForDevice(vec,userDeviceDataModel->GetSinkApplication()->GetNode()->GetId()))
    {
        RegisterForHandoverForDevice(userDeviceDataModel);
    }  
    */
    dat_file_out << measurementReport.GetDeviceID() <<", \t " << record_time << ", \t "<< snr  << ", \t " << 0<<", \t "<< 0<< ", \t "<<0<< std::endl;   

   return true;
 }

bool CentralizedController::WiFiAPMeasurementReports(Ptr<NetDevice> netDevice,Ptr<const Packet> packet,uint16_t protocolId, const Address & address)
{
    return false;
}

Ptr<Ipv4StaticRouting> CentralizedController::GetStaticRouting (Ptr<Ipv4> ipv4) const
 {
   Ptr<Ipv4RoutingProtocol> ipv4rp = ipv4->GetRoutingProtocol ();
   NS_ASSERT_MSG (ipv4rp, "No routing protocol associated with Ipv4");
   if (DynamicCast<Ipv4StaticRouting> (ipv4rp))
   {
       return DynamicCast<Ipv4StaticRouting> (ipv4rp); 
   } 

   if (DynamicCast<Ipv4ListRouting> (ipv4rp))
     {
       Ptr<Ipv4ListRouting> lrp = DynamicCast<Ipv4ListRouting> (ipv4rp);
       int16_t priority;
       for (uint32_t i = 0; i < lrp->GetNRoutingProtocols ();  i++)
         {
           Ptr<Ipv4RoutingProtocol> temp = lrp->GetRoutingProtocol (i, priority);
           if (DynamicCast<Ipv4StaticRouting> (temp))
             {
               return DynamicCast<Ipv4StaticRouting> (temp);
             }
         }
     }
   return 0;
 }

std::pair<int,uint32_t>  CentralizedController::GetWiFiNetDeviceInterfaceIndex (Ptr<Node> node)
{
    for (uint32_t i = 0; i < node->GetNDevices(); i++)
    {
       Ptr<NetDevice> netDevice = node->GetDevice(i);
       Ptr <WifiNetDevice> wifiNetDevice = DynamicCast<WifiNetDevice>(netDevice);
       if (wifiNetDevice)
       {
           return std::make_pair(1,i);
       }
       
    }

    return std::make_pair(-1,-1);
}



void CentralizedController::TransferApplicationLiFiToWiFi(Ptr<UserDeviceDataModel> userDeviceDataModel)
{
   //std::cout<<"void CentralizedController::TransferApplicationLiFiToWiFi(Ptr<UserDeviceDataModel> userDeviceDataModel)"<<std::endl;
   Ptr<Node> node = GetCCNode();
   bool isRouteRemoved = false;
   Ptr<Ipv4> ipv4proto = node->GetObject<Ipv4> ();
   Ptr < Ipv4StaticRouting > staticRouting   = GetStaticRouting(ipv4proto);
   Ipv4RoutingTableEntry flagedRouteEntry;

   for (uint32_t i = 0; i < staticRouting->GetNRoutes(); i++)
   {       
       Ipv4RoutingTableEntry routeEntry = staticRouting->GetRoute(i);
        //std::cout<<"userDeviceDataModel->GetDestinationAddress() userDeviceDataModel->GetDestinationAddress() "<<userDeviceDataModel->GetDestinationAddress()<<" routeEntry.GetDest () "<<routeEntry.GetDest ()<<std::endl;
       if (routeEntry.GetDest () == userDeviceDataModel->GetDestinationAddress())
       {
           staticRouting->RemoveRoute(i);
           isRouteRemoved = true;
           break;
       }  
   }

   if (isRouteRemoved)
   {
        Ptr < Ipv4StaticRouting > staticRouting   = GetStaticRouting(ipv4proto);
        std::pair<int,uint32_t> ind_pair = GetWiFiNetDeviceInterfaceIndex(GetCCNode());
        if (ind_pair.first != -1)
        {
            Ptr<Node> udnode = userDeviceDataModel->GetSinkApplication()->GetNode();
            Ptr<Ipv4> ipv4A1                        = udnode->GetObject<Ipv4> ();//node->GetDevice(2)
            Ptr<WifiApNetDevice> txNetDev = ns3::DynamicCast <WifiApNetDevice> (udnode->GetDevice(1));
            int32_t interfaceIndex1                = ipv4A1->GetInterfaceForDevice (txNetDev);

            Ipv4InterfaceAddress ipv4Addr = Ipv4InterfaceAddress (userDeviceDataModel->GetDestinationAddress(), Ipv4Mask ("/24"));
            ipv4A1->AddAddress (interfaceIndex1, ipv4Addr);
            ipv4A1->SetMetric (interfaceIndex1, 1);
            ipv4A1->SetUp (interfaceIndex1);
              Simulator::Schedule (Seconds (0.9), &CentralizedController::PrintDroppedPackets,this,userDeviceDataModel); 
            
            
        }
   }
}

void CentralizedController::PrintDroppedPackets(Ptr<UserDeviceDataModel> userDeviceDataModel)
{
}

ns3::ChannelStats CentralizedController::AveragePerformanceMetrics (std::vector<ns3::ChannelStats> chStatsVect)
{
  ns3::ChannelStats agg_chStats;
  std::stringstream m_CSVfileNamestream;
  m_CSVfileNamestream<<"LiFi-Network/LiFi-Channel_Stats_For AP_1";
  std::string cSVfileNamestream = m_CSVfileNamestream.str();
  std::ofstream dat_file_out (cSVfileNamestream.c_str (), std::ios::app);

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


void CentralizedController::KPI_Monitoring()
{
 Ptr<UserDeviceDataModel> userDeviceDataModel = 0;   

 for (auto& itr: m_userDataModelMaps) 
 {
      std::unordered_map<uint32_t, Ptr<UserDeviceDataModel> >  userDeviceMap = itr.second; 
      for(std::unordered_map <uint32_t, Ptr<UserDeviceDataModel> >::iterator it = userDeviceMap.begin(); it != userDeviceMap.end(); ++it)
      {
       userDeviceDataModel = it->second;
        if (userDeviceDataModel->IsHandoverHandoverRequired())
        {
            userDeviceDataModel->InitiateHandover();
        }         
      }
  }
  Simulator::Schedule (MilliSeconds (userDeviceDataModel->GetSchedularCheckTime()), &CentralizedController::KPI_Monitoring,this); 
}


void CentralizedController::PerformVerticalHandover (Ptr<UserDeviceDataModel> userDeviceDataModel)
{
    this->TransferApplicationLiFiToWiFi(userDeviceDataModel);
}

void CentralizedController::RegisterForHandoverForDevice(Ptr<UserDeviceDataModel> userDeviceDataModel)
{
}
    
}