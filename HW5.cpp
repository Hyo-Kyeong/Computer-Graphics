#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <malloc.h>
#include <time.h>
#include <math.h>
#include "GL\glut.h"
#include "trackball.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


GLfloat light0_emission[] = { 1.0, 1.0, 1.0, 0.0 };
GLfloat light1_emission[] = { 0.3, 0.3, 0.3, 0.0 };
GLfloat light2_emission[] = { 0.3, 0.3, 0.3, 0.0 };
GLfloat light2_ambient[] = { 0.0, 0.0, 0.2 };
GLfloat light2_day[] = { 0.8, 0.3, 0.2 };
GLfloat light2_night[] = { 0.0, 0.0, 0.2 };

static float fovy_angle = 45;
static float scale = 5;

static int left_mouse, right_mouse, middle_mouse;
static int texture_on = 1;
static int texture_min_mode = 1;
static int texture_mag_mode = 1;

static int xtrans = 0;
static int ytrans = 0;

static int mouseClick;
static int mousex, mousey;
static int zoomx, zoomy;

static int light0 = 1;
static int light1 = 0;
static int light2 = 0;

static int repeat = 1;

void changeState(void);

int curX, curY, visible;

#define MOVE_SPEED 0.2
static float move_x, move_y;
static float curquat[4];
static int doubleBuffer = 1;
static int depth = 1;


static float light_px = 1;
static float light_py = 1;
static float light_pz = 1;

static int W = 800, H = 600;
static GLint viewport[4];

unsigned int texture;
typedef struct {
	float x, y, z;
}furniture;

GLfloat tex_right = 1.0, tex_top = 1.0;

GLfloat texture_coord[][2] = {
	{ 1.0, 1.0 }, //top-right
{ 1.0, 0.0 }, //bottom-right
{ 0.0, 0.0 }, //bottom-left
{ 0.0, 1.0 } //top-left
};

int tex_faces[][3] = {
	{ 2, 1, 0 },
{ 2, 0, 3 }
};

GLfloat bed[][3] = {
	{ 0.0, 0.0, 0.6 }, // vertex 0
{ 0.35, 0.0, 0.6 }, // vertex 1
{ 0.35, 0.2, 0.6 }, // vertex 2
{ 0.0, 0.2, 0.6 }, // vertex 3
{ 0.0, 0.0, 0.0 }, // vertex 4
{ 0.35, 0.0, 0.0 }, // vertex 5
{ 0.35, 0.2, 0.0 }, //vertex 6
{ 0.0, 0.2, 0.0 } // vertex 7
};

GLfloat bed_head[][3] = {
	{ 0.0, 0.0, 0.05 }, // vertex 0
{ 0.35, 0.0, 0.05 }, // vertex 1
{ 0.35, 0.25, 0.05 }, // vertex 2
{ 0.0, 0.25, 0.05 }, // vertex 3
{ 0.0, 0.0, 0.0 }, // vertex 4
{ 0.35, 0.0, 0.0 }, // vertex 5
{ 0.35, 0.25, 0.0 }, //vertex 6
{ 0.0, 0.25, 0.0 } // vertex 7
};

GLfloat blanket[][3] = {
	{ 0.0, 0.0, 0.4 }, // vertex 0
{ 0.36, 0.0, 0.4 }, // vertex 1
{ 0.36, 0.02, 0.4 }, // vertex 2
{ 0.0, 0.02, 0.4 }, // vertex 3
{ 0.0, 0.0, 0.0 }, // vertex 4
{ 0.36, 0.0, 0.0 }, // vertex 5
{ 0.36, 0.02, 0.0 }, //vertex 6
{ 0.0, 0.02, 0.0 } // vertex 7
};

GLfloat blanket_bottom[][3] = {
	{ 0.0, 0.0, 0.01 }, // vertex 0
{ 0.36, 0.0, 0.01 }, // vertex 1
{ 0.36, 0.1, 0.01 }, // vertex 2
{ 0.0, 0.1, 0.01 }, // vertex 3
{ 0.0, 0.0, 0.0 }, // vertex 4
{ 0.36, 0.0, 0.0 }, // vertex 5
{ 0.36, 0.1, 0.0 }, //vertex 6
{ 0.0, 0.1, 0.0 } // vertex 7
};

GLfloat blanket_side[][3] = {
	{ 0.0, 0.0, 0.39 }, // vertex 0
{ 0.01, 0.0, 0.39 }, // vertex 1
{ 0.01, 0.1, 0.39 }, // vertex 2
{ 0.0, 0.1, 0.39 }, // vertex 3
{ 0.0, 0.0, 0.0 }, // vertex 4
{ 0.01, 0.0, 0.0 }, // vertex 5
{ 0.01, 0.1, 0.0 }, //vertex 6
{ 0.0, 0.1, 0.0 } // vertex 7
};

GLfloat bottom[][3] = {
	{ 0.0, 0.0, 1.0 }, // vertex 0
{ 1.2, 0.0, 1.0 }, // vertex 1
{ 1.2, 0.1, 1.0 }, // vertex 2
{ 0.0, 0.1, 1.0 }, // vertex 3
{ 0.0, 0.0, 0.0 }, // vertex 4
{ 1.2, 0.0, 0.0 }, // vertex 5
{ 1.2, 0.1, 0.0 }, //vertex 6
{ 0.0, 0.1, 0.0 } // vertex 7
};

GLfloat bottom2[][3] = {
	{ 0.0, 0.0, 0.4 }, // vertex 0
{ 0.3, 0.0, 0.4 }, // vertex 1
{ 0.3, 0.1, 0.4 }, // vertex 2
{ 0.0, 0.1, 0.4 }, // vertex 3
{ 0.0, 0.0, 0.0 }, // vertex 4
{ 0.3, 0.0, 0.0 }, // vertex 5
{ 0.3, 0.1, 0.0 }, //vertex 6
{ 0.0, 0.1, 0.0 } // vertex 7
};

GLfloat back_wall[][3] = {
	{ 0.0, 0.0, 0.1 }, // vertex 0
{ 1.4, 0.0, 0.1 }, // vertex 1
{ 1.4, 1.5, 0.1 }, // vertex 2
{ 0.0, 1.5, 0.1 }, // vertex 3
{ 0.0, 0.0, 0.0 }, // vertex 4
{ 1.4, 0.0, 0.0 }, // vertex 5
{ 1.4, 1.5, 0.0 }, //vertex 6
{ 0.0, 1.5, 0.0 } // vertex 7
};

GLfloat back_wall2[][3] = {
	{ 0.0, 0.0, 0.1 }, // vertex 0
{ 0.3, 0.0, 0.1 }, // vertex 1
{ 0.3, 1.5, 0.1 }, // vertex 2
{ 0.0, 1.5, 0.1 }, // vertex 3
{ 0.0, 0.0, 0.0 }, // vertex 4
{ 0.3, 0.0, 0.0 }, // vertex 5
{ 0.3, 1.5, 0.0 }, //vertex 6
{ 0.0, 1.5, 0.0 } // vertex 7
};

GLfloat side_wall[][3] = {
	{ 0.0, 0.0, 1.0 }, // vertex 0
{ 0.1, 0.0, 1.0 }, // vertex 1
{ 0.1, 1.5, 1.0 }, // vertex 2
{ 0.0, 1.5, 1.0 }, // vertex 3
{ 0.0, 0.0, 0.0 }, // vertex 4
{ 0.1, 0.0, 0.0 }, // vertex 5
{ 0.1, 1.5, 0.0 }, //vertex 6
{ 0.0, 1.5, 0.0 } // vertex 7
};

GLfloat side_wall2[][3] = {
	{ 0.0, 0.0, 0.6 }, // vertex 0
{ 0.1, 0.0, 0.6 }, // vertex 1
{ 0.1, 1.5, 0.6 }, // vertex 2
{ 0.0, 1.5, 0.6 }, // vertex 3
{ 0.0, 0.0, 0.0 }, // vertex 4
{ 0.1, 0.0, 0.0 }, // vertex 5
{ 0.1, 1.5, 0.0 }, //vertex 6
{ 0.0, 1.5, 0.0 } // vertex 7
};

GLfloat side_wall3[][3] = {
	{ 0.0, 0.0, 0.4 }, // vertex 0
{ 0.1, 0.0, 0.4 }, // vertex 1
{ 0.1, 1.5, 0.4 }, // vertex 2
{ 0.0, 1.5, 0.4 }, // vertex 3
{ 0.0, 0.0, 0.0 }, // vertex 4
{ 0.1, 0.0, 0.0 }, // vertex 5
{ 0.1, 1.5, 0.0 }, //vertex 6
{ 0.0, 1.5, 0.0 } // vertex 7
};


GLfloat lamp[][3] = {
	{ 0.0, 0.0, 0.35 }, // vertex 0
{ 0.2, 0.0, 0.35 }, // vertex 1
{ 0.2, 0.15, 0.35 }, // vertex 2
{ 0.0, 0.15, 0.35 }, // vertex 3
{ 0.0, 0.0, 0.0 }, // vertex 4
{ 0.2, 0.0, 0.0 }, // vertex 5
{ 0.2, 0.15, 0.0 }, //vertex 6
{ 0.0, 0.15, 0.0 } // vertex 7
};

GLfloat lamp_side[][3] = {
	{ 0.0, 0.0, 0.35 }, // vertex 0
{ 0.07, 0.0, 0.35 }, // vertex 1
{ 0.07, 0.15, 0.35 }, // vertex 2
{ 0.0, 0.15, 0.35 }, // vertex 3
{ 0.0, 0.0, 0.0 }, // vertex 4
{ 0.07, 0.0, 0.0 }, // vertex 5
{ 0.07, 0.15, 0.0 }, //vertex 6
{ 0.0, 0.15, 0.0 } // vertex 7
};

GLfloat window[][3] = {
	{ 0.0, 0.0, 0.8 }, // vertex 0
{ 0.001, 0.0, 0.8 }, // vertex 1
{ 0.001, 0.9, 0.8 }, // vertex 2
{ 0.0, 0.9, 0.8 }, // vertex 3
{ 0.0, 0.0, 0.0 }, // vertex 4
{ 0.001, 0.0, 0.0 }, // vertex 5
{ 0.001, 0.9, 0.0 }, //vertex 6
{ 0.0, 0.9, 0.0 } // vertex 7
};

