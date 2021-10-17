#include "ns3/lifi-tx-net-device.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("LiFiTxNetDevice");
NS_OBJECT_ENSURE_REGISTERED (LiFiTxNetDevice);

ns3::TypeId LiFiTxNetDevice::GetTypeId(void)	// returns meta-information about VLC_ErrorModel class
		{ 	// including parent class, group name, constructor, and attributes

	static ns3::TypeId tid = ns3::TypeId("LiFiTxNetDevice").SetParent<
			LiFiNetDevice>().AddConstructor<LiFiTxNetDevice>().AddAttribute(
			"SemiAngle", "Semi angle for the TX device", DoubleValue(0),
			MakeDoubleAccessor(&LiFiTxNetDevice::m_semiangle),
			MakeDoubleChecker<double>()).AddAttribute("AngleOfRadiance",
			"Angle of radiance for the TX device", DoubleValue(0),
			MakeDoubleAccessor(&LiFiTxNetDevice::m_angleOfRadiance),
			MakeDoubleChecker<double>()).AddAttribute("LambertianOrder",
			"Lambertian Order for the TX device", DoubleValue(0),
			MakeDoubleAccessor(&LiFiTxNetDevice::m_lOrder),
			MakeDoubleChecker<double>()).AddAttribute("Gain",
			"TX gain for the TX device", DoubleValue(0),
			MakeDoubleAccessor(&LiFiTxNetDevice::m_TXGain),
			MakeDoubleChecker<double>()).AddAttribute("Bias",
			"Biasing voltage for the TX device", DoubleValue(0),
			MakeDoubleAccessor(&LiFiTxNetDevice::m_bias),
			MakeDoubleChecker<double>());
	return tid;

}

LiFiTxNetDevice::LiFiTxNetDevice() :
	m_TMAX(1) {
	LiFiNetDevice();
	m_semiangle = 0;
	m_angleOfRadiance = 0;
	m_lOrder = 1;
	m_TXGain = 0;
	m_bias = 0;

}

void LiFiTxNetDevice::AddReceiverModel (Ptr<LiFiUserDeviceInfos> recieverModel)
{
  //m_ReceiverModelList.push_back(recieverModel);
}

void LiFiTxNetDevice::RemoveRxDevice (uint32_t index)
{
 /*uint32_t itemIndex=-1;
  for (uint32_t i = 0; i < m_ReceiverModelList.size(); i++)
  {
    if (m_ReceiverModelList[i]->GetDeviceID() == index)
    {
      itemIndex = i;
      break;
    }
  }

  if (itemIndex >= 0)
  {
    m_ReceiverModelList.erase(m_ReceiverModelList.begin() + itemIndex);
  }*/
}
/*
void LiFiTxNetDevice::DoInitialize (void)
{
  m_mac->Initialize ();
  LiFiNetDevice::DoInitialize ();
}

void
LiFiTxNetDevice::DoDispose (void)
{
  m_mac->Dispose ();
  m_mac = 0;
  m_channel = 0;
  m_tdmaController = 0;

  // chain up.
  LiFiNetDevice::DoDispose ();
}

uint32_t
LiFiTxNetDevice::GetIfIndex (void) const
{
  return LiFiNetDevice::GetIfIndex();
}

void LiFiTxNetDevice::CompleteConfig (void)
{
  NS_LOG_FUNCTION_NOARGS ();
std::cout<<"m_mac "<<m_mac<<" , "<<" LiFiNetDevice::GetNode () "<<LiFiNetDevice::GetNode ()<<" m_channel "<<m_channel<<"   m_tdmaController "<<m_tdmaController<<" m_configComplete "<<m_configComplete<<std::endl;
  if (m_mac == 0 || LiFiNetDevice::GetNode () == 0 || m_channel == 0 || m_tdmaController == 0 || m_configComplete ||  m_ReceiverModelList.size() <= 0)
    {
      return;
    }

    for (uint32_t i = 0; i < m_ReceiverModelList.size(); i++)
	{
		  Ptr<LiFiUserDeviceInfos>  receiver = m_ReceiverModelList [i];
		  Ptr<TdmaMac> mac = receiver->GetMac();
		 //setup mac
		  mac->SetTdmaController (m_tdmaController);
		  mac->SetChannel (m_channel);
		  // setup callbacks
		  mac->SetLinkUpCallback (MakeCallback (&LiFiNetDevice::LinkUp, this));
		  mac->SetLinkDownCallback (MakeCallback (&LiFiNetDevice::LinkDown, this));
		  mac->SetTxQueueStartCallback (MakeCallback (&LiFiTxNetDevice::TxQueueStart, this));
		  mac->SetTxQueueStopCallback (MakeCallback (&LiFiTxNetDevice::TxQueueStop, this));
   }

  //setup mac
  m_mac->SetTdmaController (m_tdmaController);
  m_mac->SetChannel (m_channel);
  // setup callbacks
  m_mac->SetLinkUpCallback (MakeCallback (&LiFiNetDevice::LinkUp, this));
  m_mac->SetLinkDownCallback (MakeCallback (&LiFiNetDevice::LinkDown, this));
  m_mac->SetTxQueueStartCallback (MakeCallback (&LiFiTxNetDevice::TxQueueStart, this));
  m_mac->SetTxQueueStopCallback (MakeCallback (&LiFiTxNetDevice::TxQueueStop, this));

  m_configComplete = true;
}
*/

