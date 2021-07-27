#include "ns3/lifi-phy.h"
#include "ns3/lifi-channel.h"
#include "ns3/lifi-mac-low.h"
#include "ns3/lifi-net-device.h"
#include "ns3/llc-snap-header.h"
#include "ns3/packet.h"
#include "ns3/uinteger.h"
#include "ns3/pointer.h"
#include "ns3/node.h"
#include "ns3/trace-source-accessor.h"
#include "ns3/log.h"

NS_LOG_COMPONENT_DEFINE ("LiFiPhy");

namespace ns3 {
// This value conforms to the 802.11 specification
  static const uint16_t MAX_MSDU_SIZE = 2304;
NS_OBJECT_ENSURE_REGISTERED (LiFiPhy);

TypeId
LiFiPhy::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::LiFiPhy")
    .SetParent<Object> ()
    .AddConstructor<LiFiPhy> ().AddTraceSource ("ChannelData", "Channel Data",
                     MakeTraceSourceAccessor (&LiFiPhy::m_monitorRxDataCallback),
                     "ns3::LiFiChannel::MonitorRxDataCallback");
  return tid;
}

LiFiPhy::LiFiPhy ()
{
	NS_LOG_FUNCTION_NOARGS ();
//m_mobilityModel = ns3::CreateObject<MobilityModel>();
	//m_error = ns3::CreateObject<ns3::LiFiErrorModel>();
	//m_mobilityModel->SetAzimuth(0);
	//m_mobilityModel->SetElevation(0);
	//m_mobilityModel->SetPosition(ns3::Vector(0.0, 0.0, 0.0));
//  LogComponentEnable ("TdmaNetDevice", LOG_LEVEL_DEBUG);
startStatsCollection = false;
}

LiFiPhy::~LiFiPhy ()
{
  NS_LOG_FUNCTION_NOARGS ();
}

void LiFiPhy::SetChannel (Ptr<LiFiChannel> channel)
{
	m_channel = channel;
	//std::cout<<"void LiFiPhy::SetChannel (Ptr<LiFiChannel> channel) "<<m_channel<<std::endl;

}

Ptr<Channel> LiFiPhy::GetChannel (void) const
{
   return m_channel;
}

double LiFiPhy::GetLambertianOrder() 
{
    return this->m_lOrder;
}

//before setting Lambertian Order make sure the semiangle value is set
//Need to setup error handling when semiangle is not set
void LiFiPhy::SetLambertainOrder()
{
	this->m_lOrder = (-1 * log(2)) / (log(cos(this->m_semiangle)));
}

void LiFiPhy::SetErrorRateModel(Ptr<LiFiErrorModel> errorModel)
{
	m_error = errorModel;
}

void LiFiPhy::SetMacLow(Ptr<LiFiMacLow> macLow)
{
m_mac = macLow;
}

Ptr<LiFiMacLow> LiFiPhy::GetMacLow()
{
return m_mac;
}

double LiFiPhy::GetSemiangle() {
	return this->m_semiangle;
}

void LiFiPhy::SetSemiangle(double angle) {
	this->m_semiangle = angle * M_PI / 180;
	this->SetLambertainOrder();
}

void LiFiPhy::SetMobilityModel()
{

}

void LiFiPhy::SetFilterGain(double filterGain)
{
	m_filterGain = filterGain;
}

double LiFiPhy::GetFilterGain()
{
	return m_filterGain;
}

//use this function after setting up FOV and refractive index
void LiFiPhy::SetConcentrationGain() 
{
	NS_LOG_FUNCTION(this);
	this->m_concentrationGain = std::pow(this->m_refIndex, 2) / std::pow((float) std::sin((float) m_FOVangle), 2);
}

double LiFiPhy::GetConcentrationGain() 
{
	NS_LOG_FUNCTION(this);
	return this->m_concentrationGain;
}

double LiFiPhy::GetRXGain() 
{
	NS_LOG_FUNCTION(this);
	return this->m_RXGain;
}


double LiFiPhy::GetPhotoDetectorArea() 
{
	NS_LOG_FUNCTION(this);
	return this->m_photodetectorArea;
}

void LiFiPhy::SetPhotoDectectorArea(double pArea) 
{
	NS_LOG_FUNCTION(this<<pArea);
	this->m_photodetectorArea = pArea;
}

double LiFiPhy::GetFOVAngle() 
{
	NS_LOG_FUNCTION(this);
	return this->m_FOVangle;
}

void LiFiPhy::SetFOVAngle(double angle) 
{
	NS_LOG_FUNCTION(this<<angle);
	this->m_FOVangle = angle * M_PI / 180;
}

double LiFiPhy::GetRefractiveIndex() 
{
	NS_LOG_FUNCTION(this);
	return this->m_refIndex;
}
void LiFiPhy::SetRefractiveIndex(double angle) 
{
	NS_LOG_FUNCTION(this << angle);
	this->m_refIndex = angle;
}

void LiFiPhy::SetSNR(Ptr<LiFiSnr> snr)
{
  m_LifiSNR = snr;
}

void LiFiPhy::SetLossModel(Ptr<LiFiPropagationLossModel> propgationLossModel)
{
	m_propgationLossModel = propgationLossModel;
}

Ptr<LiFiSnr> LiFiPhy::GetSNR(void) const
{
  return m_LifiSNR;
}

