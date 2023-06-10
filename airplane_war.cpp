#include <graphics.h>
#include <conio.h>
#include <cmath>
#include <thread>

// 引用 Windows Multimedia API
#pragma comment(lib,"Winmm.lib")

#define High 800  // 游戏画面尺寸
#define Width 590
#define MAX_ENEMIES 6  // 定义最大敌机数量
#define MAX_BULLETS 10  // 定义最大敌机子弹数量
#define BULLET_SPEED 1  // 定义敌机子弹速度

struct Enemy {
	float x, y;  // 敌机位置
	bool isActive;  // 敌机是否活动中（未被击中）
	bool canShoot;  // 敌机是否可以发射子弹
	Enemy() = default;
};

struct Bullet {
	float x, y;  // 子弹位置
	bool isActive;  // 子弹是否活动中（未击中目标）
	Bullet() = default;
};

Enemy enemies[MAX_ENEMIES];  // 创建敌机数组
Bullet bullets[MAX_BULLETS];  // 创建敌机子弹数组

IMAGE img_bk; // 背景图片
float position_x, position_y; // 飞机位置
float bullet_x, bullet_y; // 子弹位置
IMAGE img_planeNormal1, img_planeNormal2; // 正常飞机图片
IMAGE img_planeExplode1, img_planeExplode2; // 爆炸飞机图片
IMAGE img_bullet1, img_bullet2; // 子弹图片
IMAGE img_enemyPlane1, img_enemyPlane2; // 敌机图片
int isExpolde = 0; // 飞机是否爆炸
int score = 0; // 得分
bool cleanEnemy = false;	// 清空敌机技能释放标识

void startup()
{
	mciSendString("open resources/game_music.mp3 alias bkmusic", NULL, 0, NULL); //打开背景音乐
	mciSendString("play bkmusic repeat", NULL, 0, NULL);  // 循环播放
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

	// 将子弹的初始位置设为画面外
	bullet_x = -100;
	bullet_y = -100;

	// 初始化敌人数组
	for (int i = 0; i < MAX_ENEMIES; i++) {
		enemies[i].x = rand() % Width;
		enemies[i].y = -100 - i * 100; // 每个敌机的位置都比上一个敌机再远一些，并且远离屏幕
		enemies[i].isActive = true;
		enemies[i].canShoot = rand() % 2;  // 随机决定敌机是否可以发射子弹
	}


	// 初始化子弹数组
	for (int i = 0; i < MAX_BULLETS; i++) {
		bullets[i].x = -1;
		bullets[i].y = -1;
		bullets[i].isActive = false;
	}

	BeginBatchDraw();
}

void show()
{
	putimage(0, 0, &img_bk);	// 显示背景	
	if (isExpolde == 0)
	{
		putimage(position_x - 50, position_y - 60, &img_planeNormal1, NOTSRCERASE); // 显示正常飞机	
		putimage(position_x - 50, position_y - 60, &img_planeNormal2, SRCINVERT);

		putimage(bullet_x - 7, bullet_y, &img_bullet1, NOTSRCERASE); // 显示子弹	
		putimage(bullet_x - 7, bullet_y, &img_bullet2, SRCINVERT);

		// 显示敌机子弹
		for (int i = 0; i < MAX_BULLETS; i++) {
			if (bullets[i].isActive) {
				putimage(bullets[i].x, bullets[i].y, &img_bullet1, NOTSRCERASE); // 显示子弹	
				putimage(bullets[i].x, bullets[i].y, &img_bullet2, SRCINVERT);
			}
		}

		// 显示敌机
		for (int i = 0; i < MAX_ENEMIES; i++) {
			if (enemies[i].isActive) {
				putimage(enemies[i].x, enemies[i].y, &img_enemyPlane1, NOTSRCERASE);
				putimage(enemies[i].x, enemies[i].y, &img_enemyPlane2, SRCINVERT);
			}
		}
	}
	else
	{
		putimage(position_x - 50, position_y - 60, &img_planeExplode1, NOTSRCERASE); // 显示爆炸飞机	
		putimage(position_x - 50, position_y - 60, &img_planeExplode2, SRCINVERT);
	}
	outtextxy(Width * 0.48, High * 0.95, "得分：");
	char s[5];
	sprintf_s(s, "%d", score);
	outtextxy(Width * 0.55, High * 0.95, s);
	FlushBatchDraw();
	Sleep(2);
}

void cleanSkillThread()
{
	cleanEnemy = true;
	std::this_thread::sleep_for(std::chrono::seconds(10)); // 休眠5秒
	cleanEnemy = false;
}

void cleanSkill()
{
	if (!cleanEnemy)
	{
		// 创建一个新线程来处理清屏技能的冷却机制
		std::thread t(cleanSkillThread);
		t.detach(); // 我们调用detach，让新线程在后台运行，不需要主线程来等待其完成
	}
}

