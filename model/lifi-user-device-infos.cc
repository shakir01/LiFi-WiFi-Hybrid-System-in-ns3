#include "lifi-user-device-infos.h"

namespace ns3 {
NS_OBJECT_ENSURE_REGISTERED (LiFiUserDeviceInfos);
NS_LOG_COMPONENT_DEFINE ("LiFiUserDeviceInfos");

TypeId LiFiUserDeviceInfos::GetTypeId()	// returns meta-information about VLC_ErrorModel class
		{ 	// including parent class, group name, constructor, and attributes

	static ns3::TypeId tid = ns3::TypeId("LiFiUserDeviceInfos").SetParent<Object>().AddConstructor<LiFiUserDeviceInfos>();/*AddAttribute("RxName",
					"Rx Device Name", StringValue(""),
					MakeStringAccessor(&LiFiUserDeviceInfos::m_name),
					MakeStringChecker<double>()).AddAttribute("RxID",
					"Rx Device ID", IntegerValue(0),
					MakeIntegerAccessor(&LiFiUserDeviceInfos::m_deviceID),
					MakeIntegerChecker<int>()).AddAttribute("AvgThrougput",
					"Avergage Throughput at Rx", DoubleValue(0),
					MakeDoubleAccessor(&LiFiUserDeviceInfos::m_throughput),
					MakeDoubleChecker<Double>()).AddAttribute("RxQoS",
					"Service Requirements of Rx", DoubleValue(0),
					MakeDoubleAccessor(&LiFiUserDeviceInfos::m_serviceRequirements),
					MakeDoubleChecker<double>())*/;
	return tid;

}

void LiFiUserDeviceInfos::ResetCurrentSlot ()
{
    m_current_slot = 0;
}

void LiFiUserDeviceInfos::UpdateCurrentSlot ()
{
    m_current_slot = m_current_slot + 1;
}

uint16_t LiFiUserDeviceInfos::GetCurrentSlotNumber ()
{
  return m_current_slot;
}

LiFiUserDeviceInfos::LiFiUserDeviceInfos()	// constructor
{
   m_name = "";
   m_deviceID = 0;
   m_throughput ="";
   m_serviceRequirements = "";
}

LiFiUserDeviceInfos::~LiFiUserDeviceInfos() 	//destructor
{

}

void LiFiUserDeviceInfos::SetDestinationAddress (Mac48Address macAddress)
{
  m_desntationAddress = macAddress;
}

Mac48Address LiFiUserDeviceInfos::GetDestinationAddress ()
{
  return m_desntationAddress;
}

bool LiFiUserDeviceInfos::IsUplinkHeavy()
{
  return m_uplinkHeavy;
}

bool LiFiUserDeviceInfos::IsDownlinkHeavy()
{
  return m_downlinkHeavy;
}

void LiFiUserDeviceInfos::SetServiceType (uint8_t trafficType)
{
  m_ServiceType = trafficType;
}
	
uint8_t LiFiUserDeviceInfos::GeetServiceType ()
{
  return m_ServiceType;
}

bool LiFiUserDeviceInfos::IsSymmetric()
{
  return m_symmetric;
}

void LiFiUserDeviceInfos::SetUplinkHeavy(bool link)
{
  m_uplinkHeavy = link;
}

void LiFiUserDeviceInfos::SetDownlinkHeavy(bool link)
{
  m_downlinkHeavy = link;
}

void LiFiUserDeviceInfos::SetSymmetricTraffic(bool link)
{
  m_symmetric = link;
}

void LiFiUserDeviceInfos::SetNodeId(uint32_t ndId)
{
  this->m_NodeID = ndId;
}
	
uint32_t LiFiUserDeviceInfos::GetNodeId()
{
  return  m_NodeID;
}

void LiFiUserDeviceInfos::SetDeviceName (std::string name)
{
  m_name = name;
}

std::string LiFiUserDeviceInfos::GetDeviceName ()
{
  return m_name;	
}

void LiFiUserDeviceInfos::SetDeviceID (uint32_t deviceID)
{
  m_deviceID = deviceID;
}

void LiFiUserDeviceInfos::SetQoSRequirements(uint8_t qos)
{
  m_qos_category = qos;
}

uint8_t LiFiUserDeviceInfos::GetQoSRequirements(void)
{
  return m_qos_category;
}

uint32_t LiFiUserDeviceInfos::GetDeviceID ()
{
  return m_deviceID;
}

void LiFiUserDeviceInfos::SetSinrLevel (double sinrLevel)
{
  m_sinrLevel = sinrLevel;
}

double LiFiUserDeviceInfos::GetSinrLevel ()
{
  return m_sinrLevel;
}

void LiFiUserDeviceInfos::SetThroughput (std::string throughput)
{
  m_throughput = throughput;
}

std::string LiFiUserDeviceInfos::GetThroughput ( )
{
  return m_throughput;
}

void LiFiUserDeviceInfos::SetServiceRequirements (std::string serviceRequirements)
{
  m_serviceRequirements = serviceRequirements;
}

std::string LiFiUserDeviceInfos::GetServiceRequirements ( ) 
{
  return m_serviceRequirements;
}
}

