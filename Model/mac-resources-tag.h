#ifndef MAC_RESOURCES_TAG_H
#define MAC_RESOURCES_TAG_H

#include "ns3/object.h"
#include "ns3/tdma-mac.h"
#include <string>
#include "ns3/address.h"
#include <iostream>
#include "ns3/log.h"

namespace ns3 {

class MacResourcesTag: public Tag	// This class is a subclass of Object class
{
public:
	// Public methods
   	static TypeId GetTypeId (void);
	virtual TypeId GetInstanceTypeId (void) const;  
	virtual uint32_t GetSerializedSize (void) const;
	virtual void Serialize (TagBuffer i) const;
    virtual void Deserialize (TagBuffer i);
     // virtual void Print (std::ostream &os) const;  
	void SetDeviceID (uint32_t);
	uint32_t GetDeviceID ();  
	
	void  SetTotalSlots (uint8_t*);
	uint8_t* GetTotalSlots (void);
	uint8_t* GetDownlinkSlots (void);
	uint8_t* GetUplinkSlots (void);  
	void SetDownlinkSlots (uint8_t* slots);
	void SetUplinkSlots (uint8_t* slots);
	void SetDestinationAddress(Mac48Address);
	Mac48Address GetDestinationAddress(void);

private:
	uint32_t m_deviceId;
	uint8_t* slotsArray;
	uint8_t* downlinkSlotsArray;
	uint8_t* uplinkSlotsArray;
	Mac48Address m_destinationAddress;
};

} // namespace ns3
#endif
 