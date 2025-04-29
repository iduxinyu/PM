#version 330 core
out vec4 FragColor;

in vec3 ourColor;

void main()
{
    FragColor = vec4(ourColor, 1.0f);

    /*
    if(FragColor.x<0.1)
        FragColor.x=0.3;
    else if(FragColor.x<0.25)
        FragColor.x=0.6;
    else if(FragColor.x<0.4)
        FragColor.x=0.8;
    else
        FragColor.x=1.0;
    
    if(FragColor.y<0.1)
        FragColor.y=0.3;
    else if(FragColor.y<0.2)
        FragColor.y=0.6;
    else if(FragColor.y<0.3)
        FragColor.y=0.8;
    else
        FragColor.y=1.0;

    if(FragColor.z<0.1)
        FragColor.z=0.3;
    else if(FragColor.z<0.2)
        FragColor.z=0.6;
    else if(FragColor.z<0.3)
        FragColor.z=0.8;
    else
        FragColor.z=1.0;

    */
    
}