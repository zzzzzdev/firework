#include<stdio.h>
#include<graphics.h>
#include<math.h>
#include<time.h>
#include<conio.h>
#include<mmsystem.h>//设置媒体接口的头文件//
#pragma comment (lib,"winmm.lib")//设置多媒体库文件//
#define PI 3.1415
#define NUM 10
#define WIDTH 960
#define HEIGHT 640
//烟花弹的结构体//
struct Jet
{
	int x, y;//初始生成的位置//
	int hx, hy;//最高的位置//
	IMAGE Img[2];//循环贴图//
	byte n : 1;//位段
	bool isLaunch;//烟花弹是否在发射中//
}Jet[NUM];
struct fire 
{
	int x, y;
	int r;//当前绽放半径//
	int width, height;
	int cen_x, cen_y;//焰火的圆心坐标//
	int  max_r;//焰火的最大半径//
	DWORD  pixel[240][240];//**图片像素数组//
	bool isShow;//烟花是否准备好了//
	bool isDraw;//是否绘制烟花//
}fire[NUM];

//开场动画//
void welcome()
{
	mciSendString(L"open ./res/小幸运.mp3", 0, 0, 0);//**先打开//注意寻找的路径,不是放在DEBUG里面//
	mciSendString(L"play ./res/小幸运.mp3", 0, 0, 0);//**再播放//
	for (int i = 1; i < 40; i++)
	{
		cleardevice();//清除屏幕//
		int x = 150 - 100 * cos(2*PI / 60 * i);
		int y = 150 - 100* sin( 2*PI / 60 * i);
		settextstyle(i,i, L"楷体");
		setbkmode(TRANSPARENT);
		settextcolor(RGB(0, 202, 0));
		outtextxy(x, y, L"新年快乐，哈哈哈哈！");
		Sleep(40);
		settextstyle(20, 20, L"楷体");
		outtextxy(400, 600, L"按任意键继续");
	}
	getchar();//按任意键继续//
	cleardevice();
	settextstyle(40, 40, L"楷体");
	outtextxy(300, 250, L"你是猪吗？");
	outtextxy(300, 300, L"哈哈哈哈哈"); 
	outtextxy(300, 350, L"祝你新年快乐"); 
	outtextxy(300, 400, L"请欣赏烟花表演");

}
void InitData(int i)
{
	fire[i].cen_x = 120;
	fire[i].cen_y = 120;
	fire[i].max_r = 120;
	fire[i].r = 0;
	fire[i].width = 240;
	fire[i].height = 240;
	fire[i].isDraw = false;
	fire[i].isShow = false;
}
//提取照片//
void loadImg()
{
	//初始化焰火//
	IMAGE fireImg, tfire;
	loadimage(&fireImg, L"./res/flower.jpg", 3120, 240);
	for (int i = 0; i < NUM; i++)
	{
		InitData(i);//**初始化焰火，肯定是需要初始化数据的//
		SetWorkingImage(&fireImg);
		getimage(&tfire, i * 240, 0, 240, 240);
		SetWorkingImage(&tfire);
		for (int a = 0; a < 240; a++)
		{
			for (int b = 0; b < 240; b++)
			{
				fire[i].pixel[a][b] = getpixel(a, b);
			}
		}
	}
	
	//初始化烟花弹//
	IMAGE Jetimg;
	loadimage(&Jetimg, L"./res/launch.jpg", 200, 50);
	SetWorkingImage(&Jetimg);
	for (int i = 0; i < NUM; i++)
	{
		int n = rand() % 5;//**随机数是模除
		getimage(&Jet[i].Img[0], n * 20, 0, 20, 50);
		getimage(&Jet[i].Img[1], n * 20 + 100, 0, 20, 50);
		Jet[i].isLaunch = false;//**严重区分赋值和判断条件**//
	}
	SetWorkingImage();
}
//产生烟花弹//**需要对结构体里面所有的数据都赋初值//
void createJet()
{
	int i = rand() % NUM;
	if (Jet[i].isLaunch == false)
	{
		Jet[i].x = rand() % (WIDTH - 20);//设置初始化随机位置//**深刻理解这个模除形成随机数//
		Jet[i].y = rand() % 100 + HEIGHT;
		Jet[i].hx = Jet[i].x;
		Jet[i].hy = rand() % (HEIGHT / 3 * 2);
		Jet[i].isLaunch = true;
	}
}
//发射烟花弹//
void launch()
{
	for (int i = 0; i < NUM; i++)
	{
		if (Jet[i].isLaunch)
		{
			putimage(Jet[i].x, Jet[i].y, &Jet[i].Img[Jet[i].n], SRCINVERT);
			if (Jet[i].y > Jet[i].hy)//**判断停止操作//
			{
				Jet[i].y -= 5;
				Jet[i].n++;
			}
			putimage(Jet[i].x, Jet[i].y, &Jet[i].Img[Jet[i].n], SRCINVERT);
			if(Jet[i].y <= Jet[i].hy)
			{
				Jet[i].isLaunch = false;//**停止的判断依据//
				fire[i].x = Jet[i].x;
				fire[i].y = Jet[i].y;
				fire[i].isShow = true;
			}
		}
	}
}

