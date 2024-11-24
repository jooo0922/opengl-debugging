#version 330 core

// 프래그먼트 쉐이더 최종 색상 출력 변수 선언
out vec4 FragColor;

// 버텍스 쉐이더에서 보간되어 전송된 uv 좌표값 입력 변수 선언
in vec2 TexCoords;

// 텍스쳐 샘플러 변수 선언
uniform sampler2D tex;

void main() {
  // 텍스쳐 샘플링하여 적용
  FragColor = texture(tex, TexCoords);
}