LiFiTxNetDevice::~LiFiTxNetDevice() {

}

/*void LiFiTxNetDevice::SetAddress (Address address)
{
  m_mac->SetAddress (Mac48Address::ConvertFrom (address));
}

Address LiFiTxNetDevice::GetAddress (void) const
{
  return m_mac->GetAddress ();
}

void LiFiTxNetDevice::SetChannel (Ptr<LiFiChannel> channel)
{
  if (channel != 0)
    {

      m_channel = channel;
      LiFiNetDevice::SetChannel (m_channel);
      CompleteConfig ();
    }
}

Ptr<Channel> LiFiTxNetDevice::GetChannel (void) const
{
  return m_channel;
}

Ptr<LiFiChannel> LiFiTxNetDevice::DoGetChannel (void) const
{
  return m_channel;
}

void
LiFiTxNetDevice::SetQueueStateChangeCallback (Callback<void,uint32_t> callback)
{
  m_queueStateChanges.ConnectWithoutContext (callback);
}

bool
LiFiTxNetDevice::TxQueueStart (uint32_t index)
{
  m_queueStateChanges (index);
  return true;
}

bool
LiFiTxNetDevice::TxQueueStop (uint32_t index)
{
  m_queueStateChanges (index);
  return true;
}

uint32_t
LiFiTxNetDevice::GetQueueState (uint32_t index)
{
  return m_mac->GetQueueState (index);
}

uint32_t
LiFiTxNetDevice::GetNQueues (void)
{
  return m_mac->GetNQueues ();
}

void LiFiTxNetDevice::SetMac (Ptr<TdmaMac> mac)
{
  m_mac = mac;
  m_mac->SetDevice (this);
  CompleteConfig ();
}

Ptr<TdmaMac> LiFiTxNetDevice::GetMac (void) const
{
  return m_mac;
}


void
LiFiTxNetDevice::SetTdmaController (Ptr<TdmaController> controller)
{
  m_tdmaController = controller;
  CompleteConfig ();
}

Ptr<TdmaController>
LiFiTxNetDevice::GetTdmaController (void) const
{
  return m_tdmaController;
}


//Marked for Tx
bool
LiFiTxNetDevice::SupportsSendFrom (void) const
{
  return m_mac->SupportsSendFrom ();
}

*/

bool LiFiTxNetDevice::Send (Ptr<Packet> packet, const Address& dest, uint16_t protocolNumber)
{
  NS_LOG_FUNCTION (*packet << " Dest:" << dest << " ProtocolNo:" << protocolNumber);
 
  NS_ASSERT (Mac48Address::IsMatchingType (dest));
  //Mac48Address realTo   = Mac48Address::ConvertFrom (dest);
  //Mac48Address realFrom = Mac48Address::ConvertFrom (GetAddress ());
  LlcSnapHeader llc;
  llc.SetType (protocolNumber);
  packet->AddHeader (llc);
  /*Ptr<LiFiUserDeviceInfos> LiFiUserDeviceInfos = SearchForReceiverModel (dest);
	if (LiFiUserDeviceInfos)
  {
		  LiFiUserDeviceInfos->GetMac()->Enqueue (packet, realTo,realFrom);
  std::cout<<"bool LiFiTxNetDevice::Send  At Time "<<Simulator::Now().GetSeconds()<<*packet << " Dest:" << dest << " ProtocolNo:" <<protocolNumber<<std::endl;
  }*/
  //std::cout<<"bool LiFiTxNetDevice::Send  At Time "<<Simulator::Now().GetSeconds()<<*packet << " Dest:" << dest << " ProtocolNo:" <<protocolNumber<<std::endl;

  //GetMac()->Enqueue (packet, realTo,realFrom);
  return true;
}

