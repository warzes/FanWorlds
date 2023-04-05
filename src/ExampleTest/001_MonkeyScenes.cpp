#include "stdafx.h"
#include "001_MonkeyScenes.h"
//#define __VSM__
//-----------------------------------------------------------------------------
bool _001MonkeyScenes::Create()
{
	Print("001MonkeyScenes Create");
	auto& renderSystem = GetRenderSystem();
    auto& graphicsSystem = GetGraphicsSystem();

	{
		const char* vertexShaderText = R"(
#version 330 core

layout (location = 0) in vec3 position;

out vec4 _position;

uniform mat4 lightSpaceMatrix;
uniform mat4 model;

void main()
{
    gl_Position = lightSpaceMatrix * model * vec4(position, 1.0);
    _position = gl_Position;
}  
)";

		const char* fragmentShaderText = R"(
#version 330 core

in vec4 _position;

out vec4 color;

void main()
{             
    //gl_FragDepth = gl_FragCoord.z;

    float depth = _position.z / _position.w;
    depth = depth * 0.5 + 0.5;

    float dx = dFdx(depth);
    float dy = dFdy(depth);

    float moment1 = depth;
    float moment2 = depth * depth - 0.25 * (dx * dx + dy * dy);

    color = vec4(moment1, moment2, 0.0, 1.0);
}  
)";
		simpleDepthShader = renderSystem.CreateShaderProgram(vertexShaderText, fragmentShaderText);
	}

	{
		const char* vertexShaderText = R"(
#version 330

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 _uv;

out vec2 uv;

void main()
{
    gl_Position = vec4(position, 1.0);
    uv = _uv;
};
)";

		const char* fragmentShaderText = R"(
#version 330
in vec2 uv;

uniform sampler2D textureSource;

out vec4 color;

uniform bool horizontal;
uniform float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

void main()
{
    vec2 textureOffset = 1.0 / textureSize(textureSource, 0); // gets size of single texel
    vec3 result = texture(textureSource, uv).rgb * weight[0]; // current fragment's contribution
    if(horizontal)
    {
        for(int i = 1; i < 5; ++i)
        {
            result += texture(textureSource, uv + vec2(textureOffset.x * i, 0.0)).rgb * weight[i];
            result += texture(textureSource, uv - vec2(textureOffset.x * i, 0.0)).rgb * weight[i];
        }
    }
    else
    {
        for(int i = 1; i < 5; ++i)
        {
            result += texture(textureSource, uv + vec2(0.0, textureOffset.y * i)).rgb * weight[i];
            result += texture(textureSource, uv - vec2(0.0, textureOffset.y * i)).rgb * weight[i];
        }
    }
    color = vec4(result, 1.0);
}
)";
		blurShader = renderSystem.CreateShaderProgram(vertexShaderText, fragmentShaderText);
	}

	{
		const char* vertexShaderText = R"(
#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uv;
layout (location = 2) in vec3 normal;

out VertexOutput
{
    vec3 position;
    vec3 normal;
    vec2 uv;
    vec4 positionLightSpace;
} vertexOutput;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform mat4 lightSpaceMatrix;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0);
    vertexOutput.position = vec3(model * vec4(position, 1.0));
    vertexOutput.normal = transpose(inverse(mat3(model))) * normal;
    vertexOutput.uv = uv;
    vertexOutput.positionLightSpace = lightSpaceMatrix * vec4(vertexOutput.position, 1.0);
}
)";

		const char* fragmentShaderText = R"(
#version 330 core
out vec4 outputColor;

in VertexOutput
{
    vec3 position;
    vec3 normal;
    vec2 uv;
    vec4 positionLightSpace;
} fragmentInput;

struct Light
{
    vec3 direction;  
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
}; 

uniform Light light;

uniform sampler2D diffuseTexture;
uniform sampler2D shadowMap;

uniform vec3 viewPosition;

