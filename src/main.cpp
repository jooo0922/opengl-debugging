#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <shader/shader.hpp>

#include <iostream>
#include <string>

/** 콜백함수 전방 선언 */

// GLFW 윈도우 resizing 콜백함수
void framebuffer_size_callback(GLFWwindow *window, int width, int height);

// GLFW 윈도우 키 입력 콜백함수
void processInput(GLFWwindow *window);

/** 스크린 해상도 선언 */
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

/**
 * glGetError() 를 wrapping 하여 에러를 출력하는 함수를 매크로 전처리기로 정의
 *
 * 참고로, __FILE__와 __LINE__ 는 C++ 에 정의된 Predefined Macro 라고 보면 됨.
 * 컴파일러가 해당 매크로들을 각각 '현재 소스 파일 경로'와 '소스 파일 내에서 현재 매크로가 사용된 라인 번호' 로 치환함.
 *
 * 즉, 에러가 발생한 소스 파일과 라인 번호를 출력하기 위해 전달하는 파라미터
 */
GLenum glCheckError_(const char *file, int line)
{
  /**
   * X11 같은 분산형 시스템에서는 동시에 여러 error flags 가 설정되므로,
   * 모든 error flags 를 확인 및 초기화하려면, GL_NO_ERROR 가 반환될 때까지
   * loop 를 돌려서 초기화해야 함.
   */
  GLenum errorCode;
  while ((errorCode = glGetError() != GL_NO_ERROR))
  {
    // 숫자로 반환되는 error code 를 문자열로 변환하여 출력
    std::string error;
    switch (errorCode)
    {
    case GL_INVALID_ENUM:
      error = "INVALID_ENUM";
      break;
    case GL_INVALID_VALUE:
      error = "INVALID_VALUE";
      break;
    case GL_INVALID_OPERATION:
      error = "INVALID_OPERATION";
      break;
    case GL_STACK_OVERFLOW:
      error = "STACK_OVERFLOW";
      break;
    case GL_STACK_UNDERFLOW:
      error = "STACK_UNDERFLOW";
      break;
    case GL_OUT_OF_MEMORY:
      error = "OUT_OF_MEMORY";
      break;
    case GL_INVALID_FRAMEBUFFER_OPERATION:
      error = "INVALID_FRAMEBUFFER_OPERATION";
      break;
    }
    std::cout << error << " | " << file << " (" << line << ")" << std::endl;
  }
  return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)

/**
 * OpenGL Debug Output Context 에 전달할 콜백함수 정의
 *
 * Debug Output Context 는 glGetError() 보다 더 많은 에러 정보들을
 * 콜백 함수의 인자들로부터 전달받아서 개발자의 입맛에 맞게 가공해서 출력할 수 있음.
 *
 * 이때, 해당 콜백함수를 호출하는 주체는 OpenGL 그래픽 드라이버이기 때문에,
 * 그래픽 드라이버가 호출하는 함수를 선언할 때에는 항상 '호출 규약' 을 명시해야 함.
 *
 * '호출 규약' 이란,
 * 함수가 호출될 때 매개변수를 어떻게 전달하고 반환값을 어떻게 처리하는지에 대한
 * 규칙을 정의하는 개념으로,
 *
 * 아래의 APIENTRY 매크로는 OpenGL 이 어떤 플랫폼에서 컴파일되던지
 * 콜백 함수의 호출 규약을 동일하게 유지하기 위해서 선언한 것이라고 보면 됨.
 * 실제로 Windows 에서 해당 매크로는 __stdcall(Windows API 에서 널리 사용되는 표준 호출 규약)
 * 으로 컴파일됨.
 */
void APIENTRY glDebugOutput(GLenum source,
                            GLenum type,
                            unsigned int id,
                            GLenum severity,
                            GLsizei length,
                            const char *message,
                            const void *userParam)
{
  // 상대적으로 덜 중요한 에러 코드들(ex> 131185 : '버퍼 생성 성공'을 의미)은 무시
  if (id == 131169 || id == 131185 || id == 131218 || id == 131204)
    return;

  std::cout << "---------------" << std::endl;
  std::cout << "Debug message (" << id << "): " << message << std::endl;

  // 에러 발생 출처 출력
  switch (source)
  {
  case GL_DEBUG_SOURCE_API:
    std::cout << "Source: API";
    break;
  case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
    std::cout << "Source: Window System";
    break;
  case GL_DEBUG_SOURCE_SHADER_COMPILER:
    std::cout << "Source: Shader Compiler";
    break;
  case GL_DEBUG_SOURCE_THIRD_PARTY:
    std::cout << "Source: Third Party";
    break;
  case GL_DEBUG_SOURCE_APPLICATION:
    std::cout << "Source: Application";
    break;
  case GL_DEBUG_SOURCE_OTHER:
    std::cout << "Source: Other";
    break;
  }
  std::cout << std::endl;

  // 에러 타입 출력
  switch (type)
  {
  case GL_DEBUG_TYPE_ERROR:
    std::cout << "Type: Error";
    break;
  case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
    std::cout << "Type: Deprecated Behaviour";
    break;
  case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
    std::cout << "Type: Undefined Behaviour";
    break;
  case GL_DEBUG_TYPE_PORTABILITY:
    std::cout << "Type: Portability";
    break;
  case GL_DEBUG_TYPE_PERFORMANCE:
    std::cout << "Type: Performance";
    break;
  case GL_DEBUG_TYPE_MARKER:
    std::cout << "Type: Marker";
    break;
  case GL_DEBUG_TYPE_PUSH_GROUP:
    std::cout << "Type: Push Group";
    break;
  case GL_DEBUG_TYPE_POP_GROUP:
    std::cout << "Type: Pop Group";
    break;
  case GL_DEBUG_TYPE_OTHER:
    std::cout << "Type: Other";
    break;
  }
  std::cout << std::endl;

  // 에러 심각도 출력
  switch (severity)
  {
  case GL_DEBUG_SEVERITY_HIGH:
    std::cout << "Severity: high";
    break;
  case GL_DEBUG_SEVERITY_MEDIUM:
    std::cout << "Severity: medium";
    break;
  case GL_DEBUG_SEVERITY_LOW:
    std::cout << "Severity: low";
    break;
  case GL_DEBUG_SEVERITY_NOTIFICATION:
    std::cout << "Severity: notification";
    break;
  }
  std::cout << std::endl;
}

