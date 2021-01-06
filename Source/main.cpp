#include <iostream>
#include "parser.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "definitions.h"
#include "utility.h"

//////-------- Global Variables -------/////////

GLuint gpuVertexBuffer;
GLuint gpuNormalBuffer;
GLuint gpuIndexBuffer;

// Sample usage for reading an XML scene file
parser::Scene scene;
static GLFWwindow *win = NULL;

static void errorCallback(int error, const char *description)
{
    fprintf(stderr, "Error: %s\n", description);
}

// My code starts here.

Utility util;
// For FPS timings.
double lastTime;
int nbFrames;

void ApplyTransformation(const Transformation &transformation)
{
    const char type = transformation.transformation_type[0];
    switch (type)
    {
    case 'S':
    {
        const Vec3f &scaling = scene.scalings[transformation.id - 1];
        glScalef(scaling.x, scaling.y, scaling.z);
        break;
    }
    case 'R':
    {
        const Vec4f &rotation = scene.rotations[transformation.id - 1];
        glRotatef(rotation.x, rotation.y, rotation.z, rotation.w);
        break;
    }
    case 'T':
    {
        const Vec3f &translation = scene.translations[transformation.id - 1];
        glTranslatef(translation.x, translation.y, translation.z);
        break;
    }
    default:
    {
        std::cerr << "This shouldn't happen, at all." << std::endl;
        break;
    }
    }
}

void Display()
{
    static bool first_time = true;
    static size_t vertices_in_bytes = 0;

    if (first_time)
    {
        first_time = false;

        // Get vertex data.
        GLuint vertex_buffer;
        const std::vector<Vec3f> vertex_data = scene.vertex_data;
        vertices_in_bytes = vertex_data.size() * 3 * sizeof(GLfloat);
        const size_t vertex_count = vertex_data.size();
        std::vector<Vec3f> normal_data(vertex_count);
        std::vector<size_t> normal_count(vertex_count);
        GLfloat *vertex_pos = new GLfloat[vertex_count * 3];
        int v_counter = 0;
        for (const Vec3f &vertex : vertex_data)
        {
            vertex_pos[v_counter++] = vertex.x;
            vertex_pos[v_counter++] = vertex.y;
            vertex_pos[v_counter++] = vertex.z;
        }
        // Once all vertex positions are set, proceed to set buffers and clean up vertex_data.
        glGenBuffers(1, &vertex_buffer);
        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
        glBufferData(GL_ARRAY_BUFFER, 2 * vertices_in_bytes, 0, GL_STATIC_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER, 0, vertices_in_bytes, vertex_pos);
        delete[] vertex_pos;
        // Now we can save indices.
        GLuint index_buffer;
        glGenBuffers(1, &index_buffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
        GLuint *indices = new GLuint[scene.face_count * 3];
        GLfloat *vertex_normals = new GLfloat[vertex_data.size() * 3];
        int index_counter = 0;
        const std::vector<Mesh> meshes = scene.meshes;
        for (const Mesh &mesh : meshes)
        {
            const std::vector<Face> faces = mesh.faces;
            for (const Face &face : faces)
            {
                indices[index_counter++] = face.v0_id;
                indices[index_counter++] = face.v1_id;
                indices[index_counter++] = face.v2_id;
                normal_data[face.v0_id] = util.Add(normal_data[face.v0_id], face.normal);
                normal_data[face.v1_id] = util.Add(normal_data[face.v1_id], face.normal);
                normal_data[face.v1_id] = util.Add(normal_data[face.v1_id], face.normal);
                normal_count[face.v0_id]++;
                normal_count[face.v1_id]++;
                normal_count[face.v2_id]++;
            }
        }
        for (size_t i = 0; i < vertex_count; i++)
        {
            const Vec3f normal = util.Normalize(normal_data[i]);
            vertex_normals[3 * i] = normal.x;
            vertex_normals[3 * i + 1] = normal.y;
            vertex_normals[3 * i + 2] = normal.z;
        }
        int indices_in_bytes = scene.face_count * 3 * sizeof(GLuint);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_in_bytes, indices, GL_STATIC_DRAW);
        delete[] indices;
        glBufferSubData(GL_ARRAY_BUFFER, vertices_in_bytes, vertices_in_bytes, vertex_normals);
        delete[] vertex_normals;
    }

    glVertexPointer(3, GL_FLOAT, 0, 0);
    glNormalPointer(GL_FLOAT, 0, (const void *)vertices_in_bytes);
    size_t mesh_face_offset = 0;
    for (const Mesh &mesh : scene.meshes)
    {
        glPushMatrix();
        int transformation_count = mesh.transformations.size();
        // Apply transformations in reverse order!
        for (int i = transformation_count - 1; i >= 0; i--)
        {
            const Transformation &transformation = mesh.transformations[i];
            ApplyTransformation(transformation);
        }
        const Material material = scene.materials[mesh.material_id];
        const GLfloat ambient[4] = {
            material.ambient.x,
            material.ambient.y,
            material.ambient.z,
            1.};
        const GLfloat diffuse[4] = {
            material.diffuse.x,
            material.diffuse.y,
            material.diffuse.z,
            1.};
        const GLfloat specular[4] = {
            material.specular.x,
            material.specular.y,
            material.specular.z,
            1.};
        const GLfloat phong_exponent[1] = {material.phong_exponent};
        glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
        glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
        glMaterialfv(GL_FRONT, GL_SHININESS, phong_exponent);
        if (mesh.mesh_type == "Solid")
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
        else
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }
        glDrawElements(GL_TRIANGLES,
                       mesh.faces.size() * 3,
                       GL_UNSIGNED_INT,
                       (const void *)(mesh_face_offset * 3 * sizeof(GLuint)));
        mesh_face_offset += mesh.faces.size();
        glPopMatrix();
    }
}

