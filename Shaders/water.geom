#version 430

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in vec3 tePosition[3];
in vec3 teNormal[3];
in vec3 tePatchDistance[3];
out vec3 gNormal;
out vec3 gFacetNormal;
out vec3 gPatchDistance;
out vec3 gTriDistance;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main()
{
    gFacetNormal = normalize(cross(tePosition[1] - tePosition[0], tePosition[2] - tePosition[0]));
	gNormal = teNormal[0];
    gPatchDistance = tePatchDistance[0];
    gTriDistance = vec3(1, 0, 0);
    gl_Position = gl_in[0].gl_Position; EmitVertex();

    gFacetNormal = normalize(cross(tePosition[2] - tePosition[1], tePosition[0] - tePosition[1]));
	gNormal = teNormal[1];
    gPatchDistance = tePatchDistance[1];
    gTriDistance = vec3(0, 1, 0);
    gl_Position = gl_in[1].gl_Position; EmitVertex();

    gFacetNormal = normalize(cross(tePosition[0] - tePosition[2], tePosition[1] - tePosition[2]));
	gNormal = teNormal[2];
    gPatchDistance = tePatchDistance[2];
    gTriDistance = vec3(0, 0, 1);
    gl_Position = gl_in[2].gl_Position; EmitVertex();

    EndPrimitive();
}