#pragma once

float verticesTexturedRectangle[] = {
	// positions          // colors           // texture coords		// normals
	 0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,			0.0f, 0.0f, 1.0f,	// top right
	 0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,  			0.0f, 0.0f, 1.0f,	// bottom right
	-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,  			0.0f, 0.0f, 1.0f,	// bottom left
	-0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f,  			0.0f, 0.0f, 1.0f	// top left
};

unsigned int indicesTexturedRectangle[] = {
	0, 1, 3,   // first triangle
	1, 2, 3    // second triangle
};

// https://stackoverflow.com/questions/25349620/use-one-gl-element-array-buffer-to-reference-each-attribute-from-0
// "When using buffers for your vertex attributes, you need to create a vertex for each unique combination of vertex attributes."
// https://stackoverflow.com/questions/11148567/rendering-meshes-with-multiple-indices
// "Therefore, every unique combination of components must have its own separate index."
float verticesCube[] = {
	// position				// tex coords			// normal
	-0.5f, -0.5f, -0.5f,  	0.0f, 0.0f,  0.0f,  	0.0f, -1.0f,
	 0.5f, -0.5f, -0.5f,  	1.0f, 0.0f,  0.0f,  	0.0f, -1.0f,
	 0.5f,  0.5f, -0.5f,  	1.0f, 1.0f,  0.0f,  	0.0f, -1.0f,
	 0.5f,  0.5f, -0.5f,  	1.0f, 1.0f,  0.0f,  	0.0f, -1.0f,
	-0.5f,  0.5f, -0.5f,  	0.0f, 1.0f,  0.0f,  	0.0f, -1.0f,
	-0.5f, -0.5f, -0.5f,  	0.0f, 0.0f,  0.0f,  	0.0f, -1.0f,

	-0.5f, -0.5f,  0.5f,  	0.0f, 0.0f,  0.0f,  	0.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  	1.0f, 0.0f,  0.0f,  	0.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  	1.0f, 1.0f,  0.0f,  	0.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  	1.0f, 1.0f,  0.0f,  	0.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,  	0.0f, 1.0f,  0.0f,  	0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  	0.0f, 0.0f,  0.0f,  	0.0f, 1.0f,

	-0.5f,  0.5f,  0.5f,  	1.0f, 0.0f, -1.0f,  	0.0f,  0.0f,
	-0.5f,  0.5f, -0.5f,  	1.0f, 1.0f, -1.0f,  	0.0f,  0.0f,
	-0.5f, -0.5f, -0.5f,  	0.0f, 1.0f, -1.0f,  	0.0f,  0.0f,
	-0.5f, -0.5f, -0.5f,  	0.0f, 1.0f, -1.0f,  	0.0f,  0.0f,
	-0.5f, -0.5f,  0.5f,  	0.0f, 0.0f, -1.0f,  	0.0f,  0.0f,
	-0.5f,  0.5f,  0.5f,  	1.0f, 0.0f, -1.0f,  	0.0f,  0.0f,

	 0.5f,  0.5f,  0.5f,  	1.0f, 0.0f,  1.0f,  	0.0f,  0.0f,
	 0.5f,  0.5f, -0.5f,  	1.0f, 1.0f,  1.0f,  	0.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  	0.0f, 1.0f,  1.0f,  	0.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  	0.0f, 1.0f,  1.0f,  	0.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  	0.0f, 0.0f,  1.0f,  	0.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  	1.0f, 0.0f,  1.0f,  	0.0f,  0.0f,

	-0.5f, -0.5f, -0.5f,  	0.0f, 1.0f,  0.0f, 		1.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  	1.0f, 1.0f,  0.0f, 		1.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  	1.0f, 0.0f,  0.0f, 		1.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  	1.0f, 0.0f,  0.0f, 		1.0f,  0.0f,
	-0.5f, -0.5f,  0.5f,  	0.0f, 0.0f,  0.0f, 		1.0f,  0.0f,
	-0.5f, -0.5f, -0.5f,  	0.0f, 1.0f,  0.0f, 		1.0f,  0.0f,

	-0.5f,  0.5f, -0.5f,  	0.0f, 1.0f,  0.0f,  	1.0f,  0.0f,
	 0.5f,  0.5f, -0.5f,  	1.0f, 1.0f,  0.0f,  	1.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  	1.0f, 0.0f,  0.0f,  	1.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  	1.0f, 0.0f,  0.0f,  	1.0f,  0.0f,
	-0.5f,  0.5f,  0.5f,  	0.0f, 0.0f,  0.0f,  	1.0f,  0.0f,
	-0.5f,  0.5f, -0.5f,  	0.0f, 1.0f,  0.0f,  	1.0f,  0.0f
};

float verticesLine[] = {
	// position
	 0.0f, 0.0f, 0.0f,
	 0.9f, 0.0f, 0.0f
};
