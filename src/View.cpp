#include "View.h"


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

View::View()
{
    
    initWindow();//初始化窗口
                 //初始化参数和设置


    //PM 
    //scene =Scene();
    //scene.init();
    //scene.castPhotons();
    //scene.kdTree();            
    //display();

    //SPPM
    float initRadius=0.5;
    scene=new SPPMScene(initRadius);
    scene->init();

    //Load Model
    Model ourModel("./model/water/water.obj");
    scene->model=&ourModel;

    displaySPPM();

    
}

int View::initWindow()
{


    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }    

 
    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    stbi_set_flip_vertically_on_load(true);

    
    return 0;

    
    
}

int View::display()
{
    /*
    //init Shader
    Shader photonMapShader=Shader("./shaders/vPhotonMap.vert", "./shaders/fPhotonMap.frag");


   
    
    
    //init camera
    
    mainCamera=Camera(glm::vec3(0.0f, 0.0f, 3.0f));
    scene=new Scene();

    scene->rayTracing(mainCamera);

    
    
        

    
    //init data
    
    //PhotonData photondatas(scene.globalMap->photons,scene.globalMap->storedPhotons);
    VertexData rayTraceData(scene->rayTraceResult, SCR_WIDTH*SCR_HEIGHT*5);

    std::cout<<"init data ok"<<std::endl;


    while(!glfwWindowShouldClose(window))
    {

         //input
        processInput(window);

        // render command

        glClearColor(0.5f, 0.4f, 0.6f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

       
        //draw scene

        photonMapShader.use();

        glm::mat4 view = mainCamera.GetViewMatrix();
        photonMapShader.setMat4("view", view);

		// pass projection matrix to shader (note that in this case it could change every frame)
        glm::mat4 projection = mainCamera.GetProjMatrix(SCR_WIDTH, SCR_HEIGHT, false);
        photonMapShader.setMat4("projection", projection);

        glm::mat4 model=glm::mat4(1.0f);
        photonMapShader.setMat4("model", model);

        glBindVertexArray(rayTraceData.VAO);

        //glPointSize(2.0f);

        glDrawArrays(GL_POINTS, 0, SCR_HEIGHT*SCR_WIDTH); 
        //glDrawArrays(GL_POINT, 0, 36); 


        

        

        // 检查并调用事件，交换缓冲
        glfwPollEvents();
        glfwSwapBuffers(window);
        
    }


    glfwTerminate();
    

   */
    
    return 0;
    
} 

int View::displaySPPM()
{
    
    //init Shader
    Shader photonMapShader=Shader("./shaders/vPhotonMap.vert", "./shaders/fPhotonMap.frag");
    Shader testShader =Shader("./shaders/modelLoad.vert", "./shaders/modelLoad.frag");


    //init camera
    
    mainCamera=Camera(glm::vec3(0.0f, 0.0f, 3.0f));

    int round=0;
    
    //init data
    float alpha=0.75;
    int allPhoton=0;
    int roundPhoton=40000;


     //初始化VAO VBO
     //PhotonData photondatas(scene.globalMap->photons,scene.globalMap->storedPhotons);
     VertexData rayTraceData=VertexData();



     
     

     /*
     for(int i=0;i<SCR_WIDTH;i++)
     for(int j=0;j<SCR_HEIGHT;j++)
     {
         std::cout<<"uv:: ("<<scene->rayTraceResult[i*SCR_WIDTH*5+5*j+0]<<", "<<scene->rayTraceResult[i*SCR_WIDTH*5+5*j+1]<<")  color: ("<<scene->rayTraceResult[i*SCR_WIDTH*5+5*j+2]<<", "<<scene->rayTraceResult[i*SCR_WIDTH*5+5*j+3]<<", "<<scene->rayTraceResult[i*SCR_WIDTH*5+5*j+4]<<")"<<std::endl;
         
     }
    */
    while(!glfwWindowShouldClose(window))
    {

         //input
        processInput(window);

        // render command

        glClearColor(0.5f, 0.4f, 0.6f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

       
       
        //draw scene

        
        photonMapShader.use();
        /*
        glm::mat4 view = mainCamera.GetViewMatrix();
        testShader.setMat4("view", view);

		// pass projection matrix to shader (note that in this case it could change every frame)
        glm::mat4 projection = mainCamera.GetProjMatrix(SCR_WIDTH, SCR_HEIGHT, false);
        testShader.setMat4("projection", projection);

        glm::mat4 model=glm::mat4(1.0f);
        //model=glm::scale(model,glm::vec3(0.5));
        testShader.setMat4("model", model);
        
        ourModel.Draw(testShader);
        */
        

         //根据轮数渲染，小于30 轮的时候就一直更新顶点数据
        if(round<20)
        {
            allPhoton+=roundPhoton;
            
   
            scene->SPPM(mainCamera, alpha, round, allPhoton, roundPhoton);

            rayTraceData.update(scene->rayTraceResult, SCR_WIDTH*SCR_HEIGHT*5);

            round++;

        }

        photonMapShader.use();
        glBindVertexArray(rayTraceData.VAO);


        glDrawArrays(GL_POINTS, 0, SCR_HEIGHT*SCR_WIDTH); 
        //glDrawArrays(GL_TRIANGLES, 0, 36); 
        //std::cout<<"0: "<<glGetError()<<std::endl;

        

        // 检查并调用事件，交换缓冲
        glfwPollEvents();
        glfwSwapBuffers(window);

        
        
    }


    glfwTerminate();
    

   
    
    return 0;
    
} 




// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

//用于按键输入
void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}