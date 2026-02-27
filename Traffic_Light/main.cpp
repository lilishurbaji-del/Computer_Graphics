#include <iostream>

// 1. تضمين المكتبات
// ملاحظة: يجب تضمين GLEW قبل GLFW دائماً
#define GLEW_STATIC // لأننا نستخدم مكتبة glew32s.lib (Static)
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define PI 3.14159265358979323846f
const int CIRCLE_SEGMENTS = 12; // 12 مثلث كافية لدائرة بسيطة
// إعدادات النافذة

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

float car1 = -1.2f;
float carSpeed = 0.0005f;
float car2 = -1.8f;
float carSpeed2 = 0.0003f;
char lightState = 'G';
bool light1 = false;
bool light2 = false;
float trafficLightLocation = 0.75f; // موقع إشارة المرور على الطريق
float stopLine1 = 0.45f; // خط توقف السيارة الأولى (قبل الإشارة)
float stopLine2 = 0.15f; // خط توقف السيارة الثانية (خلف الأولى)
bool autoMode = true; // النظام يبدأ بالعمل تلقائياً

// --- 2. كود المظللات (Shaders) ---

// A. كود Vertex Shader
// وظيفته: تحديد موقع رؤوس المثلث في الفضاء
const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n" // نستقبل البيانات في الموقع 0
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n" // نمرر الموقع كما هو
"}\0";

// B. كود Fragment Shader
// وظيفته: تحديد لون البكسلات (هنا أخضر عشبي)
const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"uniform vec3 ourColor;\n"
"void main()\n"
"{\n"
" FragColor = vec4(ourColor, 1.0f);\n"
"}\n\0";

// دالة لمعالجة تغيير حجم النافذة من قبل المستخدم
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

// دالة لمعالجة المدخلات (مثل الضغط على زر ESC)
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	// ازرار تفاعلية للتحكم بالوان اشارة المرور

	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
		lightState = 'R'; // أحمر
		autoMode = false; // إيقاف الوضع التلقائي
	}
	if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS) {
		lightState = 'Y'; // أصفر
		autoMode = false;
	}
	if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) {
		lightState = 'G'; // أخضر
		autoMode = false;
		//0 ,1 التحكم بالاضاءة عن طريق 
	}
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
		light1 = true;

	}
	if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS) {
		light1 = false; // يطفئ الضوء عند ضغط K (أو أي زر آخر تختارينه)
	}
}
void DrawCircle(float* vertices, float centerX, float centerY, float radius) {
	float twicePi = 2.0f * PI;
	int index = 0;
	for (int i = 0; i < CIRCLE_SEGMENTS; i++) {
		// نقطة المركز لكل مثلث
		vertices[index++] = centerX;
		vertices[index++] = centerY;
		vertices[index++] = 0.0f;

		// النقطة الأولى على المحيط
		vertices[index++] = centerX + (radius * cos(i * twicePi / CIRCLE_SEGMENTS));
		vertices[index++] = centerY + (radius * sin(i * twicePi / CIRCLE_SEGMENTS));
		vertices[index++] = 0.0f;

		// النقطة الثانية على المحيط (لتشكيل المثلث)
		vertices[index++] = centerX + (radius * cos((i + 1) * twicePi / CIRCLE_SEGMENTS));
		vertices[index++] = centerY + (radius * sin((i + 1) * twicePi / CIRCLE_SEGMENTS));
		vertices[index++] = 0.0f;
	}
}

