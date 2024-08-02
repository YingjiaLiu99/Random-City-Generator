#include "realtime.h"

#include <QCoreApplication>
#include <QMouseEvent>
#include <QKeyEvent>
#include <iostream>
#include "settings.h"
#include "src/utils/shaderloader.h"
#include <glm/gtx/string_cast.hpp>
#include <cstring>
#include <string>
#include <random>

Realtime::Realtime(QWidget *parent)
    : QOpenGLWidget(parent)
{
    m_prev_mouse_pos = glm::vec2(size().width()/2, size().height()/2);
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);

    m_keyMap[Qt::Key_W]       = false;
    m_keyMap[Qt::Key_A]       = false;
    m_keyMap[Qt::Key_S]       = false;
    m_keyMap[Qt::Key_D]       = false;
    m_keyMap[Qt::Key_Control] = false;
    m_keyMap[Qt::Key_Space]   = false;

    // If you must use this function, do not edit anything above this
}

void Realtime::finish() {
    killTimer(m_timer);
    this->makeCurrent();
    glDeleteBuffers(1,&m_vbo_sphere);
    glDeleteVertexArrays(1,&m_vao_sphere);
    glDeleteBuffers(1,&m_vbo_cube);
    glDeleteVertexArrays(1,&m_vao_cube);
    glDeleteBuffers(1,&m_vbo_cone);
    glDeleteVertexArrays(1,&m_vao_cone);
    glDeleteBuffers(1,&m_vbo_cylinder);
    glDeleteVertexArrays(1,&m_vao_cylinder);
    glDeleteBuffers(1,&m_vbo_skyscraper);
    glDeleteVertexArrays(1,&m_vao_skyscraper);
    glDeleteProgram(m_shader);
    this->doneCurrent();
}