GLfloat window_side[][3] = {
	{ 0.0, 0.0, 0.05 }, // vertex 0
{ 0.005, 0.0, 0.05 }, // vertex 1
{ 0.005, 0.9, 0.05 }, // vertex 2
{ 0.0, 0.9, 0.05 }, // vertex 3
{ 0.0, 0.0, 0.0 }, // vertex 4
{ 0.005, 0.0, 0.0 }, // vertex 5
{ 0.005, 0.9, 0.0 }, //vertex 6
{ 0.0, 0.9, 0.0 } // vertex 7
};

GLfloat window_top[][3] = {
	{ 0.0, 0.0, 0.9 }, // vertex 0
{ 0.005, 0.0, 0.9 }, // vertex 1
{ 0.005, 0.05, 0.9 }, // vertex 2
{ 0.0, 0.05, 0.9 }, // vertex 3
{ 0.0, 0.0, 0.0 }, // vertex 4
{ 0.005, 0.0, 0.0 }, // vertex 5
{ 0.005, 0.05, 0.0 }, //vertex 6
{ 0.0, 0.05, 0.0 } // vertex 7
};

GLfloat desk_leg[][3] = {
	{ 0.0, 0.0, 0.2 }, // vertex 0
{ 0.05, 0.0, 0.2 }, // vertex 1
{ 0.05, 0.25, 0.2 }, // vertex 2
{ 0.0, 0.25, 0.2 }, // vertex 3
{ 0.0, 0.0, 0.0 }, // vertex 4
{ 0.05, 0.0, 0.0 }, // vertex 5
{ 0.05, 0.25, 0.0 }, //vertex 6
{ 0.0, 0.25, 0.0 } // vertex 7
};

GLfloat desk[][3] = {
	{ 0.0, 0.0, 0.2 }, // vertex 0
{ 0.4, 0.0, 0.2 }, // vertex 1
{ 0.4, 0.05, 0.2 }, // vertex 2
{ 0.0, 0.05, 0.2 }, // vertex 3
{ 0.0, 0.0, 0.0 }, // vertex 4
{ 0.4, 0.0, 0.0 }, // vertex 5
{ 0.4, 0.05, 0.0 }, //vertex 6
{ 0.0, 0.05, 0.0 } // vertex 7
};

GLfloat desk_drawer[][3] = {
	{ 0.0, 0.0, 0.2 }, // vertex 0
{ 0.12, 0.0, 0.2 }, // vertex 1
{ 0.12, 0.05, 0.2 }, // vertex 2
{ 0.0, 0.05, 0.2 }, // vertex 3
{ 0.0, 0.0, 0.0 }, // vertex 4
{ 0.12, 0.0, 0.0 }, // vertex 5
{ 0.12, 0.05, 0.0 }, //vertex 6
{ 0.0, 0.05, 0.0 } // vertex 7
};

GLfloat desk_back[][3] = {
	{ 0.0, 0.0, 0.01 }, // vertex 0
{ 0.4, 0.0, 0.01 }, // vertex 1
{ 0.4, 0.25, 0.01 }, // vertex 2
{ 0.0, 0.25, 0.01 }, // vertex 3
{ 0.0, 0.0, 0.0 }, // vertex 4
{ 0.4, 0.0, 0.0 }, // vertex 5
{ 0.4, 0.25, 0.0 }, //vertex 6
{ 0.0, 0.25, 0.0 } // vertex 7
};

GLfloat desk_side[][3] = {
	{ 0.0, 0.0, 0.08 }, // vertex 0
{ 0.01, 0.0, 0.08 }, // vertex 1
{ 0.01, 0.25, 0.08 }, // vertex 2
{ 0.0, 0.25, 0.08 }, // vertex 3
{ 0.0, 0.0, 0.0 }, // vertex 4
{ 0.01, 0.0, 0.0 }, // vertex 5
{ 0.01, 0.25, 0.0 }, //vertex 6
{ 0.0, 0.25, 0.0 } // vertex 7
};

GLfloat desk_board[][3] = {
	{ 0.0, 0.0, 0.08 }, // vertex 0
{ 0.09, 0.0, 0.08 }, // vertex 1
{ 0.09, 0.01, 0.08 }, // vertex 2
{ 0.0, 0.01, 0.08 }, // vertex 3
{ 0.0, 0.0, 0.0 }, // vertex 4
{ 0.09, 0.0, 0.0 }, // vertex 5
{ 0.09, 0.01, 0.0 }, //vertex 6
{ 0.0, 0.01, 0.0 } // vertex 7
};

GLfloat desk_mboard[][3] = {
	{ 0.0, 0.0, 0.08 }, // vertex 0
{ 0.18, 0.0, 0.08 }, // vertex 1
{ 0.18, 0.01, 0.08 }, // vertex 2
{ 0.0, 0.01, 0.08 }, // vertex 3
{ 0.0, 0.0, 0.0 }, // vertex 4
{ 0.18, 0.0, 0.0 }, // vertex 5
{ 0.18, 0.01, 0.0 }, //vertex 6
{ 0.0, 0.01, 0.0 } // vertex 7
};

GLfloat dressing[][3] = {
	{ 0.0, 0.0, 0.1 }, // vertex 0
{ 0.2, 0.0, 0.1 }, // vertex 1
{ 0.2, 0.05, 0.1 }, // vertex 2
{ 0.0, 0.05, 0.1 }, // vertex 3
{ 0.0, 0.0, 0.0 }, // vertex 4
{ 0.2, 0.0, 0.0 }, // vertex 5
{ 0.2, 0.05, 0.0 }, //vertex 6
{ 0.0, 0.05, 0.0 } // vertex 7
};

GLfloat dressing_leg[][3] = {
	{ 0.0, 0.0, 0.03 }, // vertex 0
{ 0.03, 0.0, 0.03 }, // vertex 1
{ 0.03, 0.2, 0.03 }, // vertex 2
{ 0.0, 0.2, 0.03 }, // vertex 3
{ 0.0, 0.0, 0.0 }, // vertex 4
{ 0.03, 0.0, 0.0 }, // vertex 5
{ 0.03, 0.2, 0.0 }, //vertex 6
{ 0.0, 0.2, 0.0 } // vertex 7
};

GLfloat book_back[][3] = {
	{ 0.0, 0.0, 0.01 }, // vertex 0
{ 0.25, 0.0, 0.01 }, // vertex 1
{ 0.25, 0.5, 0.01 }, // vertex 2
{ 0.0, 0.5, 0.01 }, // vertex 3
{ 0.0, 0.0, 0.0 }, // vertex 4
{ 0.25, 0.0, 0.0 }, // vertex 5
{ 0.25, 0.5, 0.0 }, //vertex 6
{ 0.0, 0.5, 0.0 } // vertex 7
};

GLfloat book_top[][3] = {
	{ 0.0, 0.0, 0.12 }, // vertex 0
{ 0.24, 0.0, 0.12 }, // vertex 1
{ 0.24, 0.03, 0.12 }, // vertex 2
{ 0.0, 0.03, 0.12 }, // vertex 3
{ 0.0, 0.0, 0.0 }, // vertex 4
{ 0.24, 0.0, 0.0 }, // vertex 5
{ 0.24, 0.03, 0.0 }, //vertex 6
{ 0.0, 0.03, 0.0 } // vertex 7
};

GLfloat book_side[][3] = {
	{ 0.0, 0.0, 0.12 }, // vertex 0
{ 0.005, 0.0, 0.12 }, // vertex 1
{ 0.005, 0.5, 0.12 }, // vertex 2
{ 0.0, 0.5, 0.12 }, // vertex 3
{ 0.0, 0.0, 0.0 }, // vertex 4
{ 0.005, 0.0, 0.0 }, // vertex 5
{ 0.005, 0.5, 0.0 }, //vertex 6
{ 0.0, 0.5, 0.0 } // vertex 7
};

GLfloat door[][3] = {
	{ 0.0, 0.0, 0.3 }, // vertex 0
{ 0.01, 0.0, 0.3 }, // vertex 1
{ 0.01, 0.8, 0.3 }, // vertex 2
{ 0.0, 0.8, 0.3 }, // vertex 3
{ 0.0, 0.0, 0.0 }, // vertex 4
{ 0.01, 0.0, 0.0 }, // vertex 5
{ 0.01, 0.8, 0.0 }, //vertex 6
{ 0.0, 0.8, 0.0 } // vertex 7
};

GLfloat closet[][3] = {
	{ 0.0, 0.0, 0.15 }, // vertex 0
{ 0.01, 0.0, 0.15 }, // vertex 1
{ 0.01, 0.85, 0.15 }, // vertex 2
{ 0.0, 0.85, 0.15 }, // vertex 3
{ 0.0, 0.0, 0.0 }, // vertex 4
{ 0.01, 0.0, 0.0 }, // vertex 5
{ 0.01, 0.85, 0.0 }, //vertex 6
{ 0.0, 0.85, 0.0 } // vertex 7
};


GLfloat mirror[][3] = {
	{ 0.0, 0.0, 0.01 }, // vertex 0
{ 0.2, 0.0, 0.01 }, // vertex 1
{ 0.2, 0.35, 0.01 }, // vertex 2
{ 0.0, 0.35, 0.01 }, // vertex 3
{ 0.0, 0.0, 0.0 }, // vertex 4
{ 0.2, 0.0, 0.0 }, // vertex 5
{ 0.2, 0.35, 0.0 }, //vertex 6
{ 0.0, 0.35, 0.0 } // vertex 7
};

int faces[][3] = {
	{ 0, 1, 2 }, //face1
{ 0, 2, 3 },
{ 1, 5, 6 }, //face2
{ 1, 6, 2 },
{ 3, 2, 6 }, //face3
{ 3, 6, 7 },
{ 4, 0, 3 }, //face4
{ 4, 3, 7 },
{ 1, 0, 4 }, //face5
{ 1, 4, 5 },
{ 5, 4, 7 }, //face6
{ 5, 7, 6 }
};
GLfloat normals[][3] = {
	{ 0.0, 0.0, 1.0 }, //+z
{ 1.0, 0.0, 0.0 }, //+x
{ 0.0, 1.0, 0.0 }, //+y
{ -1.0, 0.0, 0.0 }, //-x
{ 0.0, -1.0, 0.0 }, //-y
{ 0.0, 0.0, -1.0 } //-z
};

int width[13], height[15], nrChannels[15];
unsigned char *data[15];

