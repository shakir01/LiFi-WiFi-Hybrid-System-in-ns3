
#include "ns3/lifi-constant-velocity-mobility-model.h"
#include "ns3/simulator.h"
#include <iostream>
#include <fstream>
#include <sstream>

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (LiFiConstantVelocityMobilityModel);


ns3::TypeId LiFiConstantVelocityMobilityModel::GetTypeId(void) {
	static ns3::TypeId tid = ns3::TypeId("ns3::LiFiConstantVelocityMobilityModel").SetParent<
			MobilityModel>().SetGroupName("Mobility").AddConstructor<
			LiFiConstantVelocityMobilityModel>().AddAttribute("Azimuth",
			"The Left and right rotation of the device", DoubleValue(1.0),
			MakeDoubleAccessor(&LiFiConstantVelocityMobilityModel::m_azimuth),
			MakeDoubleChecker<double>()).AddAttribute("Elevation",
			"Up and Down rotation of the device", DoubleValue(1.0),
			MakeDoubleAccessor(&LiFiConstantVelocityMobilityModel::m_elevation),
			MakeDoubleChecker<double>());
	return tid;
}

LiFiConstantVelocityMobilityModel::LiFiConstantVelocityMobilityModel ()
{
        this->m_azimuth = 0;
	this->m_elevation = 0;
}

LiFiConstantVelocityMobilityModel::~LiFiConstantVelocityMobilityModel ()
{
}

void
LiFiConstantVelocityMobilityModel::SetVelocity (const Vector &speed)
{
	//std::cout<<"LiFiConstantVelocityMobilityModel::SetVelocity (const Vector &speed)"<<speed.x<<" "<<speed.y<<std::endl;
  m_helper.Update ();
  m_helper.SetVelocity (speed);
  m_helper.Unpause ();
  NotifyCourseChange ();
}

Vector
LiFiConstantVelocityMobilityModel::DoGetPosition (void) const
{
  Vector vec = m_helper.GetCurrentPosition ();
  //std::cout<<"ConstantVelocityMobilityModel::DoGetPosition (void) const x "<<vec.x<<" y "<<vec.y<<" z "<<vec.z<<std::endl;
  m_helper.Update ();
  NotifyCourseChange ();
  static bool firstbool =false;
  std::stringstream m_CSVlififileNamestream;
  m_CSVlififileNamestream<<"LiFi-Network/"<<"CVMobility";
  std::string m_CSVlififileName = m_CSVlififileNamestream.str();
  std::ofstream dat_lifi_file_out (m_CSVlififileName.c_str (), std::ios::app);

  if (firstbool == false)
  {
      dat_lifi_file_out<<"Time\t "<<" \t Curr_P.x "<<" \t Curr_P.y "<< std::endl;
      firstbool = true;
  }
  
  dat_lifi_file_out<<Simulator::Now().GetSeconds()<<" \t "<<vec.x<<" \t "<<vec.y<< std::endl;

  return vec;
}
void 
LiFiConstantVelocityMobilityModel::DoSetPosition (const Vector &position)
{
 //std::cout<<"ConstantVelocityMobilityModel::DoSetPosition (void) const x "<<position.x<<" y "<<position.y<<" z "<<position.z<<std::endl;
  m_helper.SetPosition (position);
  NotifyCourseChange();
}
Vector
LiFiConstantVelocityMobilityModel::DoGetVelocity (void) const
{
  return m_helper.GetVelocity ();
}

void LiFiConstantVelocityMobilityModel::SetAzimuth(double angle) 
{
	m_azimuth = angle * (M_PI / 180);
}

void LiFiConstantVelocityMobilityModel::SetElevation(double angle) 
{
	m_elevation = angle * (M_PI / 180);
}

double LiFiConstantVelocityMobilityModel::GetAzimuth(void) 
{
	return m_azimuth;
}
double LiFiConstantVelocityMobilityModel::GetElevation(void) 
{
	return m_elevation;
}

} // namespace ns3
