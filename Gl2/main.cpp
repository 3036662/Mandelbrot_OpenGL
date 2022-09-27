#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <Mandelbrot.h>


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

// Константы
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

const double X_START=-1.8;
const double X_END=0.5;
const double Y_START=-1.2;
const double Y_END=1.1;


const unsigned int MAX_ITER=50;// максимальное количество итераций

const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec2 aPos;\n"
    "uniform mat4 tm;"
    "void main()\n"
    "{\n"
       "gl_Position = tm*vec4(aPos.x, aPos.y, 0.0, 1.0);\n"
    "}\0";

const char *fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    "}\n\0";


int main()
{
    // glfw: инициализация и конфигурирование
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // glfw: создание окна
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Mandelbrot", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: загрузка всех указателей на OpenGL-функции
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }



    //рассчет
    Mandelbrot fractal;
    // пара -количество элементов в массиве,  указатель на массив точек point
    std::pair<size_t,double*> points;
    points=fractal.calc(SCR_WIDTH,SCR_HEIGHT,X_START,X_END,Y_START,Y_END,MAX_ITER);
    // создать бувер для точек (vertex bufer)
    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(double)*points.first, points.second, GL_STATIC_DRAW);
    // Vertex Array Object
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    // аттрибуты данных:  индекс аттрибута=0, количество аттрибутов=2, тип аттрибута= double
    // необходимость нормализации = false, смещение между наборами аттрибутов =2, смещение до первого аттрибута=0
    glVertexAttribPointer(0, 2, GL_DOUBLE, GL_FALSE, 2 * sizeof(double), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // необходимо получить NDC  нормализованные координаты
    // для того, чтобы рисунок влез в окно, необходимо его сначала отмасшатабировать
    // пори этом пропорции рисунка должны сохраниться
    float scale_x=2/(X_END-X_START); //0.87
    float scale_y=2/(Y_END-Y_START); // 0.87
    float scale{1};
    scale_x<=scale_y ? scale=scale_x : scale=scale_y; // выбрать наименьший коэффициент масштабирования

    // также необходимо перенести  изображение так чтобы
    // все координаты влезли в диапазон NDC ( от -1 до 1) - нормализованные координаты
    // крайняя левая коордианата (-1.8) после масштабирования будет равна -1.8*0.0.87=-1.566 ее нужно переместить на 0,566 вправо
    // крайняя правая координата (0.5) после масштабирования будет равна 0,435, после переноса на 0,566 , перейдет в 1.0
    float move_x=-1-X_START*scale;  // перенос по x
    float move_y=-1-Y_START*scale; // перенос по y
    // параметры преобразования известны
    // задать матрицу преобразования
    float transf_matrix[]{
                           scale,      0.0f,     0.0f,         0,
                           0.0f,      scale,     0.0f,         0,
                           0.0f,       0.0f,    scale,      0.0f,
                           move_x,   move_y,    0.0f,       1.0f
                           };

    std::cout<<"scale="<<scale<<std::endl;
    std::cout<<"move_x="<<move_x<<std::endl;
    std::cout<<"move_y="<<move_y<<std::endl;
    // шейдеры
    // вершинный шейдер
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // проверка на ошибки
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // фрагментный шейдер
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // проверка на ошибки
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // связать шейдеры в программу
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // проверка на ошибки
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    glUseProgram(shaderProgram);
    // отправить матрицу трансформации в вершинный шейдер
    int modelLoc = glGetUniformLocation(shaderProgram, "tm");
    std::cout<<modelLoc;
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, transf_matrix);


    // Цикл рендеринга
    while (!glfwWindowShouldClose(window))
    {
        // Обработка ввода
        processInput(window);



		// Выполнение рендеринга
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        //glUseProgram(shaderProgram);
        glDrawArrays(GL_POINTS, 0, points.first/2);
        // glfw: обмен содержимым front- и back-буферов. Отслеживание событий ввода/вывода (была ли нажата/отпущена кнопка, перемещен курсор мыши и т.п.)
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: завершение, освобождение всех ранее задействованных GLFW-ресурсов
    glfwTerminate();
    return 0;
}


// Обработка всех событий ввода: запрос GLFW о нажатии/отпускании клавиш на клавиатуре в текущем кадре и соответствующая обработка данных событий
void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: всякий раз, когда изменяются размеры окна (пользователем или операционной системой), вызывается данная callback-функция
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // Убеждаемся, что окно просмотра соответствует новым размерам окна.
    //std::cout<<"Window changed"<<std::endl;
    glViewport(0, 0, width, height);
}
