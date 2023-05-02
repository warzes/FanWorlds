#pragma once

#include "sat.h"

struct Capsule;

///////////////////////////////////////////////////////////////////////////////////////////////////

scalar ProjectEdge(const Capsule* hull, const Plane& plane);

FaceQuery QueryFaceSeparation(const Transform& xf1, const Hull* hull1, const Transform& xf2, const Capsule* hull2);

///////////////////////////////////////////////////////////////////////////////////////////////////

scalar ProjectEdge(const Vec3& P1, const Vec3& E1, const Vec3& P2, const Vec3& E2, const Vec3& C2);

EdgeQuery QueryEdgeSeparation(const Transform& xf1, const Hull* hull1, const Transform& xf2, const Capsule* hull2);