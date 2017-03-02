#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <GL/glx.h>
#include <GL/glext.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>

#define PI 3.14159265

//global variables containing polygon information for bunny
int face_count;
GLuint *faces;

GLfloat *read_object_file(char* fileName) {

	//open file
	FILE* fp = fopen(fileName,"r"); 
	if (!fp) {
		fprintf(stderr, "Failed to open %s\n", fileName);
		exit(1);
	}

	//get file length      
	fseek(fp, 0, SEEK_END);
	int length = ftell(fp);
	rewind(fp);

	//skip unnessesary
	char tmp[100];
	int i, x;
	for (i = 0; i < 12; i++) x = fscanf(fp, "%s", tmp);

	int vertex_count;
	x = fscanf(fp, "%d", &vertex_count);
	GLfloat *vertex;
	vertex = (GLfloat *) malloc(sizeof(GLfloat) * 6 * vertex_count);

	//skip unnessesary
	for (i = 0; i < 20; i++) x = fscanf(fp, "%s", tmp);

	x = fscanf(fp, "%d", &face_count);
	faces = (GLuint *) malloc(sizeof(GLuint)*3*face_count);

	for (i = 0; i < 6; i++) x = fscanf(fp, "%s", tmp);

	for (i = 0; i < vertex_count; i++) {
		x = fscanf(fp,"%f %f %f %f %f %f", &vertex[i*6], 
			&vertex[i*6+1],&vertex[i*6+2],&vertex[i*6+3],
			&vertex[i*6+4],&vertex[i*6+5]);
	}

	for (i = 0; i < face_count; i++) {
		x = fscanf(fp,"%d %d %d %d", &x, &faces[i*3],&faces[i*3+1],&faces[i*3+2]);
	}

	fclose(fp);

	return vertex;
}

char *read_shader_program(char *filename) 
{
	FILE *fp;
	char *content = NULL;
	int fd, count;
	fd = open(filename,O_RDONLY);
	count = lseek(fd,0,SEEK_END);
	close(fd);
	content = (char *)calloc(1,(count+1));
	fp = fopen(filename,"r");
	count = fread(content,sizeof(char),count,fp);
	content[count] = '\0';
	fclose(fp);
	return content;
}

struct point {
	float x, y, z;
};

void setup_viewvolume()
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

void normalize(struct point *p) {
	float mag = sqrt(p->x*p->x + p->y*p->y + p->z*p->z);
	p->x /= mag;
	p->y /= mag;
	p->z /= mag;
}

void init_lights()
{
	//set up hollywood lighting based on eye position
	struct point eye, light0, light1, light2;
	eye.x = 0.12; eye.y = .267; eye.z = .325;
	light0.x = eye.x * cos(PI/12) + eye.z * sin(PI/12);
	light0.y = eye.y;
	light0.z = eye.z * cos(PI/12) - eye.x * sin(PI/12);

	light1.x = eye.x * cos(-PI/3) + eye.z * sin(-PI/3);
	light1.y = eye.y;
	light1.z = eye.z * cos(-PI/3) - eye.x * sin(-PI/3);

	light2.x = -eye.x;
	light2.y = eye.y + .24;
	light2.z = -eye.z;

	//key light
	float light0_diffuse[] = { 0.60, 0.60, 0.60, 0.0 }; 
	float light0_specular[] = { 0.75, 0.75, 0.75, 0.0 }; 
	float light0_position[] = { light0.x, light0.y, light0.z, 1.0 };

	//fill light
	float light1_diffuse[] = { 0.55, 0.55, 0.4, 0.0 }; 
	float light1_specular[] = { 0.4, 0.4, 0.6, 0.0 }; 
	float light1_position[] = { light1.x, light1.y, light1.z, 1.0 };

	//back light
	float light2_diffuse[] = { 0.2, 0.2, 0.9, 0.0 }; 
	float light2_specular[] = { 0.25, 0.25, 1.25, 0.0 }; 
	float light2_position[] = { light2.x, light2.y, light2.z, 1.0 };
	
	//enable key light
	glLightfv(GL_LIGHT0,GL_POSITION,light0_position); 
	glLightfv(GL_LIGHT0,GL_DIFFUSE,light0_diffuse); 
	glLightfv(GL_LIGHT0,GL_SPECULAR,light0_specular); 

	//enable fill light
	glLightfv(GL_LIGHT1,GL_POSITION,light1_position); 
	glLightfv(GL_LIGHT1,GL_DIFFUSE,light1_diffuse); 
	glLightfv(GL_LIGHT1,GL_SPECULAR,light1_specular); 

	//enable back light
	glLightfv(GL_LIGHT2,GL_POSITION,light2_position); 
	glLightfv(GL_LIGHT2,GL_DIFFUSE,light2_diffuse); 
	glLightfv(GL_LIGHT2,GL_SPECULAR,light2_specular);
}