void Realtime::initializeGL() {
    m_devicePixelRatio = this->devicePixelRatio();
    m_defaultFBO = 2; // here is the default FBO number

    m_timer = startTimer(1000/60);
    m_elapsedTimer.start();

    // Initializing GL.
    // GLEW (GL Extension Wrangler) provides access to OpenGL functions.
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        std::cerr << "Error while initializing GL: " << glewGetErrorString(err) << std::endl;
    }
    std::cout << "Initialized GL: Version " << glewGetString(GLEW_VERSION) << std::endl;

    // Allows OpenGL to draw objects appropriately on top of one another
    glEnable(GL_DEPTH_TEST);
    // Tells OpenGL to only draw the front face
    glEnable(GL_CULL_FACE);
    // Tells OpenGL how big the screen is
    glViewport(0, 0, size().width() * m_devicePixelRatio, size().height() * m_devicePixelRatio);

    // Anything requiring OpenGL calls when the program starts should be done here
    m_shader = ShaderLoader::createShaderProgram(":/resources/shaders/default.vert", ":/resources/shaders/default.frag");
    m_texture_shader = ShaderLoader::createShaderProgram(":/resources/shaders/texture.vert",":/resources/shaders/texture.frag");

    updateParam(settings.shapeParameter1, settings.shapeParameter2);

    // initialize cube, sphere, cylinder, cone's VAO and VBO
    // ------------------------------ sphere -------------------------------- //
    glGenBuffers(1, &m_vbo_sphere);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_sphere);
    glBufferData(GL_ARRAY_BUFFER,m_sphereData.size()*sizeof(GLfloat),m_sphereData.data(),GL_STATIC_DRAW);
    glGenVertexArrays(1, &m_vao_sphere);
    glBindVertexArray(m_vao_sphere);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,6 * sizeof(GLfloat),reinterpret_cast<void *>(0));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,6*sizeof(GLfloat),reinterpret_cast<void*>(3*sizeof(GLfloat)));
    // ------------------------------ cube --------------------------------- //
    glGenBuffers(1, &m_vbo_cube);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_cube);
    glBufferData(GL_ARRAY_BUFFER,m_cubeData.size()*sizeof(GLfloat),m_cubeData.data(),GL_STATIC_DRAW);
    glGenVertexArrays(1, &m_vao_cube);
    glBindVertexArray(m_vao_cube);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,6 * sizeof(GLfloat),reinterpret_cast<void *>(0));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,6*sizeof(GLfloat),reinterpret_cast<void*>(3*sizeof(GLfloat)));
    // ------------------------------ cone --------------------------------- //
    glGenBuffers(1, &m_vbo_cone);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_cone);
    glBufferData(GL_ARRAY_BUFFER,m_coneData.size()*sizeof(GLfloat),m_coneData.data(),GL_STATIC_DRAW);
    glGenVertexArrays(1, &m_vao_cone);
    glBindVertexArray(m_vao_cone);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,6 * sizeof(GLfloat),reinterpret_cast<void *>(0));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,6*sizeof(GLfloat),reinterpret_cast<void*>(3*sizeof(GLfloat)));
    // ----------------------------- cylinder ------------------------------ //
    glGenBuffers(1, &m_vbo_cylinder);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_cylinder);
    glBufferData(GL_ARRAY_BUFFER,m_cylinderData.size()*sizeof(GLfloat),m_cylinderData.data(),GL_STATIC_DRAW);
    glGenVertexArrays(1, &m_vao_cylinder);
    glBindVertexArray(m_vao_cylinder);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,6 * sizeof(GLfloat),reinterpret_cast<void *>(0));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,6*sizeof(GLfloat),reinterpret_cast<void*>(3*sizeof(GLfloat)));
    // ----------------------------- skyscrapper ------------------------------ //
    glGenBuffers(1, &m_vbo_skyscraper);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_skyscraper);
    glBufferData(GL_ARRAY_BUFFER,m_skyscraperData.size()*sizeof(GLfloat),m_skyscraperData.data(),GL_STATIC_DRAW);
    glGenVertexArrays(1, &m_vao_skyscraper);
    glBindVertexArray(m_vao_skyscraper);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,6 * sizeof(GLfloat),reinterpret_cast<void *>(0));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,6*sizeof(GLfloat),reinterpret_cast<void*>(3*sizeof(GLfloat)));
    // ------------------------------------------------------------------------ //


    glActiveTexture(GL_TEXTURE0);
    glUseProgram(m_texture_shader);
    GLint locationSampler = glGetUniformLocation(m_texture_shader,"sampler");
    glUniform1i(locationSampler,0);
    std::vector<GLfloat> fullscreen_quad_data =
        { //     POSITIONS    //
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
            1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
            1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            1.0f, -1.0f, 0.0f, 1.0f, 0.0f
        };
    // Generate and bind a VBO and a VAO for a fullscreen quad
    glGenBuffers(1, &m_fullscreen_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_fullscreen_vbo);
    glBufferData(GL_ARRAY_BUFFER, fullscreen_quad_data.size()*sizeof(GLfloat), fullscreen_quad_data.data(), GL_STATIC_DRAW);
    glGenVertexArrays(1, &m_fullscreen_vao);
    glBindVertexArray(m_fullscreen_vao);    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), nullptr);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), reinterpret_cast<void*>(3*sizeof(GLfloat)));
    // Unbind the fullscreen quad's VBO and VAO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    makeFBO();

    GenerateCity();

    glInitialized = true;
}

