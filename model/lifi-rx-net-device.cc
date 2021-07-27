#include "ns3/lifi-rx-net-device.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("LiFiRxNetDevice");

NS_OBJECT_ENSURE_REGISTERED (LiFiRxNetDevice);

ns3::TypeId LiFiRxNetDevice::GetTypeId(void)	// returns meta-information about VlcErrorModel class
		{ 	// including parent class, group name, constructor, and attributes

	static ns3::TypeId tid = ns3::TypeId("LiFiRxNetDevice").SetParent<
			LiFiNetDevice>().AddConstructor<LiFiRxNetDevice>().AddAttribute(
			"FilterGain", "filter gain for the RX device", DoubleValue(0),
			MakeDoubleAccessor(&LiFiRxNetDevice::m_filterGain),
			MakeDoubleChecker<double>()).AddAttribute("PhotoDetectorArea",
			"photo detector area for the RX device", DoubleValue(0),
			MakeDoubleAccessor(&LiFiRxNetDevice::m_photodetectorArea),
			MakeDoubleChecker<double>()).AddAttribute("FOVAngle",
			"field of view angle for the RX device", DoubleValue(0),
			MakeDoubleAccessor(&LiFiRxNetDevice::m_FOVangle),
			MakeDoubleChecker<double>()).AddAttribute("RefractiveIndex",
			"refractive index of the RX device", DoubleValue(0),
			MakeDoubleAccessor(&LiFiRxNetDevice::m_refIndex),
			MakeDoubleChecker<double>()).AddAttribute("AngleOfIncidence",
			"angle of incidence of the RX device", DoubleValue(0),
			MakeDoubleAccessor(&LiFiRxNetDevice::m_angleOfIncidence),
			MakeDoubleChecker<double>()).AddAttribute("ConcentrationGain",
			"concentration gain for the RX device", DoubleValue(0),
			MakeDoubleAccessor(&LiFiRxNetDevice::m_concentrationGain),
			MakeDoubleChecker<double>()).AddAttribute("RXGain",
			"RX gain for the RX device", DoubleValue(0),
			MakeDoubleAccessor(&LiFiRxNetDevice::m_RXGain),
			MakeDoubleChecker<double>()).AddAttribute("Bias",
			"Bias for the RX device", DoubleValue(0),
			MakeDoubleAccessor(&LiFiRxNetDevice::m_bias),
			MakeDoubleChecker<double>());

	return tid;
}

LiFiRxNetDevice::LiFiRxNetDevice() {

  //NS_LOG_FUNCTION(this);
   m_filterGain = 0;
   m_photodetectorArea = 0;
   m_FOVangle = 0;
   m_refIndex = 0;
   m_angleOfIncidence = 0;
   m_concentrationGain = 0;
   m_RXGain = 0;
   m_bias = 0;
   m_error = ns3::CreateObject<ns3::LiFiErrorModel>();
   LiFiNetDevice();
}

void LiFiRxNetDevice::SetForwardingPeerNetDeviceCallback (Callback<void, Ptr<Packet>,const Address&,uint16_t> linkLayerForwardingCallback)
{
    m_linkLayerForwardingCallback = linkLayerForwardingCallback;
}

bool LiFiRxNetDevice::Send (Ptr<Packet> packet, const Address& dest, uint16_t protocolNumber)
{
    NS_LOG_FUNCTION (*packet << " Dest:" << dest << " ProtocolNo:" << protocolNumber);
	NS_ASSERT (Mac48Address::IsMatchingType (dest));
	Mac48Address realTo   = Mac48Address::ConvertFrom (dest);
	Mac48Address realFrom = Mac48Address::ConvertFrom (GetAddress ());
	LlcSnapHeader llc;
	llc.SetType (protocolNumber);
	packet->AddHeader (llc);

	WifiMacHeader hdr;
	//hdr.SetTypeData ();
	//hdr.SetType(WifiMacType::WIFI_MAC_DATA);
	hdr.SetType(WIFI_MAC_DATA);
	hdr.SetAddr1 (realTo);
	hdr.SetAddr3 (realFrom);
	hdr.SetDsFrom ();
	hdr.SetDsNotTo ();
	packet->AddHeader (hdr);
	
	//WifiMacHeader hdr1;
	//packet->RemoveHeader (hdr1);
	
//	LlcSnapHeader llc1;
	//packet->RemoveHeader (llc1);
//packet->AddHeader (llc);

//packet->AddHeader (hdr);
//	DoGetChannel()->Send (packet, this);
	//std::cout<<"\n\n\n\n";
   // llc1.Print(std::cout);
   // std::cout<<"\n\n\n\n";
	//std::cout<<"bool LiFiRxNetDevice::Send (Ptr<Packet> packet, const Address& dest, uint16_t protocolNumber) protocolNumber  protocolNumber protocolNumber "<<protocolNumber<<" realTo "<<realTo<<"  realFrom "<<realFrom<<" llc.SetType "<<llc1.GetType()<<std::endl;
	return true;
}

void LiFiRxNetDevice::SetRecievedDataModel (Ptr<ReceivedDataModel> datamodel)
{
m_ReceivedDataModelVector.push_back(datamodel);
}

