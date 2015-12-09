#version 430

layout(vertices = 3) out;
in vec3 vPosition[];
in vec3 vNormal[];
out vec3 tcNormal[];
out vec3 tcPosition[];

uniform mat4 mvp;
uniform float quadtree_level;
uniform float baseTessellationLevel;
uniform float tessellationCutoffLevel;

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

		// Get world distance to camera from each vertex.
		vec4 clipPos[3];
		clipPos[0] = mvp * vec4(vPosition[0], 1.0);
		clipPos[1] = mvp * vec4(vPosition[1], 1.0);
		clipPos[2] = mvp * vec4(vPosition[2], 1.0);

		// Using the Z buffer hack here as well to avoid float errors and
		// is also probably a nicer value for tessellation
        clipPos[0].z = (2.0 * log(near * clipPos[0].w + 1.0) / log(near * far +  1) - 1) * clipPos[0].w;
        clipPos[1].z = (2.0 * log(near * clipPos[1].w + 1.0) / log(near * far +  1) - 1) * clipPos[1].w;
        clipPos[2].z = (2.0 * log(near * clipPos[2].w + 1.0) / log(near * far +  1) - 1) * clipPos[2].w;

        // We'll get a value [0,1] when within frustum
        float cameraDist[3];
        cameraDist[0] = pow((clipPos[0].z / clipPos[0].w + 1.0) / 2.0, 1.0 / tessellationCutoffLevel);
        cameraDist[1] = pow((clipPos[1].z / clipPos[1].w + 1.0) / 2.0, 1.0 / tessellationCutoffLevel);
        cameraDist[2] = pow((clipPos[2].z / clipPos[2].w + 1.0) / 2.0, 1.0 / tessellationCutoffLevel);

		// Calculate tessellation level based on camera distance.
        float tessLevelOuter[3];
        tessLevelOuter[0] = clamp(baseTessellationLevel * (1 - (cameraDist[1] + cameraDist[2]) / 2.0) - 2 * quadtree_level, 1.0, baseTessellationLevel);
        tessLevelOuter[1] = clamp(baseTessellationLevel * (1 - (cameraDist[2] + cameraDist[0]) / 2.0) - 2 * quadtree_level, 1.0, baseTessellationLevel);
        tessLevelOuter[2] = clamp(baseTessellationLevel * (1 - (cameraDist[0] + cameraDist[1]) / 2.0) - 2 * quadtree_level, 1.0, baseTessellationLevel);

        gl_TessLevelOuter[0] = tessLevelOuter[0];
        gl_TessLevelOuter[1] = tessLevelOuter[1];
        gl_TessLevelOuter[2] = tessLevelOuter[2];

		// Let inner tesselation be mean of outer tessellation levels
        gl_TessLevelInner[0] = (tessLevelOuter[0] + tessLevelOuter[1] + tessLevelOuter[2]) / 3.0;
    }
}
