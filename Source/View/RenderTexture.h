#pragma once
#include <iostream>
#include <GL/glew.h>
#include <GL/GL.h>
#include <GLFW/glfw3.h>
#include "Application.h"
struct RenderTexture {
	RenderTexture(unsigned int width = Application::width, unsigned int height = Application::height) : width(width), height(height) { initialize(); }

	void initialize() {
		glGenRenderbuffers(1, &renderbuffer);
		glGenFramebuffers(1, &framebuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		//Create color texture
		glActiveTexture(GL_TEXTURE0);
		glGenTextures(1, &color);
		glBindTexture(GL_TEXTURE_2D, color);
		std::cout << " Created color texture with id " << color << std::endl;
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 4.0f);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color, 0);
		//Create depth texture
		glActiveTexture(GL_TEXTURE1);
		glGenTextures(1, &depth);
		glBindTexture(GL_TEXTURE_2D, depth);
		std::cout << " Created depth texture with id " << depth << std::endl;
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
		glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth, 0);
		//Print status
		GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (status == GL_FRAMEBUFFER_COMPLETE) {
			std::cout << "RenderTexture creation is completed." << std::endl;
		}
		else {
			std::cerr << "RenderTexture creation failed." << std::endl;
		}
		//Switch back to screen
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);
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
			if ((w > 0) && (h > 0) && (w < 65536) && (h < 65536)) {
				_last_width = viewport[2] - viewport[0];
				_last_height = viewport[3] - viewport[1];
			}
		}

		out != nullptr ? glViewport(0, 0, out->width, out->height) : glViewport(0, 0, _last_width, _last_height);

		if (out != nullptr) {
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
	static GLint _last_width, _last_height;
};