#include <graphics.h>
#include <conio.h>
#include <cmath>
#include <thread>

// ���� Windows Multimedia API
#pragma comment(lib,"Winmm.lib")

#define High 800  // ��Ϸ����ߴ�
#define Width 590
#define MAX_ENEMIES 6  // �������л�����
#define MAX_BULLETS 10  // �������л��ӵ�����
#define BULLET_SPEED 1  // ����л��ӵ��ٶ�

struct Enemy {
	float x, y;  // �л�λ��
	bool isActive;  // �л��Ƿ��У�δ�����У�
	bool canShoot;  // �л��Ƿ���Է����ӵ�
	Enemy() = default;
};

struct Bullet {
	float x, y;  // �ӵ�λ��
	bool isActive;  // �ӵ��Ƿ��У�δ����Ŀ�꣩
	Bullet() = default;
};

Enemy enemies[MAX_ENEMIES];  // �����л�����
Bullet bullets[MAX_BULLETS];  // �����л��ӵ�����

IMAGE img_bk; // ����ͼƬ
float position_x, position_y; // �ɻ�λ��
float bullet_x, bullet_y; // �ӵ�λ��
IMAGE img_planeNormal1, img_planeNormal2; // �����ɻ�ͼƬ
IMAGE img_planeExplode1, img_planeExplode2; // ��ը�ɻ�ͼƬ
IMAGE img_bullet1, img_bullet2; // �ӵ�ͼƬ
IMAGE img_enemyPlane1, img_enemyPlane2; // �л�ͼƬ
int isExpolde = 0; // �ɻ��Ƿ�ը
int score = 0; // �÷�
bool cleanEnemy = false;	// ��յл������ͷű�ʶ

void startup()
{
	mciSendString("open resources/game_music.mp3 alias bkmusic", NULL, 0, NULL); //�򿪱�������
	mciSendString("play bkmusic repeat", NULL, 0, NULL);  // ѭ������
	initgraph(Width, High);
	loadimage(&img_bk, "resources/background.jpg");
	loadimage(&img_planeNormal1, "resources/planeNormal_1.jpg");
	loadimage(&img_planeNormal2, "resources/planeNormal_2.jpg");
	loadimage(&img_bullet1, "resources/bullet1.jpg");
	loadimage(&img_bullet2, "resources/bullet2.jpg");
	loadimage(&img_enemyPlane1, "resources/enemyPlane1.jpg");
	loadimage(&img_enemyPlane2, "resources/enemyPlane2.jpg");
	loadimage(&img_planeExplode1, "resources/planeExplode_1.jpg");
	loadimage(&img_planeExplode2, "resources/planeExplode_2.jpg");
	position_x = Width * 0.5;
	position_y = High * 0.7;

	// ���ӵ��ĳ�ʼλ����Ϊ������
	bullet_x = -100;
	bullet_y = -100;

	// ��ʼ����������
	for (int i = 0; i < MAX_ENEMIES; i++) {
		enemies[i].x = rand() % Width;
		enemies[i].y = -100 - i * 100; // ÿ���л���λ�ö�����һ���л���ԶһЩ������Զ����Ļ
		enemies[i].isActive = true;
		enemies[i].canShoot = rand() % 2;  // ��������л��Ƿ���Է����ӵ�
	}


	// ��ʼ���ӵ�����
	for (int i = 0; i < MAX_BULLETS; i++) {
		bullets[i].x = -1;
		bullets[i].y = -1;
		bullets[i].isActive = false;
	}

	BeginBatchDraw();
}

void show()
{
	putimage(0, 0, &img_bk);	// ��ʾ����	
	if (isExpolde == 0)
	{
		putimage(position_x - 50, position_y - 60, &img_planeNormal1, NOTSRCERASE); // ��ʾ�����ɻ�	
		putimage(position_x - 50, position_y - 60, &img_planeNormal2, SRCINVERT);

		putimage(bullet_x - 7, bullet_y, &img_bullet1, NOTSRCERASE); // ��ʾ�ӵ�	
		putimage(bullet_x - 7, bullet_y, &img_bullet2, SRCINVERT);

		// ��ʾ�л��ӵ�
		for (int i = 0; i < MAX_BULLETS; i++) {
			if (bullets[i].isActive) {
				putimage(bullets[i].x, bullets[i].y, &img_bullet1, NOTSRCERASE); // ��ʾ�ӵ�	
				putimage(bullets[i].x, bullets[i].y, &img_bullet2, SRCINVERT);
			}
		}

		// ��ʾ�л�
		for (int i = 0; i < MAX_ENEMIES; i++) {
			if (enemies[i].isActive) {
				putimage(enemies[i].x, enemies[i].y, &img_enemyPlane1, NOTSRCERASE);
				putimage(enemies[i].x, enemies[i].y, &img_enemyPlane2, SRCINVERT);
			}
		}
	}
	else
	{
		putimage(position_x - 50, position_y - 60, &img_planeExplode1, NOTSRCERASE); // ��ʾ��ը�ɻ�	
		putimage(position_x - 50, position_y - 60, &img_planeExplode2, SRCINVERT);
	}
	outtextxy(Width * 0.48, High * 0.95, "�÷֣�");
	char s[5];
	sprintf_s(s, "%d", score);
	outtextxy(Width * 0.55, High * 0.95, s);
	FlushBatchDraw();
	Sleep(2);
}

void cleanSkillThread()
{
	cleanEnemy = true;
	std::this_thread::sleep_for(std::chrono::seconds(10)); // ����5��
	cleanEnemy = false;
}