/*
float chebyshevUpperBound(vec4 fragPosLightSpace)
{
    float p = 0.0;

    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    // We retrieve the two moments previously stored (depth and depth * depth)
    vec2 moments = texture(shadowMap, projCoords.xy).rg;

    // Surface is fully lit. as the current fragment is before the light occluder
    if (projCoords.z <= moments.x)
        p = 1.0;

    // The fragment is either in shadow or penumbra. We now use chebyshev's upperBound to check
    // How likely this pixel is to be lit (p_max)
    float variance = moments.y - (moments.x * moments.x);
    variance = max(variance, 0.000001);

    float d = projCoords.z - moments.x;
    float pMax = variance / (variance + d * d);

    return max(p, pMax);
}
*/
void main()
{          
    vec3 color = vec3(texture(diffuseTexture, fragmentInput.uv));
    
    // ambient
    vec3 ambient = light.ambient * color;
    
    // diffuse 
    vec3 norm = normalize(fragmentInput.normal);
    vec3 lightDir = normalize(light.direction);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * vec3(texture(diffuseTexture, fragmentInput.uv));
    
    // specular
    vec3 viewDir = normalize(viewPosition - fragmentInput.position);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 16.0);
    vec3 specular = light.specular * spec * vec3(1.0);

    //float visibility = chebyshevUpperBound(fragmentInput.positionLightSpace);
    float visibility = 1.0;
    vec3 lighting = (ambient + visibility * (diffuse + specular)) * color;    
    
    outputColor = vec4(pow(lighting, vec3(1.0 / 2.2)), 1.0);
}
)";
		shader = renderSystem.CreateShaderProgram(vertexShaderText, fragmentShaderText);
	}

    {
        const char* vertexShaderText = R"(
#version 330 core
layout (location = 0) in vec2 position;
layout (location = 1) in vec2 _uv;

out vec2 uv;

void main()
{
    gl_Position = vec4(position.x, position.y, 0.0, 1.0); 
    uv = _uv;
}  
)";
        const char* fragmentShaderText = R"(
#version 330 core
out vec4 color;
in vec2 uv;

uniform sampler2D depthMap;

void main()
{             
    //float depthValue = texture(depthMap, uv).r;
    color = vec4(vec3(texture(depthMap, uv).r), 1.0);
    //color = vec4(1, 0, 1, 1);
}  
)";
        debugQuadShader = renderSystem.CreateShaderProgram(vertexShaderText, fragmentShaderText);
    }


    {
        m = graphicsSystem.CreateModel("../ExampleData/models/scene.obj", "../ExampleData/models/");
        plane = graphicsSystem.CreateModel("../ExampleData/models/plane.obj", "../ExampleData/models/");

        t1 = renderSystem.CreateTexture2D("../ExampleData/models/textureA.png");
        t2 = renderSystem.CreateTexture2D("../ExampleData/models/textureB.png");
    }

#ifdef __VSM__
    shadowMapTexture = std::make_unique<Texture>(GL_RG32F, SHADOWMAP_SIZE, SHADOWMAP_SIZE, GL_RG, GL_FLOAT);
    shadowMapFbo = std::make_unique<Framebuffer>(GL_COLOR_ATTACHMENT0, shadowMapTexture->getId());

    blurTexture = std::make_unique<Texture>(GL_RG32F, SHADOWMAP_SIZE, SHADOWMAP_SIZE, GL_RG, GL_FLOAT);
    blurFbo = std::make_unique<Framebuffer>(GL_COLOR_ATTACHMENT0, blurTexture->getId());
