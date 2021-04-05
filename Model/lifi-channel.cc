#include "lifi-channel.h"
#include "ns3/simulator.h"
#include "ns3/packet.h"
#include "ns3/node.h"
#include "ns3/log.h"
#include "ns3/double.h"
#include "ns3/uinteger.h"
#include "ns3/ptr.h"

#include "ns3/lifi-rx-net-device.h"
#include "ns3/lifi-tx-net-device.h"
#include "ns3/lifi-random-waypoint-mobility-model.h"
#include "ns3/random-waypoint-mobility-model.h"
#include "ns3/lifi-constant-velocity-mobility-model.h"

#include "ns3/received-data-model.h"

NS_LOG_COMPONENT_DEFINE ("LiFiChannel");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (LiFiChannel);

TypeId
LiFiChannel::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::LiFiChannel")
    .SetParent<SimpleChannel> ()
    .AddConstructor<LiFiChannel> ()
    .AddAttribute ("MaxRange",
    "Maximum Transmission Range (meters)",
    DoubleValue (250),
    MakeDoubleAccessor (&LiFiChannel::m_range),
    MakeDoubleChecker<double> ())
    .AddTraceSource ("ChannelData", "Channel Data",
                     MakeTraceSourceAccessor (&LiFiChannel::m_monitorRxDataCallback),
                     "ns3::LiFiChannel::MonitorRxDataCallback")
  ;
  return tid;
}

LiFiChannel::LiFiChannel ()
  : m_range (0),m_lifiTxNetDevice (0)
{
     m_loss = CreateObject<LiFiPropagationLossModel>();
     startStatsCollection = false;
     m_TxRxDistance = 0;
}

void LiFiChannel::AddPhy (Ptr<LiFiPhy> phy)
{
  m_PhyVector.push_back(phy);
}

double LiFiChannel::GetDistance(ns3::Ptr<ns3::MobilityModel> aTX,ns3::Ptr<ns3::MobilityModel> bRX, Ptr<LiFiRxNetDevice> rxd)
{
     double dist = 0;

     Ptr<ns3::LiFiMobilityModel>  txmobmodel= ns3::DynamicCast < LiFiMobilityModel>  (aTX);
     Vector rx= bRX->GetPosition();
     Vector tx = txmobmodel->GetPosition();
     dist = std::pow((tx.x - rx.x), 2) + std::pow((tx.y - rx.y), 2) + std::pow((tx.z - rx.z), 2);
     dist = std::sqrt(dist);
     return dist;
}

double LiFiChannel::GetDistance(ns3::Ptr<ns3::MobilityModel> aTX,ns3::Ptr<ns3::MobilityModel> bRX, Ptr<LiFiNetDevice> rxd)
{
     double dist = 0;

     Ptr<ns3::LiFiMobilityModel>  txmobmodel= ns3::DynamicCast < LiFiMobilityModel>  (aTX);
     Vector rx= bRX->GetPosition();
     Vector tx = txmobmodel->GetPosition();
     dist = std::pow((tx.x - rx.x), 2) + std::pow((tx.y - rx.y), 2) + std::pow((tx.z - rx.z), 2);
     dist = std::sqrt(dist);
     return dist;
}

