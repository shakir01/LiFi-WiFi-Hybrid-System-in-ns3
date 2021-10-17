
#ifndef LIFI_FRAME_HEADER_H
#define LIFI_FRAME_HEADER_H

#include "ns3/object.h"
#include "ns3/tdma-mac.h"
#include <string>
#include "ns3/address.h"
#include <iostream>
#include "ns3/log.h"

namespace ns3 {

class LiFiFrameHeader: public Tag	// This class is a subclass of Object class
{
public:
	// Public methods
   	static TypeId GetTypeId (void);
	virtual TypeId GetInstanceTypeId (void) const;  
	virtual uint32_t GetSerializedSize (void) const;
	virtual void Serialize (TagBuffer i) const;
    virtual void Deserialize (TagBuffer i);
     // virtual void Print (std::ostream &os) const;  
	void SetCurrentSlot (uint8_t);
	uint8_t GetCurrentSlot ();
private:
	uint8_t m_currentSlot;
};

} // namespace ns3
#endif
 