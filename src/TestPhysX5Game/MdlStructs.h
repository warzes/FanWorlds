#pragma once

// mdl file format references:
// https://github.com/id-Software/Quake/blob/master/WinQuake/modelgen.h
// https://www.gamers.org/dEngine/quake/spec/quake-spec34/qkspec_5.htm
// https://formats.kaitai.io/quake_mdl/index.html

using vec3_t = glm::vec3;

typedef struct
{
	int32_t ident;                  // 0x4F504449 = "IDPO" for IDPOLYGON
	int32_t version;                // Version = 6
	vec3_t scale;                   // Model scale factors.
	vec3_t scale_origin;            // Model origin.
	float boundingradius;           // Model bounding radius.
	vec3_t eyeposition;             // Eye position (useless?)
	int32_t numskins;               // the number of skin textures
	int32_t skinwidth;              // Width of skin texture must be multiple of 8
	int32_t skinheight;             // Height of skin texture must be multiple of 8
	int32_t numverts;               // Number of vertices
	int32_t numtris;                // Number of triangles surfaces
	int32_t numframes;              // Number of frames
	int32_t synctype;               // 0 = synchron, 1 = random
	int32_t flags;                  // 0 (see Alias models)
	float size;                     // average size of triangles
} mdl_t;

static_assert(sizeof(mdl_t) == 84);

typedef struct
{
	int32_t onseam;                 // 0 or 0x20
	int32_t s;                      // position, horizontally in range [0,skinwidth)
	int32_t t;                      // position, vertically in range [0,skinheight)
} stvert_t;

static_assert(sizeof(stvert_t) == 12);

typedef struct
{
	int32_t facesfront;             // 0 or 0x10
	int32_t vertindex[3];           // Index of 3 triangle vertices in range [0,numverts)
} dtriangle_t;

static_assert(sizeof(dtriangle_t) == 16);

typedef struct
{
	uint8_t v[3];                   // X,Y,Z coordinate, packed on 0-255
	uint8_t lightnormalindex;       // index of the vertex normal
} trivertx_t;

static_assert(sizeof(trivertx_t) == 4);

typedef struct
{
	trivertx_t bboxmin;             // minimum values of X,Y,Z
	trivertx_t bboxmax;             // maximum values of X,Y,Z
	char name[16];                  // name of frame
} daliasframe_t;

static_assert(sizeof(daliasframe_t) == 24);