bool LiFiTxNetDevice::SendFrom (Ptr<Packet> packet, const Address& source, const Address& dest, uint16_t protocolNumber)
{
  NS_LOG_FUNCTION_NOARGS ();
  NS_ASSERT (Mac48Address::IsMatchingType (dest));
  NS_ASSERT (Mac48Address::IsMatchingType (source));
  //Mac48Address realTo = Mac48Address::ConvertFrom (dest);
  //Mac48Address realFrom = Mac48Address::ConvertFrom (source);
  LlcSnapHeader llc;
  llc.SetType (protocolNumber);
  packet->AddHeader (llc);
  //Ptr<LiFiUserDeviceInfos> LiFiUserDeviceInfos = SearchForReceiverModel (dest);

  //LiFiUserDeviceInfos->GetMac()->Enqueue (packet, realTo,realFrom);
  return true;
}

double LiFiTxNetDevice::GetLambertianOrder() {
      return this->m_lOrder;
}

//before setting Lambertian Order make sure the semiangle value is set
//Need to setup error handling when semiangle is not set
void LiFiTxNetDevice::SetLambertainOrder()
{
	this->m_lOrder = (-1 * log(2)) / (log(cos(this->m_semiangle)));
}

void LiFiTxNetDevice::AddTXOpticalPowerSignal(double power) {
	this->m_TXOpticalPower.push_back(power);
}

std::vector<double>& LiFiTxNetDevice::GetTXOpticalPowerSignal() {

	return this->m_TXOpticalPower;
}

double LiFiTxNetDevice::GetOpticalPowerSignalAtInstant(int time) {
	return this->m_TXOpticalPower.at(time);
}

void LiFiTxNetDevice::SetTXOpticalPowerSignal(std::vector<double> &powerSignal) {
	this->m_TXOpticalPower = powerSignal;
}

double LiFiTxNetDevice::GetTXPowerMax() {
	return this->m_TMAX;
}

void LiFiTxNetDevice::SetCapacity(int size) {
	m_TXOpticalPower.reserve(size);
	m_signal.reserve(size);
}

double LiFiTxNetDevice::GetSemiangle() {
	return this->m_semiangle;
}

void LiFiTxNetDevice::SetSemiangle(double angle) {
	this->m_semiangle = angle * M_PI / 180;
	this->SetLambertainOrder();
}

void LiFiTxNetDevice::SetAngleOfRadiance(double angle) {
	this->m_angleOfRadiance = angle * M_PI / 180;
}

double LiFiTxNetDevice::GetAngleOfRadiance() {
	return this->m_angleOfRadiance;
}

double LiFiTxNetDevice::GetTXGain() {
	return this->m_TXGain;
}
void LiFiTxNetDevice::SetTXGain() {
	//Channel loss is divided into TX gain and RX gain, which are used to calculate the loss.

	this->m_TXGain = ((this->m_lOrder + 1) / (2 * M_PI))
			* std::pow(std::cos((long double) this->m_angleOfRadiance),
					this->m_lOrder);
}

void LiFiTxNetDevice::AddSignal(double signal) {
	this->m_signal.push_back(signal);
}

std::vector<double>& LiFiTxNetDevice::GetSignal() {
	return this->m_signal;
}

double LiFiTxNetDevice::GetSignalAtInstant(int time) {
	return this->m_signal.at(time);
}

void LiFiTxNetDevice::SetSignal(std::vector<double> &signal) {
	this->m_signal = signal;
}

void LiFiTxNetDevice::SetBias(double bias) {
	this->m_bias = bias;
}

double LiFiTxNetDevice::GetBias() {
	return m_bias;
}

void LiFiTxNetDevice::BoostSignal() {

	m_TXOpticalPower.clear();

	for (unsigned int i = 0; i < m_signal.size(); i++) {
		m_TXOpticalPower.push_back((double) (m_signal.at(i) + m_bias));
		if (m_TXOpticalPower.at(i) > m_TMAX) {
			m_TXOpticalPower.at(i) = m_TMAX;
		}
	}
}

double LiFiTxNetDevice::GetAveragePowerSignalPower() {
	double pMax = *std::max_element(this->m_TXOpticalPower.begin(), this->m_TXOpticalPower.end());
	double pMin = *std::min_element(this->m_TXOpticalPower.begin(), this->m_TXOpticalPower.end());
	return (pMax + pMin) / 2;
}

double LiFiTxNetDevice::GetAverageSignal() {
	double pMax = *std::max_element(this->m_signal.begin(), this->m_signal.end());
	double pMin = *std::min_element(this->m_signal.begin(), this->m_signal.end());
	return (pMax + pMin) / 2;
}

void LiFiTxNetDevice::EnqueueDataPacket(Ptr<Packet> p) {
	//Ptr<LiFiChannel> m_channel = LiFiNetDevice::DoGetChannel ();
	//m_channel->TransmitDataPacket(p);
}

} /* namespace vlc */
