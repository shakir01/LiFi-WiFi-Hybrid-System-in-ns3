#ifndef LIFI_USER_DEVICE_INFOS_H
#define LIFI_USER_DEVICE_INFOS_H

#include "ns3/object.h"
#include <string>
#include "ns3/address.h"
#include <iostream>
#include "ns3/log.h"
#include "ns3/commons.h"

namespace ns3 {
class TdmaMac;

class LiFiUserDeviceInfos: public Object	// This class is a subclass of Object class
{
public:
	// Public methods
	static TypeId GetTypeId(void); // returns meta-information about PAMErrorModel class 

	LiFiUserDeviceInfos();	// constructor
	~LiFiUserDeviceInfos();	//destructor
	void SetDeviceName (std::string);
    std::string GetDeviceName ();
	void SetDeviceID (uint32_t);
	uint32_t GetDeviceID ();
	void SetSinrLevel (double);
	double GetSinrLevel ();

	
	void SetThroughput (std::string);
	void SetDestinationAddress (Mac48Address macAddress);
	Mac48Address GetDestinationAddress ();
	std::string GetThroughput ();
    void SetServiceRequirements (std::string serviceRequirements);
    std::string GetServiceRequirements ( ); 
	void SetQoSRequirements(uint8_t);
	uint8_t GetQoSRequirements(void);
	void SetNodeId(uint32_t ndId);
	uint32_t GetNodeId();
	void UpdateCurrentSlot ();
	uint16_t GetCurrentSlotNumber ();
	void ResetCurrentSlot ();
	
	bool IsUplinkHeavy();
	bool IsDownlinkHeavy();
	bool IsSymmetric();

	void SetUplinkHeavy(bool);
	void SetDownlinkHeavy(bool);
	void SetSymmetricTraffic(bool);

	void SetServiceType (uint8_t);
	uint8_t GeetServiceType ();



private:
	bool m_uplinkHeavy;
	bool m_downlinkHeavy;
	bool m_symmetric;

    std::string m_name;
    uint32_t m_deviceID;
	uint32_t m_NodeID;

    double m_sinrLevel;
    std::string m_throughput;
	std::string m_serviceRequirements;
	uint8_t m_qos_category;
	uint8_t m_ServiceType;
	Mac48Address m_desntationAddress;
	uint16_t m_current_slot;
};

} // namespace ns3
#endif
