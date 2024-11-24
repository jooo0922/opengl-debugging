#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <shader/shader.hpp>

#include <iostream>

/** 콜백함수 전방 선언 */

// GLFW 윈도우 resizing 콜백함수
void framebuffer_size_callback(GLFWwindow *window, int width, int height);

// GLFW 윈도우 키 입력 콜백함수
void processInput(GLFWwindow *window);

/** 스크린 해상도 선언 */
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

int main()
{
  // GLFW 초기화 및 윈도우 설정 구성
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // GLFW 윈도우 생성 및 현재 OpenGL 컨텍스트로 등록
  GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "OpenGL Debugging", nullptr, nullptr);
  glfwMakeContextCurrent(window);
  if (window == NULL)
  {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return -1;
  }

  // GLFW 윈도우 resizing 콜백함수 등록
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  // GLFW 마우스 커서 입력 모드 설정
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  // GLAD 를 사용하여 OpenGL 표준 API 호출 시 사용할 현재 그래픽 드라이버에 구현된 함수 포인터 런타임 로드
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
  {
    // 함수 포인터 로드 실패
    std::cout << "Failed to initialized GLAD" << std::endl;
    return -1;
  }

  // OpenGL 전역 상태 설정
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);

  // Shader 객체 생성
  Shader shader("resources/shaders/debugging.vs", "resources/shaders/debugging.fs");

  /** cube VAO, VBO 설정 */
  unsigned int cubeVAO, cubeVBO;
  float vertices[] = {
      // back face
      -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, // bottom-left
      0.5f, 0.5f, -0.5f, 1.0f, 1.0f,   // top-right
      0.5f, -0.5f, -0.5f, 1.0f, 0.0f,  // bottom-right
      0.5f, 0.5f, -0.5f, 1.0f, 1.0f,   // top-right
      -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, // bottom-left
      -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,  // top-left
      // front face
      -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, // bottom-left
      0.5f, -0.5f, 0.5f, 1.0f, 0.0f,  // bottom-right
      0.5f, 0.5f, 0.5f, 1.0f, 1.0f,   // top-right
      0.5f, 0.5f, 0.5f, 1.0f, 1.0f,   // top-right
      -0.5f, 0.5f, 0.5f, 0.0f, 1.0f,  // top-left
      -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, // bottom-left
      // left face
      -0.5f, 0.5f, 0.5f, -1.0f, 0.0f,   // top-right
      -0.5f, 0.5f, -0.5f, -1.0f, 1.0f,  // top-left
      -0.5f, -0.5f, -0.5f, -0.0f, 1.0f, // bottom-left
      -0.5f, -0.5f, -0.5f, -0.0f, 1.0f, // bottom-left
      -0.5f, -0.5f, 0.5f, -0.0f, 0.0f,  // bottom-right
      -0.5f, 0.5f, 0.5f, -1.0f, 0.0f,   // top-right
      // right face
      0.5f, 0.5f, 0.5f, 1.0f, 0.0f,   // top-left
      0.5f, -0.5f, -0.5f, 0.0f, 1.0f, // bottom-right
      0.5f, 0.5f, -0.5f, 1.0f, 1.0f,  // top-right
      0.5f, -0.5f, -0.5f, 0.0f, 1.0f, // bottom-right
      0.5f, 0.5f, 0.5f, 1.0f, 0.0f,   // top-left
      0.5f, -0.5f, 0.5f, 0.0f, 0.0f,  // bottom-left
      // bottom face
      -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, // top-right
      0.5f, -0.5f, -0.5f, 1.0f, 1.0f,  // top-left
      0.5f, -0.5f, 0.5f, 1.0f, 0.0f,   // bottom-left
      0.5f, -0.5f, 0.5f, 1.0f, 0.0f,   // bottom-left
      -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,  // bottom-right
      -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, // top-right
      // top face
      -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, // top-left
      0.5f, 0.5f, 0.5f, 1.0f, 0.0f,   // bottom-right
      0.5f, 0.5f, -0.5f, 1.0f, 1.0f,  // top-right
      0.5f, 0.5f, 0.5f, 1.0f, 0.0f,   // bottom-right
      -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, // top-left
      -0.5f, 0.5f, 0.5f, 0.0f, 0.0f   // bottom-left
  };

  // VAO, VBO 객체 생성
  glGenVertexArrays(1, &cubeVAO);
  glGenBuffers(1, &cubeVBO);

  // VBO 객체에 메모리 할당 및 데이터 쓰기
  glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  // VAO 객체 설정 및 정점 데이터 해석 방식 정의
  glBindVertexArray(cubeVAO);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));

  // VAO 객체 설정 완료 후 바인딩 해제
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  /** cube Texture 로드 */
  unsigned int texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  int width, height, nrComponents;
  unsigned char *data = stbi_load("resources/textures/wood.png", &width, &height, &nrComponents, 0);
  if (data)
  {
    // glTexImage2D(GL_FRAMEBUFFER, 0, nrComponents, width, height, 0, nrComponents, GL_UNSIGNED_BYTE, data); // error version
    glTexImage2D(GL_TEXTURE_2D, 0, nrComponents, width, height, 0, nrComponents, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  }
  else
  {
    std::cout << "Failed to load texture" << std::endl;
  }
  stbi_image_free(data);

  /** projection matrix 계산 및 쉐이더 전송 */
  glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10.0f);
  shader.setMat4("projection", projection);
  shader.setInt("tex", 0);

  return 0;
}

/** 콜백함수 구현부 */

// GLFW 윈도우 키 입력 콜백함수
void processInput(GLFWwindow *window)
{
  // ESC 키 입력 시 렌더링 루프 및 프로그램 종료
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
  {
    glfwSetWindowShouldClose(window, true);
  }
}

// GLFW 윈도우 resizing 콜백함수
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
  glViewport(0, 0, width, height);
}
