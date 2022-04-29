#define GLUT_DISABLE_ATEXIT_HACK
#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "model.h"
#include "draw.h"
#include "camera.h"
#include <sys/time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdbool.h>

#define resolution 1   // 1: 1920*1080   0: 1366*768
#define fullscreen 1   // 1: fullscreen  0: windowed

typedef GLubyte Pixel;
GLfloat light_position[] = {0, 0, 0, 0};
GLfloat light_ambient[] = { 0.5, 0.5, 0.5, 0 };
GLfloat light_diffuse[] = { 0.5, 0.5, 0, 0 };
GLfloat light_specular[] = { 1, 1, 1, 0 };

struct Camera camera;
struct Action action;
struct Action
{
	int move_forward;
	int move_f_forward;
	int move_backward;
	int move_f_backward;
	int step_left;
	int step_right;
	int move_up;
	int move_down;
	int rotate_earth_in_galaxy;
	int move_earth_in_galaxy;
	int move_moon_around;
	int call_falcon;
	int increase_light;
	int decrease_light;
};

World world;
Rotate rotate;
Move move;
Move* movee;

int WINDOW_WIDTH;
int WINDOW_HEIGHT;
int mouse_x, mouse_y;
int previous_time;
int help, help_on = 0;
float speed = 30;
float angle = 135;
double degree = 0;
double distance_a = 4000;
double distance_b = 2000;
int falcon=0;
int evening=1;

double calc_elapsed_time()
{
	int current_time;
	double elapsed_time;

	current_time = glutGet(GLUT_ELAPSED_TIME);
	elapsed_time = (double)(current_time - previous_time) / 1000.0;
	previous_time=current_time;

	return elapsed_time;
}

void update_camera_position(struct Camera* camera, double elapsed_time)
{
	double distance;

	distance = elapsed_time * MOVE_SPEED * speed;

	if (action.move_forward == TRUE) {
		move_camera_forward(camera, distance);
	}

	if (action.move_backward == TRUE) {
		move_camera_backward(camera, distance, movee);
	}

	if (action.step_left == TRUE) {
		step_camera_left(camera, distance);
	}

	if (action.step_right == TRUE) {
		step_camera_right(camera, distance);
	}

	if (action.move_up == TRUE) {
		move_camera_up(camera, distance);
	}

	if (action.move_down == TRUE) {
		move_camera_down(camera, distance);
	}

	if (action.increase_light == TRUE) {
		if (light_ambient[0] < 1)
			light_ambient[0] = light_ambient[1] = light_ambient[2] += 0.01;
	}

	if (action.decrease_light == TRUE) {
		if (light_ambient[0] > -0.51)
			light_ambient[0] = light_ambient[1] = light_ambient[2] -= 0.01;
	}

	don_not_head_up_against_the_wall (camera, move);

}

void specialFunc(int key, int x, int y) {
	switch (key) {
	case GLUT_KEY_F1:
		if (help_on) {
			help_on = 0;

			glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, light_ambient);
			glLightModelfv(GL_LIGHT_MODEL_AMBIENT, light_ambient);
		}else {
			help_on = 1;
			GLfloat ones[] = { 1, 1, 1, 1 };
			glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ones);
			glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ones);
		}
	}

}

void reshape(GLsizei width, GLsizei height) {
	WINDOW_WIDTH = width;
	WINDOW_HEIGHT = height;

	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (!help_on)
		gluPerspective(50.0, (GLdouble)width / (GLdouble)height, 0.1, 20000.0);
	else
		gluOrtho2D(0, width, height, 0);
}

void draw_help() {

	GLfloat ones[] = { 1, 1, 1, 1 };
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ones);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ones);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, help);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex3f(0, 0, 0);
	glTexCoord2f(1, 0);
	glVertex3f(WINDOW_WIDTH, 0, 0);
	glTexCoord2f(1, 1);
	glVertex3f(WINDOW_WIDTH, WINDOW_HEIGHT, 0);
	glTexCoord2f(0, 1);
	glVertex3f(0, WINDOW_HEIGHT, 0);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	reshape(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutSwapBuffers();
}

