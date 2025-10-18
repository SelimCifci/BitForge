#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/detail/qualifier.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/glm.hpp>

#include <shader.hpp>
#include <camera.hpp>
#include <model.hpp>

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void process_input(GLFWwindow *window);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

// settings
const unsigned int SCR_WIDTH = 600;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool first_mouse = true;

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
    glfwWindowHintString(GLFW_WAYLAND_APP_ID, "BitForge");

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "BitForge", NULL, NULL);
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
    Shader object_shader("resources/shaders/default.vert", "resources/shaders/default.frag");
    Shader light_shader("resources/shaders/light.vert", "resources/shaders/light.frag");

    // load models
    // -----------
    Model backpack_model("resources/models/backpack/backpack.obj");
    Model light_model("resources/models/sphere/sphere.obj");
    Model cube_model("resources/models/cube/cube.obj");
    
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

        // input
        // -----
        process_input(window);

        // render
        // ------
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();

        // render "sun"
        light_shader.use();
        light_shader.setMat4("projection", projection);
        light_shader.setMat4("view", view);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.7f, 0.2f, 2.0f));
        model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
        light_shader.setMat4("model", model);
        light_model.Draw(light_shader);

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
        object_shader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
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
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
        object_shader.setMat4("model", model);
        backpack_model.Draw(object_shader);

        // render transparent models
        glDisable(GL_CULL_FACE);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(4.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
        model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.0f, 1.0f, 0.0f));
        object_shader.setMat4("model", model);
        cube_model.Draw(object_shader);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -5.0f, 0.0f));
        model = glm::scale(model, glm::vec3(10.0f, 1.0f, 10.0f));
        object_shader.setMat4("model", model);
        cube_model.Draw(object_shader);

        glEnable(GL_CULL_FACE);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

void process_input(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
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

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

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
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        glfwSetCursorPosCallback(window, mouse_callback);
        glfwSetScrollCallback(window, scroll_callback);
    }
}