void Realtime::paintGL() {   
    if(upload == true){
        // std::cout<<"paintGL() called!"<<std::endl;
        glUseProgram(m_shader);

        glBindFramebuffer(GL_FRAMEBUFFER,m_fbo);
        glViewport(0,0,size().width(), size().height());
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // ------------------------------------------- //
        glm::vec4 cam = glm::inverse(m_view) * glm::vec4(0,0,0,1);
        GLint locationcam = glGetUniformLocation(m_shader,"cam_pos");
        glUniform4fv(locationcam,1,&cam[0]);        
        for(RenderShapeData& shape : m_data.shapes){
            glm::mat4 CTM = shape.ctm;
            GLint locationM = glGetUniformLocation(m_shader,"modelM");
            glUniformMatrix4fv(locationM,1,GL_FALSE,&CTM[0][0]);
            glm::mat4 MVP = m_proj * m_view * CTM;
            GLint locationMVP = glGetUniformLocation(m_shader, "MVP");
            glUniformMatrix4fv(locationMVP,1,GL_FALSE,&MVP[0][0]);
            GLint locations = glGetUniformLocation(m_shader,"shine");
            glUniform1f(locations,shape.primitive.material.shininess);
            GLint locationcA = glGetUniformLocation(m_shader,"cA");
            glUniform4fv(locationcA,1,&shape.primitive.material.cAmbient[0]);
            GLint locationcD = glGetUniformLocation(m_shader,"cD");
            glUniform4fv(locationcD,1,&shape.primitive.material.cDiffuse[0]);
            GLint locationcS = glGetUniformLocation(m_shader,"cS");
            glUniform4fv(locationcS,1,&shape.primitive.material.cSpecular[0]);
            if(shape.primitive.type==PrimitiveType::PRIMITIVE_SPHERE){                
                glBindVertexArray(m_vao_sphere);
                glDrawArrays(GL_TRIANGLES, 0, m_sphereData.size() / 6);
                glBindVertexArray(0);
            }
            if(shape.primitive.type==PrimitiveType::PRIMITIVE_CUBE){                
                glBindVertexArray(m_vao_cube);                
                glDrawArrays(GL_TRIANGLES, 0, m_cubeData.size() / 6);
                glBindVertexArray(0);
            }
            if(shape.primitive.type==PrimitiveType::PRIMITIVE_CYLINDER){                
                glBindVertexArray(m_vao_cylinder);                
                glDrawArrays(GL_TRIANGLES, 0, m_cylinderData.size() / 6);
                glBindVertexArray(0);
            }
            if(shape.primitive.type==PrimitiveType::PRIMITIVE_CONE){                
                glBindVertexArray(m_vao_cone);                
                glDrawArrays(GL_TRIANGLES, 0, m_coneData.size() / 6);
                glBindVertexArray(0);
            }
        }

        // paint the ground plain:
        glm::mat4 groundCTM = m_CTM_collection[0];
        glm::vec4 Ambient = glm::vec4(1.0,1.0,1.0,0.0);
        glm::vec4 Diffuse = glm::vec4(0.7,0.7,0.7,0.0);
        GLint locationM = glGetUniformLocation(m_shader,"modelM");
        glUniformMatrix4fv(locationM,1,GL_FALSE,&groundCTM[0][0]);
        glm::mat4 MVP = m_proj * m_view * groundCTM;
        GLint locationMVP = glGetUniformLocation(m_shader, "MVP");
        glUniformMatrix4fv(locationMVP,1,GL_FALSE,&MVP[0][0]);
        GLint locations = glGetUniformLocation(m_shader,"shine");
        glUniform1f(locations,15);
        GLint locationcD = glGetUniformLocation(m_shader,"cD");
        glUniform4fv(locationcD,1,&Diffuse[0]);
        glBindVertexArray(m_vao_cube);
        glDrawArrays(GL_TRIANGLES, 0, m_cubeData.size() / 6);
        glBindVertexArray(0);

        // paint the street:
        for(int i = 0; i < 10; i++){
            glm::mat4 CTM = m_building_arr[i].CTM;
            glm::vec4 Ambient = glm::vec4(0.5,0.5,0.5,0.0);
            glm::vec4 Diffuse = glm::vec4(0.7,0.7,0.5,0.0);
            // glm::vec4 Specular = glm::vec4(1.0,1.0,1.0,1.0);
            GLint locationM = glGetUniformLocation(m_shader,"modelM");
            glUniformMatrix4fv(locationM,1,GL_FALSE,&CTM[0][0]);
            glm::mat4 MVP = m_proj * m_view * CTM;
            GLint locationMVP = glGetUniformLocation(m_shader, "MVP");
            glUniformMatrix4fv(locationMVP,1,GL_FALSE,&MVP[0][0]);
            GLint locations = glGetUniformLocation(m_shader,"shine");
            glUniform1f(locations,15);
            GLint locationcA = glGetUniformLocation(m_shader,"cA");
            glUniform4fv(locationcA,1,&Ambient[0]);
            GLint locationcD = glGetUniformLocation(m_shader,"cD");
            glUniform4fv(locationcD,1,&Diffuse[0]);
            glBindVertexArray(m_vao_cube);
            glDrawArrays(GL_TRIANGLES, 0, m_cubeData.size() / 6);
            glBindVertexArray(0);
        }

        // generate light within each building

        glm::vec3 color = glm::vec3(0.5, 0.5, 0.25);
        glm::vec3 atten = glm::vec3(0.05, 0.8, 2.0);

        GLint totalPointLights = glGetUniformLocation(m_shader,"num_point_lights");
        glUniform1i(totalPointLights, m_point_lights.size() + m_building_arr.size() - 10);

        int lightStart = m_point_lights.size();
        int numOfBuildings = m_building_arr.size() - 10;
        int buildingIndex = numOfStreet;
        for (int i = lightStart; i < (numOfBuildings + lightStart); i++) {
            glm::vec3 pos(m_building_arr[buildingIndex].position[0] + 0.3, m_building_arr[buildingIndex].position[1] - 1.f, m_building_arr[buildingIndex].position[2] - 0.3);
            GLint colorLoc = glGetUniformLocation(m_shader, ("point_light_color[" + std::to_string(i)+ "]").c_str());
            glUniform4f(colorLoc, color[0], color[1], color[2], 0.0f);
            GLint posLoc = glGetUniformLocation(m_shader, ("point_light_pos[" + std::to_string(i)+ "]").c_str());
            glUniform3f(posLoc, pos[0], pos[1], pos[2]);
            GLint attLoc = glGetUniformLocation(m_shader,("point_light_func[" + std::to_string(i)+ "]").c_str());
            glUniform3f(attLoc, atten[0], atten[1], atten[2]);
            buildingIndex++;
        }

        // paint all the buildings
        for(int i = 0; i < m_building_arr.size(); i++){
            glm::mat4 CTM = m_building_arr[i].CTM;
            glm::vec4 Ambient = glm::vec4(0.5,0.5,0.5,0.0);
            glm::vec4 Diffuse = glm::vec4(0.7,0.7,0.5,0.0);
            glm::vec4 Specular = glm::vec4(1.0,1.0,1.0,1.0);
            GLint locationM = glGetUniformLocation(m_shader,"modelM");
            glUniformMatrix4fv(locationM,1,GL_FALSE,&CTM[0][0]);
            glm::mat4 MVP = m_proj * m_view * CTM;
            GLint locationMVP = glGetUniformLocation(m_shader, "MVP");
            glUniformMatrix4fv(locationMVP,1,GL_FALSE,&MVP[0][0]);
            GLint locations = glGetUniformLocation(m_shader,"shine");
            glUniform1f(locations,15);
            GLint locationcA = glGetUniformLocation(m_shader,"cA");
            glUniform4fv(locationcA,1,&Ambient[0]);
            GLint locationcD = glGetUniformLocation(m_shader,"cD");
            glUniform4fv(locationcD,1,&Diffuse[0]);
            GLint locationcS = glGetUniformLocation(m_shader,"cS");
            glUniform4fv(locationcS,1,&Specular[0]);

            float type = m_building_arr[i].type;
            if (type < 1){
                glBindVertexArray(m_vao_skyscraper);
                glDrawArrays(GL_TRIANGLES, 0, m_skyscraperData.size() / 6);
                glBindVertexArray(0);
            }

        }

        // -------------------------------------------------------------- //
        glBindFramebuffer(GL_FRAMEBUFFER,m_defaultFBO);
        glViewport(0,0,size().width() * m_devicePixelRatio, size().height() * m_devicePixelRatio);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        paintTexture(m_fbo_texture);

        glUseProgram(0);
    }
}