void
LiFiChannel::Send (Ptr<const Packet> p, Ptr<LiFiPhy> sender)
{
  NS_LOG_FUNCTION (p << sender);
      //std::cout<<"LiFiChannel::Send (Ptr<const Packet> p, Ptr<NetDevice> sender)"<<std::endl;

  for (std::vector<Ptr<LiFiPhy> >::const_iterator i = m_PhyVector.begin (); i != m_PhyVector.end (); ++i)
    {
    Ptr<LiFiPhy> rxDevice = *i;
    if (rxDevice == sender)
    {
     // std::cout<<"LiFiChannel::Send (Ptr<const Packet> p, Ptr<NetDevice> sender)"<<std::endl;
        continue;
    }

    //NS_LOG_DEBUG ("Distance: " << distance << " Max Range: " << m_range);
    /*Ptr<LiFiRxNetDevice> rxNetDev = DynamicCast <LiFiRxNetDevice> (rxDevice);
    if (rxNetDev)
    {
        Ptr<MobilityModel> b = rxNetDev->GetNode()->GetObject<MobilityModel>();
        Ptr<MobilityModel> a = m_lifiTxNetDevice->GetMobilityModel();
        NS_ASSERT_MSG (a && b, "Error:  nodes must have mobility models");
        double distance = GetDistance (a,b,rxNetDev);
       // ErrorForRxNetDevice(rxNetDev);
       // std::cout<<"rxNetDev "<<std::endl;

        Time propagationTime = NanoSeconds (uint64_t (3.3 * distance));
        Simulator::ScheduleWithContext (rxNetDev->GetNode ()->GetId (),(propagationTime), &LiFiNetDevice::ForwardUp, rxDevice, p->Copy ());
    }
    else
    {
        Ptr<MobilityModel> b = rxDevice->GetNode()->GetObject<MobilityModel>();
        Ptr<MobilityModel> a = m_lifiTxNetDevice->GetMobilityModel();
        NS_ASSERT_MSG (a && b, "Error:  nodes must have mobility models");
        double distance = GetDistance (a,b,rxDevice);*/
       // ErrorForRxNetDevice(rxDevice);
       /// std::cout<<"rxNetDev "<<std::endl;
        Time propagationTime = NanoSeconds (uint64_t (3.3 * 10));
     // std::cout<<"        Simulator::ScheduleWithContext (rxDevice->GetDevice()->GetNode ()->GetId ();"<<rxDevice->GetDevice()->GetNode ()->GetId ()<<std::endl;
        Simulator::ScheduleWithContext (rxDevice->GetDevice()->GetNode ()->GetId (),(propagationTime), &LiFiPhy::Receive,rxDevice,p->Copy ());
    //}

    }

    if(startStatsCollection==false)
    {
      startStatsCollection = true;
      Simulator::Schedule (MilliSeconds (100), &LiFiChannel::SaveSNRBERLoss,this);
    }
}

  void LiFiChannel::Add (Ptr<LiFiNetDevice> rxNetDevice)
{
  NS_LOG_DEBUG (this << " " << rxNetDevice);
  m_LiFiRxNetDevicesList.push_back (rxNetDevice);
  NS_LOG_DEBUG ("current m_tdmaMacLowList size: " << m_LiFiRxNetDevicesList.size ());
}

void LiFiChannel::RemoveUserDevice (uint32_t index)
{
  size_t itemIndex=-1;
  for (size_t i = 0; i < m_LiFiRxNetDevicesList.size(); i++)
  {
    if (m_LiFiRxNetDevicesList[i]->GetNode()->GetId() == index)
    {
      itemIndex = i;
      break;
    }
  }

  if (itemIndex >= 0)
  {
    m_LiFiRxNetDevicesList.erase(m_LiFiRxNetDevicesList.begin() + itemIndex);
  }

}

ns3::Ptr < ns3::LiFiErrorModel > LiFiChannel::ErrorForRxNetDevice (ns3::Ptr<LiFiRxNetDevice> rxNetDevice)
{
      double pAvg = 48.57;
      uint32_t key = rxNetDevice->GetNode()->GetId();
      ns3::Ptr<ns3::LiFiPropagationLossModel> m_propagationLossModel = m_propagationLossMap[key];
	    double loss = m_propagationLossModel->CalcRxPower(pAvg,m_lifiTxNetDevice->GetMobilityModel(), rxNetDevice->GetNode()->GetObject<MobilityModel>());
      ns3::Ptr<ns3::LiFiSnr> m_SNR = m_LiFiSnrMap[key];
      m_SNR->SetReceivedPower(loss);
      m_SNR->CalculateSNR();		// caluclates the SNR value
      double snr = m_SNR->GetSNR();

      ns3::Ptr < ns3::LiFiErrorModel > rxErrorModel = rxNetDevice->GetErrorModel();

      rxErrorModel->SetSNR(snr);
      double erRate = rxErrorModel->CalculateErrorRate();
      /*Ptr<ns3::LiFiConstantVelocityMobilityModel>  lifiConstantVelModel  = ns3::DynamicCast < LiFiConstantVelocityMobilityModel>  (rxNetDevice->GetNode()->GetObject<MobilityModel>());
      if (lifiConstantVelModel  != 0)
      {
          std::vector<double> statVec;
          statVec.push_back(key);
          statVec.push_back(Simulator::Now().GetSeconds());
          statVec.push_back(snr);
          statVec.push_back(erRate);
          statVec.push_back(loss);
          m_statsVector.push_back(statVec);
      }*/

      std::unordered_map<uint32_t, std::vector<ChannelStats> >:: iterator it1;
      it1 = channelStatsMap.find(key);
      ChannelStats chStats;
      chStats.u_id = key;
      chStats.simTime = Simulator::Now().GetSeconds();
      chStats.snr = snr;
      chStats.errRate = erRate;
      chStats.loss = loss;
      chStats.throughput = 0;
      std::vector<ChannelStats> statsVector;
      if (it1 != channelStatsMap.end())
      {
          statsVector = it1->second;
          statsVector.push_back(chStats);
          channelStatsMap[key] = statsVector;
      }
      else
      {
        statsVector.push_back (chStats);
        channelStatsMap.insert(make_pair(key, statsVector));
      }

      rxErrorModel->SetRate(erRate);
      return rxErrorModel;
}

