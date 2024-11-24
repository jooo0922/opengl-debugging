#version 330 core

// 버퍼로 전달할 각 vertex attribute 의 location 선언.
layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoords;

// 변환행렬 관련 uniform 변수 선언
uniform mat4 projection;
uniform mat4 model;

// 프래그먼트 쉐이더로 출력할 uv 출력 변수 선언
out vec2 TexCoords;

void main() {
  gl_Position = projection * model * vec4(position, 1.0);
  TexCoords = texCoords;
}
