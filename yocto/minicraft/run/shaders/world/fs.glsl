#version 400

//Variables en entree
in vec3 normal;
in vec4 color;
in vec2 uv;
in vec4 wPos;
flat in float type;

#define CUBE_HERBE 0.0
#define CUBE_TERRE 1.0
#define CUBE_EAU 4.0
#define CUBE_AIR 39.0

uniform vec3 sunPos;
uniform vec3 sunDir;

uniform float elapsed;
uniform vec3 camPos;
uniform vec3 sunColor;
uniform sampler2D myTexture;

out vec4 color_out;

const float ambientLevel = 0.4;

float noise(vec4 position)
{
    return 0.5f * sin(position.x / 3 + elapsed) 
    + 0.05f * sin((position.y+position.x) + elapsed) 
    -1;
}

void main()
{
    vec3 toLight = normalize(sunPos);

    vec3 newNorm = normal;

    float specular = 0;

        if(type == CUBE_EAU)
    {
        vec4 A = wPos;
        vec4 B = wPos + vec4(0.2f,0,0,0);
        vec4 C = wPos + vec4(0,0.2f,0,0);

        A.z += noise(A);
        B.z += noise(B);
        C.z += noise(C);

        newNorm = normalize(cross(vec3(B-A),vec3(C-A)));

        //these two lines are fucking up the specular
        vec3 toCam =normalize( camPos - A.xyz);
        vec3 halfVec = normalize(sunDir + toCam);
        //
        float angle = max(dot(halfVec, newNorm), 0.0);
        specular  = pow(angle,300)*50;
        
    }    
    
    //Diffuse
    float diffuse = max(0,dot(toLight,newNorm));

    if(type == CUBE_HERBE)
    {
        color_out = vec4(texture(myTexture, vec2((uv.x + type)/32, uv.y/2)).xyz * ambientLevel * diffuse + sunColor.xyz * specular * 0.97 + 0.03 * vec3(0.8,0.9,1) ,color.a);
    }
    else if(type == CUBE_EAU)
    {
        color_out = vec4(texture(myTexture, vec2((uv.x + 23)/32, uv.y/2)).xyz * ambientLevel * diffuse + sunColor.xyz * specular * 0.97 + 0.03 * vec3(0.8,0.9,1), 0.2);
        color_out += vec4(0.0, 0.1, 1.0, 0.5);
    }
    else //CUBE_TERRE et autre
    {
        color_out = vec4(texture(myTexture, vec2((uv.x + 3)/32, uv.y/2)).xyz * ambientLevel * diffuse + sunColor.xyz * specular * 0.97 + 0.03 * vec3(0.8,0.9,1),color.a);
    }
}