//----------------------------------------------------------------------------//
// SmoothableRadian.h                                                          //
// Copyright (C) 2009 Lukas Meindl											  //
//----------------------------------------------------------------------------//

#ifndef SMOOTHABLERADIAN_H
#define SMOOTHABLERADIAN_H

//----------------------------------------------------------------------------//
// Includes                                                                   //
//----------------------------------------------------------------------------//

#include "SmoothValue.h"
#include "OgreMath.h"

using namespace Ogre;

//----------------------------------------------------------------------------//
// Classes																	  //
//----------------------------------------------------------------------------//

namespace Mathematics
{

	class SmoothableRadian
	{
	public:
		SmoothableRadian(Real smoothTime, Radian currentValue = Radian(0),
			Radian targetValue = Radian(0), Radian velocity = Radian(0), Radian maxSpeed = Radian(0));

		void smoothValue(unsigned long timeSinceLastFrame);

		Radian getTargetValue();
		void setTargetValue(Radian newTargetValue, bool considerPeriods = false);
		void setTargetValueWithAdjustedSmoothtime(Radian newTargetValue, bool considerPeriods = true);
		void increaseTargetValue(Radian modifier);

		void setSmoothTime(Real newSmoothTime);

		Radian getCurrentValue();

	private:
		Radian currentValue;
		Radian targetValue;
		Radian velocity;

		Real smoothTime;
		Real originalSmoothTime;
		Radian maxSpeed;
	};

}

#endif