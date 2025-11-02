#version 450

layout(location = 0) out vec4 outColor;

void main() {
    float result = 0.0;
    for(int i = 0; i < 10000; i++) {
        result += sin(float(i) * 0.001) * cos(float(i) * 0.001) * tan(float(i) * 0.0001);
        result = sqrt(abs(result + 0.1));
    }
    outColor = vec4(1.0, 1.0, 1.0, 1.0);
}