#include "CVector2.h"
#ifndef _Collision_H_INCLUDED_
#define _Collision_H_INCLUDED_
class Collision
{
private:

public:
	Collision() {};
	~Collision() {};
	bool PointToPointCollision(float objectOnePoint, float objectTwoPoint, float setDistance)
	{
		float distanceY = objectTwoPoint - objectOnePoint;
		float distance = sqrt((distanceY * distanceY));

		return(distance < setDistance);
	}
};
extern Collision* CollisionDetector;
#endif //_Collision_H_INCLUDED_
