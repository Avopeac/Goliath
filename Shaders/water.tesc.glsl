#version 430

layout(vertices = 3) out;
in vec3 vPosition[];
in vec3 vNormal[];
out vec3 tcNormal[];
out vec3 tcPosition[];
uniform vec3 wCameraPos;

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
        float cameraDist[3];
        cameraDist[0] = length(vPosition[0] - wCameraPos);
        cameraDist[1] = length(vPosition[1] - wCameraPos);
        cameraDist[2] = length(vPosition[2] - wCameraPos);

		// Calculate tessellation level based on camera distance.
        float tessLevelOuter[3];
        tessLevelOuter[0] = 1000.0 / (cameraDist[1] + cameraDist[2]);
        tessLevelOuter[1] = 1000.0 / (cameraDist[0] + cameraDist[2]);
        tessLevelOuter[2] = 1000.0 / (cameraDist[0] + cameraDist[1]);

		if (false && tessLevelOuter[0] > 10.0) {
			// When closer, use height over ground to get infinizoom
            vec3 toCamera[3];
            toCamera[0] = wCameraPos - vPosition[0];
            toCamera[1] = wCameraPos - vPosition[1];
            toCamera[2] = wCameraPos - vPosition[2];
            float cameraHeight[3];
            cameraHeight[0] = abs(dot(toCamera[0], vNormal[0]));
            cameraHeight[1] = abs(dot(toCamera[1], vNormal[1]));
            cameraHeight[2] = abs(dot(toCamera[2], vNormal[2]));

            tessLevelOuter[0] += 1.0 / (cameraHeight[1] + cameraHeight[2]);
            tessLevelOuter[1] += 1.0 / (cameraHeight[0] + cameraHeight[2]);
            tessLevelOuter[2] += 1.0 / (cameraHeight[0] + cameraHeight[1]);
        }

        gl_TessLevelOuter[0] = tessLevelOuter[0];
        gl_TessLevelOuter[1] = tessLevelOuter[1];
        gl_TessLevelOuter[2] = tessLevelOuter[2];

		// Let inner tesselation be mean of outer tessellation levels
        gl_TessLevelInner[0] = (tessLevelOuter[0] + tessLevelOuter[1] + tessLevelOuter[2]) / 3.0;
    }
}
