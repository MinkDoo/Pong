#include <SDL.h>
#include<SDL_mixer.h>
#include<SDL_image.h>
#include <SDL_ttf.h>
#include <iostream>
#include<string>



//Setting kích thước tốc độ
#define WIDTH 720
#define HEIGHT 360
#define FONT_SIZE 32
#define BALL_SPEED 20
#define PADDLE_SPEED 9
#define SIZE 16
#define PI 3.14159265358979323846
Mix_Music* backgroundmusic;

Mix_Chunk* collision_sound;
//tạo render
SDL_Renderer* renderer;
//tạo cửa sổ
SDL_Window* window;
//Khởi tạo font
TTF_Font* font;
//Set color
SDL_Color color;
bool running;


int frameCount, timerFPS, lastFrame, fps;


//Paddle,ball and score_board
SDL_Rect l_paddle, r_paddle, ball, score_board, menu;
//Vectical X , Vectical Y
float velX, velY;
std::string score;
int l_s, r_s;
bool turn = 1;
void update();
void input();
void serve();
void render();
void write();
void play();

void init_sound(std::string path);
SDL_Texture* loadTexture(std::string path);


// MAIN
int main(int argc, char** argv) {
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0) std::cout << "Failed to initialize" << std::endl;
	if (SDL_CreateWindowAndRenderer(WIDTH, HEIGHT, 0, &window, &renderer) < 0) std::cout << "Failed to Create and Render" << std::endl;
	TTF_Init();
	font = TTF_OpenFont("Minecraft.ttf", FONT_SIZE);




	color.r = color.g = color.b = 255;
	//Set paddle and ball
	l_s = r_s = 0;
	l_paddle.x = 32; l_paddle.h = HEIGHT / 4;
	l_paddle.y = (HEIGHT / 2) - (l_paddle.h / 2);
	l_paddle.w = 12;
	r_paddle = l_paddle;
	r_paddle.x = WIDTH - r_paddle.w - 32;
	ball.w = ball.h = SIZE;

	serve();

	running = 1;
	play();


	TTF_CloseFont(font);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}
//Setup ván mới
void serve() {

	const Uint8* keystates = SDL_GetKeyboardState(NULL);
	l_paddle.y = r_paddle.y = (HEIGHT / 2) - (l_paddle.h / 2);
	if (turn) {
		ball.x = l_paddle.x + (l_paddle.w * 4);





	}
	else {
		ball.x = r_paddle.x - (r_paddle.w * 4);

	}
	velX = 0;
	velY = 0;
	ball.y = HEIGHT / 2 - (SIZE / 2);
}

void write(std::string text, int x, int y) {
	SDL_Surface* surface;
	SDL_Texture* texture;
	const char* t = text.c_str();
	surface = TTF_RenderText_Solid(font, t, color);
	texture = SDL_CreateTextureFromSurface(renderer, surface);
	score_board.w = surface->w;
	score_board.h = surface->h;
	score_board.x = x - score_board.w;
	score_board.y = y - score_board.h;
	SDL_FreeSurface(surface);
	SDL_RenderCopy(renderer, texture, NULL, &score_board);
	SDL_DestroyTexture(texture);
}

void update() {
	//Xử lý va chạm 
	if (SDL_HasIntersection(&ball, &r_paddle)) {
		double rel = (r_paddle.y + (r_paddle.h / 2)) - (ball.y + (SIZE / 2));
		double norm = rel / (r_paddle.h / 2);
		double bounce = norm * (5 * PI / 12);
		velX = -BALL_SPEED / 2 * cos(bounce);
		velY = BALL_SPEED / 2 * -sin(bounce);
		init_sound("MIXER/vacham.mp3");

	}
	if (SDL_HasIntersection(&ball, &l_paddle)) {
		// Khoảng cách từ bóng đến tâm vợt
		double rel = (l_paddle.y + (l_paddle.h / 2)) - (ball.y + (SIZE / 2));
		//độ dốc của bóng
		double norm = rel / (l_paddle.h / 2);
		//Góc tối ưu là 5pi/12, độ bật ra của bóng

		double bounce = norm * (5 * PI / 12);
		velX = BALL_SPEED / 2 * cos(bounce);
		velY = BALL_SPEED / 2 * -sin(bounce);
		init_sound("MIXER/vacham.mp3");
	}
	if (ball.y > r_paddle.y + (r_paddle.h / 2) && ball.x > WIDTH / 2 - SIZE) r_paddle.y += PADDLE_SPEED / 2;
	if (ball.y < r_paddle.y + (r_paddle.h / 2) && ball.x>WIDTH / 2 - SIZE) r_paddle.y -= PADDLE_SPEED / 2;


	if (ball.x + SIZE >= WIDTH) { r_paddle.y = HEIGHT / 2 - r_paddle.h / 2; }
	if (ball.y <= 0 || ball.y + SIZE >= HEIGHT) {
		velY = -velY;
	}
	ball.x += velX;
	ball.y += velY;
	score = "You: " + std::to_string(l_s) + "    " + "Com: " + std::to_string(r_s);
	if (l_paddle.y < 0)l_paddle.y = 0;
	if (l_paddle.y + l_paddle.h > HEIGHT)l_paddle.y = HEIGHT - l_paddle.h;
	if (r_paddle.y < 0)r_paddle.y = 0;
	if (r_paddle.y + r_paddle.h > HEIGHT)r_paddle.y = HEIGHT - r_paddle.h;
}