void Realtime::paintTexture(GLuint texture){
    glUseProgram(m_texture_shader);

    GLint locationPer = glGetUniformLocation(m_texture_shader,"per_pixel");
    glUniform1i(locationPer,settings.perPixelFilter);
    GLint locationKer = glGetUniformLocation(m_texture_shader,"kernel");
    glUniform1i(locationKer,settings.kernelBasedFilter);

    glBindVertexArray(m_fullscreen_vao);
    // Bind "texture" to slot 0
    glActiveTexture(0);
    glBindTexture(GL_TEXTURE_2D,texture);

    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
    glUseProgram(0);
}

void Realtime::resizeGL(int w, int h) {
    // Tells OpenGL how big the screen is
    glViewport(0, 0, size().width() * m_devicePixelRatio, size().height() * m_devicePixelRatio);

    // anything requiring OpenGL calls when the program starts should be done here
    updateParam2(size().width(),size().height(),settings.farPlane,settings.nearPlane);
    makeFBO();
    update();
}

void Realtime::sceneChanged() {
    SceneParser::parse(settings.sceneFilePath,m_data);
    m_cam_pos = m_data.cameraData.pos;
    m_cam_look = m_data.cameraData.look;
    m_cam_up = m_data.cameraData.up;
    m_view = getViewMatrix(m_cam_pos,m_cam_look,m_cam_up);
    hAngle = m_data.cameraData.heightAngle;    
    updateParam2(width(),height(),settings.farPlane,settings.nearPlane);
    m_dir_lights.clear();
    m_point_lights.clear();
    m_spot_lights.clear();
    m_sphereData.clear();
    m_cubeData.clear();
    m_cylinderData.clear();
    m_coneData.clear();
    m_skyscraperData.clear();
    for(SceneLightData light : m_data.lights){
        if(light.type == LightType::LIGHT_DIRECTIONAL){
            m_dir_lights.push_back(light);
        }
        if(light.type == LightType::LIGHT_POINT){
            m_point_lights.push_back(light);
        }
        if(light.type == LightType::LIGHT_SPOT){
            m_spot_lights.push_back(light);
        }
    }
    m_shapes = m_data.shapes;
    m_ka = m_data.globalData.ka;
    m_kd = m_data.globalData.kd;
    m_ks = m_data.globalData.ks;

    upload = true;
    updateParam(settings.shapeParameter1,settings.shapeParameter2);
    // std::cout<<"scene Changed called"<<std::endl;
    updateVBOs(); // update VBOs of all the primitives

    glUseProgram(m_shader);

    GLint locationka = glGetUniformLocation(m_shader,"k_a");
    glUniform1f(locationka,m_ka);
    GLint locationkd = glGetUniformLocation(m_shader,"k_d");
    glUniform1f(locationkd,m_kd);
    GLint locationks = glGetUniformLocation(m_shader,"k_s");
    glUniform1f(locationks,m_ks);
    GLint locationDL = glGetUniformLocation(m_shader,"num_dir_lights");
    glUniform1i(locationDL, m_dir_lights.size());
    GLint locationPL = glGetUniformLocation(m_shader,"num_point_lights");
    glUniform1i(locationPL, m_point_lights.size());
    GLint locationSL =  glGetUniformLocation(m_shader,"num_spot_lights");
    glUniform1i(locationSL, m_spot_lights.size());
    GLint locationMaxLightDistance = glGetUniformLocation(m_shader, "max_light_distance");
    glUniform1f(locationMaxLightDistance, MAX_LIGHT_DISTANCE);

    for(int j = 0; j < m_dir_lights.size(); j++){
        GLint loc = glGetUniformLocation(m_shader, ("dir_light_dir[" + std::to_string(j)+ "]").c_str());
        glUniform4f(loc,m_dir_lights[j].dir[0],m_dir_lights[j].dir[1],m_dir_lights[j].dir[2],m_dir_lights[j].dir[3]);
        GLint loc2 = glGetUniformLocation(m_shader, ("dir_light_color[" + std::to_string(j)+ "]").c_str());
        glUniform4f(loc2,m_dir_lights[j].color[0],m_dir_lights[j].color[1],m_dir_lights[j].color[2],m_dir_lights[j].color[3]);
    }
    for(int j = 0; j < m_point_lights.size(); j++){        
        GLint loc4 = glGetUniformLocation(m_shader, ("point_light_color[" + std::to_string(j)+ "]").c_str());
        glUniform4f(loc4,m_point_lights[j].color[0],m_point_lights[j].color[1],m_point_lights[j].color[2],m_point_lights[j].color[3]);
        GLint locpos = glGetUniformLocation(m_shader, ("point_light_pos[" + std::to_string(j)+ "]").c_str());
        glUniform3f(locpos,m_point_lights[j].pos[0],m_point_lights[j].pos[1],m_point_lights[j].pos[2]);
        GLint locfunc = glGetUniformLocation(m_shader,("point_light_func[" + std::to_string(j)+ "]").c_str());
        glUniform3f(locfunc,m_point_lights[j].function[0],m_point_lights[j].function[1],m_point_lights[j].function[2]);
    }
    for(int j = 0; j < m_spot_lights.size(); j++){
        GLint loc5 = glGetUniformLocation(m_shader, ("spot_light_dir[" + std::to_string(j)+ "]").c_str());
        glUniform4f(loc5,m_spot_lights[j].dir[0],m_spot_lights[j].dir[1],m_spot_lights[j].dir[2],m_spot_lights[j].dir[3]);
        GLint loc6 = glGetUniformLocation(m_shader, ("spot_light_color[" + std::to_string(j)+ "]").c_str());
        glUniform4f(loc6,m_spot_lights[j].color[0],m_spot_lights[j].color[1],m_spot_lights[j].color[2],m_spot_lights[j].color[3]);
        GLint locpos2 = glGetUniformLocation(m_shader, ("spot_light_pos[" + std::to_string(j)+ "]").c_str());
        glUniform3f(locpos2,m_spot_lights[j].pos[0],m_spot_lights[j].pos[1],m_spot_lights[j].pos[2]);
        GLint locfunc2 = glGetUniformLocation(m_shader,("spot_light_func[" + std::to_string(j)+ "]").c_str());
        glUniform3f(locfunc2,m_spot_lights[j].function[0],m_spot_lights[j].function[1],m_spot_lights[j].function[2]);
        GLint locp = glGetUniformLocation(m_shader,("penumbra[" + std::to_string(j)+ "]").c_str());
        glUniform1f(locp,m_spot_lights[j].penumbra);
        GLint loca = glGetUniformLocation(m_shader,("angle[" + std::to_string(j)+ "]").c_str());
        glUniform1f(loca,m_spot_lights[j].angle);
    }
    glUseProgram(0);

    update(); // asks for a PaintGL() call to occur
}