int main()
{
	// --- 3. تهيئة GLFW وإعداد النافذة ---
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // إصدار OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // النمط الحديث

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // خاص بأجهزة ماك
#endif

	// إنشاء كائن النافذة
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL - First Triangle", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window); // جعل هذه النافذة هي سياق الرسم الحالي
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); // ربط دالة تغيير الحجم

	// --- 4. تهيئة GLEW ---
	glewExperimental = GL_TRUE; // تفعيل التقنيات الحديثة
	if (glewInit() != GLEW_OK)
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		return -1;
	}

	// --- 5. بناء وتجميع برنامج الشيدر (Shader Program) ---

	// أ. تجميع Vertex Shader
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	// فحص الأخطاء
	int success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	// ب. تجميع Fragment Shader
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	// فحص الأخطاء
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	// ج. ربط الشيدرز في برنامج واحد (Shader Program)
	unsigned int shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	// فحص أخطاء الربط
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}

	// حذف الشيدرز المنفصلة لأننا ربطناها في البرنامج ولم نعد بحاجة لها
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	// لغير الالوان بالمكان يلي بدي ياه
	int colorLocation = glGetUniformLocation(shaderProgram, "ourColor");

	// --- 6. تعريف البيانات (المستطيل) ---
// مستطيل أفقي يعبي النص السفلي من الشاشة
	float vertices[] = {
		// المثلث الأول
		-1.0f, -1.0f, 0.0f,  // 0: يسار أسفل (أسفل الشاشة)
		 1.0f, -1.0f, 0.0f,  // 1: يمين أسفل (أسفل الشاشة)
		-1.0f,  0.0f, 0.0f,  // 2: يسار أعلى (منتصف الشاشة)

		// المثلث الثاني
		 1.0f, -1.0f, 0.0f,  // 3: يمين أسفل (أسفل الشاشة)
		 1.0f,  0.0f, 0.0f,  // 4: يمين أعلى (منتصف الشاشة)
		-1.0f,  0.0f, 0.0f   // 5: يسار أعلى (منتصف الشاشة)

	};
	float roadVertices[] = {
	-1.0f, -0.2f,0.0f,
	 1.0f, -0.2f,0.0f,
	 1.0f, -0.5f,0.0f,

	-1.0f, -0.2f,0.0f,
	 1.0f, -0.5f,0.0f,
	-1.0f, -0.5f,0.0f
	};
	//الخط الابيض 
	float lineVertices[] = {
	-0.6f, -0.36f, 0.0f,
	-0.2f, -0.36f, 0.0f,
	-0.2f, -0.34f, 0.0f,

	-0.6f, -0.36f, 0.0f,
	 -0.2f, -0.34f, 0.0f,
	-0.6f, -0.34f, 0.0f
	};
	//الخط الابيض 
	float lineVertices2[] = {
	0.2f, -0.36f, 0.0f,
	 0.6f, -0.36f, 0.0f,
	 0.6f, -0.34f, 0.0f,

	0.2f, -0.36f, 0.0f,
	 0.6f, -0.34f, 0.0f,
	0.2f, -0.34f, 0.0f
	};

	//مستطيل اشارة المرور 
	float trafficLight[] = {
		// المثلث الأول
 0.75f,  0.2f, 0.0f,  // يسار أعلى
 0.85f,  0.2f, 0.0f,  // يمين أعلى
 0.85f, -0.2f, 0.0f,  // يمين أسفل

 // المثلث الثاني
 0.75f,  0.2f, 0.0f,  // يسار أعلى
 0.85f, -0.2f, 0.0f,  // يمين أسفل
 0.75f, -0.2f, 0.0f   // يسار أسفل
	};

	//  العمود الذي يحمل اشارة المرور
	float poleVertices[] = {
	0.79f, -0.2f, 0.0f,
	0.81f, -0.2f, 0.0f,
	0.81f, -0.5f, 0.0f,

	0.79f, -0.2f, 0.0f,
	0.81f, -0.5f, 0.0f,
	0.79f, -0.5f, 0.0f
	};
	//  اضواء اشارة المرور

	//  اضواء اشارة المرور - دوائر بسيطة

