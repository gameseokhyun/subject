#include <stdio.h>;
#include <string>;
#include <windows.h>;
#include <ctime>;
#include <conio.h>

using namespace std;

class Player {
	int pos = 20;
	char face[100] = "(^_^)";
	int nAnimations = 0;
	int hp = 10;
public:
	void move(int inc) {
		pos += inc;
	}
	int getPosition() const { return pos; }
	void check_range(int screen_size) {
		if (pos < 0) pos = 0;
		if (pos >= screen_size) pos = screen_size - 1;
	}
	void draw(char* screen, int screen_size)
	{
		check_range(screen_size);
		strncpy(screen + pos, face, strlen(face));
	}
	void update() {
		if (nAnimations == 0) return;
		nAnimations--;

		if (nAnimations == 0) {
			strcpy(face, "(^_^)");
		}
	}
	void onHit() {
		if (nAnimations == 0) {
			nAnimations = 30;
			strcpy(face, "");
			hp -= 2;
		}
	}
	int getHp() {
		return hp;
	}
};

class Enemy {
	int pos;
	char face[100] = "(ㅡ_ㅡ)";
	char faceAttacked[100] = "(>_<)";
	int nAnimations = 0;
	int nMAnimations = 60;
	int hp = 10;
	bool isDie = false;
public:
	Enemy() {
	}
	void create(int index_) {
		pos = 50 + (rand() % 20) + (index_ * 10);
	}
	void move(int inc) {
		pos += inc;
	}
	int getPosition() const { return pos; }
	void check_range(int screen_size) {
		if (pos < 0) pos = 0;
		if (pos >= screen_size) pos = screen_size - 1;
	}
	void draw(char* screen, int screen_size)
	{
		check_range(screen_size);
		strncpy(screen + pos, face, strlen(face));
	}

	void update() {
		if (nAnimations == 0 && nMAnimations == 0) return;
		if (nAnimations > 0)
			nAnimations--;
		if (nMAnimations > 0)
			nMAnimations--;
		if (hp <= 0) {
			strcpy(face, "");
			isDie = true;
		}
		else {
			if (nAnimations == 0) {
				strcpy(face, "(ㅡ_ㅡ)");
			}
			if (nMAnimations == 0) {
				move(-2);
				nMAnimations = 60;
			}
		}
	}
	void onHit() {
		nAnimations = 15;
		hp--;
		strcpy(face, faceAttacked);
	}

	bool isDied() {
		return isDie;
	}
	int getHp() {
		return hp;
	}
};

class Bullet {
	int pos = 10;
	char shape = '*';

public:
	Bullet(int player_pos) : pos(player_pos) {};
	void move(int inc) {
		pos += inc;
	}
	int getPosition() { return pos; }
	void check_range(int screen_size) {
		if (pos < 0) pos = 0;
		if (pos >= screen_size) pos = screen_size - 1;
	}

	void draw(char* screen, int screen_size)
	{
		screen[pos] = shape;
	}

	void update(const Player* player, const Enemy* enemy) {
		if (!player | !enemy) return;
		if (player->getPosition() < enemy->getPosition())
			move(1);
		else
			move(-1);
	}
};



class Game {
	static const int len = 120;
	char screen[len + 1];
	Player player;
	Enemy enemy[5];
	Bullet* pBullet[10];
	bool fire_flag;
	int bullet_count, enemy_count;
	string bullet_shape[1000];
	int point;
	const int MS_PER_FRAME = 16;
	clock_t last_tick, current_tick, elapsed_ms, start_tick, bullet_cooltime;
	void clear_screen();
	void process_input();
	int update();
	void render();
public:
	Game();
	//~Game();
	void main_loop();
	void create();
};

Game::Game() : bullet_count(0), point(0), bullet_cooltime(0), start_tick(clock()), enemy_count(2) {
	for (int i = 0; i < 10; i++) {
		pBullet[i] = nullptr;
	}
	for (int i = 0; i < 5; i++) {
		//enemy[i] = new Enemy();
	}
}

void Game::main_loop() {
	while (true) {
		clear_screen();
		process_input();
		if (update() == 1) break;
		render();

	}
}

int main() {
	Game game;
	game.create();
	game.main_loop();
	while (true) {

	}
	return 0;
}

void Game::clear_screen() {
	last_tick = clock();
	memset(screen, ' ', len);
}

void Game::create() {
	for (int ie = 0; ie < enemy_count; ie++) {
		enemy[ie].create(ie);
	}
}



void Game::process_input() {
	if (!_kbhit()) return;
	int _ch = _getch();
	switch (_ch) {
	case 32: {
		if (bullet_count == 10) {
			if (bullet_cooltime != 0 && (float)(clock() - bullet_cooltime) / CLOCKS_PER_SEC >= 2) {
				bullet_count = 0;
				bullet_cooltime = 0;
			}
			if (bullet_cooltime == 0)
				bullet_cooltime = clock();
			return;
		}
		pBullet[bullet_count] = new Bullet(player.getPosition());
		bullet_count++;
	} break;
	case 75:player.move(-1);break;
	case 77:player.move(+1);break;
	}
}

int Game::update() {
	bool gameEnd = true;
	for (int ie = 0; ie < enemy_count; ie++) {
		if (!enemy[ie].isDied()) {
			gameEnd = false;
		}
	}

	if (player.getHp() <= 0) {
		gameEnd = true;

	}
	if (gameEnd) {
		clear_screen();
		printf("%s", screen);
		printf("게임 종료 ! 게임 진행 시간 : %f\r", (float)(clock() - start_tick) / CLOCKS_PER_SEC);
		return 1;
	}
	return 0;
}

void Game::render() {
	player.draw(screen, len);
	for (int ie = 0; ie < enemy_count; ie++) {
		if (!enemy[ie].isDied())
			enemy[ie].draw(screen, len);

	}
	for (int i = 0; i < bullet_count; i++) {
		if (pBullet[i] != nullptr) {
			pBullet[i]->draw(screen, len);
			for (int ie = 0; ie < enemy_count; ie++) {
				if (!enemy[ie].isDied()) {
					if (pBullet[i]->getPosition() == enemy[ie].getPosition()) {
						enemy[ie].onHit();
						delete pBullet[i]; //포인트가 가르키는 대상삭제
						pBullet[i] = nullptr; //포인트 주소 삭제
						break;
					}
					else {
						pBullet[i]->update(&player, &enemy[ie]);
					}
				}
			}
		}
	}
	for (int ie = 0; ie < enemy_count; ie++) {
		if (!enemy[ie].isDied()) {
			enemy[ie].update();
			if (player.getPosition() + 7 >= enemy[ie].getPosition() && player.getPosition() <= enemy[ie].getPosition()) {
				player.onHit();
				break;
			}
		}
	}
	player.update();
	screen[len - 1] = '\r';
	screen[len] = '\0';
	printf("%s", screen);
	printf("총알 : %d HP : %d \r", 10 - bullet_count, player.getHp());
	current_tick = clock();
	elapsed_ms = (current_tick - last_tick) * 1000 / CLOCKS_PER_SEC;
	if (elapsed_ms <= MS_PER_FRAME) {
		Sleep(MS_PER_FRAME - elapsed_ms);
	}
}

