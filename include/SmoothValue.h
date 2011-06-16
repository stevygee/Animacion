//----------------------------------------------------------------------------//
// SmoothValue.h                                                              //
// Copyright (C) 2010 Lukas Meindl				    			              //
//----------------------------------------------------------------------------//

#ifndef SMOOTHVALUE_H
#define SMOOTHVALUE_H

//----------------------------------------------------------------------------//
// Includes                                                                   //
//----------------------------------------------------------------------------//

#include <algorithm>
#include "OgreMath.h"

using namespace std;
using namespace Ogre;

//----------------------------------------------------------------------------//
// Functions                                                                  //
//----------------------------------------------------------------------------//


namespace Mathematics
{

	float SmoothValueFloat(float sourceValue, float targetValue,
		float &velocity, unsigned long timeSinceLastFrame, float smoothTime,
		float maxSpeed = 0.f);

	Radian SmoothValueRadian(Radian sourceValue, Radian targetValue,
		Radian &velocity, unsigned long timeSinceLastFrame, Real smoothTime,
		Radian maxSpeed = Ogre::Radian(0));
}

#endif