#include "ns3/lifi-mobility-model.h"

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (LiFiMobilityModel);

ns3::TypeId LiFiMobilityModel::GetTypeId(void) {
	static ns3::TypeId tid = ns3::TypeId("ns3::LiFiMobilityModel").SetParent<
			MobilityModel>().SetGroupName("Mobility").AddConstructor<
			LiFiMobilityModel>().AddAttribute("Azimuth",
			"The Left and right rotation of the device", DoubleValue(1.0),
			MakeDoubleAccessor(&LiFiMobilityModel::m_azimuth),
			MakeDoubleChecker<double>()).AddAttribute("Elevation",
			"Up and Down rotation of the device", DoubleValue(1.0),
			MakeDoubleAccessor(&LiFiMobilityModel::m_elevation),
			MakeDoubleChecker<double>()).AddAttribute("Position2",
			"Where the Device is", VectorValue(Vector(0.0, 0.0, 0.0)),
			MakeVectorAccessor(&LiFiMobilityModel::m_basePosition),
			MakeVectorChecker());
	return tid;
}

LiFiMobilityModel::LiFiMobilityModel() {
	this->m_azimuth = 0;
	this->m_elevation = 0;
}

LiFiMobilityModel::~LiFiMobilityModel() {

}

inline Vector LiFiMobilityModel::DoGetVelocity(void) const {
	double t = (Simulator::Now() - m_baseTime).GetSeconds();
	return Vector(m_baseVelocity.x + m_acceleration.x * t,
			m_baseVelocity.y + m_acceleration.y * t,
			m_baseVelocity.z + m_acceleration.z * t); // returns a velocity vector of the node that has this mobility model instlled on it
}

inline Vector LiFiMobilityModel::DoGetPosition(void) const {

	double t = (Simulator::Now() - m_baseTime).GetSeconds();
	double half_t_square = t * t * 0.5;
	Vector vec =  Vector(m_basePosition.x + m_baseVelocity.x * t
					+ m_acceleration.x * half_t_square,
			m_basePosition.y + m_baseVelocity.y * t
					+ m_acceleration.y * half_t_square,
			m_basePosition.z + m_baseVelocity.z * t
					+ m_acceleration.z * half_t_square);
//std::cout<<"LiFiMobilityModel::DoGetPosition(void) const x "<<vec.x<<" y "<<vec.y<<" z "<<vec.z<<std::endl;
	return  vec;// returns a acceleration vector of the node that has this mobility model instlled on it
}

void LiFiMobilityModel::DoSetPosition(const Vector &position) //called to set position via vector parameters
		{
	m_baseVelocity = DoGetVelocity();
	m_baseTime     = Simulator::Now();
	m_basePosition = position;
	NotifyCourseChange();
}

void LiFiMobilityModel::SetVelocityAndAcceleration(const Vector &velocity,
		const Vector &acceleration) // called to set velocity and acceleration via vector parameters
		{

	m_basePosition = DoGetPosition();
	m_baseTime = Simulator::Now();
	m_baseVelocity = velocity;
	m_acceleration = acceleration;
	NotifyCourseChange();
}

void LiFiMobilityModel::SetAzimuth(double angle) {
	m_azimuth = angle * (M_PI / 180);
}

void LiFiMobilityModel::SetElevation(double angle) {
	m_elevation = angle * (M_PI / 180);
}

double LiFiMobilityModel::GetAzimuth(void) {
	return m_azimuth;
}
double LiFiMobilityModel::GetElevation(void) {
	return m_elevation;
}

} /* namespace LiFi */