// returns the modulation order (M)
int LiFiPhy::GetModulationOrder(void) const 
{// 
	NS_LOG_FUNCTION(this);
	return this->m_error->GetModulationOrder();
}

// sets the modulation order value
void LiFiPhy::SetModulationOrder(int m_order) 
{
	NS_LOG_FUNCTION(this<<m_order);
	return this->m_error->SetModulationOrder(m_order);
}

// calculates the error rate value according to modulation scheme
double LiFiPhy::CalculateErrorRateForErrorModel() 
{
	NS_LOG_FUNCTION(this);
	return this->m_error->CalculateErrorRate();
}

ns3::Ptr<ns3::LiFiErrorModel> LiFiPhy::GetErrorModel() 
{
	NS_LOG_FUNCTION(this);
	return this->m_error;
}

void LiFiPhy::SetDevice(Ptr<LiFiNetDevice> lifiNetDevice)
{
  m_lifiNetDevice = lifiNetDevice;
 //std::cout<<"Ptr<LiFiNetDevice> LiFiPhy::GetDevice() const "<<m_lifiNetDevice<<std::endl;
}

void LiFiPhy::SetDeviceType(std::string deviceType)
{
	m_deviceType = deviceType;
}

std::string LiFiPhy::GetDeviceType(void)
{
	return m_deviceType;
}

Ptr<LiFiNetDevice> LiFiPhy::GetDevice() const
{
  return m_lifiNetDevice;
}

void LiFiPhy::Send (Ptr<const Packet> packet, Ptr<LiFiNetDevice> lifiNetDevice)
{
	//std::cout<<"void LiFiPhy::Send (Ptr<const Packet> packet, Ptr<LiFiNetDevice> lifiNetDevice) 11111111111 "<<m_channel<<std::endl;
  m_channel->Send (packet,this);
}

void LiFiPhy::Receive (Ptr<Packet> packet, Ptr<MobilityModel> txMobility)
{
  //std::cout<<"void LiFiPhy::Receive (Ptr<Packet> packet, Ptr<MobilityModel> aMobility) "<<this->GetDevice()->GetMobilityModel()<<std::endl;
	double pAvg = 47.34;
	double loss = 0;
	if (m_deviceType == "UD")
		{
			//std::cout<<"void LiFiPhy::Receive (Ptr<Packet> packet, Ptr<MobilityModel> txMobility) UD "<<std::endl;
			loss = m_propgationLossModel->CalcRxPower(pAvg, txMobility,this->GetDevice()->GetNode()->GetObject<MobilityModel>());
		//std::cout<<"void LiFiPhy::Receive (Ptr<Packet> packet) channel gain "<<channelGain<<std::endl;
		}
		else if (m_deviceType == "AP")
		{
		//std::cout<<"void LiFiPhy::Receive (Ptr<Packet> packet, Ptr<MobilityModel> txMobility) AP "<<std::endl;
			loss = m_propgationLossModel->CalcRxPower(pAvg, this->GetDevice()->GetMobilityModel(), txMobility);
		}
	
	m_LifiSNR->SetReceivedPower(loss);
	m_LifiSNR->CalculateSNR();		// caluclates the SNR value
	double snr = m_LifiSNR->GetSNR();
	//ns3::Ptr < ns3::LiFiErrorModel > rxErrorModel = rxNetDevice->GetErrorModel();
	m_error->SetSNR(snr);
	double error = m_error->CalculateErrorRate();
   	//std::cout<<"snr "<<snr<<" loss "<<loss<<" error "<<error<<std::endl;
	if (snr < 2.0)
	{
		snr = 2.0;
	}
	
    ChannelStats chStats;
    chStats.u_id = this->GetDevice()->GetNode()->GetId();
    chStats.simTime = Simulator::Now().GetSeconds();
    chStats.snr = snr;
    chStats.errRate = error;
    chStats.loss = loss;
    chStats.throughput = 0;
    channelStatsVector.push_back (chStats);
	if(startStatsCollection==false) 
    {
      startStatsCollection = true;
      Simulator::Schedule (MilliSeconds (5), &LiFiPhy::ReportMeasurements,this);
    }
	// std::cout<<"m_error->IsCorrupt (packet) "<<m_error->IsCorrupt(packet)<<std::endl;
	//td::cout<<"void LiFiPhy::Receive (Ptr<Packet> packet) LiFiPhy::Receive (Ptr<Packet> packet) error model "<<GetErrorModel ()<<" error "<<error<<"  snr "<<snr<<" channelGain "<<channelGain<<std::endl;
	if (m_error && m_error->IsCorrupt (packet))
	{
		//std::cout<<"Packet dropped "<<m_deviceType<<std::endl;
		return;
	}
	// std::cout<<"void LiFiPhy::Receive (Ptr<Packet> packet, Ptr<MobilityModel> aMobility) "<<std::endl;
	m_mac->Receive(packet);

	
}

void LiFiPhy::ReportMeasurements()
{
	if(channelStatsVector.size() >  0)
	{	
		m_monitorRxDataCallback(this,0,channelStatsVector);
		channelStatsVector.clear();
	}
	// dat_file_out.close();
	Simulator::Schedule (MilliSeconds (5), &LiFiPhy::ReportMeasurements,this);
}

}//end of namespace