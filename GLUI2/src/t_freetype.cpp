#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <unordered_map>
#include <string>
#include <vector>

// 字符信息结构体
struct Character {
    unsigned int TextureID;   // 字形纹理ID
    glm::ivec2 Size;    // 字形尺寸
    glm::ivec2 Bearing; // 字形相对于基线的偏移
    unsigned int Advance;     // 下一个字形的偏移量
};

// 全局字符集映射
std::unordered_map<uint32_t, Character> characters;

// 着色器源码
const char* vertexShaderSource = R"(
    #version 330 core
    layout (location = 0) in vec4 vertex; // <vec2 pos, vec2 tex>
    out vec2 TexCoords;
    
    uniform mat4 projection;

    void main() {
        gl_Position = projection * vec4(vertex.xy, 0.0, 1.0);
        TexCoords = vertex.zw;
    }
)";

const char* fragmentShaderSource = R"(
    #version 330 core
    in vec2 TexCoords;
    out vec4 color;
    
    uniform sampler2D text;
    uniform vec3 textColor;
    
    void main() {
        vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);
        color = vec4(textColor, 1.0) * sampled;
    }
)";

std::vector<uint32_t> utf8_to_unicode(const std::string& utf8_str) {
    std::vector<uint32_t> unicode_points;
    const char* ptr = utf8_str.c_str();
    while (*ptr) {
        uint32_t code = 0;
        if ((*ptr & 0x80) == 0) { // 1字节 (0xxxxxxx)
            code = *ptr++;
        }
        else if ((*ptr & 0xE0) == 0xC0) { // 2字节 (110xxxxx 10xxxxxx)
            code = ((*ptr++ & 0x1F) << 6) | (*ptr++ & 0x3F);
        }
        else if ((*ptr & 0xF0) == 0xE0) { // 3字节 (1110xxxx 10xxxxxx 10xxxxxx)
            code = ((*ptr++ & 0x0F) << 12) | ((*ptr++ & 0x3F) << 6) | (*ptr++ & 0x3F);
        }
        else if ((*ptr & 0xF8) == 0xF0) { // 4字节 (11110xxx 10xxxxxx 10xxxxxx 10xxxxxx)
            code = ((*ptr++ & 0x07) << 18) | ((*ptr++ & 0x3F) << 12) | ((*ptr++ & 0x3F) << 6) | (*ptr++ & 0x3F);
        }
        else {
            ptr++; // 无效UTF-8，跳过
            continue;
        }
        unicode_points.push_back(code);
    }
    return unicode_points;
}

void load_unicode(FT_Face face, uint32_t c) {
    if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
        std::cerr << "Failed to load Glyph!" << std::endl;
    }

    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RED,
        face->glyph->bitmap.width,
        face->glyph->bitmap.rows,
        0,
        GL_RED,
        GL_UNSIGNED_BYTE,
        face->glyph->bitmap.buffer
    );

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    Character character = {
        texture,
        glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
        glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
        static_cast<unsigned int>(face->glyph->advance.x)
    };
    characters.insert(std::pair<uint32_t, Character>(c, character));

}

// 渲染文字函数
void renderText(FT_Face face, unsigned int shader, std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color) {
    glUseProgram(shader);
    glUniform3f(glGetUniformLocation(shader, "textColor"), color.x, color.y, color.z);
    glActiveTexture(GL_TEXTURE0);

    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glBindVertexArray(VAO);
    auto utf8_chars = utf8_to_unicode(text);
    auto c = utf8_chars.begin();
    for (; c != utf8_chars.end(); c++) {
        if (!characters.count(*c)) {
            load_unicode(face, *c);
        }
        Character ch = characters[*c];

        GLfloat xpos = x + ch.Bearing.x * scale;
        GLfloat ypos = y - (ch.Size.y - ch.Bearing.y) * scale;
        GLfloat w = ch.Size.x * scale;
        GLfloat h = ch.Size.y * scale;

        GLfloat vertices[6][4] = {
            { xpos,     ypos + h,   0.0, 0.0 },
            { xpos,     ypos,       0.0, 1.0 },
            { xpos + w, ypos,       1.0, 1.0 },
            { xpos,     ypos + h,   0.0, 0.0 },
            { xpos + w, ypos,       1.0, 1.0 },
            { xpos + w, ypos + h,   1.0, 0.0 }
        };

        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        x += (ch.Advance >> 6) * scale;
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

int main() {
    // 1. 初始化 GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW!" << std::endl;
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, u8"FreeType 你好 OpenGL Text", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window!" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // 2. 初始化 GLEW
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW!" << std::endl;
        return -1;
    }

    // 3. 配置 OpenGL
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // 4. 编译着色器
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // 5. 初始化 FreeType
    FT_Library ft;
    if (FT_Init_FreeType(&ft)) {
        std::cerr << "Failed to initialize FreeType!" << std::endl;
        return -1;
    }

    FT_Face face;
    // 请替换为你系统中的实际字体路径
    const char* fontPath = "C:/Windows/Fonts/msyh.ttc"; // 微软雅黑
    if (FT_New_Face(ft, fontPath, 0, &face)) {
        std::cerr << "Failed to load font!" << std::endl;
        return -1;
    }

    FT_Set_Pixel_Sizes(face, 0, 48); // 设置字体大小
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // 禁用字节对齐限制

    // 7. 设置投影矩阵
    glm::mat4 projection = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f);
    glUseProgram(shaderProgram);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    // 8. 主循环
    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // 渲染文字
        renderText(face, shaderProgram, u8"Hello 你好 FreeType + OpenGL!", 25.0f, 550.0f, 1.0f, glm::vec3(0.5f, 0.8f, 0.2f));
        renderText(face, shaderProgram, "This is a test text.", 25.0f, 480.0f, 0.8f, glm::vec3(0.3f, 0.7f, 0.9f));

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // 9. 清理资源
    FT_Done_Face(face);
    FT_Done_FreeType(ft);
    for (auto& c : characters) {
        glDeleteTextures(1, &c.second.TextureID);
    }
    glDeleteProgram(shaderProgram);
    glfwTerminate();

    return 0;
}