void initTexture()
{
	//바닥
	data[0] = stbi_load("texture/bottom.jpg", &width[0], &height[0], &nrChannels[0], 0);
	//벽
	data[1] = stbi_load("texture/wall2.jpg", &width[1], &height[1], &nrChannels[1], 0);
	//침대
	data[2] = stbi_load("texture/bed.jpg", &width[2], &height[2], &nrChannels[2], 0);
	//전등1
	data[3] = stbi_load("texture/light.jpg", &width[3], &height[3], &nrChannels[3], 0);
	//천장
	data[4] = stbi_load("texture/walltop.jpg", &width[4], &height[4], &nrChannels[4], 0);
	//책장
	data[5] = stbi_load("texture/bookcase.jpg", &width[5], &height[5], &nrChannels[5], 0);
	//거울
	data[6] = stbi_load("texture/mirror.jpg", &width[6], &height[6], &nrChannels[6], 0);
	//창문
	data[7] = stbi_load("texture/window.jpg", &width[7], &height[7], &nrChannels[7], 0);
	//이불
	data[8] = stbi_load("texture/blanket.jpg", &width[8], &height[8], &nrChannels[8], 0);
	//창틀
	data[9] = stbi_load("texture/window2.jpg", &width[9], &height[9], &nrChannels[9], 0);
	//화장대다리
	data[10] = stbi_load("texture/dressing_leg.jpg", &width[10], &height[10], &nrChannels[10], 0);
	//문
	data[11] = stbi_load("texture/door.jpg", &width[11], &height[11], &nrChannels[11], 0);
	//장롱
	data[12] = stbi_load("texture/closet.jpg", &width[12], &height[12], &nrChannels[12], 0);
	//장롱2
	data[13] = stbi_load("texture/closet2.jpg", &width[13], &height[13], &nrChannels[13], 0);

}
float* calcNormal(float v1[3], float v2[3], float v3[3])
{
	float vec1[3], vec2[3];
	static const int x = 0;
	static const int y = 1;
	static const int z = 2;
	float out[3];

	// Calculate two vectors from the three points
	vec1[x] = v1[x] - v2[x];
	vec1[y] = v1[y] - v2[y];
	vec1[z] = v1[z] - v2[z];

	vec2[x] = v3[x] - v2[x];
	vec2[y] = v3[y] - v2[y];
	vec2[z] = v3[z] - v2[z];

	// Take the cross product of the two vectors to get
	// the normal vector which will be stored in out[]
	out[x] = vec2[y] * vec1[z] - vec2[z] * vec1[y];
	out[y] = vec2[z] * vec1[x] - vec2[x] * vec1[z];
	out[z] = vec2[x] * vec1[y] - vec2[y] * vec1[x];

	float length = sqrt(out[0] * out[0] + out[1] * out[1] + out[2] * out[2]);
	out[x] *= 1.0 / length;
	out[y] *= 1.0 / length;
	out[z] *= 1.0 / length;

	//printf("x=%f y=%f z=%f\n", out[x], out[y], out[z]);
	// Normalize the vector (shorten length to one)
	return out;
}

#define GL_PI 3.1415f
void createCylinder(GLfloat centerx, GLfloat centery, GLfloat centerz, GLfloat radius, GLfloat h)
{

	/* function createCyliner()
	원기둥의 중심 x,y,z좌표, 반지름, 높이를 받아 원기둥을 생성하는 함수(+z방향으로 원에서 늘어남)
	centerx : 원기둥 원의 중심 x좌표
	centery : 원기둥 원의 중심 y좌표
	centerz : 원기둥 원의 중심 z좌표
	radius : 원기둥의 반지름
	h : 원기둥의 높이
	*/
	GLfloat x, y, angle;

	glBegin(GL_TRIANGLE_FAN);           //원기둥의 윗면
	glNormal3f(0.0f, 0.0f, -1.0f);
	glColor3ub(139, 69, 19);
	glVertex3f(centerx, centery, centerz);


	for (angle = 0.0f; angle < (2.0f*GL_PI); angle += (GL_PI / 8.0f))
	{
		x = centerx + radius * sin(angle);
		y = centery + radius * cos(angle);
		glNormal3f(0.0f, 0.0f, -1.0f);
		glVertex3f(x, y, centerz);
	}
	glEnd();

	glBegin(GL_QUAD_STRIP);            //원기둥의 옆면
	for (angle = 0.0f; angle < (2.0f*GL_PI); angle += (GL_PI / 8.0f))
	{
		x = centerx + radius * sin(angle);
		y = centery + radius * cos(angle);
		glNormal3f(sin(angle), cos(angle), 0.0f);
		glVertex3f(x, y, centerz);
		glVertex3f(x, y, centerz + h);
	}
	glEnd();

	glBegin(GL_TRIANGLE_FAN);           //원기둥의 밑면
	glNormal3f(0.0f, 0.0f, 1.0f);
	glVertex3f(centerx, centery, centerz + h);
	for (angle = (2.0f*GL_PI); angle > 0.0f; angle -= (GL_PI / 8.0f))
	{
		x = centerx + radius * sin(angle);
		y = centery + radius * cos(angle);
		glNormal3f(0.0f, 0.0f, 1.0f);
		glVertex3f(x, y, centerz + h);
	}
	glEnd();
}

void createHalfSphere(GLfloat x, GLfloat y, GLfloat z, GLfloat radius)
{     /* function createHalfSphere()
	  구의 중심 x, y, z 좌표를 받아 반구를 만드는 함수
	  x : 반구의 중심 x 좌표
	  y : 반구의 중심 y 좌표
	  z : 반구의 중심 z 좌표
	  raidus : 반구의 반지름     */
	GLfloat angley; //y축 값을 구하기 위한 각도  
	GLfloat nexty; //다음 y축 값을 구하기 위한 각도  
	GLfloat anglex; //x, y축 값을 구하기 위한 각도    
	glColor3ub(136, 206, 250); //반구의 색 지정     
	glBegin(GL_QUAD_STRIP);
	for (angley = 0.0f; angley <= (0.5f*GL_PI); angley += ((0.5f*GL_PI) / 8.0f)) //반구만 그려야 하므로 0.5곱함    
	{
		y = radius * sin(angley); //y축 값 계산      
		nexty = angley + ((0.5f*GL_PI) / 8.0f); //다음 angley값 저장     
		for (anglex = 0.0f; anglex < (2.0f*GL_PI); anglex += (GL_PI / 8.0f))
		{
			x = radius * cos(angley)*sin(anglex);
			z = radius * cos(angley)*cos(anglex);
			glNormal3f(-cos(angley)*sin(anglex), -sin(angley), -cos(angley)*cos(anglex));
			//반구의 안쪽으로 normal 벡터 생성            
			glVertex3f(x, y, z);
			x = radius * cos(nexty)*sin(anglex);
			z = radius * cos(nexty)*cos(anglex);
			glNormal3f(-cos(nexty)*sin(anglex), -sin(nexty), -cos(nexty)*cos(anglex));
			glVertex3f(x, radius*sin(nexty), z);
		}
	}
	glEnd();
}



void drawWall()
{


	glBindTexture(GL_TEXTURE_2D, texture);

	if (data[0])
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width[0], height[0], 0, GL_RGB, GL_UNSIGNED_BYTE, data[0]);

	}
	else
	{
		printf("Failed to load texture\n");
	}
	//stbi_image_free(data);

	GLfloat mat_ambient[] = { 0.6, 0.4, 0.4, 1.0 };
	GLfloat mat_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat mat_shininess[] = { 5 };
	GLfloat mat_emission[] = { 0.33, 0.33, 0.33, 0.0 };

	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
	glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission);



	//밑판
	glPushMatrix();
	glTranslatef(0.0, -0.1, 0.0);
	glBegin(GL_TRIANGLES);
	for (int i = 0; i < 6; i++) {
		//v = calcNormal(bottom[faces[i * 2][0]], bottom[faces[i * 2][1]], bottom[faces[i * 2][2]]);
		//glNormal3fv(v);

		glNormal3fv(normals[i]);
		for (int j = 0; j < 2; j++) {
			for (int k = 0; k < 3; k++) {
				glTexCoord2f(texture_coord[tex_faces[j][k]][0]*2, texture_coord[tex_faces[j][k]][1]*2);
				glVertex3fv(bottom[faces[2 * i + j][k]]);
			}
		}

	}
	glEnd();


	//오목 밑판
	glPushMatrix();
	glTranslatef(1.2, 0.0, 0.6);
	glBegin(GL_TRIANGLES);
	for (int i = 0; i < 6; i++) {
		glNormal3fv(normals[i]);
		for (int j = 0; j < 2; j++) {
			for (int k = 0; k < 3; k++) {
				glTexCoord2f(texture_coord[tex_faces[j][k]][0] / 2, texture_coord[tex_faces[j][k]][1] / 2);
				glVertex3fv(bottom2[faces[2 * i + j][k]]);
			}
		}

	}
	glEnd();
	glPopMatrix();

	if (data[1])
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width[1], height[1], 0, GL_RGB, GL_UNSIGNED_BYTE, data[1]);

	}
	else
	{
		printf("Failed to load texture\n");
	}

	GLfloat mat_ambient1[] = { 0.6, 0.6, 0.4, 1.0 };
	GLfloat mat_emission1[] = { 0.34, 0.34, 0.34, 0.0 };
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient1);
	glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission1);
	//뒷벽
	glPushMatrix();
	glTranslatef(-0.1, 0.0, -0.1);
	glBegin(GL_TRIANGLES);
	for (int i = 0; i < 6; i++) {
		glNormal3fv(normals[i]);
		for (int j = 0; j < 2; j++) {
			for (int k = 0; k < 3; k++) {
				glTexCoord2f(texture_coord[tex_faces[j][k]][0] * 3, texture_coord[tex_faces[j][k]][1]);
				glVertex3fv(back_wall[faces[2 * i + j][k]]);
			}
		}

	}
	glEnd();

	//오목 뒷벽
	glTranslatef(1.4, 0.0, 0.6);
	glBegin(GL_TRIANGLES);
	for (int i = 0; i < 6; i++) {
		glNormal3fv(normals[i]);
		for (int j = 0; j < 2; j++) {
			for (int k = 0; k < 3; k++) {
				glTexCoord2fv(texture_coord[tex_faces[j][k]]);
				glVertex3fv(back_wall2[faces[2 * i + j][k]]);
			}
		}

	}
	glEnd();
	glPopMatrix();

	//옆벽
	GLfloat mat_emission2[] = { 0.35, 0.35, 0.35, 0.0 };
	glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission1);
	glPushMatrix();
	glTranslatef(-0.1, 0.0, 0.0);
	glBegin(GL_TRIANGLES);
	for (int i = 0; i < 6; i++) {
		glNormal3fv(normals[i]);
		for (int j = 0; j < 2; j++) {
			for (int k = 0; k < 3; k++) {
				glTexCoord2f(texture_coord[tex_faces[j][k]][0] * 2, texture_coord[tex_faces[j][k]][1]);
				glVertex3fv(side_wall[faces[2 * i + j][k]]);
			}
		}

	}
	glEnd();


	//오른쪽 벽
	glTranslatef(1.3, 0.0, 0.0);
	glPushMatrix();
	glBegin(GL_TRIANGLES);
	for (int i = 0; i < 6; i++) {
		glNormal3fv(normals[i]);
		for (int j = 0; j < 2; j++) {
			for (int k = 0; k < 3; k++) {
				glTexCoord2f(texture_coord[tex_faces[j][k]][0] / 1.5, texture_coord[tex_faces[j][k]][1]);
				glVertex3fv(side_wall2[faces[2 * i + j][k]]);
			}
		}

	}
	glEnd();
	glPopMatrix();

	//오목벽
	glTranslatef(0.3, 0.0, 0.6);
	glPushMatrix();
	glBegin(GL_TRIANGLES);
	for (int i = 0; i < 6; i++) {
		glNormal3fv(normals[i]);
		for (int j = 0; j < 2; j++) {
			for (int k = 0; k < 3; k++) {
				glTexCoord2fv(texture_coord[tex_faces[j][k]]);
				glVertex3fv(side_wall3[faces[2 * i + j][k]]);
			}
		}

	}
	glEnd();
	glPopMatrix();

	glPopMatrix();


	GLfloat mat_emission3[] = { 0.4, 0.4, 0.4, 0.0 };
	glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission3);
	glTranslatef(0.0, 1.4, 0.0);
	//윗판
	glBegin(GL_TRIANGLES);
	for (int i = 0; i < 6; i++) {
		glNormal3fv(normals[i]);

		for (int j = 0; j < 2; j++) {
			for (int k = 0; k < 3; k++) {
				glTexCoord2f(texture_coord[tex_faces[j][k]][0] * 2.5, texture_coord[tex_faces[j][k]][1]);
				glVertex3fv(bottom[faces[2 * i + j][k]]);
			}
		}

	}
	glEnd();

	glTranslatef(1.2, 0.0, 0.6);
	//오목 윗판
	glBegin(GL_TRIANGLES);
	for (int i = 0; i < 6; i++) {
		glNormal3fv(normals[i]);

		for (int j = 0; j < 2; j++) {
			for (int k = 0; k < 3; k++) {
				glTexCoord2f(texture_coord[tex_faces[j][k]][0] / 1.5, texture_coord[tex_faces[j][k]][1]);
				glVertex3fv(bottom2[faces[2 * i + j][k]]);
			}
		}

	}
	glEnd();
	glPopMatrix();
	glBindTexture(GL_TEXTURE_2D, 0);
}

