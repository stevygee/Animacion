//----------------------------------------------------------------------------//
// SmoothableRadian.cpp                                                       //
// Copyright (C) 2010 Lukas Meindl											  //
//----------------------------------------------------------------------------//


//----------------------------------------------------------------------------//
// Includes                                                                   //
//----------------------------------------------------------------------------//

#include "SmoothableRadian.h"

//----------------------------------------------------------------------------//
// Classes																	  //
//----------------------------------------------------------------------------//

namespace Mathematics
{

	SmoothableRadian::SmoothableRadian(Real smoothTime, Radian currentValue,
		Radian targetValue, Radian velocity, Radian maxSpeed)
	{
		this->currentValue = currentValue;
		this->targetValue = targetValue;
		this->velocity = velocity;
		this->smoothTime = smoothTime;
		this->originalSmoothTime = smoothTime;
		this->maxSpeed = maxSpeed;
	}

	void SmoothableRadian::smoothValue(unsigned long timeSinceLastFrame)
	{
		currentValue = SmoothValueRadian(currentValue, targetValue,
			velocity, timeSinceLastFrame, smoothTime, maxSpeed);
	}

	void SmoothableRadian::setTargetValueWithAdjustedSmoothtime(Radian newTargetValue, bool considerPeriods)
	{
		setTargetValue(newTargetValue, considerPeriods);
		
		//Low difference means shorter smoothTime
		Radian difference = Ogre::Math::Abs(currentValue - targetValue);
		//PI = Maximum difference
		Real ratio = difference.valueRadians() / Math::PI;
		if(ratio > 0.0001f)
			smoothTime = originalSmoothTime * ratio;

		if (smoothTime < 120.f)
			smoothTime = 120.f;
		
	}

	void SmoothableRadian::setTargetValue(Radian newTargetValue, bool considerPeriods)
	{
		if(considerPeriods)
		{

			if(currentValue <= Radian(0))
				currentValue += Radian(Math::TWO_PI);
			else if (currentValue > Radian(Math::TWO_PI))
				currentValue -=  Radian(Math::TWO_PI);

			if(newTargetValue <= Radian(0))
				newTargetValue += Radian(Math::TWO_PI);
			else if (currentValue > Radian(Math::TWO_PI))
				newTargetValue -=  Radian(Math::TWO_PI);


			Radian alternativeNewTargetValue;
			Radian sign;
			if(newTargetValue > Radian(0.))
				sign = Radian(1);
			else
				sign = Radian(-1);

			if(Math::Abs(newTargetValue) > Radian(Ogre::Math::PI))
				alternativeNewTargetValue = newTargetValue - sign * Radian(Ogre::Math::TWO_PI);
			else
				alternativeNewTargetValue = newTargetValue + sign * Radian(Ogre::Math::TWO_PI);
			 
			if(Ogre::Math::Abs(currentValue - newTargetValue) > Ogre::Math::Abs(currentValue - alternativeNewTargetValue))
				newTargetValue = alternativeNewTargetValue;
		}
		targetValue = newTargetValue;
	}

	void SmoothableRadian::setSmoothTime(Real newSmoothTime)
	{
		smoothTime = newSmoothTime;
		originalSmoothTime = newSmoothTime;
	}

	void SmoothableRadian::increaseTargetValue(Radian modifier)
	{
		targetValue += modifier;
	}


	Radian SmoothableRadian::getTargetValue()
	{
		return targetValue;
	}


	Radian SmoothableRadian::getCurrentValue()
	{
		return currentValue;
	}

}



