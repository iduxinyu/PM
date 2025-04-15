#pragma once

#include <glad/glad.h> // holds all OpenGL type declarations

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Shader.h"

#include <string>
#include <vector>
using namespace std;

#define NOT_INTERSECTED (1.0e6) //对应无限大

struct Vertex{
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
    glm::vec3 Tangent;
    glm::vec3 Bitangent;

};

struct Texture{
    unsigned int id;
    string type;
    string path;
};

class Mesh{

public:
    //mesh Data
    vector<Vertex> vertices;
    vector<unsigned int> indices;
    vector<Texture> textures;
    unsigned int VAO;

private:
    unsigned int VBO;
    unsigned int EBO;

public:

    Mesh(vector<Vertex> vertices,  vector<unsigned int> indices, vector<Texture> textures)
    {
        this->vertices = vertices;
        this->indices = indices;
        this->textures = textures;

        //set the vertex buffers and its attribute pointers.
        setupMesh();
    }

    void Draw(Shader &shader)
    {
        //bind textures
        unsigned int diffuseNr  = 1;
        unsigned int specularNr = 1;
        unsigned int normalNr   = 1;
        unsigned int heightNr   = 1;
        unsigned int unknownNr  = 1;

        for(unsigned int i=0;i<textures.size();i++)
        {
            glActiveTexture(GL_TEXTURE0+i);

            string number;
            string name=textures[i].type;

            
            if(name == "texture_diffuse")
                number = std::to_string(diffuseNr++);
            else if(name == "texture_specular")
                number = std::to_string(specularNr++); // transfer unsigned int to string
            else if(name == "texture_normal")
                number = std::to_string(normalNr++); // transfer unsigned int to string
            else if(name == "texture_height")
                number = std::to_string(heightNr++); // transfer unsigned int to string
            else if(name == "texture_unknown")
                number = std::to_string(unknownNr++);
             // now set the sampler to the correct texture unit
            glUniform1i(glGetUniformLocation(shader.ID, (name + number).c_str()), i);
            // and finally bind the texture
            glBindTexture(GL_TEXTURE_2D, textures[i].id);

        }

        // draw mesh
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // always good practice to set everything back to defaults once configured.
        glActiveTexture(GL_TEXTURE0);
    }

    float intersect(glm::vec3 origin, glm::vec3 direction, glm::vec3* normal)
    {
        //先和包围盒判断相交，如果失败则无需查找细节


        //每三个一组遍历 index,即一个面
        float mint=NOT_INTERSECTED;
        glm::vec3 minNorm=glm::vec3(0.0);

        for(int i=0;i<indices.size();i+=3)
        {

            Vertex v1=vertices[indices[i]];
            Vertex v2=vertices[indices[i+1]];
            Vertex v3=vertices[indices[i+2]];

            //和三角形所在的面判断交点
            glm::vec3 norm=glm::cross(v2.Position-v1.Position,v3.Position-v1.Position);
            norm=glm::normalize(norm);

            float t=glm::dot(v1.Position-origin,norm)/glm::dot(direction,norm);

            if(t<=0.0)
            {
                continue;
            }

            glm::vec3 p=origin+t*direction;
            //然后判断是否在三角形内
            glm::vec3 c1=glm::cross(v2.Position-v1.Position,p-v1.Position);
            glm::vec3 c2=glm::cross(v3.Position-v2.Position,p-v2.Position);
            glm::vec3 c3=glm::cross(v1.Position-v3.Position,p-v3.Position);

            if(glm::dot(c1,c2)>0 && glm::dot(c2,c3)>0 && t<mint)
            {
                mint=t;
                minNorm=norm;
            }

        }
        normal->x=minNorm.x;
        normal->y=minNorm.y;
        normal->z=minNorm.z;
        return mint;
    }

private:

    void setupMesh()
    {
        //create buffers
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER,VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

        // set the vertex attribute pointers
        // vertex Positions
        glEnableVertexAttribArray(0);	
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        // vertex normals
        glEnableVertexAttribArray(1);	
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
        // vertex texture coords
        glEnableVertexAttribArray(2);	
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
         // vertex tangent
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
        // vertex bitangent
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));
   
        glBindVertexArray(0);
   
   
    }


};