//焰火炸开//
void bloom(DWORD *pMem)
{
	for (int i = 0; i < NUM; i++)
	{
		if (fire[i].isShow)
		{
			if (fire[i].r < fire[i].max_r)
			{
				fire[i].r++;
				fire[i].isDraw = true;
			}
			if (fire[i].r >= fire[i].max_r)
			{
				InitData(i);
			}
		}
		if (fire[i].isDraw)//是否可以开始绘制//
		{
			for (double a = 0; a < 2 * PI; a += 0.01)
			{
				int img_x = fire[i].cen_x + fire[i].r*cos(a);
				int img_y = fire[i].cen_y + fire[i].r*sin(a);
				if (img_x>0&&img_x<fire[i].width&&img_y>0&&img_y<fire[i].height)
				{
					int win_x = fire[i].x + fire[i].r*cos(a);
					int win_y = fire[i].y + fire[i].r*sin(a);
					if (win_x > 0 && win_x < WIDTH&&win_y>0 && win_y < HEIGHT)
					{
						pMem[win_y * WIDTH + win_x] = BGR(fire[i].pixel[img_x][img_y]);
					}
				}
				
			}
		}
	}
}
int main()
{
	initgraph(960, 640);
	srand((unsigned)time(NULL) + clock());
	DWORD *pMem = GetImageBuffer();//**获取窗口内存指针//
	welcome();
	loadImg();
	while (1)
	{
		for (int  i = 0; i < WIDTH; i++)
		{
			for (int k = 0; k < 4; k++)
			{
				int x = rand() % WIDTH;
				int y = rand() % HEIGHT;
				if (y < HEIGHT)
				{
					pMem[y * WIDTH + x] = BLACK;//**注意这里是中括号//
				}
			}
		}
		for (int i = 0; i < WIDTH; i++)
		{
			for (int k = 0; k < 4; k++)
			{
				int x = rand() % WIDTH;
				int y = rand() % HEIGHT;
				if (y < HEIGHT)
				{
					pMem[y * WIDTH + x] = BLACK;//**注意这里是中括号//
				}
			}
		}
		for (int i = 0; i < WIDTH; i++)
		{
			for (int k = 0; k < 4; k++)
			{
				int x = rand() % WIDTH;
				int y = rand() % HEIGHT;
				if (y < HEIGHT)
				{
					pMem[y * WIDTH + x] = BLACK;//**注意这里是中括号//
				}
			}
		}
		for (int i = 0; i < WIDTH; i++)
		{
			for (int k = 0; k < 4; k++)
			{
				int x = rand() % WIDTH;
				int y = rand() % HEIGHT;
				if (y < HEIGHT)
				{
					pMem[y * WIDTH + x] = BLACK;//**注意这里是中括号//
				}
			}
		}
		for (int i = 0; i < WIDTH; i++)
		{
			for (int k = 0; k < 4; k++)
			{
				int x = rand() % WIDTH;
				int y = rand() % HEIGHT;
				if (y < HEIGHT)
				{
					pMem[y * WIDTH + x] = BLACK;//**注意这里是中括号//
				}
			}
		}
		createJet();
		launch();
		bloom(pMem);
		Sleep(10);
	}
	closegraph();
	return 0;
}