void drawLamp()
{
	glBindTexture(GL_TEXTURE_2D, texture);

	if (data[3])
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width[3], height[3], 0, GL_RGB, GL_UNSIGNED_BYTE, data[3]);

	}
	else
	{
		printf("Failed to load texture\n");
	}

	glPushMatrix();
	GLfloat mat_ambient[] = { 0.8, 0.8, 0.8, 1.0 };
	GLfloat mat_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat mat_shininess[] = { 5 };

	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
	glMaterialfv(GL_FRONT, GL_EMISSION, light0_emission);

	//램프
	glPushMatrix();
	glTranslatef(0.55, 1.24, 0.45);
	glBegin(GL_TRIANGLES);
	for (int i = 0; i < 6; i++) {
		//v = calcNormal(bottom[faces[i * 2][0]], bottom[faces[i * 2][1]], bottom[faces[i * 2][2]]);
		//glNormal3fv(v);

		glNormal3fv(normals[i]);
		for (int j = 0; j < 2; j++) {
			for (int k = 0; k < 3; k++) {
				glTexCoord2f(texture_coord[tex_faces[j][k]][0], texture_coord[tex_faces[j][k]][1]);
				glVertex3fv(lamp_side[faces[2 * i + j][k]]);
			}
		}

	}
	glEnd();

	glTranslatef(0.08, 0.0, 0.0);
	glBegin(GL_TRIANGLES);
	for (int i = 0; i < 6; i++) {
		//v = calcNormal(bottom[faces[i * 2][0]], bottom[faces[i * 2][1]], bottom[faces[i * 2][2]]);
		//glNormal3fv(v);

		glNormal3fv(normals[i]);
		for (int j = 0; j < 2; j++) {
			for (int k = 0; k < 3; k++) {
				glTexCoord2f(texture_coord[tex_faces[j][k]][0], texture_coord[tex_faces[j][k]][1]);
				glVertex3fv(lamp[faces[2 * i + j][k]]);
			}
		}

	}
	glEnd();

	glTranslatef(0.21, 0.0, 0.0);
	glBegin(GL_TRIANGLES);
	for (int i = 0; i < 6; i++) {
		//v = calcNormal(bottom[faces[i * 2][0]], bottom[faces[i * 2][1]], bottom[faces[i * 2][2]]);
		//glNormal3fv(v);

		glNormal3fv(normals[i]);
		for (int j = 0; j < 2; j++) {
			for (int k = 0; k < 3; k++) {
				glTexCoord2f(texture_coord[tex_faces[j][k]][0], texture_coord[tex_faces[j][k]][1]);
				glVertex3fv(lamp_side[faces[2 * i + j][k]]);
			}
		}

	}
	glEnd();
	glPopMatrix();

	glBindTexture(GL_TEXTURE_2D, 0);
	// 보조등
	GLfloat mat_ambient1[] = { 0.7, 0.7, 0.7, 1.0 };

	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient1);
	glMaterialfv(GL_FRONT, GL_EMISSION, light1_emission);

	glPushMatrix();
	glTranslatef(0.18, 1.3, 0.45);
	glRotatef(90, 1.0, 0.0, 0.0);
	createCylinder(0.0, 0.0, 0.0, 0.15, 0.02);
	glPopMatrix();

	glBindTexture(GL_TEXTURE_2D, texture);

	if (data[7])
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width[7], height[7], 0, GL_RGB, GL_UNSIGNED_BYTE, data[7]);

	}
	else
	{
		printf("Failed to load texture\n");
	}
	// 창문
	glMaterialfv(GL_FRONT, GL_AMBIENT, light2_ambient);
	glMaterialfv(GL_FRONT, GL_EMISSION, light2_emission);

	glPushMatrix();
	glTranslatef(0.0, 0.3, 0.1);
	glBegin(GL_TRIANGLES);
	for (int i = 0; i < 6; i++) {
		glNormal3fv(normals[i]);
		for (int j = 0; j < 2; j++) {
			for (int k = 0; k < 3; k++) {
				glTexCoord2f(texture_coord[tex_faces[j][k]][0], texture_coord[tex_faces[j][k]][1]);
				glVertex3fv(window[faces[2 * i + j][k]]);
			}
		}

	}
	glEnd();
	glPopMatrix();

	glBindTexture(GL_TEXTURE_2D, texture);

	if (data[9])
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width[9], height[9], 0, GL_RGB, GL_UNSIGNED_BYTE, data[9]);

	}
	else
	{
		printf("Failed to load texture\n");
	}
	//창틀
	GLfloat mat_ambient3[] = { 0.8, 0.8, 1.0, 1.0 };
	GLfloat mat_emission3[] = { 0.3, 0.3, 0.3, 0.0 };

	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient3);
	glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission3);

	glPushMatrix();
	glTranslatef(0.0, 0.3, 0.05);
	glBegin(GL_TRIANGLES);
	for (int i = 0; i < 6; i++) {
		glNormal3fv(normals[i]);
		for (int j = 0; j < 2; j++) {
			for (int k = 0; k < 3; k++) {
				glTexCoord2f(texture_coord[tex_faces[j][k]][0], texture_coord[tex_faces[j][k]][1]);
				glVertex3fv(window_side[faces[2 * i + j][k]]);
			}
		}
	}
	glEnd();

	glTranslatef(0.0, 0.0, 0.425);
	glBegin(GL_TRIANGLES);
	for (int i = 0; i < 6; i++) {
		glNormal3fv(normals[i]);
		for (int j = 0; j < 2; j++) {
			for (int k = 0; k < 3; k++) {
				glTexCoord2f(texture_coord[tex_faces[j][k]][0], texture_coord[tex_faces[j][k]][1]);
				glVertex3fv(window_side[faces[2 * i + j][k]]);
			}
		}
	}
	glEnd();

	glTranslatef(0.0, 0.0, 0.425);
	glBegin(GL_TRIANGLES);
	for (int i = 0; i < 6; i++) {
		glNormal3fv(normals[i]);
		for (int j = 0; j < 2; j++) {
			for (int k = 0; k < 3; k++) {
				glTexCoord2f(texture_coord[tex_faces[j][k]][0], texture_coord[tex_faces[j][k]][1]);
				glVertex3fv(window_side[faces[2 * i + j][k]]);
			}
		}
	}
	glEnd();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0, 0.25, 0.05);
	glBegin(GL_TRIANGLES);
	for (int i = 0; i < 6; i++) {
		glNormal3fv(normals[i]);
		for (int j = 0; j < 2; j++) {
			for (int k = 0; k < 3; k++) {
				glTexCoord2f(texture_coord[tex_faces[j][k]][0], texture_coord[tex_faces[j][k]][1]);
				glVertex3fv(window_top[faces[2 * i + j][k]]);
			}
		}
	}
	glEnd();

	glTranslatef(0.0, 0.9, 0.0);
	glBegin(GL_TRIANGLES);
	for (int i = 0; i < 6; i++) {
		glNormal3fv(normals[i]);
		for (int j = 0; j < 2; j++) {
			for (int k = 0; k < 3; k++) {
				glTexCoord2f(texture_coord[tex_faces[j][k]][0], texture_coord[tex_faces[j][k]][1]);
				glVertex3fv(window_top[faces[2 * i + j][k]]);
			}
		}
	}
	glEnd();
	glPopMatrix();

	glPopMatrix();
	glBindTexture(GL_TEXTURE_2D, 0);

}