void Realtime::settingsChanged() {
    updateParam(settings.shapeParameter1, settings.shapeParameter2);
    updateParam2(width(),height(),settings.farPlane,settings.nearPlane);
    if(glInitialized){updateVBOs();}// update VBOs of all the primitives
    update(); // asks for a PaintGL() call to occur
    // std::cout<<"settings Changed called"<<std::endl;
}

void Realtime::updateVBOs(){
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_sphere);
    glBufferData(GL_ARRAY_BUFFER,m_sphereData.size()*sizeof(GLfloat),m_sphereData.data(),GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_cube);
    glBufferData(GL_ARRAY_BUFFER,m_cubeData.size()*sizeof(GLfloat),m_cubeData.data(),GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_cone);
    glBufferData(GL_ARRAY_BUFFER,m_coneData.size()*sizeof(GLfloat),m_coneData.data(),GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_cylinder);
    glBufferData(GL_ARRAY_BUFFER,m_cylinderData.size()*sizeof(GLfloat),m_cylinderData.data(),GL_STATIC_DRAW);
    // Unbind the fullscreen quad's VBO and VAO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

}

void Realtime::keyPressEvent(QKeyEvent *event) {
    m_keyMap[Qt::Key(event->key())] = true;
}

void Realtime::keyReleaseEvent(QKeyEvent *event) {
    m_keyMap[Qt::Key(event->key())] = false;
}

