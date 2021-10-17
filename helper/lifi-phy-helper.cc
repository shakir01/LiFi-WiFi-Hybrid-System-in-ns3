#include "ns3/lifi-phy-helper.h"
#include <string>
//NS_LOG_COMPONENT_DEFINE ("ns3::LiFiPhyHelper");
//NS_LOG_COMPONENT_DEFINE ("ns3::LiFiChannelHelper");

namespace ns3
{
//NS_OBJECT_ENSURE_REGISTERED (LiFiPhyHelper);
//NS_OBJECT_ENSURE_REGISTERED (LiFiChannelHelper);
TypeId LiFiChannelHelper::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::LiFiChannelHelper").SetParent<Object> ().AddConstructor<LiFiChannelHelper> ();
  return tid;
}

LiFiChannelHelper::LiFiChannelHelper () 
{
}

LiFiChannelHelper::~LiFiChannelHelper () 
{
}

Ptr<LiFiChannel> LiFiChannelHelper::Create () 
{
  Ptr<LiFiChannel> channel = CreateObject<LiFiChannel> ();
  return channel;
}

void LiFiChannelHelper::Create (ns3::Ptr<Node> node) 
{
  Ptr<LiFiChannel> channel = CreateObject<LiFiChannel> ();
  channels_map.insert (std::pair<uint32_t,Ptr<LiFiChannel> >(node->GetId(), channel)); 
}

void LiFiChannelHelper::AddPhy (ns3::Ptr<Node> node,Ptr<LiFiPhy> phy) 
{
  Ptr<LiFiChannel> channel = 0;
  std::map <uint32_t, Ptr<ns3::LiFiChannel> >:: iterator it1;
  it1 = channels_map.find(node->GetId());
  if (it1 != channels_map.end())
  {
    	channel = it1->second;
  }
  
  if (channel)
  {
	 channel->AddPhy(phy);
  }
}

ns3::Ptr<LiFiChannel> LiFiChannelHelper::GetChannel (uint32_t id)
{
  std::map <uint32_t, Ptr<ns3::LiFiChannel> >:: iterator it1;
  //std::map<std::string, std::vector<LiFiWiFiFlowStats> >;
  it1 = channels_map.find(id);
  if (it1 != channels_map.end())
  {
    	return it1->second;
  }

  return NULL;
}

LiFiPhyHelper::LiFiPhyHelper () 
{
}

LiFiPhyHelper::~LiFiPhyHelper () 
{
}

TypeId LiFiPhyHelper::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::LiFiPhyHelper").SetParent<Object> ().AddConstructor<LiFiPhyHelper> ();
  return tid;
}

Ptr<LiFiPhy> LiFiPhyHelper::CreatePhy () 
{
	Ptr<LiFiPhy> phy = CreateObject<LiFiPhy> ();
	return phy;
}

void LiFiPhyHelper::Create (NodeContainer nodeContainer, LifiPhyParameters parameters) 
{
	for (size_t i = 0; i < nodeContainer.GetN(); i++)
	{
		Ptr<Node> node = nodeContainer.Get(i);
		Ptr<LiFiPhy> phy = CreatePhy();
		SetParameters(phy,parameters);
		AttachPropagationLossModel(phy);
		AddError(phy,parameters);
		AttachSNRModel(phy);
		std::map <uint32_t, Ptr<ns3::LiFiPhy>>:: iterator it1;
		it1 = phy_map.find(node->GetId());
		if (it1 == phy_map.end())
		{
			phy_map.insert (it1,{node->GetId(), phy}); 
		}
	} 
}

Ptr<LiFiPhy> LiFiPhyHelper::GetPhy (uint32_t id)
{
  std::map <uint32_t, Ptr<ns3::LiFiPhy> >:: iterator it1;
  //std::map<std::string, std::vector<LiFiWiFiFlowStats> >;
  it1 = phy_map.find(id);
  if (it1 != phy_map.end())
  {
    	return it1->second;
  }

  return NULL;
}

void LiFiPhyHelper::AttachPropagationLossModel(ns3::Ptr<LiFiPhy> phy)
{
  ns3::Ptr<ns3::LiFiPropagationLossModel >  m_loss = CreateObject<LiFiPropagationLossModel> ();
  m_loss->SetFilterGain(phy->GetFilterGain());
  m_loss->SetConcentratorGain(phy->GetConcentrationGain());
  m_loss->SetArea(phy->GetPhotoDetectorArea());
  phy->SetLossModel(m_loss);
}

void LiFiPhyHelper::SetParameters(Ptr<LiFiPhy> phy, LifiPhyParameters parameters)
{
	double semiAngle = parameters.semiAngle* M_PI / 180;
	phy->SetSemiangle(semiAngle);
	phy->SetFilterGain(parameters.filterGain);
	phy->SetPhotoDectectorArea(parameters.photoDetectorArea);
	phy->SetFOVAngle((parameters.FOVAngle* M_PI / 180));
	phy->SetRefractiveIndex(parameters.refractiveIndex);
	phy->SetConcentrationGain();
}

void LiFiPhyHelper::AddError (ns3::Ptr<LiFiPhy> appPhy,LifiPhyParameters parameters) 
{
	Ptr<LiFiErrorModel> error = CreateObject<LiFiErrorModel> ();
	if(LiFiErrorModel::VPPM == parameters.modulation)
	{
		error->SetAlpha(parameters.dutyCycle);
	}
	error->SetBeta(parameters.beta);
	error->SetModulationOrder(parameters.modulation);
	appPhy->SetLambertainOrder();
	appPhy->SetErrorRateModel (error);
	appPhy->SetModulationOrder( parameters.modulation );
}

void LiFiPhyHelper::AttachSNRModel(ns3::Ptr<LiFiPhy> phy)
{
	ns3::Ptr<ns3::LiFiSnr >  m_snr = CreateObject<LiFiSnr>();
  m_snr->SetWavelength(380, 780);
  m_snr->SetTemperature(295);
  m_snr->SetElectricNoiseBandWidth(3 * 1e5);
  m_snr->CalculateNoiseVar(phy->GetPhotoDetectorArea());
	phy->SetSNR(m_snr);
}

}//end namespace ns3