void drawBed()
{
	GLfloat mat_ambient[] = { 0.5, 0.4, 0.05, 1.0 };
	GLfloat mat_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat mat_shininess[] = { 5 };
	GLfloat mat_emission[] = { 0.3, 0.3, 0.3, 0.0 };

	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
	glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission);

	glBindTexture(GL_TEXTURE_2D, texture);

	if (data[2])
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width[2], height[2], 0, GL_RGB, GL_UNSIGNED_BYTE, data[2]);

	}
	else
	{
		printf("Failed to load texture\n");
	}

	//bed
	glPushMatrix();
	glBegin(GL_TRIANGLES);

	for (int i = 0; i < 6; i++) {
		glNormal3fv(normals[i]);

		for (int j = 0; j < 2; j++) {
			for (int k = 0; k < 3; k++) {
				glTexCoord2f(texture_coord[tex_faces[j][k]][0] * 2, texture_coord[tex_faces[j][k]][1] * 2);
				glVertex3fv(bed[faces[2 * i + j][k]]);
			}
		}

	}
	glEnd();

	glTranslatef(0.0, 0.2, 0.0);
	glBegin(GL_TRIANGLES);
	for (int i = 0; i < 6; i++) {
		glNormal3fv(normals[i]);

		for (int j = 0; j < 2; j++) {
			for (int k = 0; k < 3; k++) {
				glTexCoord2f(texture_coord[tex_faces[j][k]][0] * 2, texture_coord[tex_faces[j][k]][1]);
				glVertex3fv(bed_head[faces[2 * i + j][k]]);
			}
		}

	}
	glEnd();

	glBindTexture(GL_TEXTURE_2D, 0);
	//베개
	glPushMatrix();
	GLfloat mat_ambient2[] = { 0.8, 0.8, 0.8, 1.0 };
	GLfloat mat_diffuse2[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat mat_specular2[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat mat_shininess2[] = { 32 };
	GLfloat mat_emission2[] = { 0.5, 0.5, 0.5, 0.0 };

	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient2);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse2);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular2);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess2);
	glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission2);

	glTranslatef(0.1, 0.02, 0.15);
	glRotatef(90, 0.0, 1.0, 0.0);
	glScalef(1.0, 0.5, 1.0);
	createCylinder(0.0, 0.0, 0.0, 0.05, 0.15);
	glPopMatrix();

	glBindTexture(GL_TEXTURE_2D, texture);

	if (data[8])
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width[8], height[8], 0, GL_RGB, GL_UNSIGNED_BYTE, data[8]);

	}
	else
	{
		printf("Failed to load texture\n");
	}

	//이불
	GLfloat mat_ambient3[] = { 0.8, 0.4, 0.2, 1.0 };
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient3);

	glTranslatef(0.0, 0.0, 0.21);
	glBegin(GL_TRIANGLES);
	for (int i = 0; i < 6; i++) {
		glNormal3fv(normals[i]);

		for (int j = 0; j < 2; j++) {
			for (int k = 0; k < 3; k++) {
				glTexCoord2f(texture_coord[tex_faces[j][k]][0], texture_coord[tex_faces[j][k]][1]);
				glVertex3fv(blanket[faces[2 * i + j][k]]);
			}
		}

	}
	glEnd();

	glPushMatrix();
	glTranslatef(0.0, -0.1, 0.39);
	glBegin(GL_TRIANGLES);
	for (int i = 0; i < 6; i++) {
		glNormal3fv(normals[i]);
		for (int j = 0; j < 2; j++) {
			for (int k = 0; k < 3; k++) {
				glTexCoord2f(texture_coord[tex_faces[j][k]][0], texture_coord[tex_faces[j][k]][1] * 0.3);
				glVertex3fv(blanket_bottom[faces[2 * i + j][k]]);
			}
		}

	}
	glEnd();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.35, -0.1, 0.0);
	glBegin(GL_TRIANGLES);
	for (int i = 0; i < 6; i++) {
		glNormal3fv(normals[i]);
		for (int j = 0; j < 2; j++) {
			for (int k = 0; k < 3; k++) {
				glTexCoord2f(texture_coord[tex_faces[j][k]][0], texture_coord[tex_faces[j][k]][1] * 0.3);
				glVertex3fv(blanket_side[faces[2 * i + j][k]]);
			}
		}

	}
	glEnd();
	glPopMatrix();

	glPopMatrix();
	glBindTexture(GL_TEXTURE_2D, 0);
}

void drawDesk()
{
	glBindTexture(GL_TEXTURE_2D, texture);

	if (data[2])
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width[2], height[2], 0, GL_RGB, GL_UNSIGNED_BYTE, data[2]);

	}
	else
	{
		printf("Failed to load texture\n");
	}

	GLfloat mat_ambient[] = { 0.5, 0.2, 0.0, 1.0 };
	GLfloat mat_diffuse[] = { 0.5, 0.5, 0.5, 1.0 };
	GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat mat_shininess[] = { 32 };
	GLfloat mat_emission[] = { 0.3, 0.3, 0.3, 0.0 };

	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
	glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission);

	// desk
	glPushMatrix();
	glTranslatef(0.35, 0.0, 0.0);
	glPushMatrix();
	//desk leg1
	glBegin(GL_TRIANGLES);
	for (int i = 0; i < 6; i++) {
		glNormal3fv(normals[i]);
		for (int j = 0; j < 2; j++) {
			for (int k = 0; k < 3; k++) {
				glTexCoord2f(texture_coord[tex_faces[j][k]][0] * 0.1, texture_coord[tex_faces[j][k]][1] * 0.4);
				glVertex3fv(desk_leg[faces[2 * i + j][k]]);
			}
		}

	}
	glEnd();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0, 0.25, 0.0);
	//desk top
	glBegin(GL_TRIANGLES);
	for (int i = 0; i < 6; i++) {
		glNormal3fv(normals[i]);
		for (int j = 0; j < 2; j++) {
			for (int k = 0; k < 3; k++) {
				glTexCoord2f(texture_coord[tex_faces[j][k]][0] * 0.2, texture_coord[tex_faces[j][k]][1] * 0.2);
				glVertex3fv(desk[faces[2 * i + j][k]]);
			}
		}

	}
	glEnd();

	glTranslatef(0.0, 0.05, 0.0);
	//desk back
	glBegin(GL_TRIANGLES);
	for (int i = 0; i < 6; i++) {
		glNormal3fv(normals[i]);
		for (int j = 0; j < 2; j++) {
			for (int k = 0; k < 3; k++) {
				glTexCoord2f(texture_coord[tex_faces[j][k]][0] * 0.5, texture_coord[tex_faces[j][k]][1] * 0.3);
				glVertex3fv(desk_back[faces[2 * i + j][k]]);
			}
		}

	}
	glEnd();

	glTranslatef(0.0, 0.0, 0.01);
	//desk side1
	glBegin(GL_TRIANGLES);
	for (int i = 0; i < 6; i++) {
		glNormal3fv(normals[i]);
		for (int j = 0; j < 2; j++) {
			for (int k = 0; k < 3; k++) {
				glTexCoord2f(texture_coord[tex_faces[j][k]][0] * 0.4, texture_coord[tex_faces[j][k]][1] * 0.2);
				glVertex3fv(desk_side[faces[2 * i + j][k]]);
			}
		}

	}
	glEnd();

	glPushMatrix();
	glTranslatef(0.01, 0.05, 0.0);
	//desk board1
	glBegin(GL_TRIANGLES);
	for (int i = 0; i < 6; i++) {
		glNormal3fv(normals[i]);
		for (int j = 0; j < 2; j++) {
			for (int k = 0; k < 3; k++) {
				glTexCoord2f(texture_coord[tex_faces[j][k]][0] * 0.2, texture_coord[tex_faces[j][k]][1] * 0.2);
				glVertex3fv(desk_board[faces[2 * i + j][k]]);
			}
		}

	}
	glEnd();

	glTranslatef(0.0, 0.12, 0.0);
	//desk board2
	glBegin(GL_TRIANGLES);
	for (int i = 0; i < 6; i++) {
		glNormal3fv(normals[i]);
		for (int j = 0; j < 2; j++) {
			for (int k = 0; k < 3; k++) {
				glTexCoord2f(texture_coord[tex_faces[j][k]][0] * 0.2, texture_coord[tex_faces[j][k]][1] * 0.2);
				glVertex3fv(desk_board[faces[2 * i + j][k]]);
			}
		}

	}
	glEnd();
	glPopMatrix();

	glTranslatef(0.1, 0.0, 0.0);
	//desk side2
	glBegin(GL_TRIANGLES);
	for (int i = 0; i < 6; i++) {
		glNormal3fv(normals[i]);
		for (int j = 0; j < 2; j++) {
			for (int k = 0; k < 3; k++) {
				glTexCoord2f(texture_coord[tex_faces[j][k]][0] * 0.4, texture_coord[tex_faces[j][k]][1] * 0.2);
				glVertex3fv(desk_side[faces[2 * i + j][k]]);
			}
		}

	}
	glEnd();

	glPushMatrix();
	glTranslatef(0.01, 0.08, 0.0);
	//desk mboard1
	glBegin(GL_TRIANGLES);
	for (int i = 0; i < 6; i++) {
		glNormal3fv(normals[i]);
		for (int j = 0; j < 2; j++) {
			for (int k = 0; k < 3; k++) {
				glTexCoord2f(texture_coord[tex_faces[j][k]][0] * 0.2, texture_coord[tex_faces[j][k]][1] * 0.2);
				glVertex3fv(desk_mboard[faces[2 * i + j][k]]);
			}
		}

	}
	glEnd();

	glTranslatef(0.0, 0.07, 0.0);
	//desk mboard2
	glBegin(GL_TRIANGLES);
	for (int i = 0; i < 6; i++) {
		glNormal3fv(normals[i]);
		for (int j = 0; j < 2; j++) {
			for (int k = 0; k < 3; k++) {
				glTexCoord2f(texture_coord[tex_faces[j][k]][0] * 0.2, texture_coord[tex_faces[j][k]][1] * 0.2);
				glVertex3fv(desk_mboard[faces[2 * i + j][k]]);
			}
		}

	}
	glEnd();
	glPopMatrix();

	glTranslatef(0.19, 0.0, 0.0);
	//desk side3
	glBegin(GL_TRIANGLES);
	for (int i = 0; i < 6; i++) {
		glNormal3fv(normals[i]);
		for (int j = 0; j < 2; j++) {
			for (int k = 0; k < 3; k++) {
				glTexCoord2f(texture_coord[tex_faces[j][k]][0] * 0.4, texture_coord[tex_faces[j][k]][1] * 0.2);
				glVertex3fv(desk_side[faces[2 * i + j][k]]);
			}
		}

	}
	glEnd();

	glPushMatrix();
	glTranslatef(0.01, 0.05, 0.0);
	//desk board3
	glBegin(GL_TRIANGLES);
	for (int i = 0; i < 6; i++) {
		glNormal3fv(normals[i]);
		for (int j = 0; j < 2; j++) {
			for (int k = 0; k < 3; k++) {
				glTexCoord2f(texture_coord[tex_faces[j][k]][0] * 0.2, texture_coord[tex_faces[j][k]][1] * 0.2);
				glVertex3fv(desk_board[faces[2 * i + j][k]]);
			}
		}

	}
	glEnd();

	glTranslatef(0.0, 0.12, 0.0);
	//desk board4
	glBegin(GL_TRIANGLES);
	for (int i = 0; i < 6; i++) {
		glNormal3fv(normals[i]);
		for (int j = 0; j < 2; j++) {
			for (int k = 0; k < 3; k++) {
				glTexCoord2f(texture_coord[tex_faces[j][k]][0] * 0.2, texture_coord[tex_faces[j][k]][1] * 0.2);
				glVertex3fv(desk_board[faces[2 * i + j][k]]);
			}
		}

	}
	glEnd();

	glPopMatrix();

	glTranslatef(0.1, 0.0, 0.0);
	//desk side4
	glBegin(GL_TRIANGLES);
	for (int i = 0; i < 6; i++) {
		glNormal3fv(normals[i]);
		for (int j = 0; j < 2; j++) {
			for (int k = 0; k < 3; k++) {
				glTexCoord2f(texture_coord[tex_faces[j][k]][0] * 0.2, texture_coord[tex_faces[j][k]][1] * 0.2);
				glVertex3fv(desk_side[faces[2 * i + j][k]]);
			}
		}

	}
	glEnd();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.35, 0.0, 0.0);
	//desk leg2
	glBegin(GL_TRIANGLES);
	for (int i = 0; i < 6; i++) {
		glNormal3fv(normals[i]);
		for (int j = 0; j < 2; j++) {
			for (int k = 0; k < 3; k++) {
				glTexCoord2f(texture_coord[tex_faces[j][k]][0] * 0.1, texture_coord[tex_faces[j][k]][1] * 0.4);
				glVertex3fv(desk_leg[faces[2 * i + j][k]]);
			}
		}

	}
	glEnd();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.23, 0.094, 0.0);
	//desk drawer1
	glBegin(GL_TRIANGLES);
	for (int i = 0; i < 6; i++) {
		glNormal3fv(normals[i]);
		for (int j = 0; j < 2; j++) {
			for (int k = 0; k < 3; k++) {
				glTexCoord2f(texture_coord[tex_faces[j][k]][0] * 0.1, texture_coord[tex_faces[j][k]][1] * 0.1);
				glVertex3fv(desk_drawer[faces[2 * i + j][k]]);
			}
		}

	}
	glEnd();

	//손잡이1
	glPushMatrix();
	glTranslatef(0.06, 0.025, 0.21);
	glutSolidSphere(0.01, 20, 20);
	glPopMatrix();


	glTranslatef(0.0, 0.052, 0.0);
	//desk drawer2
	glBegin(GL_TRIANGLES);
	for (int i = 0; i < 6; i++) {
		glNormal3fv(normals[i]);
		for (int j = 0; j < 2; j++) {
			for (int k = 0; k < 3; k++) {
				glTexCoord2f(texture_coord[tex_faces[j][k]][0] * 0.1, texture_coord[tex_faces[j][k]][1] * 0.1);
				glVertex3fv(desk_drawer[faces[2 * i + j][k]]);
			}
		}

	}
	glEnd();

	//손잡이2
	glPushMatrix();
	glTranslatef(0.06, 0.025, 0.21);
	glutSolidSphere(0.01, 20, 20);
	glPopMatrix();

	glTranslatef(0.0, 0.052, 0.0);
	//desk drawer3
	glBegin(GL_TRIANGLES);
	for (int i = 0; i < 6; i++) {
		glNormal3fv(normals[i]);
		for (int j = 0; j < 2; j++) {
			for (int k = 0; k < 3; k++) {
				glTexCoord2f(texture_coord[tex_faces[j][k]][0] * 0.1, texture_coord[tex_faces[j][k]][1] * 0.1);
				glVertex3fv(desk_drawer[faces[2 * i + j][k]]);
			}
		}

	}
	glEnd();

	//손잡이3
	glPushMatrix();
	glTranslatef(0.06, 0.025, 0.21);
	glutSolidSphere(0.01, 20, 20);
	glPopMatrix();

	glPopMatrix();
	glPopMatrix();
	glBindTexture(GL_TEXTURE_2D, 0);
}

