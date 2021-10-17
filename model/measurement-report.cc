#include "measurement-report.h"

   
namespace ns3 {
 TypeId 
 MeasurementReport::GetTypeId (void)
 {
   static TypeId tid = TypeId ("ns3::MeasurementReport")
     .SetParent<Tag> ()
     .AddConstructor<MeasurementReport> ()
   ;
   return tid;
 }

MeasurementReport::MeasurementReport()
 {
    m_device_id = 0;
 }

 void MeasurementReport::SetDeviceID (uint32_t devId)
 {
   m_device_id = devId;
 }

uint32_t MeasurementReport::GetDeviceID ()
{
  return m_device_id;
}  

 MeasurementReport::~MeasurementReport()
{
//delete downlinkSlotsArray;
//	delete uplinkSlotsArray;
}

TypeId 
 MeasurementReport::GetInstanceTypeId (void) const
 {
   return GetTypeId ();
 }
 uint32_t 
 MeasurementReport::GetSerializedSize (void) const
 {
   return sizeof(uint32_t)+sizeof(uint16_t) + sizeof(double)*m_no_measurements /*+ sizeof(double)*m_no_measurements + sizeof(uint32_t)*m_no_measurements"*/;
 }

 void MeasurementReport::Serialize (TagBuffer i) const
 {
  i.WriteU32 (m_device_id);
  i.WriteU16 (m_no_measurements);
  if (m_no_measurements > 0)
  {    
      for (size_t ii = 0; ii < m_no_measurements; ii++)
      {
        double val = m_snr_vector[ii];
        i.WriteDouble (val);
       
      }

      /*{for (size_t ii = 0; ii < m_no_measurements; ii++)
      {
        i.WriteDouble (m_error_vector[ii]);
       
      }

      for (size_t ii = 0; ii < m_no_measurements; ii++)
      {
        i.WriteU32 (m_dropped_vector[ii]);
      }*/    
  }
  //delete slotsArray;
  
 }

 void MeasurementReport::Deserialize (TagBuffer i)
 {
  m_device_id              = i.ReadU32 ();
  m_no_measurements        = i.ReadU16 ();
  //std::cout<<"m_device_id: "<<m_device_id<<" m_no_measurements: "<<m_no_measurements<<std::endl;

  if (m_no_measurements > 0)
  {    

       for (size_t ii = 0; ii < m_no_measurements; ii++)
      {
        double snrRead = i.ReadDouble ();
       // std::cout<<"snrRead: "<<snrRead<<std::endl;

        m_snr_vector.push_back(snrRead);
      }

      /* for (size_t ii = 0; ii < m_no_measurements; ii++)
      {
        m_error_vector[ii] = i.ReadDouble ();
      }

       for (size_t ii = 0; ii < m_no_measurements; ii++)
      {
        m_dropped_vector[ii] = i.ReadU32 ();
      }*/
  }

 }

 void 
 MeasurementReport::Print (std::ostream &os) const
 {
  // os << "v=" << (uint32_t)m_simpleValue;m_ids_vector
  } 

	void MeasurementReport::SetSNR (std::vector<double> snrV)
  {
    m_snr_vector = snrV;
    m_no_measurements = snrV.size();
  }

	std::vector<double>  MeasurementReport::GetSNR (void)
  {
    return m_snr_vector;
  }

	void MeasurementReport::SetErrorRate (std::vector<double> errorV)
  {
    m_error_vector = errorV;
  }

	std::vector<double> MeasurementReport::GetErrorRate (void)
  {
    return m_error_vector;
  }

  void MeasurementReport::SetDroppedPackets (std::vector<uint32_t> droppedPacketV)
  {
    m_dropped_vector = droppedPacketV;
  }

	std::vector<uint32_t> MeasurementReport::GetDroppedPackets (void)
  {
    return m_dropped_vector;
  }

}