GLuint load_texture(char* filename)
{
	GLuint TextureID = 0;

	SDL_Surface* Surface = IMG_Load(filename);
 
	glGenTextures(1, &TextureID);
	glBindTexture(GL_TEXTURE_2D, TextureID);
 
	int Mode = GL_RGB;
 
	if(Surface->format->BytesPerPixel == 4) {
		Mode = GL_RGBA;
	}
 
	glTexImage2D(GL_TEXTURE_2D, 0, Mode, Surface->w, Surface->h, 0, Mode, GL_UNSIGNED_BYTE, Surface->pixels);
 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	return TextureID;
	
}

void rotation_of_objects(Rotate* rotate) {

	if (action.rotate_earth_in_galaxy == TRUE)
		rotate->earth_rotation+=0.5;

	rotate->sun_rotation+=0.05;

}

void movement_of_objects (Move* move){

	if (action.move_earth_in_galaxy == TRUE){
		double distance_a = 4000;
		double distance_b = 2000;
		degree+=0.1;
		double angle = degree_to_radian(degree);
		move->earth.x = cos(angle) * distance_a;
		move->earth.y = sin(angle) * distance_b;
		move->earth.z = 0;
	}
	else
		if (move->earth.x == 0){
			move->earth.x = 4000;
			move->earth.y = 0;
			move->earth.z = 0;
		}

		/*move->falcon.x=2300;
		move->falcon.y=-27;
		move->falcon.z=-50;*/
	if (action.call_falcon == TRUE && move->falcon.x<6000){
		if (falcon==0){
		move->falcon.x=-6000;
		move->falcon.y=1500;
		move->falcon.z=400;
	}
	move->falcon.x+=15.0;
	falcon=1;			}
	else
		if (action.call_falcon == TRUE && move->falcon.x>=6000){
		move->falcon.x=-20000;
		action.call_falcon = FALSE;
		falcon=0;}
	else
		if (action.call_falcon == FALSE){
		move->falcon.x=-20000;}


}

void loadevening(){
	world.skybox.texture = load_texture("textures\\sky.png");
}

void loadmorning(){
	world.skybox.texture2 = load_texture("textures\\sky1.png");
}

void changeTex(Move* move){
	
	if(move->moon.x == 4700 && move->moon.y == 700){

		evening = 1;

	}else
		evening = 0;
		
	if(evening == 0){
		world.evening == false;
		evening = 1;
	}else{
		world.evening == true;
		evening=0;
	}
		
	if(move->moon.x > 3300.959325){
		if(move->moon.x < 3301.3056){			
			if(world.evening==TRUE){
				world.skybox.texture2;
				world.evening = false;
			}
		}
	}
	if(move->moon.x >  4699.893386){
		if(move->moon.x <  4699.973346){			
			if(world.evening==false){
				world.skybox.texture;
				world.evening=true;
			}
		}
	}
	
}

void move_moon(Move* move){
		
	if(action.move_moon_around == TRUE){
		distance_a=700;
		distance_b=700;
		degree+=0.5;
		double angle = degree_to_radian(degree);
	
		move->moon.x = cos(angle) * distance_a + 4000;
		move->moon.y = sin(angle) * distance_b;
		move->moon.z = 0;
		
	}else
		if (move->moon.x == 0){
			move->moon.x = 4700;
			move->moon.y = 0;
			move->moon.z = 0;
	}
	
	changeTex(move);
	
}

void display() {
		
	if (!help_on) {
		double elapsed_time;
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		elapsed_time = calc_elapsed_time();
		update_camera_position(&camera, elapsed_time);
		set_view_point(&camera);

		glLightfv(GL_LIGHT1, GL_POSITION, light_position);
        glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient);
        glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular);
		glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse);
		glMaterialfv(GL_FRONT, GL_SPECULAR, light_ambient);
		glEnable(GL_LIGHT1);

		draw_environment(world, &rotate, move);
		glutWireCone(0.005,0.1,20,20);
		movement_of_objects(&move);
		move_moon(&move);
		rotation_of_objects(&rotate);
		reshape(WINDOW_WIDTH, WINDOW_HEIGHT);
		glutSwapBuffers();

	}
	else {
		draw_help();
	}
}

