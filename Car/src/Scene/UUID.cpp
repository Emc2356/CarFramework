#include "Car/Scene/UUID.hpp"


namespace Car {
	static std::random_device sRandomDevice;
	static std::mt19937_64 sEngine(sRandomDevice());
	static std::uniform_int_distribution<uint64_t> sUniformDistribution;

	UUID::UUID() : mUUID(sUniformDistribution(sEngine)) {}
}
