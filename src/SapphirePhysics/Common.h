#pragma once

#define	SE_MAX_U8 (0xFF)
#define	SE_MAX_U32 (0xFFFFFFFF)

// Pi is computed using double precision by default.
#define SE_PI scalar(3.14159265358979323846)

// Collision

// How much an AABB in the broad-phase should be extended by 
// to disallow unecessary proxy updates.
// A larger value increases performance when there are 
// no objects closer to the AABB because no contacts are 
// even created.
#define SE_AABB_EXTENSION scalar(0.2)

// This is used to extend AABBs in the broad-phase. 
// Is used to predict the future position based on the current displacement.
// This is a dimensionless multiplier.
#define SE_AABB_MULTIPLIER scalar(2)

// Collision and constraint tolerance.
#define SE_LINEAR_SLOP scalar(0.005)
#define SE_LINEAR_SLOP_SQ scalar(0.005 * 0.005)
#define SE_ANGULAR_SLOP (scalar(2.0) / scalar(180) * SE_PI)

// The radius of the hull shape skin.
#define SE_HULL_RADIUS (scalar(0.0) * SE_LINEAR_SLOP)

// Number of contact points per manifold. 
// Don't change this value unless you know what you're doing.
#define SE_MAX_MANIFOLD_POINTS (4)

// Dynamics

// The maximum number of manifolds that can be build for all contacts. 
// Don't change this value unless you know what you're doing.
#define SE_MAX_MANIFOLDS (3)

// Maximum translation per step to prevent numerical instability due to large linear velocity.
#define SE_MAX_TRANSLATION scalar(2.0)
#define SE_MAX_TRANSLATION_SQUARED (SE_MAX_TRANSLATION * SE_MAX_TRANSLATION)

// Maximum rotation per step to prevent numerical instability due to large angular velocity.
#define SE_MAX_ROTATION (scalar(0.5) * B3_PI)
#define SE_MAX_ROTATION_SQUARED (SE_MAX_ROTATION * SE_MAX_ROTATION)

// The maximum position correction used when solving constraints. This helps to prevent overshoot.
#define SE_MAX_LINEAR_CORRECTION scalar(0.2)
#define SE_MAX_ANGULAR_CORRECTION (scalar(8.0) / scalar(180) * B3_PI)

// This controls how faster overlaps should be resolved per step.
// This is less than and would be close to 1, so that the all overlap is resolved per step.
// However values very close to 1 may lead to overshoot.
#define SE_BAUMGARTE scalar(0.1)

// If the relative velocity of a contact point is below 
// the threshold then restitution is not applied.
#define SE_VELOCITY_THRESHOLD scalar(1.0)

// Time to sleep in seconds
#define SE_TIME_TO_SLEEP scalar(0.2)