int main()
{
  // GLFW 초기화 및 윈도우 설정 구성
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // debug output context 사용 시, GLFW 같은 windowing system 에 debug context 를 사용할 것임을 요청해야 함.
  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);

// 현재 운영체제가 macos 일 경우, 미래 버전의 OpenGL 을 사용해서 GLFW 창을 생성하여 버전 호환성 이슈 해결
#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

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

  // debug output context 가 성공적으로 초기화 되었는지 query 하기
  int flags;
  glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
  if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
  {
    // debug output context 가 성공적으로 초기화 되었다면, GL_DEBUG_OUTPUT 을 활성화함.
    glEnable(GL_DEBUG_OUTPUT);

    // debug output context 에 등록한 콜백함수를 '동기적 방식'으로 호출 (하단 필기 참고)
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

    // debug output context 에 콜백함수 등록
    glDebugMessageCallback(glDebugOutput, nullptr);

    // 받고 싶은 debug output 만 필터링할 수 있는 API -> 아래와 같이 설정하면 별도로 필터링하지 않겠다는 의미.
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
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
    glTexImage2D(GL_FRAMEBUFFER, 0, nrComponents, width, height, 0, nrComponents, GL_UNSIGNED_BYTE, data); // error version
    // glTexImage2D(GL_TEXTURE_2D, 0, nrComponents, width, height, 0, nrComponents, GL_UNSIGNED_BYTE, data);
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

  /** rendering loop */
  while (!glfwWindowShouldClose(window))
  {
    processInput(window);

    // 버퍼 초기화
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // 쉐이더 바인딩
    shader.use();

    // model matrix 계산 및 쉐이더 전송
    float rotationSpeed = 10.0f;
    float angle = (float)glfwGetTime() * rotationSpeed;
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, -2.5f));
    model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 1.0f, 1.0f));

    // draw call
    glBindTexture(GL_TEXTURE_2D, texture);
    glBindVertexArray(cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);

    // Back 버퍼에 렌더링된 최종 이미지를 Front 버퍼에 교체 -> blinking 현상 방지
    glfwSwapBuffers(window);

    // 키보드, 마우스 입력 이벤트 발생 검사 후 등록된 콜백함수 호출 + 이벤트 발생에 따른 GLFWwindow 상태 업데이트
    glfwPollEvents();
  }

  // GLFW 종료 및 메모리 반납
  glfwTerminate();

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

/**
 * glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS)
 *
 *
 * debug output context 에 등록하는 콜백함수를 '동기적으로' 호출하도록 설정함.
 * 즉, 에러가 발생하는 순간마다 즉시 콜백함수를 호출함.
 *
 * 반대로 '비동기(asynchronous)' 방식으로 디버깅 출력 시,
 * 에러가 발생할 때마다 디버그 메시지를 버퍼에 저장해두었다가,
 * '나중에' 한꺼번에 콜백함수를 호출함.
 *
 * 이렇게 했을 때의 차이점은,
 * '동기적 호출' 은 에러가 발생할 때마다 그때그때 콜백함수를 호출함으로써
 * 에러 발생 시점을 정확하게 파악할 수 있다는 장점이 있으나, 이 콜백함수가 호출되는 시점마다
 * 디버깅 메시지가 처리될 때까지 OpenGL 이 기다리게 되어 성능 저하가 발생할 수 있음.
 *
 * 반대로, '비동기적 호출' 은
 * OpenGL 커맨드를 우선적으로 실행하고, 에러 메시지를 일정 시점에
 * '일괄적으로' 호출하기 때문에, OpenGL 실행 흐름을 방해하지 않아 퍼포먼스 측면에서 유리함.
 * 그러나, 에러 발생 시점과 메시지 호출 시점에 차이가 생겨 정확한 디버깅이 어려울 수 있음.
 */
