#ifndef MY_TAG_H
#define MY_TAG_H
#include "ns3/tag.h"
#include "ns3/packet.h"
#include "ns3/uinteger.h"
#include <iostream>
namespace ns3
{
class MyTag : public Tag
 {
 public:
   static TypeId GetTypeId (void);
   virtual TypeId GetInstanceTypeId (void) const;
   virtual uint32_t GetSerializedSize (void) const;
   virtual void Serialize (TagBuffer i) const;
   virtual void Deserialize (TagBuffer i);
   virtual void Print (std::ostream &os) const;
 
   // these are our accessors to our tag structure
   void SetSimpleValue (uint8_t value);
   uint8_t GetSimpleValue (void) const;
 private:
   uint8_t m_simpleValue;  
 };
 }
 #endif

 
 