void updateWithoutInput()
{
	if (isExpolde == 0)
	{
		if (bullet_y > -25)
			bullet_y = bullet_y - 2;

		// 更新敌人
		for (int i = 0; i < MAX_ENEMIES; i++) {
			if (enemies[i].isActive) {
				if (enemies[i].y < High - 25)
					enemies[i].y = enemies[i].y + 0.5;
				else
					enemies[i].y = 10;

				// 检查子弹是否击中了敌人或使用了清空敌机的技能
				if (abs(bullet_x - enemies[i].x) + abs(bullet_y - enemies[i].y) < 80 || cleanEnemy) {
					enemies[i].isActive = false;
					enemies[i].x = rand() % Width;
					enemies[i].y = -40;
					bullet_y = -85;
					score++;

					if (score > 0 && score % 5 == 0 && score % 2 != 0)
					{
						mciSendString("close 5music", NULL, 0, NULL); // 先把前面一次的音乐关闭
						mciSendString("open resources/5.mp3 alias 5music", NULL, 0, NULL); // 打开音乐
						mciSendString("play 5music", NULL, 0, NULL); // 仅播放一次
					}
					if (score % 10 == 0)
					{
						mciSendString("close 10music", NULL, 0, NULL); // 先把前面一次的音乐关闭
						mciSendString("open resources/10.mp3 alias 10music", NULL, 0, NULL); // 打开音乐
						mciSendString("play 10music", NULL, 0, NULL); // 仅播放一次
					}
				}
			}
			else {
				// 一段时间后重新激活敌人
				enemies[i].y += 0.5;
				if (enemies[i].y > High) {
					enemies[i].isActive = true;
				}
			}
		}

		// 检查敌机是否撞击我们的飞机
		for (int i = 0; i < MAX_ENEMIES; i++) {
			if (enemies[i].isActive && (abs(position_x - enemies[i].x) + abs(position_y - enemies[i].y) < 75)) {	// 继续使用曼哈顿距离，慎用欧式距离，尽管精确，但对于敌机模型精确度不高的情况下用曼哈顿距离减少误判，尽可能大地保留玩家游戏体验
				isExpolde = 1;

				mciSendString("close exmusic", NULL, 0, NULL); // 先把前面一次的音乐关闭
				mciSendString("open resources/explode.mp3 alias exmusic", NULL, 0, NULL); // 打开音乐
				mciSendString("play exmusic", NULL, 0, NULL); // 仅播放一次
			}
		}

		// 检查是否有敌人的子弹击中我们的飞机
		for (int i = 0; i < MAX_BULLETS; i++) {
			if (bullets[i].isActive && (pow(position_x - bullets[i].x, 2) + pow(position_y - bullets[i].y, 2)) < pow(50, 2)) {	// 改用欧氏距离代替曼哈顿距离
				isExpolde = 1;

				mciSendString("close exmusic", NULL, 0, NULL); // 先把前面一次的音乐关闭
				mciSendString("open resources/explode.mp3 alias exmusic", NULL, 0, NULL); // 打开音乐
				mciSendString("play exmusic", NULL, 0, NULL); // 仅播放一次
			}
		}

		// 更新敌机子弹
		for (int i = 0; i < MAX_BULLETS; i++) {
			if (bullets[i].isActive) {
				bullets[i].y += BULLET_SPEED;
				if (bullets[i].y > High) {
					bullets[i].isActive = false;
				}
			}
		}

		// 检查敌机是否需要发射子弹
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
		MOUSEMSG m;		// 定义鼠标消息
		while (MouseHit())  //这个函数用于检测当前是否有鼠标消息
		{
			m = GetMouseMsg();
			if (m.uMsg == WM_MOUSEMOVE)
			{
				// 飞机的位置等于鼠标所在的位置
				position_x = m.x;
				position_y = m.y;
			}
			else if (m.uMsg == WM_LBUTTONDOWN)
			{
				// 按下鼠标左键，发射子弹
				bullet_x = position_x;
				bullet_y = position_y - 85;

				mciSendString("close fgmusic", NULL, 0, NULL); // 先把前面一次的音乐关闭
				mciSendString("open resources/f_gun.mp3 alias fgmusic", NULL, 0, NULL); // 打开音乐
				mciSendString("play fgmusic", NULL, 0, NULL); // 仅播放一次
			}
			else if (m.uMsg == WM_RBUTTONDOWN)
			{
				// 按下鼠标右键，使用清屏技能
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
	startup();  // 数据初始化
	while (1)  //  游戏循环执行
	{
		show();  // 显示画面
		updateWithoutInput();  // 与用户输入无关的更新
		updateWithInput();     // 与用户输入有关的更新
	}
	gameover();     // 游戏结束、后续处理
	return 0;
}