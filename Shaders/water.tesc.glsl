#version 430

layout(vertices = 3) out;
in vec3 vPosition[];
in vec3 vNormal[];
out vec3 tcNormal[];
out vec3 tcPosition[];

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
uniform float tessellationFactor;

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
		vec3 clipPos[3];
		clipPos[0] = vec3(proj * view * model * vec4(vPosition[0], 1.0));
		clipPos[1] = vec3(proj * view * model * vec4(vPosition[1], 1.0));
		clipPos[2] = vec3(proj * view * model * vec4(vPosition[2], 1.0));

        float cameraDist[3];
        cameraDist[0] = clipPos[0].z;
        cameraDist[1] = clipPos[1].z;
        cameraDist[2] = clipPos[2].z;

		// Calculate tessellation level based on camera distance.
        float tessLevelOuter[3];
        tessLevelOuter[0] = tessellationFactor * 100.0 / (cameraDist[1] + cameraDist[2]);
        tessLevelOuter[1] = tessellationFactor * 100.0 / (cameraDist[0] + cameraDist[2]);
        tessLevelOuter[2] = tessellationFactor * 100.0 / (cameraDist[0] + cameraDist[1]);

        gl_TessLevelOuter[0] = tessLevelOuter[0];
        gl_TessLevelOuter[1] = tessLevelOuter[1];
        gl_TessLevelOuter[2] = tessLevelOuter[2];

		// Let inner tesselation be mean of outer tessellation levels
        gl_TessLevelInner[0] = (tessLevelOuter[0] + tessLevelOuter[1] + tessLevelOuter[2]) / 3.0;
    }
}