void mouse_handler(int button, int state, int x, int y)
{
	mouse_x = x;
	mouse_y = y;
}

void motion_handler(int x, int y)
{
	double horizontal, vertical;

	horizontal = mouse_x - x;
	vertical = mouse_y - y;

	rotate_camera(&camera, horizontal, vertical);

	mouse_x = x;
	mouse_y = y;

	glutPostRedisplay();
}

void key_handler(unsigned char key, int x, int y)
{
	
	switch (key) {
	case 'w':
		action.move_forward = TRUE;
		break;
	case 's':
		action.move_backward = TRUE;
		break;
	case 'a':
		action.step_left = TRUE;
		break;
	case 'd':
		action.step_right = TRUE;
		break;
	case 'c':
		action.move_down = TRUE;
		break;
	case 32:
		action.move_up = TRUE;
		break;
	case 'q':
		if (action.rotate_earth_in_galaxy == FALSE)
			action.rotate_earth_in_galaxy = TRUE;
		else action.rotate_earth_in_galaxy = FALSE;
		break;
	case 'm':
		if (action.move_moon_around == FALSE){
			action.move_moon_around = TRUE;
		}
		else action.move_moon_around = FALSE;
		break;
	case 'e':
		if (action.move_earth_in_galaxy == FALSE)
			action.move_earth_in_galaxy = TRUE;
		else action.move_earth_in_galaxy = FALSE;
		break;
	case 'n':
		if(evening == 0){
			world.evening == false;
			evening = 1;
			light_ambient[0] = light_ambient[1] = light_ambient[2] += 0.1;
		}else{
			world.evening == true;
			evening=0;
			light_ambient[0] = light_ambient[1] = light_ambient[2] -= 0.05;
		}
		init_bg(&world);
	case 'f':
		if (action.call_falcon == FALSE){
			action.call_falcon = TRUE;
			}
		break;
	case '+':
		action.increase_light = TRUE;
		break;
	case '-':
		action.decrease_light = TRUE;
		break;
	case 27:
		exit(0);
	}

	glutPostRedisplay();
}

void key_up_handler(unsigned char key, int x, int y)
{
	switch (key) {
	case 'w':
		action.move_forward = FALSE;
		break;
	case 's':
		action.move_backward = FALSE;
		break;
	case 'a':
		action.step_left = FALSE;
		break;
	case 'd':
		action.step_right = FALSE;
		break;
	case 'c':
		action.move_down = FALSE;
		break;
	case 32:
		action.move_up = FALSE;
		break;
	case '+':
		action.increase_light = FALSE;
		break;
	case '-':
		action.decrease_light = FALSE;
		break;
	case 27:
		exit(0);
	}

	glutPostRedisplay();
}

void idle()
{
	glutPostRedisplay();
}

void set_callbacks() {
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(key_handler);
	glutKeyboardUpFunc(key_up_handler);
	glutMouseFunc(mouse_handler);
	glutMotionFunc(motion_handler);
	glutIdleFunc(idle);
	glutSpecialFunc(specialFunc);
	if (fullscreen==1) glutFullScreen();
}


void initialize()
{
	set_callbacks();
	init_camera(&camera);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_NORMALIZE);
    glEnable(GL_AUTO_NORMAL);
    glClearColor(0.1, 0.1, 0.1, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_COLOR_MATERIAL);
    glClearDepth(1.0);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, light_ambient);
	glEnable(GL_LIGHTING);
    help = load_texture("textures//help.png");
    init_entities(&world);
	init_bg(&world);
    glEnable(GL_TEXTURE_2D);
}


int main(int argc, char** argv) {

	glutInit(&argc, argv);
	if (resolution==1){ glutInitWindowSize(1920, 1080);}
	else {glutInitWindowSize(1366, 768);}
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	int wnd = glutCreateWindow("Falcon");
	glutSetWindow(wnd);
	initialize();

    action.move_forward = FALSE;
	action.move_f_forward = false;
    action.move_backward = FALSE;
	action.move_f_backward = false;
    action.step_left = FALSE;
    action.step_right = FALSE;
    action.rotate_earth_in_galaxy = TRUE;
	action.move_moon_around = false;
	action.call_falcon = false;

	glutMainLoop();
	return 0;
}
