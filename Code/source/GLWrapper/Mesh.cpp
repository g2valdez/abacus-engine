#include <iostream>
#include <GLWrapper/Mesh.h>

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices, std::vector<Texture> textures)
{
	this->vertices = vertices;
	this->indices = indices;
	this->textures = textures;
}

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices, std::vector<Texture> textures, aiMaterial* material)
{
	Material mat;
	aiColor3D color;
	float opacity;
	material->Get(AI_MATKEY_COLOR_AMBIENT, color);
	glm::vec3 amb(color.r, color.g, color.b);
	material->Get(AI_MATKEY_COLOR_SPECULAR, color);
	glm::vec3 spec(color.r, color.g, color.b);
	material->Get(AI_MATKEY_COLOR_DIFFUSE, color);
	glm::vec3 diff(color.r, color.g, color.b);
	material->Get(AI_MATKEY_OPACITY, opacity);

	mat.ambient = amb;
	mat.specular = spec;
	mat.diffuse = diff;
	mat.opacity = opacity;
	mat.shininess = 16.0f;

	this->vertices = vertices;
	this->indices = indices;
	this->textures = textures;
	this->material = mat;
}

// Render the mesh
void Mesh::Draw(GLuint shaderID)
{
	//// Bind appropriate textures
	//GLuint diffuseNr = 1;
	//GLuint specularNr = 1;

	//GLint matAmbientLoc = glGetUniformLocation(shaderID, "material.ambient");
	//GLint matDiffuseLoc = glGetUniformLocation(shaderID, "material.diffuse");
	//GLint matSpecularLoc = glGetUniformLocation(shaderID, "material.specular");
	//GLint matShineLoc = glGetUniformLocation(shaderID, "material.shininess");
	//GLint opacityLoc = glGetUniformLocation(shaderID, "material.opacity");

	//glUniform3f(matAmbientLoc, material.ambient.x, material.ambient.y, material.ambient.z);
	//glUniform3f(matDiffuseLoc, material.diffuse.x, material.diffuse.y, material.diffuse.z);
	//glUniform3f(matSpecularLoc, material.specular.x, material.specular.y, material.specular.z);
	//glUniform1f(matShineLoc, material.shininess);
	//glUniform1f(opacityLoc, material.opacity);


	//for (GLuint i = 0; i < this->textures.size(); i++)
	//{
	//	glActiveTexture(GL_TEXTURE0 + i); // Active proper texture unit before binding
	//									  // Retrieve texture number (the N in diffuse_textureN)
	//	stringstream ss;
	//	string number;
	//	string name = this->textures[i].type;
	//	if (name == "texture_diffuse")
	//		ss << diffuseNr++; // Transfer GLuint to stream
	//	else if (name == "texture_specular")
	//		ss << specularNr++; // Transfer GLuint to stream
	//	number = ss.str();
	//	// Now set the sampler to the correct texture unit
	//	glUniform1i(glGetUniformLocation(shaderID, (name + number).c_str()), i);
	//	// And finally bind the texture
	//	glBindTexture(GL_TEXTURE_2D, this->textures[i].id);
	//}

	//// Draw mesh
	//glBindVertexArray(this->VAO);
	//glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0);
	//glBindVertexArray(0);

	//// Always good practice to set everything back to defaults once configured.
	//for (GLuint i = 0; i < this->textures.size(); i++)
	//{
	//	glActiveTexture(GL_TEXTURE0 + i);
	//	glBindTexture(GL_TEXTURE_2D, 0);
	//}




	unsigned int diffuseNr = 1;
	unsigned int specularNr = 1;
	unsigned int normalNr = 1;
	unsigned int heightNr = 1;
	for (unsigned int i = 0; i < textures.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
		// retrieve texture number (the N in diffuse_textureN)
		std::string number;
		std::string name = textures[i].type;
		if (name == "texture_diffuse")
			number = std::to_string(diffuseNr++);
		else if (name == "texture_specular")
			number = std::to_string(specularNr++); // transfer unsigned int to stream
		else if (name == "texture_normal")
			number = std::to_string(normalNr++); // transfer unsigned int to stream
		else if (name == "texture_height")
			number = std::to_string(heightNr++); // transfer unsigned int to stream

												 // now set the sampler to the correct texture unit
		glUniform1i(glGetUniformLocation(shaderID, (name + number).c_str()), i);
		// and finally bind the texture
		glBindTexture(GL_TEXTURE_2D, textures[i].id);
	}

	// draw mesh
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	// always good practice to set everything back to defaults once configured.
	glActiveTexture(GL_TEXTURE0);
}

void Mesh::Resize(const glm::vec3& min, const glm::vec3& max)
{
	glm::vec3 size = max - min;
	float absoluteMax = size.x;
	if (size.y > absoluteMax)
	{
		absoluteMax = size.y;
	}
	if (size.z > absoluteMax)
	{
		absoluteMax = size.z;
	}

	for (auto& vertex : vertices)
	{
		vertex.Position = (vertex.Position - min) / absoluteMax;
		vertex.Position -= (size / absoluteMax) * 0.5f;
	}

}

void Mesh::SetupMesh()
{
	// Create buffers/arrays
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	// Load data into vertex buffers
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// A great thing about structs is that their memory layout is sequential for all its items.
	// The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
	// again translates to 3/2 floats which translates to a byte array.
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

	// Set the vertex attribute pointers
	// Vertex Positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
	// Vertex Normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, TexCoords));
	// Vertex Texture Coords
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Normal));
	// vertex tangent
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
	// vertex bitangent
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));

	glBindVertexArray(0);
}