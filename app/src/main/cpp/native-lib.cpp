#include <jni.h>
#include <string>
#include "android/log.h"
#include <GLES2/gl2.h>
#include "../../../glm/glm.hpp"
#include "../../../glm/gtc/matrix_transform.hpp"

#define  LOG_TAG    "libgl2jni"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

float _width;
float _height;

GLuint program;
const char *vertexShader =
        "attribute vec4 vPosition;\n"
                "attribute vec4 a_color;\n"
                "uniform mat4 transM;"
                "uniform mat4 transV;"
                "uniform mat4 transP;"
                "varying vec4 v_fragmentColor;\n"
                "void main() {\n"
                "  gl_Position = transP * transV * transM * vPosition;\n"
                "  v_fragmentColor = a_color;\n"
                "}\n";
const char *fragmentShader =
        "precision mediump float;\n"
                "varying vec4 v_fragmentColor;\n"
                "void main() {\n"
                "  gl_FragColor = v_fragmentColor;;\n"
                "}\n";

GLint gvPositionHandle;
GLint gvColorHandle;
GLint gvTransMHandle;
GLint gvTransVHandle;
GLint gvTransPHandle;
GLuint verVbo;


float radius;
const float vertexBuffer[] = {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        0.0f, 0.5f, 0.0f
};
const float colorBuffer[] = {
        1.0f, 0.0f, 0.0f,  //Red
        0.0f, 1.0f, 0.0f,  //Green
        0.0f, 0.0f, 1.0f  //Blue
};


GLuint loadShader(GLenum shaderType, const char *pSource) {
    GLuint shader = glCreateShader(shaderType);
    if (shader) {
        glShaderSource(shader, 1, &pSource, NULL);
        glCompileShader(shader);
        GLint compiled = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
        if (!compiled) {
            GLint infoLen = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
            if (infoLen) {
                char *buf = (char *) malloc(infoLen);
                if (buf) {
                    glGetShaderInfoLog(shader, infoLen, NULL, buf);
                    LOGE("Could not compile shader %d:\n%s\n",
                         shaderType, buf);
                    free(buf);
                }
                glDeleteShader(shader);
                shader = 0;
            }
        }
    }
    return shader;
}

bool createProgram(const char *pVertexShader, const char *pFragmentShader) {
    GLuint vertexShader = loadShader(GL_VERTEX_SHADER, pVertexShader);
    if (!vertexShader) {
        return false;
    }

    GLuint pixelShader = loadShader(GL_FRAGMENT_SHADER, pFragmentShader);
    if (!pixelShader) {
        return false;
    }

    program = glCreateProgram();
    if (program) {
        glAttachShader(program, vertexShader);
//        checkGlError("glAttachShader");
        glAttachShader(program, pixelShader);
//        checkGlError("glAttachShader");
        glLinkProgram(program);
        GLint linkStatus = GL_FALSE;
        glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
        if (linkStatus != GL_TRUE) {
            GLint bufLength = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufLength);
            if (bufLength) {
                char *buf = (char *) malloc(bufLength);
                if (buf) {
                    glGetProgramInfoLog(program, bufLength, NULL, buf);
                    LOGE("Could not link program:\n%s\n", buf);
                    free(buf);
                }
            }
            glDeleteProgram(program);
            program = 0;
        }
        return true;
    }
    return false;
}

bool init(int width, int height) {
    _width = width;
    _height = height;

    bool success = createProgram(vertexShader, fragmentShader);
    if (success) {
        glViewport(0, 0, width, height);

        gvPositionHandle = glGetAttribLocation(program, "vPosition");
        gvColorHandle = glGetAttribLocation(program, "a_color");
        gvTransMHandle = glGetUniformLocation(program, "transM");
        gvTransVHandle = glGetUniformLocation(program, "transV");
        gvTransPHandle = glGetUniformLocation(program, "transP");

        LOGI("glGetAttribLocation(\"vPosition\") = %d\n",
             gvPositionHandle);


        glGenBuffers(1, &verVbo);
        glBindBuffer(GL_ARRAY_BUFFER, verVbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertexBuffer), vertexBuffer, GL_STATIC_DRAW);
        glVertexAttribPointer(gvPositionHandle, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid *) 0);

        glEnableVertexAttribArray(gvPositionHandle);

        GLuint vbo;
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(colorBuffer), colorBuffer, GL_STATIC_DRAW);

        glVertexAttribPointer(gvColorHandle, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid *) 0);


        glEnableVertexAttribArray(gvColorHandle);

        return true;
    }

    return false;
}

void render() {
    static float grey = 1.0f;

    //Project matrix
    glm::mat4 Projection = glm::perspective(45.0f * 3.14f / 180.0f, _width / _height, 0.1f,
                                            100.0f);


    // Camera matrix
    glm::mat4 View = glm::lookAt(
            glm::vec3(0.0f, 0.0f, 3.0f), // Camera is at (0,0,3), in World Space
            glm::vec3(0.0f, 0.0f, 0.0f), // and looks at the origin
            glm::vec3(0.0f, 1.0f, 0.0f)  // Head is up (set to 0,-1,0 to look upside-down)
    );


    glm::mat4 Model = glm::rotate(glm::mat4(1.0f), radius, glm::vec3(0, 1, 0));
    radius += 0.1;

//    glm::mat4 MVP = Projection * View * Model;

    glUniformMatrix4fv(gvTransMHandle, 1, GL_FALSE, &Model[0][0]);
    glUniformMatrix4fv(gvTransVHandle, 1, GL_FALSE, &View[0][0]);
    glUniformMatrix4fv(gvTransPHandle, 1, GL_FALSE, &Projection[0][0]);

    glClearColor(grey, grey, grey, 1.0f);

    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);


    glUseProgram(program);

    glBindBuffer(GL_ARRAY_BUFFER, verVbo);

    glDrawArrays(GL_TRIANGLES, 0, 3);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

}

extern "C"
{
JNIEXPORT jstring JNICALL
Java_com_leilei_androidglm_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}


JNIEXPORT jboolean JNICALL
Java_com_leilei_androidglm_GLNative_init(
        JNIEnv *env, jclass, int width, int height) {

    return init(width, height);
}

JNIEXPORT void JNICALL
Java_com_leilei_androidglm_GLNative_render(
        JNIEnv *env, jclass) {
    render();
}
}

