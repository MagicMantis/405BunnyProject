#include <stdio.h>
#include <stdlib.h>	
#include <string.h>
#include <GL/gl.h>
#include <GL/glut.h>

//redmoncoreyl@gmail.com

int vertex_count, face_count;

GLfloat *vertex;
GLuint *faces;

void readFile(char* fileName) {

	//open file
	FILE* fp = fopen(fileName, "r"); 
	if (!fp) {
		fprintf(stderr, "Failed to open %s\n", fileName);
		exit(1);
	}

	//get file length      
	fseek(fp, 0, SEEK_END);
	int length = ftell(fp);
	rewind(fp);

	fprintf(stderr,"length: %d\n", length);

	//skip unnessesary
	char tmp[100];
	int i, x;
	for (i = 0; i < 12; i++) {
		x = fscanf(fp, "%s", tmp);
		fprintf(stdout, "%s\n", tmp);
	}

	x = fscanf(fp, "%d", &vertex_count);
	vertex = (GLfloat *) malloc(sizeof(GLfloat) * 6 * vertex_count);

	//skip unnessesary
	for (i = 0; i < 20; i++) x = fscanf(fp, "%s", tmp);

	x = fscanf(fp, "%d", &face_count);
	faces = (GLuint *) malloc(sizeof(GLuint)*3*face_count);

	for (i = 0; i < 6; i++) x = fscanf(fp, "%s", tmp);

	fprintf(stderr,"reading %d verticies\n",vertex_count);
	for (i = 0; i < vertex_count; i++) {
		x = fscanf(fp,"%f %f %f %f %f %f", &vertex[i*6], 
			&vertex[i*6+1],&vertex[i*6+2],&vertex[i*6+3],
			&vertex[i*6+4],&vertex[i*6+5]);
		//vertex[i*6+3] = 1.0;
		//vertex[i*6+4] = 1.0;
		//vertex[i*6+5] = 1.0;
	}

	for (i = 0; i < face_count; i++) {
		x = fscanf(fp,"%d %d %d %d", &x, &faces[i*3],&faces[i*3+1],&faces[i*3+2]);
	}

	fclose(fp);

	return;
}

struct point {
	float x, y, z;
	};

void setup_the_viewvolume()
{
	struct point eye, view, up;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0,1.0,0.1,20.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	eye.x = 0.12; eye.y = .267; eye.z = .325;
	view.x = -0.025; view.y = 0.1; view.z = 0.0;
	up.x = 0.0; up.y = 1.0; up.z = 0.0;
	gluLookAt(eye.x,eye.y,eye.z,view.x,view.y,view.z,up.x,up.y,up.z);
}

void do_lights()
{
	//key light
	float light0_ambient[] = { 0.0, 0.0, 0.0, 0.0 };
	float light0_diffuse[] = { 2.0, 2.0, 2.0, 0.0 }; 
	float light0_specular[] = { 2.25, 2.25, 2.25, 0.0 }; 
	float light0_position[] = { 1.5, 2.0, 2.0, 1.0 };
	float light0_direction[] = { -1.5, -2.0, -2.0, 1.0};

	//back light
	float light1_ambient[] = { 0.0, 0.0, 0.0, 0.0 };
	float light1_diffuse[] = { 2.0, 2.0, 2.0, 0.0 }; 
	float light1_specular[] = { 2.25, 2.25, 2.25, 0.0 }; 
	float light1_position[] = { 1.5, 2.0, 2.0, 1.0 };
	float light1_direction[] = { -1.5, -2.0, -2.0, 1.0};

	//fill light
	float light2_ambient[] = { 0.0, 0.0, 0.0, 0.0 };
	float light2_diffuse[] = { 2.0, 2.0, 2.0, 0.0 }; 
	float light2_specular[] = { 2.25, 2.25, 2.25, 0.0 }; 
	float light2_position[] = { 1.5, 2.0, 2.0, 1.0 };
	float light2_direction[] = { -1.5, -2.0, -2.0, 1.0};

	// set scene default ambient 
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT,light0_ambient); 

	// make specular correct for spots 
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER,1); 
	glLightfv(GL_LIGHT0,GL_AMBIENT,light0_ambient); 
	glLightfv(GL_LIGHT0,GL_DIFFUSE,light0_diffuse); 
	glLightfv(GL_LIGHT0,GL_SPECULAR,light0_specular); 
	glLightf(GL_LIGHT0,GL_SPOT_EXPONENT,0.1); 
	glLightf(GL_LIGHT0,GL_SPOT_CUTOFF,180.0); 
	glLightf(GL_LIGHT0,GL_CONSTANT_ATTENUATION,1.0); 
	glLightf(GL_LIGHT0,GL_LINEAR_ATTENUATION,0.2); 
	glLightf(GL_LIGHT0,GL_QUADRATIC_ATTENUATION,0.01); 
	glLightfv(GL_LIGHT0,GL_POSITION,light0_position);
	glLightfv(GL_LIGHT0,GL_SPOT_DIRECTION,light0_direction);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
}

void do_material()
{
	float mat_ambient[] = {0.0,0.0,0.0,1.0}; 
	float mat_diffuse[] = {0.9,0.9,0.1,1.0}; 
	float mat_specular[] = {1.0,1.0,1.0,1.0};
	float mat_shininess[] = {2.0}; 

	glMaterialfv(GL_FRONT,GL_AMBIENT,mat_ambient);
	glMaterialfv(GL_FRONT,GL_DIFFUSE,mat_diffuse);
	glMaterialfv(GL_FRONT,GL_SPECULAR,mat_specular);
	glMaterialfv(GL_FRONT,GL_SHININESS,mat_shininess);
}


void draw_stuff()
{
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glVertexPointer(3,GL_FLOAT,6*sizeof(GLfloat),vertex);
	glNormalPointer(GL_FLOAT,6*sizeof(GLfloat),&vertex[3]);

	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glDrawElements(GL_TRIANGLES,face_count*3,GL_UNSIGNED_INT,faces);
	glFlush();
}

void getout(unsigned char key, int x, int y)
{
	switch(key) {
        case 'q':
            exit(1);
        default:
            break;
	}
}

int main(int argc, char **argv)
{
	readFile("bunnyN.ply");

	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_RGBA|GLUT_DEPTH|GLUT_MULTISAMPLE);
	glutInitWindowSize(768,768);
	glutInitWindowPosition(100,50);
	glutCreateWindow("Project 2");
	glClearColor(0.35,0.35,0.35,0.0);
	glEnable(GL_DEPTH_TEST);

	setup_the_viewvolume();
	do_lights();
	do_material();

	glutDisplayFunc(draw_stuff);
	glutKeyboardFunc(getout);
	glutMainLoop();
	return 0;
}