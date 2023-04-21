/*
* Copyright (c) 2016-2019 Irlan Robson https://irlanrobson.github.io
*
* This software is provided 'as-is', without any express or implied
* warranty.  In no event will the authors be held liable for any damages
* arising from the use of this software.
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely, subject to the following restrictions:
* 1. The origin of this software must not be misrepresented; you must not
* claim that you wrote the original software. If you use this software
* in a product, an acknowledgment in the product documentation would be
* appreciated but is not required.
* 2. Altered source versions must be plainly marked as such, and must not be
* misrepresented as being the original software.
* 3. This notice may not be removed or altered from any source distribution.
*/

#ifndef BOUNCE_H
#define BOUNCE_H

// Include this file header in your project to directly access Bounce objects.

#include <common/settings.h>
#include <common/time.h>
#include <common/draw.h>

#include <common/math/math.h>

#include <collision/gjk/gjk.h>
#include <collision/sat/sat.h>
#include <collision/collision.h>
#include <collision/broad_phase.h>

#include <collision/shapes/sphere.h>
#include <collision/shapes/capsule.h>
#include <collision/shapes/hull.h>
#include <collision/shapes/box_hull.h>
#include <collision/shapes/qhull.h>
#include <collision/shapes/mesh.h>
#include <collision/shapes/grid_mesh.h>

#include <dynamics/joints/mouse_joint.h>
#include <dynamics/joints/spring_joint.h>
#include <dynamics/joints/weld_joint.h>
#include <dynamics/joints/sphere_joint.h>
#include <dynamics/joints/revolute_joint.h>
#include <dynamics/joints/cone_joint.h>

#include <dynamics/shapes/sphere_shape.h>
#include <dynamics/shapes/capsule_shape.h>
#include <dynamics/shapes/hull_shape.h>
#include <dynamics/shapes/mesh_shape.h>

#include <dynamics/contacts/contact.h>
#include <dynamics/contacts/convex_contact.h>
#include <dynamics/contacts/mesh_contact.h>

#include <dynamics/body.h>

#include <dynamics/world.h>
#include <dynamics/world_listeners.h>

#include <rope/rope.h>

#include <cloth/cloth_mesh.h>
#include <cloth/grid_cloth_mesh.h>
#include <cloth/garment_cloth_mesh.h>
#include <cloth/cloth.h>
#include <cloth/particle.h>
#include <cloth/cloth_triangle.h>

#include <cloth/forces/strech_force.h>
#include <cloth/forces/shear_force.h>
#include <cloth/forces/spring_force.h>
#include <cloth/forces/mouse_force.h>

#include <cloth/garment/sewing_pattern.h>
#include <cloth/garment/garment.h>
#include <cloth/garment/garment_mesh.h>

#include <softbody/softbody_mesh.h>
#include <softbody/block_softbody_mesh.h>
#include <softbody/softbody.h>
#include <softbody/softbody_node.h>

#endif