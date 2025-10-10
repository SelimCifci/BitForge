#ifndef RENDERER_H
#define RENDERER_H

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include <shader.hpp>
#include <camera.hpp>

#include <iostream>
#include <vector>

class Renderer
{
public:
    // CLASS INSTANCES
    GLFWwindow* window;
    Shader shader;
    Camera camera{glm::vec3(0.0f, 0.0f, 0.0f)};

    // WINDOW DETAILS
    unsigned int scr_width, scr_height;
    const char* app_name;

    // BUFFERS
    unsigned int VBO, VAO;

    // TEXTURES
    unsigned int texture;

    // RUNTIME VARIABLES
    float lastx;
    float lasty;
    bool first_mouse = true;

    // timing
    float delta_time = 0.0f;
    float last_frame = 0.0f;

    // to render
    std::vector<float> vertices;
    std::vector<glm::vec3> cube_positions;

    Renderer(unsigned int width, unsigned int height, const char* name)
    {
        scr_width = width;
        scr_height = height;
        app_name = name;
    }

    void init()
    {
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHintString(GLFW_WAYLAND_APP_ID, "BitForge");

    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

        window = glfwCreateWindow(scr_width, scr_height, app_name, NULL, NULL);
        if (window == NULL)
        {
            std::cout << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
        }
        glfwMakeContextCurrent(window);
        glfwSetWindowUserPointer(window, this);

        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
        glfwSetCursorPosCallback(window, mouse_callback_static);
        glfwSetScrollCallback(window, scroll_callback_static);

        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            std::cout << "Failed to initialize GLAD" << std::endl;
        }

        // configure global opengl state
        glEnable(GL_DEPTH_TEST);

        shader = Shader("resources/shaders/default.vert", "resources/shaders/default.frag");

        // set up vertex data (and buffer(s)) and configure vertex attributes
        vertices = {
            -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
            0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
            0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
            0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
            0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
            0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
            0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
            -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

            -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
            -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
            -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

            0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
            0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
            0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
            0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
            0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
            0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
            0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
            0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
            0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
            0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
            0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
            0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
            -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
        };

        // world space positions of our cubes
        cube_positions = {
            glm::vec3( 0.0f,  0.0f,  0.0f),
            glm::vec3( 2.0f,  5.0f, -15.0f),
            glm::vec3(-1.5f, -2.2f, -2.5f),
            glm::vec3(-3.8f, -2.0f, -12.3f),
            glm::vec3( 2.4f, -0.4f, -3.5f),
            glm::vec3(-1.7f,  3.0f, -7.5f),
            glm::vec3( 1.3f, -2.0f, -2.5f),
            glm::vec3( 1.5f,  2.0f, -2.5f),
            glm::vec3( 1.5f,  0.2f, -1.5f),
            glm::vec3(-1.3f,  1.0f, -1.5f)
        };

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);
        
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

        // position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        // texture coord attribute
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        // texture
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        // set the texture wrapping parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        // set texture filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // load image, create texture and generate mipmaps
        int width, height, nrChannels;
        stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
        unsigned char *data = stbi_load("resources/textures/arch.jpg", &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else
        {
            std::cout << "Failed to load texture" << std::endl;
        }
        stbi_image_free(data);

        // tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
        shader.use();
    }

    void update()
    {
        // per-frame time logic
        float currentFrame = static_cast<float>(glfwGetTime());
        delta_time = currentFrame - last_frame;
        last_frame = currentFrame;

        // input
        process_input(window);

        // render
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

        // bind textures on corresponding texture units
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);

        // activate shader
        shader.use();

        // pass projection matrix to shader (note that in this case it could change every frame)
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)scr_width / (float)scr_height, 0.1f, 100.0f);
        shader.setMat4("projection", projection);

        // camera/view transformation
        glm::mat4 view = camera.GetViewMatrix();
        shader.setMat4("view", view);

        // render boxes
        glBindVertexArray(VAO);
        for (unsigned int i = 0; i < 10; i++)
        {
            // calculate the model matrix for each object and pass it to shader before drawing
            glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
            model = glm::translate(model, cube_positions[i]);
            float angle = 20.0f * i;
            model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            shader.setMat4("model", model);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    void terminate()
    {
        // optional: de-allocate all resources once they've outlived their purpose:
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);

        // glfw: terminate, clearing all previously allocated GLFW resources.
        glfwTerminate();
    }

    int getRunStatus()
    {
        return !glfwWindowShouldClose(window);
    }

private:
    void process_input(GLFWwindow *window)
    {
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            camera.ProcessKeyboard(FORWARD, delta_time);
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            camera.ProcessKeyboard(BACKWARD, delta_time);
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            camera.ProcessKeyboard(LEFT, delta_time);
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            camera.ProcessKeyboard(RIGHT, delta_time);
    }

    // glfw: whenever the window size changed (by OS or user resize) this callback function executes
    static void framebuffer_size_callback(GLFWwindow* window, int width, int height)
    {
        // make sure the viewport matches the new window dimensions; note that width and 
        // height will be significantly larger than specified on retina displays.
        glViewport(0, 0, width, height);
    }

    void mouse_callback(double xposIn, double yposIn)
    {
        float xpos = static_cast<float>(xposIn);
        float ypos = static_cast<float>(yposIn);

        if (first_mouse)
        {
            lastx = xpos;
            lasty = ypos;
            first_mouse = false;
        }

        float xoffset = xpos - lastx;
        float yoffset = lasty - ypos; // reversed since y-coordinates go from bottom to top

        lastx = xpos;
        lasty = ypos;

        camera.ProcessMouseMovement(xoffset, yoffset);
    }

    // glfw: whenever the mouse moves, this callback is called
    static void mouse_callback_static(GLFWwindow* window, double xposIn, double yposIn)
    {
        static_cast<Renderer*>(glfwGetWindowUserPointer(window))->mouse_callback(xposIn, yposIn);
    }

    void scroll_callback(double xoffset, double yoffset)
    {
        camera.ProcessMouseScroll(static_cast<float>(yoffset));
    }

    // glfw: whenever the mouse scroll wheel scrolls, this callback is called
    static void scroll_callback_static(GLFWwindow* window, double xoffset, double yoffset)
    {
        static_cast<Renderer*>(glfwGetWindowUserPointer(window))->scroll_callback(xoffset, yoffset);
    }
};

#endif