#endif

    {
        struct testVertex
        {
            glm::vec3 pos;
            glm::vec3 color;
            glm::vec2 texCoord;
        };

        constexpr testVertex vert[] =
        {
            {{-1.0f, -1.0f, 4.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},
            {{-1.0f,  1.0f, 4.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
            {{ 1.0f,  1.0f, 4.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},
            {{ 1.0f, -1.0f, 4.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}
        };

        constexpr int indexs[] =
        {
            0, 1, 2,
            2, 3, 0
        };

        constexpr const char* vertexShaderText = R"(
#version 330 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexColor;
layout(location = 2) in vec2 vertexTexCoord;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;

out vec3 fragmentColor;
out vec2 TexCoord;

void main()
{
	gl_Position   = projectionMatrix * viewMatrix * vec4(vertexPosition, 1.0);
	fragmentColor = vertexColor;
	TexCoord      = vertexTexCoord;
}
)";
        constexpr const char* fragmentShaderText = R"(
#version 330 core

in vec3 fragmentColor;
in vec2 TexCoord;

uniform sampler2D Texture;

out vec4 outColor;

void main()
{
	outColor = texture(Texture, TexCoord) * vec4(fragmentColor, 1.0);
}
)";

        m_tempShader = renderSystem.CreateShaderProgram(vertexShaderText, fragmentShaderText);
        m_tempuniformProj = renderSystem.GetUniform(m_tempShader, "projectionMatrix");
        m_tempuniformView = renderSystem.GetUniform(m_tempShader, "viewMatrix");

        constexpr const char* vertexShaderModelText = R"(
#version 330 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec3 vertexColor;
layout(location = 3) in vec2 vertexTexCoord;

uniform mat4 uWorld;
uniform mat4 uView;
uniform mat4 uProjection;

out vec3 fragmentColor;
out vec2 TexCoord;

void main()
{
	gl_Position   = uProjection * uView * uWorld * vec4(vertexPosition, 1.0);
	fragmentColor = vertexColor;
	TexCoord      = vertexTexCoord;
}
)";
        constexpr const char* fragmentShaderModelText = R"(
#version 330 core

in vec3 fragmentColor;
in vec2 TexCoord;

uniform sampler2D Texture;

out vec4 outColor;

void main()
{
	outColor = texture(Texture, TexCoord) * vec4(fragmentColor, 1.0);
}
)";

        shaderModel = renderSystem.CreateShaderProgram(vertexShaderModelText, fragmentShaderModelText);
        uniformModelWorldMatrix = renderSystem.GetUniform(shaderModel, "uWorld");
        uniformModelViewMatrix = renderSystem.GetUniform(shaderModel, "uView");
        uniformModelProjectionMatrix = renderSystem.GetUniform(shaderModel, "uProjection");


        m_geom = renderSystem.CreateGeometryBuffer(ResourceUsage::Static,
            Countof(vert), sizeof(testVertex), vert,
            Countof(indexs), IndexBufferFormat::Uint32, indexs, 
            m_tempShader);

        Texture2DInfo texInfo;
        texInfo.mipmap = false;
        texInfo.minFilter = TextureMinFilter::NearestMipmapNearest;
        texInfo.magFilter = TextureMagFilter::Nearest;

        m_texture = renderSystem.CreateTexture2D("../ExampleData/textures/1mx1m.png", true, texInfo);

        camera.position = { 0.0f, 0.0f, -10.0f };
        camera.target = { 0.0f, 0.0f, 1.0f };
        camera.upVector = { 0.0f, 1.0f, 0.0f };

        m_model = graphicsSystem.CreateModel("../ExampleData/models/rock.obj", "../ExampleData/models/");
    }

    GetInput().SetMouseLock(true);
    
	return true;
}
//-----------------------------------------------------------------------------
void _001MonkeyScenes::Destroy()
{
    GetInput().SetMouseLock(false);
	Print("001MonkeyScenes Destroy");
}
//-----------------------------------------------------------------------------
void _001MonkeyScenes::Render()
{
	if( m_windowWidth != GetWindowWidth() || m_windowHeight != GetWindowHeight() )
	{
		m_windowWidth = GetWindowWidth();
		m_windowHeight = GetWindowHeight();
        if( m_windowHeight == 0 ) m_windowHeight = 1;
	}

	auto& renderSystem = GetRenderSystem();
    auto& graphicsSystem = GetGraphicsSystem();

    const float aspectRatio = (float)m_windowWidth / (float)m_windowHeight;
    glm::mat4 proj = glm::perspective(glm::radians(45.0f), aspectRatio, 0.01f, 1000.f);
    glm::mat4 view = camera.GetViewMatrix();

    renderSystem.Bind(m_tempShader);
    renderSystem.SetUniform(m_tempuniformProj, proj);
    renderSystem.SetUniform(m_tempuniformView, view);

    renderSystem.Bind(m_texture);
    renderSystem.Draw(m_geom->vao);

    renderSystem.Bind(shaderModel);
    renderSystem.SetUniform(uniformModelViewMatrix, view);
    renderSystem.SetUniform(uniformModelProjectionMatrix, proj);
    renderSystem.SetUniform(uniformModelWorldMatrix, glm::mat4(1.0f));
    graphicsSystem.Draw(m_model);
}
//-----------------------------------------------------------------------------
void _001MonkeyScenes::Update(float deltaTime)
{
	if( GetInput().IsKeyDown(Input::KEY_ESCAPE) )
	{
		BaseClass::ExitRequest();
		return;
	}

    const float mouseSensitivity = 10.0f * deltaTime;
    const float moveSpeed = 10.0f * deltaTime;
    const glm::vec3 oldCameraPos = camera.position;

    if( GetInput().IsKeyDown(Input::KEY_W) ) camera.MoveBy(moveSpeed);
    if( GetInput().IsKeyDown(Input::KEY_S) ) camera.MoveBy(-moveSpeed);
    if( GetInput().IsKeyDown(Input::KEY_A) ) camera.StrafeBy(moveSpeed);
    if( GetInput().IsKeyDown(Input::KEY_D) ) camera.StrafeBy(-moveSpeed);

    glm::vec2 delta = GetInput().GetMouseDeltaPosition();
    if( delta.x != 0.0f )  camera.RotateLeftRight(delta.x * mouseSensitivity);
    if( delta.y != 0.0f )  camera.RotateUpDown(-delta.y * mouseSensitivity);
}
//-----------------------------------------------------------------------------