void input() {
	SDL_Event e;
	const Uint8* keystates = SDL_GetKeyboardState(NULL);
	while (SDL_PollEvent(&e)) if (e.type == SDL_QUIT) running = false;
	if (keystates[SDL_SCANCODE_ESCAPE]) running = false;
	if (keystates[SDL_SCANCODE_UP]) l_paddle.y -= PADDLE_SPEED;
	if (keystates[SDL_SCANCODE_DOWN]) l_paddle.y += PADDLE_SPEED;
	if (keystates[SDL_SCANCODE_SPACE]) {

		if (ball.x <= 0) { r_s++; turn = 1; }
		if (ball.x + SIZE >= WIDTH) { l_s++; turn = 0; }
		serve();
		if (turn) {



			velX = BALL_SPEED / 2;


		}
		else {

			velX = -BALL_SPEED / 2;
		}
	};
}

void render() {
	SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 255);
	SDL_RenderClear(renderer);

	frameCount++;
	timerFPS = SDL_GetTicks() - lastFrame;
	if (timerFPS < (1000 / 60)) {
		SDL_Delay((1000 / 60) - timerFPS);
	}

	SDL_SetRenderDrawColor(renderer, 255, color.g, 255, 255);
	SDL_RenderFillRect(renderer, &l_paddle);
	SDL_RenderFillRect(renderer, &r_paddle);

	//Ball

	int disk_radius = SIZE / 2;
	int disk_center_x = ball.x;
	int disk_center_y = ball.y;
	SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);

	for (int i = 0; i < disk_radius; i++) {
		SDL_RenderDrawLine(renderer, disk_center_x + i,
			disk_center_y + (int)(sqrt(disk_radius * disk_radius - i * i)),
			disk_center_x + i,
			disk_center_y - (int)(sqrt(disk_radius * disk_radius - i * i)));

		SDL_RenderDrawLine(renderer, disk_center_x - i,
			disk_center_y + (int)(sqrt(disk_radius * disk_radius - i * i)),
			disk_center_x - i,
			disk_center_y - (int)(sqrt(disk_radius * disk_radius - i * i)));
	}



	write(score, 120 + WIDTH / 2, 18 * 2);


	write("FPS: " + std::to_string(fps), 100 + FONT_SIZE, 18 * 2);

	if (velX == 0 && l_s == 0 && r_s == 0) {

		SDL_RenderCopy(renderer, loadTexture("IMAGE/Pong.png"), NULL, NULL);
		fps = 0;

	}
	if (ball.x <= 0) {
		bool sound = 1;
		write("YOU LOSE", WIDTH / 2 + FONT_SIZE * 2, HEIGHT / 2);
		write("PRESS SPACE TO RESTART", WIDTH / 2 + FONT_SIZE * 7, HEIGHT / 2 + 40);

		init_sound("MIXER/youlose.wav");


	}
	if (ball.x + SIZE >= WIDTH) {
		bool sound = 1;
		write("YOU WIN", WIDTH / 2 + FONT_SIZE * 2, HEIGHT / 2);
		write("PRESS SPACE TO RESTART", WIDTH / 2 + FONT_SIZE * 7, HEIGHT / 2 + 40);

		init_sound("MIXER/youwin.wav");


	}

	SDL_RenderPresent(renderer);
}
void play() {
	static int lastTime = 0;



	while (running) {
		lastFrame = SDL_GetTicks();
		if (lastFrame >= (lastTime + 1000)) {
			lastTime = lastFrame;
			fps = frameCount;

			frameCount = 0;

		}


		update();
		input();

		render();




	}
}
SDL_Texture* loadTexture(std::string path)
{
	//The final texture
	SDL_Texture* newTexture = NULL;

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load(path.c_str());
	if (loadedSurface == NULL)
	{
		printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
	}
	else
	{
		//Create texture from surface pixels
		newTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
		if (newTexture == NULL)
		{
			printf("Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
		}

		//Get rid of old loaded surface
		SDL_FreeSurface(loadedSurface);
	}

	return newTexture;
}
void init_sound(std::string path) {
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
		printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
	}
	collision_sound = Mix_LoadWAV(path.c_str());
	Mix_PlayChannel(-1, collision_sound, 0);

}