void Realtime::mousePressEvent(QMouseEvent *event) {
    if (event->buttons().testFlag(Qt::LeftButton)) {
        m_mouseDown = true;
        m_prev_mouse_pos = glm::vec2(event->position().x(), event->position().y());
    }
}

void Realtime::mouseReleaseEvent(QMouseEvent *event) {
    if (!event->buttons().testFlag(Qt::LeftButton)) {
        m_mouseDown = false;
    }
}

void Realtime::mouseMoveEvent(QMouseEvent *event) {
    if (m_mouseDown) {
        int posX = event->position().x();
        int posY = event->position().y();
        int deltaX = m_prev_mouse_pos.x - posX;
        int deltaY = m_prev_mouse_pos.y - posY;
        m_prev_mouse_pos = glm::vec2(posX, posY);

        // Use deltaX and deltaY here to rotate
        // Sensitivity factor for rotation
        float sensitivity = 0.003f;
        float angleX = deltaX * sensitivity;
        float angleY = deltaY * sensitivity;
        glm::mat4 rotX = createRotationMatrix(angleX, glm::vec3(0, 1, 0));
        glm::mat4 rotY = createRotationMatrix(angleY, glm::cross(glm::vec3(m_cam_look), glm::vec3(m_cam_up)));
        // Apply rotations to the look vector and up vector
        m_cam_look = rotX * rotY * m_cam_look;
        m_cam_up = rotX * rotY * m_cam_up;
        // Normalize the vectors to prevent scaling issues
        m_cam_look = glm::normalize(m_cam_look);
        m_cam_up = glm::normalize(m_cam_up);
        // Recompute the view matrix
        m_view = getViewMatrix(m_cam_pos, m_cam_look, m_cam_up);
        update(); // asks for a PaintGL() call to occur
    }
}