std::vector<ns3::Ptr<ReceivedDataModel> > LiFiRxNetDevice::GetMetaData ()
{
	return m_ReceivedDataModelVector;
}

//use this function after setting up FOV and refractive index
void LiFiRxNetDevice::SetConcentrationGain() {
	NS_LOG_FUNCTION(this);
	this->m_concentrationGain = std::pow(this->m_refIndex, 2)
			/ std::pow((float) std::sin((float) m_FOVangle), 2);
}

void LiFiRxNetDevice::AddRXOpticalPowerSignal(double power) {
	NS_LOG_FUNCTION(this<<power);
	this->m_RXOpticalPower.push_back(power);
}
std::vector<double>& LiFiRxNetDevice::GetRXOpticalPowerSignal() {
	NS_LOG_FUNCTION(this);
	return this->m_RXOpticalPower;
}
double LiFiRxNetDevice::GetOpticalPowerSignalAtInstant(int time) {
	NS_LOG_FUNCTION(this<<time);
	return this->m_RXOpticalPower.at(time);
}
void LiFiRxNetDevice::SetRXOpticalPowerSignal(std::vector<double> &powerSignal) {
	NS_LOG_FUNCTION(this<<powerSignal.size());
	this->m_RXOpticalPower = powerSignal;
}

void LiFiRxNetDevice::SetCapacity(int size) {
	NS_LOG_FUNCTION(this<<size);
	this->m_signal.reserve(size);
	this->m_RXOpticalPower.reserve(size);
}

double LiFiRxNetDevice::GetFilterGain() {
	NS_LOG_FUNCTION(this);
	return this->m_filterGain;
}
void LiFiRxNetDevice::SetFilterGain(double fgain) {
	NS_LOG_FUNCTION(this<<fgain);
	this->m_filterGain = fgain;
}

double LiFiRxNetDevice::GetPhotoDetectorArea() {
	NS_LOG_FUNCTION(this);
	return this->m_photodetectorArea;
}

void LiFiRxNetDevice::SetPhotoDectectorArea(double pArea) {
	NS_LOG_FUNCTION(this<<pArea);
	this->m_photodetectorArea = pArea;
}

double LiFiRxNetDevice::GetFOVAngle() {
	NS_LOG_FUNCTION(this);
	return this->m_FOVangle;
}

void LiFiRxNetDevice::SetFOVAngle(double angle) {
	NS_LOG_FUNCTION(this<<angle);
	this->m_FOVangle = angle * M_PI / 180;
}

double LiFiRxNetDevice::GetRefractiveIndex() {
	NS_LOG_FUNCTION(this);
	return this->m_refIndex;
}
void LiFiRxNetDevice::SetRefractiveIndex(double angle) {
	NS_LOG_FUNCTION(this << angle);
	this->m_refIndex = angle;
}

double LiFiRxNetDevice::GetConcentrationGain() {
	NS_LOG_FUNCTION(this);
	return this->m_concentrationGain;
}

double LiFiRxNetDevice::GetRXGain() {
	NS_LOG_FUNCTION(this);
	return this->m_RXGain;
}
void LiFiRxNetDevice::SetRXGain() {
	NS_LOG_FUNCTION(this);
	this->m_RXGain = std::cos(this->m_angleOfIncidence);
}

void LiFiRxNetDevice::SetIncidenceAngle(double angle)
{
	NS_LOG_FUNCTION(this << angle);
	this->m_angleOfIncidence = angle * M_PI / 180;
}

ns3::LiFiErrorModel::ModScheme LiFiRxNetDevice::GeModulationtScheme(void) const {	// returns the modulation scheme used
	NS_LOG_FUNCTION(this);
	return this->m_error->GetScheme();
}

void LiFiRxNetDevice::SetScheme(LiFiErrorModel::ModScheme scheme) { // sets the value of the modulation scheme used
	NS_LOG_FUNCTION(this<<scheme);
	this->m_error->SetScheme(scheme);

}

void LiFiRxNetDevice::SetRandomVariableForErrorModel(
	ns3::Ptr<ns3::RandomVariableStream> ranVar) {// assigns a random variable stream to be used by this model
	NS_LOG_FUNCTION(this<<ranVar);
	this->m_error->SetRandomVariable(ranVar);
}

int64_t LiFiRxNetDevice::AssignStreamsForErrorModel(int64_t stream) {// assigns a fixed stream number to the random variables used by this model
	NS_LOG_FUNCTION(this);
	return this->m_error->AssignStreams(stream);
}

double LiFiRxNetDevice::GetSNRFromErrorModel(void) const {// returns the signal-to-noise ratio (SNR)
	NS_LOG_FUNCTION(this);
	return this->m_error->GetSNR();
}

void LiFiRxNetDevice::SetSNRForErrorModel(double snr) {	// sets the SNR value
	NS_LOG_FUNCTION(this<<snr);
	this->m_error->SetSNR(snr);
}

double LiFiRxNetDevice::CalculateErrorRateForErrorModel() {// calculates the error rate value according to modulation scheme
	NS_LOG_FUNCTION(this);
	return this->m_error->CalculateErrorRate();
}

