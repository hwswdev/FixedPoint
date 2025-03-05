/*
 *  Created on: Mar 5, 2025
 *      Author: Evgeny Sobolev
 */


#pragma once

enum class Direction : bool {
	Negative,
	Positive
};



template< typename DistanceType = int32_t, typename StepType = int16_t >
struct Axis {

	Axis() : _stepSize(0), _stepHalfSize(0), _stepPosition(0) {};

	inline StepType distanceSteps( const DistanceType distance ) {
		const StepType stepCount = ( distance + _stepHalfSize ) / _stepSize;
		return stepCount;
	}

	inline void setStepSize( DistanceType stepSize ) {
		_stepSize = stepSize;
		_stepHalfSize = stepSize / 2;
	}

	inline void setPosition( StepType positionSteps ) {
		_stepPosition = stepPosition;
	}

	inline StepType stepPosition() {
		return _stepPosition;
	}

	inline DistanceType getStepSize() {
		return _stepSize;
	}

	inline void step( const Direction dir ) {
		if ( Direction::Positive == dir ) {
			_stepPosition++;
		} else {
			_stepPosition--;
		}
	}

private:
	DistanceType 	_stepSize;
	DistanceType	_stepHalfSize;
	StepType		_stepPosition;

};


template< typename DistanceType, typename StepType, size_t AxisCount >
struct AxisControl {

	struct MotionParams {
		StepType	addValue;
		StepType	curValue;
		Direction	dir;
	};


	AxisControl(){
		for( size_t axisIndex = 0; axisIndex < AxisCount; axisIndex++ ) {
			Axis<DistanceType, StepType>& axis = _axis[axisIndex];
			axis.setStepSize( 0x08 );
		}
	}

	void moveTo( DistanceType moveToPos[AxisCount] ) {

		MotionParams motionOnAxis[AxisCount];

		StepType maxMoveValue = 0;
		StepType maxMoveAxisIndex = 0;

		// Calculate step count to move
		for ( size_t axisIndex = 0; axisIndex < AxisCount; axisIndex++ ) {
			// Get axis
			Axis<DistanceType, StepType>& axis = _axis[axisIndex];
			// Get new position in steps
			const StepType newPosSteps = axis.distanceSteps( moveToPos[axisIndex] );
			const StepType curPosSteps = axis.stepPosition();
			const StepType moveStepCount = newPosSteps - curPosSteps;
			const Direction dir = (moveStepCount < 0) ? Direction::Negative : Direction::Positive;
			const StepType absMoveStepCount = ( Direction::Positive == dir ) ? moveStepCount : -moveStepCount;

			// Get axis index that will be used as reference
			if ( absMoveStepCount > maxMoveValue ) {
				maxMoveAxisIndex = axisIndex;
				maxMoveValue = absMoveStepCount;
			}

			// Direction
			motionOnAxis[axisIndex].dir = dir;
			// Total steps count on axis
			motionOnAxis[axisIndex].addValue = absMoveStepCount;
			motionOnAxis[axisIndex].curValue = absMoveStepCount / 2;
		}


		// When I have to move I will do this
		const StepType maxValue = motionOnAxis[maxMoveAxisIndex].addValue;
		const size_t stepCount = maxValue;

		if ( 0 == motionOnAxis[maxMoveAxisIndex].addValue ) return;


		for( size_t stepIndex = 0; stepIndex < stepCount; stepIndex++ ) {

			// That is step generation
			for ( size_t axisIndex = 0; axisIndex < AxisCount; axisIndex++ ) {
				Axis<DistanceType, StepType>& axis = _axis[axisIndex];
				MotionParams& motion = motionOnAxis[axisIndex];

				motion.curValue+= motion.addValue;
				if ( motion.curValue >= maxValue ) {
					// Update value
					motion.curValue -= maxValue;

					// Generate step pulse
					axis.step( motion.dir );

				}
			}

		}

	}
//private:
	Axis<DistanceType, StepType>	_axis[AxisCount];
};