// مصفوفات الدوائر - حجم ثابت ومناسب
	float redCircle[3 * 3 * 12];    // 12 مثلث × 3 رؤوس × 3 إحداثيات
	float yellowCircle[3 * 3 * 12];
	float greenCircle[3 * 3 * 12];


	//رسم شجرة 
	// جذع الشجرة مستطيل بني
	float treeTrunk[] = {
		-0.72f, 0.1f, 0.0f,  // يسار أعلى
		-0.68f, 0.1f, 0.0f,  // يمين أعلى
		-0.68f, -0.1f, 0.0f,  // يمين أسفل

		-0.72f, 0.1f, 0.0f,
		-0.68f, -0.1f, 0.0f,
		-0.72f, -0.1f, 0.0f
	};

	//  أوراق الشجرة مثلث أخضر كبير 
	float treeBranches[] = {
		 -0.85f, 0.1f, 0.0f,
		 -0.55f, 0.1f, 0.0f,
		 -0.70f,  0.6f, 0.0f
	};

	float Sun[3 * 3 * 20]; // مصفوفة لنقاط الشمس
	// نضع الشمس في المركز 0.0 على محور X، و 0.75 بالأعلى على محور Y
	DrawCircle(Sun, 0.0f, 0.75f, 0.15f);

	// حساب مواقع الدوائر الثلاث داخل مستطيل الإشارة
