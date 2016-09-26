/*
 * SampleRender.cpp
 *
 *  Created on: 2016-9-26
 *      Author: xll
 */
#include "engine/SampleRender.h"
#include "utils/texture_utils.h"

bool SampleRender::initDisplay() {
	printGLString("Version", GL_VERSION);
	printGLString("Vendor", GL_VENDOR);
	printGLString("Renderer", GL_RENDERER);
	printGLString("Extensions", GL_EXTENSIONS);

	GLuint vert_shader, frag_shader;
	//	char *vert_shader_pathname, *frag_shader_pathname;
	const char* strVsh = "Shaders/mufeng.vsh";
	const char* strFsh = "Shaders/mufeng.fsh";

	// Create shader program
	gProgram = glCreateProgram();
	LOGI("Created Shader %d", gProgram);

	// Create and compile vertex shader
	if (!ndk_helper::shader::CompileShader(&vert_shader, GL_VERTEX_SHADER,
			strVsh)) {
		LOGI("Failed to compile vertex shader");
		glDeleteProgram(gProgram);
		return false;
	}

	// Create and compile fragment shader
	if (!ndk_helper::shader::CompileShader(&frag_shader, GL_FRAGMENT_SHADER,
			strFsh)) {
		LOGI("Failed to compile fragment shader");
		glDeleteProgram(gProgram);
		return false;
	}

	// Attach vertex shader to program
	glAttachShader(gProgram, vert_shader);

	// Attach fragment shader to program
	glAttachShader(gProgram, frag_shader);

	// Link program
	if (!ndk_helper::shader::LinkProgram(gProgram)) {
		LOGI("Failed to link program: %d", gProgram);

		if (vert_shader) {
			glDeleteShader(vert_shader);
			vert_shader = 0;
		}
		if (frag_shader) {
			glDeleteShader(frag_shader);
			frag_shader = 0;
		}
		if (gProgram) {
			glDeleteProgram(gProgram);
		}
		return false;
	}
	if (!ndk_helper::shader::ValidateProgram(gProgram)) {
		LOGI("Failed to ValidateProgram: %d", gProgram);
		if (gProgram) {
			glDeleteProgram(gProgram);
		}
		return false;
	}
	// Get the attribute locations
	gPositionLoc = glGetAttribLocation(gProgram, "a_position");
	gTexCoordLoc = glGetAttribLocation(gProgram, "a_texCoord");
	// Get the sampler location
	gSamplerLoc = glGetUniformLocation(gProgram, "s_texture");

	// Load the texture
//	gTexture = CreateSimpleTexture2D();
//	gTexture = CreateSimpleTexture2D(app_->activity->assetManager,
//			"texture/brick.png");
	gTexture = ndk_helper::JNIHelper::GetInstance()->LoadTexture(
			"texture/brick.png");

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

//	renderer_.UpdateViewport();
	return true;
}

GLfloat vertices[] = { -0.5f, 0.5f, 0.0f, // Position 0
		0.0f, 0.0f, // TexCoord 0
		-0.5f, -0.5f, 0.0f, // Position 1
		0.0f, 1.0f, // TexCoord 1
		0.5f, -0.5f, 0.0f, // Position 2
		1.0f, 1.0f, // TexCoord 2
		0.5f, 0.5f, 0.0f, // Position 3
		1.0f, 0.0f // TexCoord 3
		};

GLushort indices[] = { 0, 1, 2, 0, 2, 3 };

void SampleRender::drawFrame() {
	//	renderer_.Update(monitor_.GetCurrentTime());

	// Just fill the screen with a color.
	glClearColor(0.5f, 0.5f, 0.5f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//	renderer_.Render();
	// Clear the color buffer
	glClear(GL_COLOR_BUFFER_BIT);

	// Use the program object
	glUseProgram(gProgram);

	// Load the vertex position
	glVertexAttribPointer(gPositionLoc, 3, GL_FLOAT, GL_FALSE,
			5 * sizeof(GLfloat), vertices);
	// Load the texture coordinate
	glVertexAttribPointer(gTexCoordLoc, 2, GL_FLOAT, GL_FALSE,
			5 * sizeof(GLfloat), vertices + 3);

	glEnableVertexAttribArray(gPositionLoc);
	glEnableVertexAttribArray(gTexCoordLoc);

	// Bind the texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gTexture);

	// Set the texture sampler to texture unit to 0
	glUniform1i(gSamplerLoc, 0);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, indices);
}
