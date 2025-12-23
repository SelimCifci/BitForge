#include <glad/glad.h>
#include <glfw/glfw3.h>

#include <shader.hpp>
#include <camera.hpp>
#include <model.hpp>
#include <framebuffer.hpp>
#include <bitforge.hpp>

#include <ostream>
#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void process_input(GLFWwindow *window);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

// settings
unsigned int scr_width = 600;
unsigned int scr_height = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = scr_width / 2.0f;
float lastY = scr_height / 2.0f;
bool first_mouse = true;
bool mouse_locked = true;

// timing
float delta_time = 0.0f;
float last_frame = 0.0f;

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(scr_width, scr_height, "BitForge", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    stbi_set_flip_vertically_on_load(true);

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glEnable(GL_CULL_FACE);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_FRAMEBUFFER_SRGB);

    glDepthFunc(GL_LEQUAL);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glCullFace(GL_BACK);

    // build and compile shaders
    // -------------------------
    Shader object_shader("default");
    Shader light_shader("light");

    // main framebuffer
    Framebuffer framebuffer(scr_width, scr_height, "framebuffer", {
        -1.0f,  1.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 0.0f,
        1.0f, -1.0f, 1.0f, 0.0f,
        1.0f,  1.0f, 1.0f, 1.0f
    });

    glfwSetWindowUserPointer(window, &framebuffer);

    // load models
    // -----------
    Model backpack_model("backpack");
    Model light_model("sphere");
    Model cube_model("cube");

    //run_starts();

    // DEBUG: draw in wireframe
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        delta_time = currentFrame - last_frame;
        last_frame = currentFrame;

        framebuffer.width = scr_width;
        framebuffer.height = scr_height;

        // input
        // -----
        process_input(window);

        //run_updates();

        framebuffer.bind();

        // render
        // ------
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)scr_width / (float)scr_height, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();

        // render "sun"
        light_shader.use();
        light_shader.setMat4("projection", projection);
        light_shader.setMat4("view", view);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.7f, 0.2f, 2.0f));
        model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
        light_shader.setMat4("model", model);
        light_model.draw(light_shader);

        // don't forget to enable shader before setting uniforms
        object_shader.use();
        object_shader.setVec3("viewPos", camera.Position);
        object_shader.setFloat("material.shininess", 64.0f);

        // directional light
        object_shader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
        object_shader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
        object_shader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
        object_shader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);

        // point light
        object_shader.setVec3("pointLights[0].position", 0.7f, 0.2f, 2.0f);
        object_shader.setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
        object_shader.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
        object_shader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
        object_shader.setFloat("pointLights[0].constant", 1.0f);
        object_shader.setFloat("pointLights[0].linear", 0.09f);
        object_shader.setFloat("pointLights[0].quadratic", 0.032f);

        // flashlight
        object_shader.setVec3("spotLight.position", camera.Position);
        object_shader.setVec3("spotLight.direction", camera.Front);
        object_shader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
        object_shader.setVec3("spoGL_CULL_FACEtLight.diffuse", 1.0f, 1.0f, 1.0f);
        object_shader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
        object_shader.setFloat("spotLight.constant", 1.0f);
        object_shader.setFloat("spotLight.linear", 0.09f);
        object_shader.setFloat("spotLight.quadratic", 0.032f);
        object_shader.setFloat("spotLight.cutOff", glm::cos(glm::radians(0.0f)));
        object_shader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(0.0f)));

        // view/projection transformations
        object_shader.setMat4("projection", projection);
        object_shader.setMat4("view", view);

        // render the loaded model
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
        object_shader.setMat4("model", model);
        backpack_model.draw(object_shader);
        model = glm::translate(model, glm::vec3(10.0f, 10.0f, 10.0f));
        object_shader.setMat4("model", model);
        //draw_all(object_shader);

        framebuffer.draw();

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    framebuffer.cleanUp();

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

void process_input(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        mouse_locked = false;
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        glfwSetCursorPosCallback(window, NULL);
        glfwSetScrollCallback(window, NULL);
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, delta_time);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, delta_time);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, delta_time);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, delta_time);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
    Framebuffer* fb = static_cast<Framebuffer*>(glfwGetWindowUserPointer(window));
    if (fb) fb->resize(width, height);

    scr_width = width;
    scr_height = height;
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (first_mouse)
    {
        lastX = xpos;
        lastY = ypos;
        first_mouse = false;
    }

    if (!mouse_locked)
    {
        lastX = xpos;
        lastY = ypos;
        mouse_locked = true;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && !mouse_locked)
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        glfwSetCursorPosCallback(window, mouse_callback);
        glfwSetScrollCallback(window, scroll_callback);
    }
}