void Realtime::timerEvent(QTimerEvent *event) {
    int elapsedms   = m_elapsedTimer.elapsed();
    float deltaTime = elapsedms * 0.001f;
    m_elapsedTimer.restart();

    // Use deltaTime and m_keyMap here to move around
    float speed = 5.0f * deltaTime; // here to adjust the time
    glm::vec3 translation(0.0f);

    if (m_keyMap[Qt::Key_W]) {
        translation += glm::vec3(glm::normalize(m_cam_look) * speed);
    }
    if (m_keyMap[Qt::Key_S]) {
        translation -= glm::vec3(glm::normalize(m_cam_look) * speed);
    }
    if (m_keyMap[Qt::Key_A]) {
        glm::vec3 left = glm::normalize(glm::cross(glm::vec3(m_cam_up), glm::vec3(m_cam_look)));
        translation += left * speed;  // Notice the minus sign to move left
    }
    if (m_keyMap[Qt::Key_D]) {
        glm::vec3 right = glm::normalize(glm::cross(glm::vec3(m_cam_look), glm::vec3(m_cam_up)));
        translation += right * speed;
    }
    if (m_keyMap[Qt::Key_Space]) {
        translation += glm::vec3(0.0f, speed, 0.0f);
    }
    if (m_keyMap[Qt::Key_Control]) {
        translation -= glm::vec3(0.0f, speed, 0.0f);
    }

    glm::mat4 transMat = createTranslationMatrix(translation.x, translation.y, translation.z);
    m_cam_pos = transMat * m_cam_pos;

    m_view = getViewMatrix(m_cam_pos, m_cam_look, m_cam_up);

    update(); // asks for a PaintGL() call to occur
}

void Realtime::makeFBO(){
    //Generate and bind an empty texture, set its min/mag filter interpolation, then unbind
    glGenTextures(1,&m_fbo_texture);
    glBindTexture(GL_TEXTURE_2D,m_fbo_texture);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,size().width(),size().height(),0,GL_RGBA,GL_UNSIGNED_BYTE,NULL);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D,0);
    //Generate and bind a renderbuffer of the right size, set its format, then unbind
    glGenRenderbuffers(1,&m_fbo_renderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER,m_fbo_renderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER,GL_DEPTH24_STENCIL8,size().width(),size().height());
    glBindRenderbuffer(GL_RENDERBUFFER,0);
    //Generate and bind an FBO
    glGenFramebuffers(1,&m_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER,m_fbo);
    //Add our texture as a color attachment, and our renderbuffer as a depth+stencil attachment, to our FBO
    glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,m_fbo_texture,0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_DEPTH_STENCIL_ATTACHMENT,GL_RENDERBUFFER,m_fbo_renderbuffer);
    //Unbind the FBO
    glBindFramebuffer(GL_FRAMEBUFFER,m_defaultFBO);
}

// DO NOT EDIT
void Realtime::saveViewportImage(std::string filePath) {
    // Make sure we have the right context and everything has been drawn
    makeCurrent();

    int fixedWidth = 1024;
    int fixedHeight = 768;

    // Create Frame Buffer
    GLuint fbo;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    // Create a color attachment texture
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, fixedWidth, fixedHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

    // Optional: Create a depth buffer if your rendering uses depth testing
    GLuint rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, fixedWidth, fixedHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Error: Framebuffer is not complete!" << std::endl;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        return;
    }

    // Render to the FBO
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glViewport(0, 0, fixedWidth, fixedHeight);

    // Clear and render your scene here
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    paintGL();

    // Read pixels from framebuffer
    std::vector<unsigned char> pixels(fixedWidth * fixedHeight * 3);
    glReadPixels(0, 0, fixedWidth, fixedHeight, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());

    // Unbind the framebuffer to return to default rendering to the screen
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Convert to QImage
    QImage image(pixels.data(), fixedWidth, fixedHeight, QImage::Format_RGB888);
    QImage flippedImage = image.mirrored(); // Flip the image vertically

    // Save to file using Qt
    QString qFilePath = QString::fromStdString(filePath);
    if (!flippedImage.save(qFilePath)) {
        std::cerr << "Failed to save image to " << filePath << std::endl;
    }

    // Clean up
    glDeleteTextures(1, &texture);
    glDeleteRenderbuffers(1, &rbo);
    glDeleteFramebuffers(1, &fbo);
}