void cleanSkill()
{
	if (!cleanEnemy)
	{
		// ����һ�����߳��������������ܵ���ȴ����
		std::thread t(cleanSkillThread);
		t.detach(); // ���ǵ���detach�������߳��ں�̨���У�����Ҫ���߳����ȴ������
	}
}

void updateWithoutInput()
{
	if (isExpolde == 0)
	{
		if (bullet_y > -25)
			bullet_y = bullet_y - 2;

		// ���µ���
		for (int i = 0; i < MAX_ENEMIES; i++) {
			if (enemies[i].isActive) {
				if (enemies[i].y < High - 25)
					enemies[i].y = enemies[i].y + 0.5;
				else
					enemies[i].y = 10;

				// ����ӵ��Ƿ�����˵��˻�ʹ������յл��ļ���
				if (abs(bullet_x - enemies[i].x) + abs(bullet_y - enemies[i].y) < 80 || cleanEnemy) {
					enemies[i].isActive = false;
					enemies[i].x = rand() % Width;
					enemies[i].y = -40;
					bullet_y = -85;
					score++;

					if (score > 0 && score % 5 == 0 && score % 2 != 0)
					{
						mciSendString("close 5music", NULL, 0, NULL); // �Ȱ�ǰ��һ�ε����ֹر�
						mciSendString("open resources/5.mp3 alias 5music", NULL, 0, NULL); // ������
						mciSendString("play 5music", NULL, 0, NULL); // ������һ��
					}
					if (score % 10 == 0)
					{
						mciSendString("close 10music", NULL, 0, NULL); // �Ȱ�ǰ��һ�ε����ֹر�
						mciSendString("open resources/10.mp3 alias 10music", NULL, 0, NULL); // ������
						mciSendString("play 10music", NULL, 0, NULL); // ������һ��
					}
				}
			}
			else {
				// һ��ʱ������¼������
				enemies[i].y += 0.5;
				if (enemies[i].y > High) {
					enemies[i].isActive = true;
				}
			}
		}

		// ���л��Ƿ�ײ�����ǵķɻ�
		for (int i = 0; i < MAX_ENEMIES; i++) {
			if (enemies[i].isActive && (abs(position_x - enemies[i].x) + abs(position_y - enemies[i].y) < 75)) {	// ����ʹ�������پ��룬����ŷʽ���룬���ܾ�ȷ�������ڵл�ģ�;�ȷ�Ȳ��ߵ�������������پ���������У������ܴ�ر��������Ϸ����
				isExpolde = 1;

				mciSendString("close exmusic", NULL, 0, NULL); // �Ȱ�ǰ��һ�ε����ֹر�
				mciSendString("open resources/explode.mp3 alias exmusic", NULL, 0, NULL); // ������
				mciSendString("play exmusic", NULL, 0, NULL); // ������һ��
			}
		}

		// ����Ƿ��е��˵��ӵ��������ǵķɻ�
		for (int i = 0; i < MAX_BULLETS; i++) {
			if (bullets[i].isActive && (pow(position_x - bullets[i].x, 2) + pow(position_y - bullets[i].y, 2)) < pow(50, 2)) {	// ����ŷ�Ͼ�����������پ���
				isExpolde = 1;

				mciSendString("close exmusic", NULL, 0, NULL); // �Ȱ�ǰ��һ�ε����ֹر�
				mciSendString("open resources/explode.mp3 alias exmusic", NULL, 0, NULL); // ������
				mciSendString("play exmusic", NULL, 0, NULL); // ������һ��
			}
		}

		// ���µл��ӵ�
		for (int i = 0; i < MAX_BULLETS; i++) {
			if (bullets[i].isActive) {
				bullets[i].y += BULLET_SPEED;
				if (bullets[i].y > High) {
					bullets[i].isActive = false;
				}
			}
		}

		// ���л��Ƿ���Ҫ�����ӵ�
		for (int i = 0; i < MAX_ENEMIES; i++) {
			if (enemies[i].isActive && enemies[i].canShoot) {
				for (int j = 0; j < MAX_BULLETS; j++) {
					if (!bullets[j].isActive) {
						bullets[j].x = enemies[i].x;
						bullets[j].y = enemies[i].y;
						bullets[j].isActive = true;
						break;
					}
				}
			}
		}
	}
}

void updateWithInput()
{
	if (isExpolde == 0)
	{
		MOUSEMSG m;		// ���������Ϣ
		while (MouseHit())  //����������ڼ�⵱ǰ�Ƿ��������Ϣ
		{
			m = GetMouseMsg();
			if (m.uMsg == WM_MOUSEMOVE)
			{
				// �ɻ���λ�õ���������ڵ�λ��
				position_x = m.x;
				position_y = m.y;
			}
			else if (m.uMsg == WM_LBUTTONDOWN)
			{
				// �����������������ӵ�
				bullet_x = position_x;
				bullet_y = position_y - 85;

				mciSendString("close fgmusic", NULL, 0, NULL); // �Ȱ�ǰ��һ�ε����ֹر�
				mciSendString("open resources/f_gun.mp3 alias fgmusic", NULL, 0, NULL); // ������
				mciSendString("play fgmusic", NULL, 0, NULL); // ������һ��
			}
			else if (m.uMsg == WM_RBUTTONDOWN)
			{
				// ��������Ҽ���ʹ����������
				cleanSkill();
			}
		}
	}
}

void gameover()
{
	EndBatchDraw();
	_getch();
	closegraph();
}

int main()
{
	startup();  // ���ݳ�ʼ��
	while (1)  //  ��Ϸѭ��ִ��
	{
		show();  // ��ʾ����
		updateWithoutInput();  // ���û������޹صĸ���
		updateWithInput();     // ���û������йصĸ���
	}
	gameover();     // ��Ϸ��������������
	return 0;
}