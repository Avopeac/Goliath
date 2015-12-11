#version 430

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in vec3 tePosition[];
in float teDisplacement[];
in vec3 teNormal[];
in vec3 tePatchDistance[];
out vec3 gPosition;
out float gDisplacement;
out vec3 gNormal;
out vec3 gPatchDistance;
out vec3 gTriDistance;

// Atmosphere variables
in vec3 te_c0[];
in vec3 te_c1[];
in float te_t0[];
out vec3 g_c0;
out vec3 g_c1;
out float g_t0;

void main()
{
	g_c0 = te_c0[0];
	g_c1 = te_c1[0];
	g_t0 = te_t0[0];

	gPosition = tePosition[0];
	gDisplacement = teDisplacement[0];
	gNormal = teNormal[0];
    gPatchDistance = tePatchDistance[0];
    gTriDistance = vec3(1, 0, 0);
    gl_Position = gl_in[0].gl_Position; EmitVertex();

	g_c0 = te_c0[1];
	g_c1 = te_c1[1];
	g_t0 = te_t0[1];

	gPosition = tePosition[1];
	gDisplacement = teDisplacement[1];
	gNormal = teNormal[1];
    gPatchDistance = tePatchDistance[1];
    gTriDistance = vec3(0, 1, 0);
    gl_Position = gl_in[1].gl_Position; EmitVertex();

	g_c0 = te_c0[2];
	g_c1 = te_c1[2];
	g_t0 = te_t0[2];

	gPosition = tePosition[2];
	gDisplacement = teDisplacement[2];
	gNormal = teNormal[2];
    gPatchDistance = tePatchDistance[2];
    gTriDistance = vec3(0, 0, 1);
    gl_Position = gl_in[2].gl_Position; EmitVertex();

    EndPrimitive();
}