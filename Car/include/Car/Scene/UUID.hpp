#pragma once

#include "Car/Core/Core.hpp"


namespace Car {
	class UUID {
	public:
		UUID();
		UUID(uint64_t uuid) : mUUID(uuid) {}
		UUID(const UUID&) = default;

		operator uint64_t() const { return mUUID; }
	private:
		uint64_t mUUID;
	};
}

namespace std {
	template <typename T> struct hash;

	template<>
	struct hash<Car::UUID> {
		std::size_t operator()(const Car::UUID& uuid) const {
			return (uint64_t)uuid;
		}
	};

}