bool LiFiRxNetDevice::IsCorrupt(ns3::Ptr<ns3::Packet> pkt) {	// determines if the packet is corrupted according to the error model
	NS_LOG_FUNCTION(this<<pkt);
	return this->m_error->IsCorrupt(pkt);
}

// methods for PAM
int LiFiRxNetDevice::GetModulationOrder(void) const {// returns the modulation order (M)
	NS_LOG_FUNCTION(this);
	return this->m_error->GetModulationOrder();
}

void LiFiRxNetDevice::SetModulationOrder(int m_order) {// sets the modulation order value
	NS_LOG_FUNCTION(this<<m_order);
	return this->m_error->SetModulationOrder(m_order);
}

// methods for VPPM
double LiFiRxNetDevice::GetAlpha(void) const {	// returns alpha value
	NS_LOG_FUNCTION(this);
	return this->m_error->GetAlpha();
}
void LiFiRxNetDevice::SetAlpha(double a) 
{		// sets alpha value
	NS_LOG_FUNCTION(this);
	if( m_error->GetScheme()!=LiFiErrorModel::VPPM ){
		throw std::logic_error("Alpha should not be set for these modulation schemes \n");
	}
	this->m_error->SetAlpha(a);
}

double LiFiRxNetDevice::GetBeta(void) const {		// returns beta value
	NS_LOG_FUNCTION(this);
	return this->m_error->GetBeta();
}
void LiFiRxNetDevice::SetBeta(double b) {		// sets beta value
	NS_LOG_FUNCTION(this<<b);
	return this->m_error->SetBeta(b);
}

double LiFiRxNetDevice::GetIncidenceAngle() {
	NS_LOG_FUNCTION(this);
	return this->m_angleOfIncidence;
}

ns3::Ptr<ns3::LiFiErrorModel> LiFiRxNetDevice::GetErrorModel() {
	NS_LOG_FUNCTION(this);
	return this->m_error;
}
//Marked for Rx
void LiFiRxNetDevice::AddLinkChangeCallback (Callback<void> callback)
{
 // m_linkChanges.ConnectWithoutContext (callback);
     LiFiNetDevice::AddLinkChangeCallback (callback);
}


//Marked for Rx
void LiFiRxNetDevice::SetPromiscReceiveCallback (NetDevice::PromiscReceiveCallback cb)
{
  m_promiscRx = cb;
}

void
LiFiRxNetDevice::Receive (Ptr<Packet> packet, uint16_t protocol, Mac48Address to, Mac48Address from)
{
  NS_LOG_FUNCTION (this << packet << protocol << to << from);
 
  //NetDevice::PacketType packetType;
//G
//bool t = false;
  if (GetErrorModel () && GetErrorModel()->IsCorrupt (packet))
    {
      //SetDropPacket(packet);
     // m_ReceivedDatamodel->SetIsCorrupted(true);
		//	std::cout<<"Packet is corrupted"<<std::endl;
      //m_phyRxDropTrace (packet);
      return;
    }
  LiFiNetDevice::Receive ( packet, protocol,to, from);
}

void LiFiRxNetDevice::ForwardUp (Ptr<Packet> packet)
{
  NS_LOG_DEBUG (*packet);

  WifiMacHeader hdr;
  packet->RemoveHeader (hdr);

  if (hdr.IsData () || hdr.IsMgt ())
    {
      NS_LOG_DEBUG ("rx group from=" << hdr.GetAddr2 ());
      WifiMacTrailer fcs;
      packet->RemoveTrailer (fcs);
      LlcSnapHeader llc;
      packet->RemoveHeader (llc);
	
    //  std::cout<<"LiFiRxNetDevice::Receive (Ptr<Packet> packet) hdr.GetAddr1 () "<<hdr.GetAddr1 ()<<" hdr.GetAddr3 () "<<hdr.GetAddr3 ()<<" llc.GetType () "<<llc.GetType ()<<std::endl;
      Receive (packet,llc.GetType (), hdr.GetAddr1 (), hdr.GetAddr3 ());
     // m_rxCallback (packet, &hdr);
    }
  else
    {
      NS_LOG_DEBUG ("Need to drop frame as it is not either data or management frame");
    }

  return;
}

void LiFiRxNetDevice::EnqueueDataPacketAfterCorruption(Ptr<Packet> p,
		bool corruptFlag) {
	NS_LOG_FUNCTION(this<<p<<corruptFlag);
	this->dataPool.push_back(p);
	this->packetCorruptionState.push_back(corruptFlag);
}

int LiFiRxNetDevice::ComputeGoodPut() {
	NS_LOG_FUNCTION(this);
	int goodPacketSize = 0;
	for (uint32_t i = 0; i < this->dataPool.size(); i++) {
		if (!this->packetCorruptionState.at(i)) {
			goodPacketSize += this->dataPool.at(i)->GetSize();
		}
	}

	return goodPacketSize;
}

LiFiRxNetDevice::~LiFiRxNetDevice() {
	// TODO Auto-generated destructor stub
}

/////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////

} /* namespace vlc */
