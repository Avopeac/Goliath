#version 430

layout(triangles, equal_spacing, ccw) in;

in vec3 tcPosition[];
in vec3 tcNormal[];
out vec3 tePosition;
out vec3 teNormal;
out vec3 tePatchDistance;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main()
{
    vec3 p0 = gl_TessCoord.x * tcPosition[0];
    vec3 p1 = gl_TessCoord.y * tcPosition[1];
    vec3 p2 = gl_TessCoord.z * tcPosition[2];
    vec3 n0 = gl_TessCoord.x * tcNormal[0];
    vec3 n1 = gl_TessCoord.y * tcNormal[1];
    vec3 n2 = gl_TessCoord.z * tcNormal[2];
    tePosition = p0 + p1 + p2;
    teNormal = normalize(n0 + n1 + n2);
    tePatchDistance = gl_TessCoord;
    gl_Position = proj * view * model * vec4(tePosition, 1.0);
}
