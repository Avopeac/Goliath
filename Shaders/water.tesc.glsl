#version 430

layout(vertices = 3) out;
in vec3 vPosition[];
in vec3 vNormal[];
out vec3 tcNormal[];
out vec3 tcPosition[];
out float tcCameraDist[];

uniform mat4 mvp;
uniform vec3 wCameraPos;
uniform float quadtree_level;
uniform float baseTessellationLevel;
uniform float maxTessLevel;

uniform float far;
uniform float near;

#define ID gl_InvocationID

void main()
{
    tcPosition[ID] = vPosition[ID];
    tcNormal[ID] = vNormal[ID];

    if (ID == 0) {
		/**
		* The important part here is the outer tessellation level which has to
		* be the same for adjacent edges or else we'll get gaps. Inner level
		* is used inside the triangle and then the GPU guarantees no gaps.
		* 
		* Using barycentric coordinates, gl_TessLevelOuter[0] is edge between
		* (0,1,0) and (0,0,1), gl_TessLevelOuter[1] is between (0,0,1) and 
		* (1,0,0) and gl_TessLevelOuter[2] is between (1,0,0) and (0,1,0).
		*/

		vec4 clipPos[3];
		clipPos[0] = mvp * vec4(vPosition[0], 1.0);
		clipPos[1] = mvp * vec4(vPosition[1], 1.0);
		clipPos[2] = mvp * vec4(vPosition[2], 1.0);

		// Calculates a value [0,1] to indicate size on screen (1 is entire screen)
		float edgeScreenLength[3];
		edgeScreenLength[0] = distance(clipPos[1].xy / clipPos[1].w, clipPos[2].xy / clipPos[2].w) / 2.0;
		edgeScreenLength[1] = distance(clipPos[2].xy / clipPos[2].w, clipPos[0].xy / clipPos[0].w) / 2.0;
		edgeScreenLength[2] = distance(clipPos[0].xy / clipPos[0].w, clipPos[1].xy / clipPos[1].w) / 2.0;

		// Calculate tessellation level based on camera distance.
        float tessLevelOuter[3];
        tessLevelOuter[0] = min(maxTessLevel, baseTessellationLevel * edgeScreenLength[0]);
        tessLevelOuter[1] = min(maxTessLevel, baseTessellationLevel * edgeScreenLength[1]);
        tessLevelOuter[2] = min(maxTessLevel, baseTessellationLevel * edgeScreenLength[2]);
		// Cull away too large values (somewhere outside screen or just way too large anyway)
		tessLevelOuter[0] *= sign(1.0 - edgeScreenLength[0]);
		tessLevelOuter[1] *= sign(1.0 - edgeScreenLength[1]);
		tessLevelOuter[2] *= sign(1.0 - edgeScreenLength[2]);

        gl_TessLevelOuter[0] = tessLevelOuter[0];
        gl_TessLevelOuter[1] = tessLevelOuter[1];
        gl_TessLevelOuter[2] = tessLevelOuter[2];

		// Let inner tesselation be mean of outer tessellation levels
        gl_TessLevelInner[0] = (tessLevelOuter[0] + tessLevelOuter[1] + tessLevelOuter[2]) / 3.0;
    }
}
