#include <iostream>

// 1. تضمين المكتبات
// ملاحظة: يجب تضمين GLEW قبل GLFW دائماً
#define GLEW_STATIC // لأننا نستخدم مكتبة glew32s.lib (Static)
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// إعدادات النافذة
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// متغيرات التحكم
float bgRed = 0.3f, bgGreen = 0.3f, bgBlue = 0.3f;  // لون الخلفية (رمادي)
float triangleRed = 1.0f, triangleGreen = 0.4f, triangleBlue = 0.7f;  // لون المثلث (زهري)
float triangleAlpha = 1.0f; //شفافية 


// --- 2. كود المظللات (Shaders) ---

// A. كود Vertex Shader
const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";

// B. كود Fragment Shader
const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"uniform vec4 triangleColor;\n"  // متغير Uniform للون المثلث
"void main()\n"
"{\n"
"   FragColor = triangleColor;\n" // زهري
"}\n\0";

// دالة لمعالجة تغيير حجم النافذة من قبل المستخدم
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

// دالة لمعالجة المدخلات
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	// زر المسافة: التبديل بين وضع الخطوط والتعبئة
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // وضع الخطوط (شبكة)
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // وضع التعبئة (الافتراضي)

		// السهم العلوي: تغيير لون الخلفية للأحمر
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
	{
		bgRed = 1.0f;
		bgGreen = 0.0f;
		bgBlue = 0.0f;
	}
	// السهم السفلي: تغيير لون المثلث عن طريق الـ Uniform
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		// تغيير لون المثلث إلى الأزرق
		triangleRed = 0.0f;
		triangleGreen = 0.0f;
		triangleBlue = 1.0f;
	}
	

	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
		glPointSize(10.0f);
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
		triangleAlpha = 0.5f;//شفافية عالية

}


int main()
{
	// --- 3. تهيئة GLFW وإعداد النافذة ---
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef APPLE
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL - First Triangle", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// --- 4. تهيئة GLEW ---
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		return -1;
	}

	glEnable(GL_BLEND);      // تفعيل الدمج (للشفافية)
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // معادلة الدمج



	// --- 5. بناء وتجميع برنامج الشيدر (Shader Program) ---
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	int success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	unsigned int shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	// --- الحصول على موقع الـ Uniform في برنامج الشيدر ---
	int colorLocation = glGetUniformLocation(shaderProgram, "triangleColor");


	// --- 6. تعريف البيانات (المثلث) ---
	float vertices[] = {
	  -0.5f, -0.5f, 0.0f,
	   0.5f, -0.5f, 0.0f,
	   0.0f,  0.5f, 0.0f,
	};

	// --- 7. إعداد VBO و VAO ---
	unsigned int VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// --- 8. حلقة الرسم (Render Loop) ---
	while (!glfwWindowShouldClose(window))
	{
		// أ. معالجة المدخلات
		processInput(window);

		// ب. تنظيف الشاشة بلون الخلفية الحالي
		glClearColor(bgRed, bgGreen, bgBlue, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// ج. الرسم
		glUseProgram(shaderProgram);

		// *** إرسال قيمة اللون إلى الـ Fragment Shader عن طريق Uniform ***
		glUniform4f(colorLocation, triangleRed, triangleGreen, triangleBlue, triangleAlpha);


		glBindVertexArray(VAO);


		glDrawArrays(GL_TRIANGLES, 0, 3);

		// د. تبديل الـ Buffers ومعالجة الأحداث
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// --- 9. التنظيف النهائي ---
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteProgram(shaderProgram);

	glfwTerminate();
	return 0;
}