void drawDressing()
{
	glBindTexture(GL_TEXTURE_2D, texture);

	if (data[10])
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width[10], height[10], 0, GL_RGB, GL_UNSIGNED_BYTE, data[10]);

	}
	else
	{
		printf("Failed to load texture\n");
	}

	GLfloat mat_ambient[] = { 0.3, 0.0, 0.5, 1.0 };
	GLfloat mat_diffuse[] = { 0.8, 0.8, 0.8, 1.0 };
	GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat mat_shininess[] = { 50 };
	GLfloat mat_emission[] = { 0.3, 0.3, 0.3, 0.0 };

	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
	glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission);

	/*****dressing*****/
	glPushMatrix();
	glTranslatef(0.75, 0.0, 0.0);

	glPushMatrix();
	//dressing leg1
	glTranslatef(0.02, 0.0, 0.001);
	glBegin(GL_TRIANGLES);
	for (int i = 0; i < 6; i++) {
		glNormal3fv(normals[i]);
		for (int j = 0; j < 2; j++) {
			for (int k = 0; k < 3; k++) {
				glTexCoord2f(texture_coord[tex_faces[j][k]][0], texture_coord[tex_faces[j][k]][1]);
				glVertex3fv(dressing_leg[faces[2 * i + j][k]]);
			}
		}

	}
	glEnd();

	//dressing leg2
	glTranslatef(0.0, 0.0, 0.058);
	glBegin(GL_TRIANGLES);
	for (int i = 0; i < 6; i++) {
		glNormal3fv(normals[i]);
		for (int j = 0; j < 2; j++) {
			for (int k = 0; k < 3; k++) {
				glTexCoord2f(texture_coord[tex_faces[j][k]][0], texture_coord[tex_faces[j][k]][1]);
				glVertex3fv(dressing_leg[faces[2 * i + j][k]]);
			}
		}

	}
	glEnd();

	//dressing leg3
	glTranslatef(0.13, 0.0, 0.0);
	glBegin(GL_TRIANGLES);
	for (int i = 0; i < 6; i++) {
		glNormal3fv(normals[i]);
		for (int j = 0; j < 2; j++) {
			for (int k = 0; k < 3; k++) {
				glTexCoord2f(texture_coord[tex_faces[j][k]][0], texture_coord[tex_faces[j][k]][1]);
				glVertex3fv(dressing_leg[faces[2 * i + j][k]]);
			}
		}

	}
	glEnd();

	//dressing leg4
	glTranslatef(0.0, 0.0, -0.058);
	glBegin(GL_TRIANGLES);
	for (int i = 0; i < 6; i++) {
		glNormal3fv(normals[i]);
		for (int j = 0; j < 2; j++) {
			for (int k = 0; k < 3; k++) {
				glTexCoord2f(texture_coord[tex_faces[j][k]][0], texture_coord[tex_faces[j][k]][1]);
				glVertex3fv(dressing_leg[faces[2 * i + j][k]]);
			}
		}

	}
	glEnd();

	glPopMatrix();

	glBindTexture(GL_TEXTURE_2D, texture);

	if (data[2])
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width[2], height[2], 0, GL_RGB, GL_UNSIGNED_BYTE, data[2]);

	}
	else
	{
		printf("Failed to load texture\n");
	}

	glTranslatef(0.0, 0.2, 0.0);
	//dressing
	glBegin(GL_TRIANGLES);
	for (int i = 0; i < 6; i++) {
		glNormal3fv(normals[i]);
		for (int j = 0; j < 2; j++) {
			for (int k = 0; k < 3; k++) {
				glTexCoord2f(texture_coord[tex_faces[j][k]][0], texture_coord[tex_faces[j][k]][1]);
				glVertex3fv(dressing[faces[2 * i + j][k]]);
			}
		}

	}
	glEnd();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0, 0.2, 0.0);
	glPopMatrix();

	glBindTexture(GL_TEXTURE_2D, 0);

	glBindTexture(GL_TEXTURE_2D, texture);

	if (data[6])
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width[6], height[6], 0, GL_RGB, GL_UNSIGNED_BYTE, data[6]);

	}
	else
	{
		printf("Failed to load texture\n");
	}

	GLfloat mat_ambient2[] = { 0.5, 0.5, 1.0, 1.0 };
	GLfloat mat_shininess2[] = { 32 };
	GLfloat mat_emission2[] = { 0.5, 0.5, 0.5, 0.0 };

	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient2);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess2);
	glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission2);
	/******mirror********/
	glPushMatrix();
	glTranslatef(0.75, 0.4, 0.0);
	glBegin(GL_TRIANGLES);
	for (int i = 0; i < 6; i++) {
		glNormal3fv(normals[i]);
		for (int j = 0; j < 2; j++) {
			for (int k = 0; k < 3; k++) {
				glTexCoord2f(texture_coord[tex_faces[j][k]][0], texture_coord[tex_faces[j][k]][1]);
				glVertex3fv(mirror[faces[2 * i + j][k]]);
			}
		}

	}
	glEnd();
	glPopMatrix();
	glBindTexture(GL_TEXTURE_2D, 0);

}

