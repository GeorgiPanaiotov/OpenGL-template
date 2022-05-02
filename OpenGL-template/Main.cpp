#include <GLEW/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

struct ShaderCodes {
	std::string	VertexSource;
	std::string FragmentSource;
};

static ShaderCodes ParseShader(const std::string& filepath) {
	std::ifstream stream(filepath);

	enum class ShaderType {
		NONE = -1,
		VERTEX = 0,
		FRAGMENT = 1
	};

	std::string line;
	std::stringstream sstream[2];
	ShaderType type = ShaderType::NONE;

	while (getline(stream, line)) {
		if (line.find("#shader") != std::string::npos) {
			if (line.find("vertex") != std::string::npos) {
				type = ShaderType::VERTEX;
			}
			else if (line.find("fragment") != std::string::npos) {
				type = ShaderType::FRAGMENT;
			}
		}
		else {
			sstream[(int)type] << line << "\n";
		}
	}

	return { sstream[0].str(), sstream[1].str() };
};

static unsigned int CompileShader(unsigned int type, const std::string& sourceCode) {
	unsigned int id = glCreateShader(type);
	const char* src = sourceCode.c_str();
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);

	return id;
}

static unsigned int CreateShader(const std::string& VertexShader, const std::string& FragmentShader) {
	unsigned int program = glCreateProgram();
	unsigned int vShader = CompileShader(GL_VERTEX_SHADER, VertexShader);
	unsigned int fShader = CompileShader(GL_FRAGMENT_SHADER, FragmentShader);

	glAttachShader(program, vShader);
	glAttachShader(program, fShader);
	glLinkProgram(program);
	glValidateProgram(program);

	glDeleteShader(vShader);
	glDeleteShader(fShader);
	return program;
}

int main() {
	if (!glfwInit()) {
		std::cout << "There was an error initiating OpenGL!" << std::endl;
		return -1;
	}

	GLFWwindow* window;
	window = glfwCreateWindow(1024, 720, "OpenGL window", NULL, NULL);

	if (!window) {
		std::cout << "There was an error while creating the window!" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	if (glewInit() != GLEW_OK) {
		std::cout << "There was an error while initiating GLEW!" << std::endl;
		glfwTerminate();
		return -1;
	}

	float positions[6] = {
		-1.0f, -1.0f,
		0.0f, 1.0f,
		1.0f, -1.0f
	};

	unsigned int buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), positions, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);

	ShaderCodes shadersSouces = ParseShader("res/shaders/Basic.shader");

	unsigned int shader = CreateShader(shadersSouces.VertexSource, shadersSouces.FragmentSource);
	glUseProgram(shader);

	while (!glfwWindowShouldClose(window)) {

		glClear(GL_COLOR_BUFFER_BIT);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteProgram(shader);
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}