// مستطيل الإشارة عندك يبدأ من x=0.75 إلى 0.85 (المركز 0.8) ومن y=-0.2 إلى 0.2
	DrawCircle(redCircle, 0.8f, 0.12f, 0.035f); // الدائرة الحمراء بالأعلى
	DrawCircle(yellowCircle, 0.8f, 0.0f, 0.035f); // الصفراء بالمنتصف
	DrawCircle(greenCircle, 0.8f, -0.12f, 0.035f); // الخضراء بالأسفل


	// --- 7. إعداد VBO و VAO ---
	unsigned int VBO, VAO;
	glGenVertexArrays(1, &VAO); // توليد المعرفات
	glGenBuffers(1, &VBO);

	// خطوة مهمة جداً: اربط VAO أولاً قبل أي شيء آخر
	glBindVertexArray(VAO);

	// اربط VBO وانسخ البيانات إليه
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// شرح البيانات لـ OpenGL (Linking Vertex Attributes)
	// 0: الموقع في الشيدر
	// 3: حجم البيانات (3 floats لكل نقطة)
	// GL_FLOAT: نوع البيانات
	// GL_FALSE: لا نريد تطبيع البيانات
	// 3 * sizeof(float): الخطوة (Stride)
	// (void*)0: الإزاحة (بداية البيانات)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0); // تفعيل السمة رقم 0

	// فك الربط (اختياري ولكنه ممارسة جيدة)
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// --- 8. حلقة الرسم (Render Loop) ---
	while (!glfwWindowShouldClose(window))
	{
		// أ. معالجة المدخلات
		processInput(window);

		// ب. التنظيف (لون الخلفية)  ازرق فاتح
		glClearColor(0.8f, 0.9f, 1.0f, 1.0f); //  (سماوي)
		glClear(GL_COLOR_BUFFER_BIT);

		//التحكم بالسيارة الاولى الزرقاء 

		if (lightState == 'R') {// توقف 
			// إذا كانت السيارة خلف خط التوقف، استمري بالمشي حتى تصلي إليه
			if (car1 < stopLine1) {
				car1 += carSpeed;
			}
			// إذا كانت السيارة قد تجاوزت الإشارة فعلياً، لا تقفي في المنتصف بل أكملي طريقك
			else if (car1 > trafficLightLocation) {
				car1 += carSpeed;
			}
		}
		else if (lightState == 'Y') { // تباطؤ
			car1 += carSpeed * 0.1f;
		}
		else { // تمشي عادي 
			car1 += carSpeed;
		}
		//// برجع السيارة الزرقاء اذا خرجت من الشاشة

		if (car1 > 1.2f)
			car1 = -1.2f;
		//  التحكم بالسيارة الثانية الحمراء

		if (lightState == 'R') {
			// السيارة الثانية تقف خلف الأولى
			if (car2 < stopLine2) {
				car2 += carSpeed2;
			}
			// إذا تجاوزت الإشارة أكملي طريقك
			else if (car2 > trafficLightLocation) {
				car2 += carSpeed2;
			}
		}

		else if (lightState == 'Y') {
			car2 += carSpeed2 * 0.1f;
		}
		else {
			car2 += carSpeed2;
		}
		// برجع السيارة االحمراء ذا خرجت من الشاشة
		if (car2 > 1.2f)
			car2 = -1.5f;



		//تعريف جسم السيارة   car1
		float carVertices[] = {
			car1,       -0.45f, 0.0f,
			car1 + 0.3f, -0.45f, 0.0f,
			car1 + 0.3f, -0.35f, 0.0f,

			car1,       -0.45f, 0.0f,
			car1 + 0.3f, -0.35f, 0.0f,
			car1,       -0.35f, 0.0f
		};
		//سقف السيارة 
		float carTop[] = {
	car1 + 0.05f, -0.35f, 0.0f,
	car1 + 0.25f, -0.35f, 0.0f,
	car1 + 0.22f, -0.30f, 0.0f,

	car1 + 0.05f, -0.35f, 0.0f,
	car1 + 0.22f, -0.30f, 0.0f,
	car1 + 0.08f, -0.30f, 0.0f
		};
		//عجلة امامية للسيارة 
		float wheel1[] = {
	car1 + 0.05f, -0.45f, 0.0f,
	car1 + 0.10f, -0.45f, 0.0f,
	car1 + 0.10f, -0.48f, 0.0f,

	car1 + 0.05f, -0.45f, 0.0f,
	car1 + 0.10f, -0.48f, 0.0f,
	car1 + 0.05f, -0.48f, 0.0f
		};

		// عجلة خلفية للسيارة
		float wheel2[] = {
	car1 + 0.20f, -0.45f, 0.0f,
	car1 + 0.25f, -0.45f, 0.0f,
	car1 + 0.25f, -0.48f, 0.0f,

	car1 + 0.20f, -0.45f, 0.0f,
	car1 + 0.25f, -0.48f, 0.0f,
	car1 + 0.20f, -0.48f, 0.0f
		};


		//تعريف جسم السيارة   car2
		float carVertices2[] = {
			car2,       -0.45f, 0.0f,
			car2 + 0.3f, -0.45f, 0.0f,
			car2 + 0.3f, -0.35f, 0.0f,

			car2,       -0.45f, 0.0f,
			car2 + 0.3f, -0.35f, 0.0f,
			car2,       -0.35f, 0.0f
		};
		//سقف السيارة 
		float carTop2[] = {
	car2 + 0.05f, -0.35f, 0.0f,
	car2 + 0.25f, -0.35f, 0.0f,
	car2 + 0.22f, -0.30f, 0.0f,

	car2 + 0.05f, -0.35f, 0.0f,
	car2 + 0.22f, -0.30f, 0.0f,
	car2 + 0.08f, -0.30f, 0.0f
		};
		// نافذة للسيارة الزرقاء
		float carWindow1[] = {
		car1 + 0.08f,-0.34f,0.0f,
		car1 + 0.22f, -0.34f,0.0f,
		car1 + 0.20f,-0.31f,0.0f,

	car1 + 0.08f, -0.34f, 0.0f,
	car1 + 0.20f, -0.31f, 0.0f,
	car1 + 0.10f, -0.31f, 0.0f  // يسار أعلى
		};

		// نافذة للسيارة الحمرااء
		float carWindow2[] = {
	car2 + 0.08f, -0.34f, 0.0f,
	car2 + 0.22f, -0.34f, 0.0f,
	car2 + 0.20f, -0.31f, 0.0f,

	car2 + 0.08f, -0.34f, 0.0f,
	car2 + 0.20f, -0.31f, 0.0f,
	car2 + 0.10f, -0.31f, 0.0f
		};
		//عجلة امامية للسيارة 
		float Wheel1[] = {
	car2 + 0.05f, -0.45f, 0.0f,
	car2 + 0.10f, -0.45f, 0.0f,
	car2 + 0.10f, -0.48f, 0.0f,

	car2 + 0.05f, -0.45f, 0.0f,
	car2 + 0.10f, -0.48f, 0.0f,
	car2 + 0.05f, -0.48f, 0.0f
		};

		// عجلة خلفية للسيارة
		float Wheel2[] = {
	car2 + 0.20f, -0.45f, 0.0f,
	car2 + 0.25f, -0.45f, 0.0f,
	car2 + 0.25f, -0.48f, 0.0f,

	car2 + 0.20f, -0.45f, 0.0f,
	car2 + 0.25f, -0.48f, 0.0f,
	car2 + 0.20f, -0.48f, 0.0f
		};

		float lightRay1[] = {
	   car1 + 0.3f,  -0.39f, 0.0f,
		car1 + 0.5f,  -0.41f, 0.0f,
		car1 + 0.5f,  -0.37f, 0.0f,
		car1 + 0.3f,  -0.39f, 0.0f,
		car1 + 0.5f,  -0.37f, 0.0f,
		car1 + 0.3f,  -0.38f, 0.0f
		};

		float lightRay2[] = {
	   car2 + 0.3f,  -0.39f, 0.0f,
		car2 + 0.5f,  -0.41f, 0.0f,
		car2 + 0.5f,  -0.37f, 0.0f,
		car2 + 0.3f,  -0.39f, 0.0f,
		car2 + 0.5f,  -0.37f, 0.0f,
		car2 + 0.3f,  -0.38f, 0.0f
		};

		if (autoMode) {
			float t = glfwGetTime();
			int cycle = (int)t % 12; // دورة كاملة كل 12 ثانية

			if (cycle < 5) lightState = 'G';      // أول 5 ثواني: أخضر
			else if (cycle < 7) lightState = 'Y'; // ثانيتين: أصفر
			else lightState = 'R';               // 5 ثواني: أحمر
		}



		// ج. الرسم
		glUseProgram(shaderProgram);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);


		// ارسم الأرض (أخضر)
		glUniform3f(colorLocation, 0.0f, 0.6f, 0.0f);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		//ارسم الطريق (رمادي)
		glUniform3f(colorLocation, 0.2f, 0.2f, 0.2f);
		glBufferData(GL_ARRAY_BUFFER, sizeof(roadVertices), roadVertices, GL_STATIC_DRAW);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		//glBindVertexArray(VAO); // لا داعي لفك الربط في كل فريم

	//الخط الابيض الاول
		glUniform3f(colorLocation, 1.0f, 1.0f, 1.0f);
		glBufferData(GL_ARRAY_BUFFER, sizeof(lineVertices), lineVertices, GL_STATIC_DRAW);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		//الخط الابيض الاول
		glBufferData(GL_ARRAY_BUFFER, sizeof(lineVertices2), lineVertices2, GL_STATIC_DRAW);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		// 1السيارة
		glUniform3f(colorLocation, 0.0f, 0.0f, 1.0f); // أزرق
		glBufferData(GL_ARRAY_BUFFER, sizeof(carVertices), carVertices, GL_STATIC_DRAW);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		//1سطح السيارة 
		glUniform3f(colorLocation, 0.0f, 0.0f, 0.8f);
		glBufferData(GL_ARRAY_BUFFER, sizeof(carTop), carTop, GL_STATIC_DRAW);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		// عجلة امامية للسيارة 1
		glUniform3f(colorLocation, 0.0f, 0.0f, 0.0f);
		glBufferData(GL_ARRAY_BUFFER, sizeof(wheel1), wheel1, GL_STATIC_DRAW);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		//عجلة خلفية للسيارة 1
		glBufferData(GL_ARRAY_BUFFER, sizeof(wheel2), wheel2, GL_STATIC_DRAW);
		glDrawArrays(GL_TRIANGLES, 0, 6);


		// 2السيارة
		glUniform3f(colorLocation, 0.8f, 0.2f, 0.2f); //احمر 
		glBufferData(GL_ARRAY_BUFFER, sizeof(carVertices2), carVertices2, GL_STATIC_DRAW);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		//2سطح السيارة
		glUniform3f(colorLocation, 0.8f, 0.2f, 0.2f);
		glBufferData(GL_ARRAY_BUFFER, sizeof(carTop2), carTop2, GL_STATIC_DRAW);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		//نافذة للسيارة1
		glUniform3f(colorLocation, 0.7f, 0.9f, 1.0f);
		glBufferData(GL_ARRAY_BUFFER, sizeof(carWindow1), carWindow1, GL_STATIC_DRAW);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		//نافذة للسيار21
		glUniform3f(colorLocation, 0.7f, 0.9f, 1.0f);
		glBufferData(GL_ARRAY_BUFFER, sizeof(carWindow2), carWindow2, GL_STATIC_DRAW);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		// عجلة امامية للسيارة 2
		glUniform3f(colorLocation, 0.0f, 0.0f, 0.0f);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Wheel1), Wheel1, GL_STATIC_DRAW);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		//عجلة خلفية للسيارة2 
		glUniform3f(colorLocation, 0.0f, 0.0f, 0.0f);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Wheel2), Wheel2, GL_STATIC_DRAW);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		// اضاءة السيارة 1
		if (light1) {
			glUniform3f(colorLocation, 1.0f, 1.0f, 0.0f);
			glBufferData(GL_ARRAY_BUFFER, sizeof(lightRay1), lightRay1, GL_STATIC_DRAW);
			glDrawArrays(GL_TRIANGLES, 0, 6);
			// اضاءة السيارة 2
			glUniform3f(colorLocation, 1.0f, 1.0f, 0.0f);
			glBufferData(GL_ARRAY_BUFFER, sizeof(lightRay2), lightRay2, GL_STATIC_DRAW);
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}
		//رسم اشارة المرور
		//مستطيل (جسم الاشارة
		glUniform3f(colorLocation, 0.05f, 0.05f, 0.05f);
		glBufferData(GL_ARRAY_BUFFER, sizeof(trafficLight), trafficLight, GL_STATIC_DRAW);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		//عمود الاشارة 
		glUniform3f(colorLocation, 0.6f, 0.6f, 0.6f);
		glBufferData(GL_ARRAY_BUFFER, sizeof(poleVertices), poleVertices, GL_STATIC_DRAW);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		// دائرة العلوي الاحمر
		if (lightState == 'R')
			glUniform3f(colorLocation, 1.0f, 0.0f, 0.0f);
		else
			glUniform3f(colorLocation, 0.2f, 0.0f, 0.0f);
		glBufferData(GL_ARRAY_BUFFER, sizeof(redCircle), redCircle, GL_STATIC_DRAW);
		glDrawArrays(GL_TRIANGLES, 0, 3 * 12); // 36 رأس

		// دائرة الاوسط الاصفر
		if (lightState == 'Y')
			glUniform3f(colorLocation, 1.0f, 1.0f, 0.0f);
		else
			glUniform3f(colorLocation, 0.3f, 0.3f, 0.0f);
		glBufferData(GL_ARRAY_BUFFER, sizeof(yellowCircle), yellowCircle, GL_STATIC_DRAW);
		glDrawArrays(GL_TRIANGLES, 0, 3 * 12);

		// دائرة السفلي الاخضر
		if (lightState == 'G')
			glUniform3f(colorLocation, 0.0f, 1.0f, 0.0f);
		else
			glUniform3f(colorLocation, 0.0f, 0.3f, 0.0f);
		glBufferData(GL_ARRAY_BUFFER, sizeof(greenCircle), greenCircle, GL_STATIC_DRAW);
		glDrawArrays(GL_TRIANGLES, 0, 3 * 12);

		//رسم الجذع بني
		glUniform3f(colorLocation, 0.45f, 0.25f, 0.1f);
		glBufferData(GL_ARRAY_BUFFER, sizeof(treeTrunk), treeTrunk, GL_STATIC_DRAW);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		// رسم الشجرة
		glUniform3f(colorLocation, 0.2f, 0.6f, 0.2f);
		glBufferData(GL_ARRAY_BUFFER, sizeof(treeBranches), treeBranches, GL_STATIC_DRAW);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		// رسم الشمس باللون الأصفر
		glUniform3f(colorLocation, 1.0f, 1.0f, 0.0f);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Sun), Sun, GL_STATIC_DRAW);
		glDrawArrays(GL_TRIANGLES, 0, 3 * 20);

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
