#pragma once

class BoisMaths {
public:
	// Spherical Linear Interpolation
	// https://stackoverflow.com/questions/60361828/unitys-vector3-slerp-equivalent-in-unreal-engine-4-c
	static FVector Slerp(const FVector& a, const FVector& b, const float t) {
		float omega = FGenericPlatformMath::Acos(FVector::DotProduct(
			a.GetSafeNormal(),
			b.GetSafeNormal()
		));
		float sinOmega = FGenericPlatformMath::Sin(omega);
		FVector termOne = a * (FGenericPlatformMath::Sin(omega * (1.0 - t)) / sinOmega);
		FVector termTwo = b * (FGenericPlatformMath::Sin(omega * (t)) / sinOmega);
		return termOne + termTwo;
	}
};