void drawBookcase()
{
	glBindTexture(GL_TEXTURE_2D, texture);

	if (data[5])
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width[5], height[5], 0, GL_RGB, GL_UNSIGNED_BYTE, data[5]);

	}
	else
	{
		printf("Failed to load texture\n");
	}

	GLfloat mat_ambient[] = { 0.2, 0.5, 0.1, 1.0 };
	GLfloat mat_diffuse[] = { 0.8, 0.8, 0.8, 1.0 };
	GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat mat_shininess[] = { 32 };
	GLfloat mat_emission[] = { 0.3, 0.3, 0.3, 0.0 };

	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
	glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission);


	/*****bookcase*****/
	glPushMatrix();
	glTranslatef(0.95, 0.0, 0.0);
	//book_back
	glBegin(GL_TRIANGLES);
	for (int i = 0; i < 6; i++) {
		glNormal3fv(normals[i]);
		for (int j = 0; j < 2; j++) {
			for (int k = 0; k < 3; k++) {
				glTexCoord2f(texture_coord[tex_faces[j][k]][0], texture_coord[tex_faces[j][k]][1]);
				glVertex3fv(book_back[faces[2 * i + j][k]]);
			}
		}

	}
	glEnd();

	glTranslatef(0.0, 0.0, 0.01);
	//book_left
	glBegin(GL_TRIANGLES);
	for (int i = 0; i < 6; i++) {
		glNormal3fv(normals[i]);
		for (int j = 0; j < 2; j++) {
			for (int k = 0; k < 3; k++) {
				glTexCoord2f(texture_coord[tex_faces[j][k]][0], texture_coord[tex_faces[j][k]][1]);
				glVertex3fv(book_side[faces[2 * i + j][k]]);
			}
		}

	}
	glEnd();

	glPushMatrix();
	glTranslatef(0.245, 0.0, 0.0);
	//book_right
	glBegin(GL_TRIANGLES);
	for (int i = 0; i < 6; i++) {
		glNormal3fv(normals[i]);
		for (int j = 0; j < 2; j++) {
			for (int k = 0; k < 3; k++) {
				glTexCoord2f(texture_coord[tex_faces[j][k]][0], texture_coord[tex_faces[j][k]][1]);
				glVertex3fv(book_side[faces[2 * i + j][k]]);
			}
		}

	}
	glEnd();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.005, 0.0, 0.0);
	//book_bottom
	glBegin(GL_TRIANGLES);
	for (int i = 0; i < 6; i++) {
		glNormal3fv(normals[i]);
		for (int j = 0; j < 2; j++) {
			for (int k = 0; k < 3; k++) {
				glTexCoord2f(texture_coord[tex_faces[j][k]][0], texture_coord[tex_faces[j][k]][1] / 3);
				glVertex3fv(book_top[faces[2 * i + j][k]]);
			}
		}
	}
	glEnd();

	glTranslatef(0.0, 0.1175, 0.0);
	//book_middle1
	glBegin(GL_TRIANGLES);
	for (int i = 0; i < 6; i++) {
		glNormal3fv(normals[i]);
		for (int j = 0; j < 2; j++) {
			for (int k = 0; k < 3; k++) {
				glTexCoord2f(texture_coord[tex_faces[j][k]][0], texture_coord[tex_faces[j][k]][1] / 3);
				glVertex3fv(book_top[faces[2 * i + j][k]]);
			}
		}
	}
	glEnd();

	glTranslatef(0.0, 0.1175, 0.0);
	//book_middle2
	glBegin(GL_TRIANGLES);
	for (int i = 0; i < 6; i++) {
		glNormal3fv(normals[i]);
		for (int j = 0; j < 2; j++) {
			for (int k = 0; k < 3; k++) {
				glTexCoord2f(texture_coord[tex_faces[j][k]][0], texture_coord[tex_faces[j][k]][1] / 3);
				glVertex3fv(book_top[faces[2 * i + j][k]]);
			}
		}
	}
	glEnd();

	glTranslatef(0.0, 0.1175, 0.0);
	//book_middle3
	glBegin(GL_TRIANGLES);
	for (int i = 0; i < 6; i++) {
		glNormal3fv(normals[i]);
		for (int j = 0; j < 2; j++) {
			for (int k = 0; k < 3; k++) {
				glTexCoord2f(texture_coord[tex_faces[j][k]][0], texture_coord[tex_faces[j][k]][1] / 3);
				glVertex3fv(book_top[faces[2 * i + j][k]]);
			}
		}
	}
	glEnd();

	glTranslatef(0.0, 0.1175, 0.0);
	//book_top
	glBegin(GL_TRIANGLES);
	for (int i = 0; i < 6; i++) {
		glNormal3fv(normals[i]);
		for (int j = 0; j < 2; j++) {
			for (int k = 0; k < 3; k++) {
				glTexCoord2f(texture_coord[tex_faces[j][k]][0], texture_coord[tex_faces[j][k]][1] / 3);
				glVertex3fv(book_top[faces[2 * i + j][k]]);
			}
		}
	}
	glEnd();

	glBindTexture(GL_TEXTURE_2D, 0);

	glTranslatef(0.15, 0.03, 0.06);
	//earth 지지대
	glPushMatrix();
	GLfloat mat_ambient1[] = { 0.5, 0.2, 0.1, 1.0 };
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient1);

	glRotatef(-90.f, 1.0, 0.0, 0.0);
	createCylinder(0.0, 0.0, 0.0, 0.05, 0.01);
	glPopMatrix();

	glPushMatrix();
	GLfloat mat_ambient2[] = { 0.4, 0.1, 0.1, 1.0 };
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient2);

	glTranslatef(0.0, 0.01, 0.003);
	glRotatef(-90.f, 1.0, 0.0, 0.0);
	createCylinder(0.0, 0.0, 0.0, 0.01, 0.1);
	glPopMatrix();

	// earth

	glPushMatrix();
	GLfloat mat_shininess3[] = { 5 };
	GLfloat mat_ambient3[] = { 0.0, 0.2, 0.7, 1.0 };
	GLfloat mat_emission3[] = { 0.4, 0.4, 0.4, 0.0 };

	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient3);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess3);
	glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission3);

	glTranslatef(0.0, 0.12, 0.0);
	glutSolidSphere(0.05, 30, 30);

	glPopMatrix();
	glPopMatrix();
	glPopMatrix();
}

void drawDoor()
{
	glBindTexture(GL_TEXTURE_2D, texture);

	if (data[12])
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width[12], height[12], 0, GL_RGB, GL_UNSIGNED_BYTE, data[12]);

	}
	else
	{
		printf("Failed to load texture\n");
	}

	GLfloat mat_ambient[] = { 0.8, 0.8, 0.8, 1.0 };
	GLfloat mat_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat mat_shininess[] = { 32 };
	GLfloat mat_emission[] = { 0.3, 0.3, 0.3, 0.0 };

	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
	glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission);

	/******closet********/
	glPushMatrix();
	glTranslatef(1.19, 0.0, 0.2);
	glBegin(GL_TRIANGLES);
	for (int i = 0; i < 6; i++) {
		glNormal3fv(normals[i]);
		for (int j = 0; j < 2; j++) {
			for (int k = 0; k < 3; k++) {
				glTexCoord2f(texture_coord[tex_faces[j][k]][0], texture_coord[tex_faces[j][k]][1]);
				glVertex3fv(closet[faces[2 * i + j][k]]);
			}
		}

	}
	glEnd();

	glBindTexture(GL_TEXTURE_2D, texture);

	if (data[13])
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width[13], height[13], 0, GL_RGB, GL_UNSIGNED_BYTE, data[13]);

	}
	else
	{
		printf("Failed to load texture\n");
	}
	glTranslatef(0.0, 0.0, 0.16);
	glBegin(GL_TRIANGLES);
	for (int i = 0; i < 6; i++) {
		glNormal3fv(normals[i]);
		for (int j = 0; j < 2; j++) {
			for (int k = 0; k < 3; k++) {
				glTexCoord2f(texture_coord[tex_faces[j][k]][0], texture_coord[tex_faces[j][k]][1]);
				glVertex3fv(closet[faces[2 * i + j][k]]);
			}
		}

	}
	glEnd();
	glPopMatrix();

	glBindTexture(GL_TEXTURE_2D, texture);

	if (data[11])
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width[11], height[11], 0, GL_RGB, GL_UNSIGNED_BYTE, data[11]);

	}
	else
	{
		printf("Failed to load texture\n");
	}

	GLfloat mat_ambient2[] = { 1.0, 1.0, 0.9, 1.0 };

	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient2);

	/******door********/
	glPushMatrix();
	glTranslatef(1.49, 0.0, 0.66);
	glBegin(GL_TRIANGLES);
	for (int i = 0; i < 6; i++) {
		glNormal3fv(normals[i]);
		for (int j = 0; j < 2; j++) {
			for (int k = 0; k < 3; k++) {
				glTexCoord2f(texture_coord[tex_faces[j][k]][0], texture_coord[tex_faces[j][k]][1]);
				glVertex3fv(door[faces[2 * i + j][k]]);
			}
		}

	}
	glEnd();

	glPopMatrix();
	glBindTexture(GL_TEXTURE_2D, 0);
}
void
drawAll(void)
{
	int i, j;
	int piece;
	float m[4][4];
	float x, y, z;
	float bottom_h, bottom_w;


	GLfloat light_position[] = { 0, 0.5, 0.0, 0.0 };
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	GLfloat light_position2[] = { -0.5, 0.5, 0.0, 0.0 };
	glLightfv(GL_LIGHT1, GL_POSITION, light_position2);

	GLfloat light_position3[] = { -10, 0.0, 0.0, 0.0 };
	glLightfv(GL_LIGHT2, GL_POSITION, light_position3);

	GLfloat light_position4[] = { 0.0, 0.0, 10.0, 0.0 };
	glLightfv(GL_LIGHT3, GL_POSITION, light_position4);

	build_rotmatrix(m, curquat);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0, 0, -10);
	glMultMatrixf(&(m[0][0]));

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	float *v;
	//
	glPushMatrix();
	//glRotatef(20.0f, 1.0f, 0.0f, 0.0f);
	//glRotatef(-45.0f, 0.0f, 1.0, 0.0f);
	glTranslatef(0.0, -0.5*scale, 0.0);

	glScalef(scale, scale, scale);
	glTranslatef(-0.6, -0.05, -0.5);

	glTranslatef(0.001*xtrans, 0.001*ytrans, 0.0);


	drawWall();
	drawLamp();
	drawBed();
	drawDesk();
	drawDressing();
	drawBookcase();
	drawDoor();

	glPopMatrix();

}

void
redraw(void)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fovy_angle, 1.0, 0.1, 100.0);


	drawAll();
	glutPostRedisplay();
	if (doubleBuffer)
		glutSwapBuffers();
	else
		glFinish();
}

void
multMatrices(const GLfloat a[16], const GLfloat b[16], GLfloat r[16])
{
	int i, j;

	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++) {
			r[i * 4 + j] =
				a[i * 4 + 0] * b[0 * 4 + j] +
				a[i * 4 + 1] * b[1 * 4 + j] +
				a[i * 4 + 2] * b[2 * 4 + j] +
				a[i * 4 + 3] * b[3 * 4 + j];
		}
	}
}

void
makeIdentity(GLfloat m[16])
{
	m[0 + 4 * 0] = 1;
	m[0 + 4 * 1] = 0;
	m[0 + 4 * 2] = 0;
	m[0 + 4 * 3] = 0;
	m[1 + 4 * 0] = 0;
	m[1 + 4 * 1] = 1;
	m[1 + 4 * 2] = 0;
	m[1 + 4 * 3] = 0;
	m[2 + 4 * 0] = 0;
	m[2 + 4 * 1] = 0;
	m[2 + 4 * 2] = 1;
	m[2 + 4 * 3] = 0;
	m[3 + 4 * 0] = 0;
	m[3 + 4 * 1] = 0;
	m[3 + 4 * 2] = 0;
	m[3 + 4 * 3] = 1;
}

