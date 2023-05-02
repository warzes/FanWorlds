#pragma once

#include "sat.h"

struct Sphere;

scalar ProjectVertex(const Sphere* hull, const Plane& plane);

FaceQuery QueryFaceSeparation(const Transform& xf1, const Hull* hull1, const Transform& xf2, const Sphere* hull2);