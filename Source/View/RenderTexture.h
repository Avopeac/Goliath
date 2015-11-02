#pragma once
#include <iostream>
#include <GL\glew.h>
#include <GL\GL.h>
#include <GLFW\glfw3.h>
struct RenderTexture {
	RenderTexture(unsigned int width = 1024, unsigned int height = 768) : width(width), height(height) {}

	void initialize() {
		glGenFramebuffers(1, &framebuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		//Create color texture
		glGenTextures(1, &color);
		glBindTexture(GL_TEXTURE_2D, color);
		std::cout << " Created color texture with id " << color << std::endl;
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color, 0);
		//Create depth texture
		glGenTextures(1, &depth);
		glBindTexture(GL_TEXTURE_2D, depth);
		std::cout << " Created depth texture with id " << depth << std::endl;
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
		//Attach textures to fbo
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth, 0);
		//Render depth
		glGenRenderbuffers(1, &renderbuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, renderbuffer);
		//Print status
		GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (status == GL_FRAMEBUFFER_COMPLETE) {
			std::cout << "RenderTexture creation is completed." << std::endl;
		}
		else {
			std::cout << "RenderTexture creation failed." << std::endl;
		}
		//Switch back to screen
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	static void use(RenderTexture *out, RenderTexture *in1, RenderTexture *in2) {

		GLint current;
		glGetIntegerv(GL_FRAMEBUFFER_BINDING, &current);
		if (current == 0) {
			GLint viewport[4] = { 0,0,0,0 };
			GLint w, h;
			glGetIntegerv(GL_VIEWPORT, viewport);
			w = viewport[2] - viewport[0];
			h = viewport[3] - viewport[1];
			if ((w > 0) && (h > 0) && (w < 65536) && (h < 65536)) // I don't believe in 64k pixel wide frame buffers for quite some time
			{
				last_width = viewport[2] - viewport[0];
				last_height = viewport[3] - viewport[1];
			}
		}

		out != nullptr ? glViewport(0, 0, out->width, out->height) : glViewport(0, 0, last_width, last_height);

		if (out != 0L) {
			glBindFramebuffer(GL_FRAMEBUFFER, out->framebuffer);
			glViewport(0, 0, out->width, out->height);
		}
		else {
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}

		glActiveTexture(GL_TEXTURE1);
		in2 != nullptr ? glBindTexture(GL_TEXTURE_2D, in2->color) :	glBindTexture(GL_TEXTURE_2D, 0);

		glActiveTexture(GL_TEXTURE0);
		in1 != nullptr ? glBindTexture(GL_TEXTURE_2D, in1->color) : glBindTexture(GL_TEXTURE_2D, 0);
	}

	void release() {
		glDeleteTextures(1, &color);
		glDeleteTextures(1, &depth);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		glDeleteFramebuffers(1, &framebuffer);
		glDeleteRenderbuffers(1, &renderbuffer);
	}

	GLuint renderbuffer, framebuffer, color, depth;
	unsigned int width, height;

private:
	static GLint last_width, last_height;
};