/*
** inverse = invert(src)
*/
int
invertMatrix(const GLfloat src[16], GLfloat inverse[16])
{
	int i, j, k, swap;
	double t;
	GLfloat temp[4][4];

	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++) {
			temp[i][j] = src[i * 4 + j];
		}
	}
	makeIdentity(inverse);

	for (i = 0; i < 4; i++) {
		/*
		** Look for largest element in column */
		swap = i;
		for (j = i + 1; j < 4; j++) {
			if (fabs(temp[j][i]) > fabs(temp[i][i])) {
				swap = j;
			}
		}

		if (swap != i) {
			/*
			** Swap rows. */
			for (k = 0; k < 4; k++) {
				t = temp[i][k];
				temp[i][k] = temp[swap][k];
				temp[swap][k] = t;

				t = inverse[i * 4 + k];
				inverse[i * 4 + k] = inverse[swap * 4 + k];
				inverse[swap * 4 + k] = t;
			}
		}
		if (temp[i][i] == 0) {
			/*
			** No non-zero pivot.  The matrix is singular, which
			shouldn't ** happen.  This means the user gave us a
			bad matrix. */
			return 0;
		}
		t = temp[i][i];
		for (k = 0; k < 4; k++) {
			temp[i][k] /= t;
			inverse[i * 4 + k] /= t;
		}
		for (j = 0; j < 4; j++) {
			if (j != i) {
				t = temp[j][i];
				for (k = 0; k < 4; k++) {
					temp[j][k] -= temp[i][k] * t;
					inverse[j * 4 + k] -= inverse[i * 4 + k] * t;
				}
			}
		}
	}
	return 1;
}


static int selected;
static int selectx, selecty;
static float selstartx, selstarty;




static int solving;
static int spinning;
static float lastquat[4];
static void
Reshape(int width, int height)
{

	W = width;
	H = height;
	glViewport(0, 0, W, H);
	glGetIntegerv(GL_VIEWPORT, viewport);
}


void
motion(int x, int y)
{
	float selx, sely;

	if (left_mouse) {
		if (mousex != x || mousey != y) {
			trackball(lastquat,
				(2.0*mousex - W) / W,
				(H - 2.0*mousey) / H,
				(2.0*x - W) / W,
				(H - 2.0*y) / H);
			spinning = 1;
		}
		else {
			spinning = 0;
		}
		changeState();
	}
	if (right_mouse) {
		xtrans += x - curX;
		ytrans += curY - y;
	}
	if (middle_mouse) {
		// x<curX  왼쪽으로 드래깅
		// x>curX  오른쪽으로 드래깅
		// y<curY  위로 드래깅
		// y>curY  아래로 드래깅

		if (zoomx > W / 2 && zoomy < H / 2) { //제 1사분면
			if (x>curX || y<curY)//위로 드래깅 or 오른쪽으로 드래깅
				fovy_angle++;
			else fovy_angle--;
		}

		if (zoomx < W / 2 && zoomy < H / 2) {  //제 2사분면
			if (x<curX || y<curY)//위로 드래깅 or 왼쪽으로 드래깅
				fovy_angle++;
			else fovy_angle--;
		}

		if (zoomx < W / 2 && zoomy > H / 2) { //제 3사분면
			if (x<curX || y>curY)//아래로 드래깅 or 왼쪽으로 드래깅
				fovy_angle++;
			else fovy_angle--;
		}

		if (zoomx > W / 2 && zoomy > H / 2) { //제 4사분면
			if (x>curX || y>curY)//아래로 드래깅 or 오른쪽으로 드래깅
				fovy_angle++;
			else fovy_angle--;
		}
	}
	curX = x;
	curY = y;

	mousex = x;
	mousey = y;

	glutPostRedisplay();
}

void
mouse(int b, int s, int x, int y)
{
	float selx, sely;

	mousex = x;
	mousey = y;

	curX = x;
	curY = y;

	if (s == GLUT_DOWN) {
		switch (b) {
		case GLUT_LEFT_BUTTON:
			left_mouse = GL_TRUE;
			break;
		case GLUT_RIGHT_BUTTON:
			right_mouse = GL_TRUE;
			break;
		case GLUT_MIDDLE_BUTTON:
			middle_mouse = GL_TRUE;
			if (mouseClick == GL_FALSE) {
				zoomx = x;
				zoomy = y;
			}
			mouseClick = GL_TRUE;
			break;
		}
	}
	else {
		switch (b) {
		case GLUT_LEFT_BUTTON:
			left_mouse = GL_FALSE;
			spinning = 0;
			glutPostRedisplay();
			break;
		case GLUT_RIGHT_BUTTON:
			right_mouse = GL_FALSE;
			glutPostRedisplay();
			break;
		case GLUT_MIDDLE_BUTTON:
			middle_mouse = GL_FALSE;
			mouseClick = GL_FALSE;
			glutPostRedisplay();
			break;
		}
	}
	motion(x, y);
}
void
keyboard(unsigned char c, int x, int y)
{
	int piece;

	switch (c) {
	case '1':
		if (light0 == 1) {
			light0 = 0;
			glDisable(GL_LIGHT0);
			light0_emission[0] = 0.3;
			light0_emission[1] = 0.3;
			light0_emission[2] = 0.3;
		}
		else {
			light0 = 1;
			glEnable(GL_LIGHT0);
			light0_emission[0] = 1.0;
			light0_emission[1] = 1.0;
			light0_emission[2] = 1.0;
		}
		break;
	case '2':
		if (light1 == 1) {
			light1 = 0;
			glDisable(GL_LIGHT1);
			light1_emission[0] = 0.3;
			light1_emission[1] = 0.3;
			light1_emission[2] = 0.3;
		}
		else {
			light1 = 1;
			glEnable(GL_LIGHT1);
			light1_emission[0] = 1.0;
			light1_emission[1] = 1.0;
			light1_emission[2] = 0.7;
		}
		break;
	case '3':
		if (light2 == 1) {
			light2 = 0;
			glDisable(GL_LIGHT2);
			light2_ambient[0] = light2_night[0];
			light2_ambient[1] = light2_night[1];
			light2_ambient[2] = light2_night[2];

			light2_emission[0] = 0.3;
			light2_emission[1] = 0.3;
			light2_emission[2] = 0.3;
		}
		else {
			light2 = 1;
			glEnable(GL_LIGHT2);
			light2_ambient[0] = light2_day[0];
			light2_ambient[1] = light2_day[1];
			light2_ambient[2] = light2_day[2];

			light2_emission[0] = 1.0;
			light2_emission[1] = 0.8;
			light2_emission[2] = 0.5;
		}
		break;
	case 'z':
		scale += 0.5;
		break;
	case 'x':
		scale -= 0.5;
		break;
	case 't':
		if (texture_on == 1) {
			glDisable(GL_TEXTURE_2D);
			texture_on = 0;
		}
		else {
			glEnable(GL_TEXTURE_2D);
			texture_on = 1;
		}
		break;
	case 'q':
		if (texture_mag_mode == 1) {
			glBindTexture(GL_TEXTURE_2D, texture);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			printf("mag_near");
			texture_mag_mode = 0;
		}
		break;
	case 'a':
		if (texture_min_mode == 1) {
			glBindTexture(GL_TEXTURE_2D, texture);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			printf("min_near");
			texture_min_mode = 0;
		}
		break;
	case 'w':
		if (texture_mag_mode == 0) {
			glBindTexture(GL_TEXTURE_2D, texture);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			printf("mag_linear");
			texture_mag_mode = 1;
		}
		break;
	case 's':
		if (texture_min_mode == 0) {
			glBindTexture(GL_TEXTURE_2D, texture);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			printf("min_linear");
			texture_min_mode = 1;
		}
		break;
	case 'r':
		if (repeat == 1) {
			glBindTexture(GL_TEXTURE_2D, texture);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
			repeat = 0;
		}
		else {
			glBindTexture(GL_TEXTURE_2D, texture);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			repeat = 1;
		}
		break;
	default:
		break;
	}
	glutPostRedisplay();
}

void
animate(void)
{
	if (spinning) {
		add_quats(lastquat, curquat, curquat);

	}
	glutPostRedisplay();
}

void
changeState(void)
{
	if (visible) {
		if (!spinning) {
			glutIdleFunc(NULL);
		}
		else {
			glutIdleFunc(animate);
		}
	}
	else {
		glutIdleFunc(NULL);
	}
}

void
init(void)
{
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_DEPTH_TEST);
	glClearDepth(1.0);

	GLfloat light_ambient[] = { 0.6, 0.6, 0.6, 1.0 };
	GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	/* light_position is NOT default value */

	GLfloat light_ambient2[] = { 0.3, 0.2, 0.1, 1.0 };
	GLfloat light_diffuse2[] = { 0.8, 0.8, 0.8, 1.0 };
	GLfloat light_specular2[] = { 1.0, 1.0, 1.0, 1.0 };

	GLfloat light_ambient3[] = { 0.5, 0.5, 0.3, 1.0 };
	GLfloat light_diffuse3[] = { 0.9, 0.9, 0.9, 1.0 };
	GLfloat light_specular3[] = { 1.0, 1.0, 1.0, 1.0 };

	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 0.8);

	glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient2);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse2);
	glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular2);
	glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 0.8);

	glLightfv(GL_LIGHT2, GL_AMBIENT, light_ambient3);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, light_diffuse3);
	glLightfv(GL_LIGHT2, GL_SPECULAR, light_specular3);
	glLightf(GL_LIGHT2, GL_CONSTANT_ATTENUATION, 0.8);


	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_NORMALIZE);

	glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);

	glClearColor(1.0, 1.0, 1.0, 0.0);

	trackball(curquat, 0.0, 0.0, 0.0, 0.0);

	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	initTexture();
}


void
visibility(int v)
{
	if (v == GLUT_VISIBLE) {
		visible = 1;
	}
	else {
		visible = 0;
	}
	changeState();
}


int
main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitWindowSize(1200, 1000);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_RGB | GLUT_DOUBLE | GLUT_MULTISAMPLE);


	glutCreateWindow("18011529김효경");
	init();
	glGetIntegerv(GL_VIEWPORT, viewport);

	glutReshapeFunc(Reshape);
	glutDisplayFunc(redraw);
	glutMotionFunc(motion);
	glutMouseFunc(mouse);
	glutKeyboardFunc(keyboard);
	glutVisibilityFunc(visibility);
	glutMainLoop();
	return 0;             /* ANSI C requires main to return int. */
}