void LiFiChannel::AddPropagationLossModel (ns3::Ptr<LiFiPropagationLossModel> propModel, uint32_t key)
{
    m_propagationLossMap[key] =  propModel;
}

void LiFiChannel::AddLiFiSNRModel (ns3::Ptr<LiFiSnr> snrModel, uint32_t key)
{
     m_LiFiSnrMap[key] =  snrModel;

}

void LiFiChannel::AddLiFiErrorModel (ns3::Ptr<LiFiErrorModel> errorModel, uint32_t key)
{
    m_LiFiErrorModelMap[key]  = errorModel;
}

  std::map<std::string, std::vector<LiFiUserData >  > LiFiChannel::GetUserData ( )
  {
    std::map<std::string, std::vector<LiFiUserData >  >  mp;
    LiFiUserData usData;
    std::vector<LiFiUserData > vect;
    vect.push_back(usData);
    mp["test"] = vect;

    return mp;
  }

void LiFiChannel::SetTxDevice (Ptr<LiFiTxNetDevice> txNetDevice)
{
  NS_LOG_DEBUG (this << " " << txNetDevice);
  m_lifiTxNetDevice = txNetDevice;
}

std::size_t LiFiChannel::GetNDevices (void) const
{
  return m_LiFiRxNetDevicesList.size ()+1;
}

Ptr<NetDevice> LiFiChannel::GetDevice (uint32_t i) const
{
  if (i == 0)
      return  m_lifiTxNetDevice;

  return m_LiFiRxNetDevicesList[i-1];
}

Ptr<NetDevice> LiFiChannel::FindDeviceById (uint32_t uid)
{
  for (size_t i = 0; i < m_LiFiRxNetDevicesList.size(); i++)
  {
    if (m_LiFiRxNetDevicesList[i]->GetNode()->GetId() == uid)
    {
      return m_LiFiRxNetDevicesList[i];
    }
  }

  return 0;
}

Ptr<NetDevice> LiFiChannel::GetTxDevice ()
{
    return m_lifiTxNetDevice;
}

void LiFiChannel::SaveSNRBERLoss()
{
  /*std::stringstream m_CSVfileNamestream;
  m_CSVfileNamestream<<"LiFi-Channel_Stats_For AP "<<m_lifiTxNetDevice->GetNode()->GetId();
  std::string cSVfileNamestream = m_CSVfileNamestream.str();
  std::ofstream dat_file_out (cSVfileNamestream.c_str (), std::ios::app);
  if (startStatsCollection ==false)
  {
      dat_file_out<<"ID \t " << "Time \t " << "\t SNR "<< "\t BER "<<"\t Loss "<< std::endl;
      startStatsCollection = true;
  }

  for(uint16_t i=0; i< m_statsVector.size(); i++)
  {
      std::vector<double> v1 = m_statsVector[i];
      dat_file_out<<v1[0]<<" \t " <<v1[1]<<"\t "<<v1[2]<< "\t"<<v1[3]<< "\t "<<v1[4]<< std::endl;
  }
*/
if (channelStatsMap.size() >  0)
{

  m_monitorRxDataCallback(this,m_lifiTxNetDevice->GetNode()->GetId(),channelStatsMap);
  channelStatsMap.clear();
}

 // dat_file_out.close();
Simulator::Schedule (MilliSeconds (100), &LiFiChannel::SaveSNRBERLoss,this);
}

Ptr<LiFiNetDevice> LiFiChannel::GetLiFiRxNetDevice (uint32_t i) const
{
  return m_LiFiRxNetDevicesList[i];
}

double
LiFiChannel::GetMaxRange (void) const
{
  return m_range;
}

} // namespace ns3
