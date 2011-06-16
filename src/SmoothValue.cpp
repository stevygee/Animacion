//----------------------------------------------------------------------------//
// SmoothValue.cpp                                                               //
// Copyright (C) 2010 Lukas Meindl						                      //
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
// Includes                                                                   //
//----------------------------------------------------------------------------//

#include "SmoothValue.h"

//----------------------------------------------------------------------------//
// Functions                                                                  //
//----------------------------------------------------------------------------//

namespace Mathematics
{

	float SmoothValueFloat(float sourceValue,
		float targetValue,
		float &velocity,
		unsigned long timeSinceLastFrame,
		float smoothTime,
		float maxSpeed)
	{
		float omega = 2.f/smoothTime;
		float x = omega * timeSinceLastFrame;
		float exp = 1.f / ( 1.f + x + 0.48f *x*x + 0.235f *x*x*x);

		float change = sourceValue - targetValue;

		if(maxSpeed > 1.f)
		{
			float maxChange = maxSpeed * smoothTime;
			float change = min(max( -maxChange, change), maxChange);
		}

		float temp = (velocity + omega*change) * timeSinceLastFrame;
		velocity = (velocity - omega*temp) * exp;
		return targetValue + (change + temp) * exp;
	}



	Radian SmoothValueRadian(Radian sourceValue,
		Radian targetValue,
		Radian &velocity,
		unsigned long timeSinceLastFrame,
		Real smoothTime,
		Radian maxSpeed)
	{
		Real omega = 2.f / smoothTime;
		Real x = omega * timeSinceLastFrame;
		Real exp = 1.f / ( 1.f + x + 0.48f *x*x + 0.235f *x*x*x);

		Radian change = sourceValue - targetValue;

		if(maxSpeed > Radian(1))
		{
			Radian maxChange = Radian(maxSpeed * smoothTime);
			Radian change = min(max( -maxChange, change), maxChange);
		}

		Radian temp = (velocity + omega*change) * (Ogre::Real)timeSinceLastFrame;
		velocity = (velocity - omega*temp) * exp;
		return targetValue + (change + temp) * exp;
	}

}