static void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

void RenderFrame()
{
    glClearColor(scene.background_color.x, scene.background_color.y, scene.background_color.z, 1);
    glClearDepth(1.0f);
    glClearStencil(0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    Display();
}

void ShowFPS(GLFWwindow *pWindow)
{
    // Measure speed
    double currentTime = glfwGetTime();
    double delta = currentTime - lastTime;
    char ss[500] = {};
    nbFrames++;
    if (delta >= 1.0)
    { // If last cout was more than 1 sec ago
        //cout << 1000.0/double(nbFrames) << endl;

        double fps = ((double)(nbFrames)) / delta;
        sprintf(ss, "CENG477 - HW3 [%1.2f FPS]", fps);

        glfwSetWindowTitle(pWindow, ss);

        nbFrames = 0;
        lastTime = currentTime;
    }
}

// My code ends here.

int main(int argc, char *argv[])
{
    scene.loadFromXml(argv[1]);
    // For debugging purposes:
    util.PrintSceneDetails(scene);
    glfwSetErrorCallback(errorCallback);

    if (!glfwInit())
    {
        exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

    win = glfwCreateWindow(scene.camera.image_width, scene.camera.image_height, "CENG477 - HW3", NULL, NULL);
    if (!win)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(win);
    printf("OpenGL version: %s - %s\n", glGetString(GL_RENDERER), glGetString(GL_VERSION));
    GLenum err = glewInit();
    if (err != GLEW_OK)
    {
        fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
        exit(EXIT_FAILURE);
    }

    glfwSetKeyCallback(win, keyCallback);

    // My code starts here.

    // TODO: Initialize.

    // To prevent circular dependency between parser and utility, I calculate normals here.
    for (Mesh &mesh : scene.meshes)
    {
        mesh.material_id--;
        for (Face &face : mesh.faces)
        {
            face.v0_id--;
            face.v1_id--;
            face.v2_id--;
            scene.face_count++;
            face.normal = util.GetNormal(scene.vertex_data[face.v0_id],
                                         scene.vertex_data[face.v1_id],
                                         scene.vertex_data[face.v2_id]);
        }
    }
    // I don't like nested branches or logical operators in conditions.
    // So I decided to use a switch case here.
    int culling_case = scene.culling_enabled * 2 + scene.culling_face;
    switch (culling_case)
    {
    case 3:
        // Frontface culled
        {
            glEnable(GL_CULL_FACE);
            glCullFace(GL_BACK);
            glFrontFace(GL_CW);
            break;
        }
    case 2:
        // Backface culled
        {
            glEnable(GL_CULL_FACE);
            glCullFace(GL_BACK);
            break;
        }
    default:
        break;
    }
    glEnable(GL_DEPTH_TEST);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnable(GL_LIGHTING);
    glShadeModel(GL_SMOOTH);

    // TODO: Set light sources.
    const GLfloat ambient[4] = {
        scene.ambient_light.x,
        scene.ambient_light.y,
        scene.ambient_light.z,
        1.};
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glEnable(GL_LIGHT0);
    const std::vector<PointLight> point_lights(scene.point_lights);
    int point_light_count = point_lights.size();
    for (size_t i = 0; i < point_light_count; i++)
    {
        const PointLight point_light = point_lights[i];
        const GLfloat light_intensity[4] = {
            point_light.intensity.x,
            point_light.intensity.y,
            point_light.intensity.z, 1.0};
        const GLfloat light_position[4] = {
            point_light.position.x,
            point_light.position.y,
            point_light.position.z, 1.0};
        glLightfv(GL_LIGHT0 + 1 + i, GL_POSITION, light_position);
        glLightfv(GL_LIGHT0 + 1 + i, GL_DIFFUSE, light_intensity);
        glLightfv(GL_LIGHT0 + 1 + i, GL_SPECULAR, light_intensity);
        glEnable(GL_LIGHT0 + 1 + i);
        
    }
    // TODO: SetCamera
    scene.camera.gaze = util.Normalize(scene.camera.gaze);
    const Camera camera = scene.camera;
    const Vec3f eye = camera.position;
    const Vec3f center = util.Add(eye, util.Scale(camera.near_distance, scene.camera.gaze));
    const Vec4f near_plane = scene.camera.near_plane;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // Ratio is simple to calcualte, but I wasn't sure how to determine angle, so I decided to just use more generalized, core function glFrustum.
    // See: https://arstechnica.com/civis/viewtopic.php?t=147154
    glFrustum(near_plane.x,
              near_plane.y,
              near_plane.z,
              near_plane.w,
              camera.near_distance,
              camera.far_distance);
    glMatrixMode(GL_MODELVIEW);    
    glLoadIdentity();

    gluLookAt(eye.x, eye.y, eye.z, center.x, center.y, center.z, camera.up.x, camera.up.y, camera.up.z);
    

    while (!glfwWindowShouldClose(win))
    {
        // TODO: Render frame.
        RenderFrame();
        ShowFPS(win);
        glfwSwapBuffers(win);
        glfwPollEvents();
    }
    // My code ends here.

    glfwDestroyWindow(win);
    glfwTerminate();

    exit(EXIT_SUCCESS);

    return 0;
}
