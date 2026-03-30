//import tmui;
//import glm;

#include "window/Window.h"
#include "component/Rectangle.h"
#include "component/Triangle.h"

#include <memory>
#include <windows.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

constexpr int WIDTH = 1000;
constexpr int HEIGHT = 800;

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
void renderText(FT_Face face, unsigned int shader, std::string text, float x, float y, float scale, glm::vec3 color) {

    glViewport(0, 0, WIDTH, HEIGHT);
    int lastShader;
    int lastVAO;
    glGetIntegerv(GL_CURRENT_PROGRAM, &lastShader);
    glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &lastVAO);

    glUseProgram(shader);
    glUniform3f(glGetUniformLocation(shader, "textColor"), color.x, color.y, color.z);
    glActiveTexture(GL_TEXTURE0);

    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
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

        float xpos = x + ch.Bearing.x * scale;
        float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;
        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;

        float vertices[6][4] = {
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
    glBindTexture(GL_TEXTURE_2D, 0);

    glUseProgram(lastShader);
    glBindVertexArray(lastVAO);

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}






using glm::vec2;

int main() {
	
	TM::Window window(WIDTH, HEIGHT, "glwindow");
	
	auto rect1 = std::make_unique<TM::Rectangle>(0, 0, 500, 400, window, 0xFF0000FF);
	auto rect2 = std::make_unique<TM::Rectangle>(100, 20, 20, 300, window, 0x00FF00FF);

	auto tri1 = std::make_unique<TM::Triangle>(
		vec2( 0, 300 ), vec2( 400, 100 ), vec2( 800, 200 ),
		window, 0xFF00FFFF
	);

	auto rect3 = std::make_unique<TM::Rectangle>(100, 50, 20, 30, window, 0x0000FFFF);
	auto rect4 = std::make_unique<TM::Rectangle>(600, 20, 20, 30, window, 0xFFFFFFFF);
	auto rect5 = std::make_unique<TM::Rectangle>(200, 20, 20, 30, window, 0xFF00FFFF);





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
    glm::mat4 projection = glm::ortho(0.0f, (float)WIDTH, 0.0f, (float)HEIGHT);
    glUseProgram(shaderProgram);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));





	window.bind();
	while (!window.closed()) {
		window.clear();

		window.draw(rect1.get());
		window.draw(tri1.get());
		window.draw(rect2.get());
		window.draw(rect3.get());
		window.draw(rect4.get());
		window.draw(rect5.get());

        TM_println("main loop");

        renderText(face, shaderProgram, u8"Hello 你好 FreeType + OpenGL!", 25.0f, 550.0f, 1.0f, glm::vec3(0.5f, 0.8f, 0.2f));
        renderText(face, shaderProgram, "This is a test text.", 25.0f, 480.0f, 0.8f, glm::vec3(0.3f, 0.7f, 0.9f));

		window.swapBuffers();
	}

	return 0;
}