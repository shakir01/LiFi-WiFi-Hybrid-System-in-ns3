#ifndef LIFI_PHY_HELPER_H
#define LIFI_PHY_HELPER_H
#include <string>
#include "ns3/attribute.h"
#include "ns3/object-factory.h"
#include "ns3/node-container.h"
#include "ns3/net-device-container.h"
#include "ns3/wifi-phy-standard.h"
#include "ns3/trace-helper.h"
#include "ns3/wifi-mac-helper.h"
#include "ns3/wifi-helper.h"
#include "ns3/lifi-channel.h"
#include "ns3/lifi-phy.h"
#include <iostream>
#include <map>
#include "ns3/commons.h"
#include "ns3/lifi-error-model.h"
#include "ns3/object-factory.h"

namespace ns3 
{
 class LiFiPhy;
 class LiFiErrorModel;
 
class LiFiChannelHelper:public Object
{
public:
 
  LiFiChannelHelper ();
  ~LiFiChannelHelper ();
    static TypeId GetTypeId (void);

  Ptr<LiFiChannel> Create (void);
  void Create (ns3::Ptr<Node> node); 
  void AddPhy (ns3::Ptr<Node> node,Ptr<LiFiPhy> phy);
  ns3::Ptr<LiFiChannel> GetChannel (uint32_t id);
private:
  std::map <uint32_t, Ptr<ns3::LiFiChannel> > channels_map;
};

class LiFiPhyHelper: public Object
{
public:
  LiFiPhyHelper ();
  ~LiFiPhyHelper ();
  static TypeId GetTypeId (void);

  Ptr<LiFiPhy> CreatePhy (); 
  Ptr<LiFiPhy> Create (Ptr<Node> node);
  void Create (NodeContainer nodeContainer, LifiPhyParameters parameters);
  void SetParameters(Ptr<LiFiPhy> node, LifiPhyParameters parameters);
  void AttachPropagationLossModel(ns3::Ptr<LiFiPhy> phy);
  void AddError (ns3::Ptr<LiFiPhy> appPhy,LifiPhyParameters parameters);
  void AttachSNRModel(ns3::Ptr<LiFiPhy> phy);
  Ptr<LiFiPhy> GetPhy (uint32_t id);
  
protected:
  //virtual Ptr<LiFiPhy> Create (Ptr<Node> node, Ptr<NetDevice> device) const;
   std::map <uint32_t, Ptr<ns3::LiFiPhy> > phy_map;
};
}
#endif