void init_material()
{
	float mat_ambient[] = {0.0,0.0,0.0,1.0}; 
	float mat_diffuse[] = {0.9,0.9,0.9,1.0}; 
	float mat_specular[] = {1.0,1.0,1.0,1.0};
	float mat_shininess[] = {2.0}; 

	glMaterialfv(GL_FRONT,GL_AMBIENT,mat_ambient);
	glMaterialfv(GL_FRONT,GL_DIFFUSE,mat_diffuse);
	glMaterialfv(GL_FRONT,GL_SPECULAR,mat_specular);
	glMaterialfv(GL_FRONT,GL_SHININESS,mat_shininess);
}

unsigned int init_shaders() {
	GLint vertCompiled, fragCompiled;
	char *vs, *fs;
	GLuint v, f, p;
	int result = -1;

	v = glCreateShader(GL_VERTEX_SHADER);
	f = glCreateShader(GL_FRAGMENT_SHADER);
	vs = read_shader_program("bunny.vert");
	fs = read_shader_program("bunny.frag");
	glShaderSource(v,1,(const char **)&vs,NULL);
	glShaderSource(f,1,(const char **)&fs,NULL);
	free(vs);
	free(fs); 
	glCompileShader(v);
	glGetShaderiv(v,GL_COMPILE_STATUS,&result);
	if (result == GL_FALSE) {
		fprintf(stderr, "Vertex Shader: \n");
		GLint maxLength = 0;
		glGetShaderiv(v, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		GLchar* errorLog = (GLchar *) malloc(sizeof(GLchar) * maxLength);
		glGetShaderInfoLog(v, maxLength, &maxLength, &errorLog[0]);
		fprintf(stderr,"%s\n", errorLog);
		free(errorLog);
	}

	glCompileShader(f);
	glGetShaderiv(f,GL_COMPILE_STATUS,&result);
	if (result == GL_FALSE) {
		fprintf(stderr, "Fragment Shader: \n");
		GLint maxLength = 0;
		glGetShaderiv(f, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		GLchar* errorLog = (GLchar *) malloc(sizeof(GLchar) * maxLength);
		glGetShaderInfoLog(f, maxLength, &maxLength, &errorLog[0]);
		fprintf(stderr,"%s\n", errorLog);
		free(errorLog);
	}

	p = glCreateProgram();
	glAttachShader(p,f);
	glAttachShader(p,v);
	glLinkProgram(p);
	glUseProgram(p);
	return p;
}

void init_objects(GLfloat *vertex) {
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glVertexPointer(3,GL_FLOAT,6*sizeof(GLfloat),vertex);
	glNormalPointer(GL_FLOAT,6*sizeof(GLfloat),&vertex[3]);
}

void render_scene()
{
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);	
	//coords for ground
	struct point bottom[4]={{-0.2,0.0,-.3},{-0.2,0.0,0.1},{.1,0.0,0.1},{.1,0.0,-.3}};

	glBegin(GL_QUADS);
	glNormal3f(0.0,1.0,0.0);
	int i;
	for(i=0;i<4;i++) glVertex3f(bottom[i].x,bottom[i].y,bottom[i].z);
	glEnd();

	glDrawElements(GL_TRIANGLES,face_count*3,GL_UNSIGNED_INT,faces);


	glutSwapBuffers();
}

void end_program(unsigned char key, int x, int y)
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
	GLfloat *vertex = read_object_file("bunnyN.ply");

	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_RGBA|GLUT_DOUBLE|GLUT_DEPTH|GLUT_MULTISAMPLE);
	glutInitWindowSize(768,768);
	glutInitWindowPosition(100,100);
	glutCreateWindow("Project 2");
	glClearColor(0.25,0.25,0.25,0.0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_MULTISAMPLE_ARB);

	setup_viewvolume();
	init_lights();
	init_material();
	init_shaders();
	init_objects(vertex);

	glutDisplayFunc(render_scene);
	glutKeyboardFunc(end_program);
	glutMainLoop();

	free(vertex);
	free(faces);
	return 0;
}
