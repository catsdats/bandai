#include "DxLib.h"
#include<stdlib.h>
#include<time.h>


//表示処理する車の変数
enum { RED, YELLOW, BLUE, TRUCK };	//車種gg
const int CAR_MAX = 4;	//車数
int imgCar[CAR_MAX];	//それぞれの車の写真
const int CAR_W[CAR_MAX] = { 32,26,26,40 };	//車の横幅
const int CAR_H[CAR_MAX] = { 48,48,48,100 };//車の縦幅


//表示する数字の変数
int score=0;
int highscore=2000;
int fuel=200;


//車を表示する関数
void drawCar(int x, int y, int type)
{
	DrawGraph(x - CAR_W[type] / 2, y - CAR_H[type] / 2, imgCar[type], TRUE);	//x,yを中心に画像を表示
}

//文字列を表示する関数
void drawText(int x, int y, int col, const char* txt, int val, int siz)
{
	SetFontSize(siz);
	DrawFormatString(x+2,y+2,0x000000,txt,val);	//文字の影
	DrawFormatString(x,y,col,txt,val);			//文字
}


//メイン関数
int WINAPI WinMain(HINSTANCE hIntance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	printf("printf");
	//定数
	const int WIDTH = 720, HEIGHT = 640;//画面サイズ

	//裏方設定
	SetWindowText("カーレース");		//ウィンドウタイトル
	SetGraphMode(WIDTH, HEIGHT, 32);	//（横、縦、カラービット）で画面を設定
	ChangeWindowMode(TRUE);				//ウィンドウモードで起動
	if (DxLib_Init() == -1)return -1;	//ライブラリの初期化、エラーが起きたら修了
	SetBackgroundColor(0, 0, 0);		//背景色設定
	SetDrawScreen(DX_SCREEN_BACK);		//描画面を裏画面へ
	srand((unsigned int)time(NULL));
	
	//ゲーム進行系
	enum{TITLE,PLAY,OVER};	//画面モード
	int scene = TITLE;		//はじめはタイトルから
	int timer = 0;			//timer初期化

	//サウンド
	int bgm = LoadSoundMem("sound/bgm.mp3");		//bgm
	int gmov = LoadSoundMem("sound/gameover.mp3");	//ゲームオーバー
	int seFuel = LoadSoundMem("sound/fuel.mp3");	//燃料
	int seCrash = LoadSoundMem("sound/crash.mp3");	//ぶつかってるとき
	ChangeVolumeSoundMem(128, bgm);					
	ChangeVolumeSoundMem(128, gmov);				

	//背景
	int bgY = 0;							//背景画面を動かす変数
	int imgBG = LoadGraph("image/bg.png");	//背景画面


	//車画像
	imgCar[RED] = LoadGraph("image/car_red.png");			//赤い車
	imgCar[YELLOW] = LoadGraph("image/car_yellow.png");		//黄色い車
	imgCar[BLUE] = LoadGraph("image/car_blue.png");			//青い車
	imgCar[TRUCK] = LoadGraph("image/truck.png");			//トラック

	//燃料
	int imgFuel = LoadGraph("image/fuel.png");
	int fuelX = WIDTH / 2;
	int fuelY = 0;

	//プレイヤー
	int playerX = WIDTH / 2;	//操作する車の変数、初期は真ん中にいる
	int playerY = HEIGHT / 2;
	int playerType = RED;		//操作する車の色
	
	//コンピューターを作る
	const int COM_MAX = 8;//コンピュータの数
	int computerX[COM_MAX];//コンピュータのいる座標
	int computerY[COM_MAX];
	int computerType[COM_MAX];//車の種類とぬかされ判定
	int computerFlag[COM_MAX];
	int computerV[COM_MAX];//速さ
	int crash[COM_MAX];//ぶつかり判定

	//障害車初期配置
	for (int i = 0; i < COM_MAX; i++)
	{
		computerX[i] = rand() % 180 + 270;
		computerY[i] = -100;
		computerType[i] = YELLOW + rand() % 3;
		computerFlag[i] = 0;
		computerV[i] = i;
		crash[i] = 0;
	}

	//ゲーム中のメイン
	while (1)
	{
		ClearDrawScreen();	//画面初期化

		//背景処理
		if (scene == PLAY)bgY = (bgY + 10) % HEIGHT;//PLAY中は背景動く
		DrawGraph(0, bgY-HEIGHT, imgBG, FALSE);		//初期値は画面の真上で画像の透過は無しで背景表示
		DrawGraph(0, bgY, imgBG, FALSE);			//初期値は画面ピッタリで画像の透過は無しで背景表示

		//動く障害車
		if (scene == PLAY)
		
		{
			for (int i = 0; i < COM_MAX; i++)
			{


				computerY[i] = computerY[i] + 1 + computerV[i];

				//もし車が渡り切ったら
				if (computerY[i] >= HEIGHT + 100)
				{
					computerY[i] = -100;
					computerX[i] = rand() % 180 + 270;
					computerType[i] = YELLOW + rand() % 3;
					computerFlag[i] = 0;
					computerV[i] = rand() % 8+2;
				}
				//障害車と自車の距離
				int dx = abs(computerX[i] - playerX);
				int dy = abs(computerY[i] - playerY);
				//障害車と自分が重なる長さ
				int wid = abs(CAR_W[playerType] / 2 + CAR_W[computerType[i]] / 2) - 4;
				int hei = abs(CAR_H[playerType] / 2 + CAR_H[computerType[i]] / 2) - 4;
				//もし重なったら
				if (dx <= wid && dy <= hei)
				{
					//もしぶつかった瞬間なら
					if (crash[i] == 0)
					{
						score -= 10;//スコア、燃料の減少が多めになる
						fuel -= 10;
						PlaySoundMem(seCrash, DX_PLAYTYPE_BACK);
						crash[i] = 1;//ぶつかってるよ
					}
					score -= 1;
					fuel -= 5;
					int col = GetColor(rand() % 256, rand() % 256, rand() % 256);
					SetDrawBlendMode(DX_BLENDMODE_ADD, 255);
					DrawBox(playerX - CAR_W[playerType] / 2, playerY - CAR_H[playerType] / 2, playerX + CAR_W[playerType] / 2, playerY + CAR_H[playerType] / 2, col, TRUE);
					SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
					
				}
				//もし自車に抜かされたら
				else if (computerY[i] > playerY && computerFlag[i] == 0)
				{
					score += 100;
					computerFlag[i] = 1;
					if (highscore < score)
					{
						highscore = score;
					}
					crash[i] = 0;
				}
				else 
				{
					crash[i] = 0;
				}
				drawCar(computerX[i], computerY[i], computerType[i]);	//各障害車表示
			}	
		}

		//燃料の処理
		if (scene == PLAY)
		{
			fuelY += 4;//燃料が上から落ちてくる
			if (fuelY > HEIGHT)fuelY = -100;//下まで行ったら上に戻す
			if (abs(fuelX - playerX) < CAR_W[playerType] / 2 + 12 && abs(fuelY - playerY) < CAR_H[playerType] / 2 + 12)//もし重なれば
			{
				fuelX = rand() % 180 + 270;
				fuelY = -500;
				fuel += 200;
				PlaySoundMem(seFuel, DX_PLAYTYPE_BACK);
			}
			DrawGraph(fuelX - 12, fuelY - 12, imgFuel, TRUE);

			//プレイヤー車処理
			GetMousePoint(&playerX, &playerY);		//マウスの位置を変数へ入力
			if (playerX < 260)playerX = 260;		//マウスは左260以下に行かない
			if (playerX > 460)playerX = 460;		//マウスは右460以上に行かない
			if (playerY < 40)playerY = 40;			//マウスは下40以下に行かない
			if (playerY > 600)playerY = 600;		//マウスは上600以上に行かない
			drawCar(playerX, playerY, playerType);	//マウスの位置に車表示
		}
		timer++;
		switch (scene)
		{
		case TITLE:
			drawText(160, 160, 0xffffff, "Car race", 0, 100);
			if (timer % 60 < 30)drawText(210, 400, 0x00ff00, "Click to start", 0, 40);
			drawText(270, 590, 0x00ffff, "esc to exit", 0, 30);
			if (GetMouseInput() & MOUSE_INPUT_LEFT)
			{
				playerX = WIDTH / 2;
				playerY = HEIGHT / 2;
				for (int i = 0; i < COM_MAX; i++)
				{
					computerY[i] = HEIGHT + 100;
					computerFlag[i] = 0;
				}
				fuelX = WIDTH / 2;
				fuelY = -100;
				score = 0;
				fuel = 1000;
				scene = PLAY;
				PlaySoundMem(bgm, DX_PLAYTYPE_LOOP);

			}
			break;

		case PLAY:
			fuel -= 1;
			if (fuel <= 0)
			{
				fuel = 0;
				scene = OVER;
				timer = 0;
				StopSoundMem(bgm);
				PlaySoundMem(gmov, DX_PLAYTYPE_BACK);
			}
			break;

		case OVER:
			printf("over");
			drawText(180, 240, 0xff0000, "GAME OVER", 0, 80);
			if (timer > 60 * 5)scene = TITLE;
			break;

		}
		//文字列表示
		drawText(10,10,0x00ffff,"SCORE %d",score,30);
		drawText(WIDTH-200,10,0xffff00,"HI-SC %d",highscore,30);
		int col = 0x00ff00;
		if (fuel < 400)col = 0xffc000;
		if (fuel < 200)col = 0xff0000;
		drawText(10, HEIGHT-40,col, "FUEL %d", fuel, 30);

		//画面を表示処理
		ScreenFlip();									//裏画面を表画面へ
		WaitTimer(16);									//一定時間待つ
		if (ProcessMessage() == -1)break;				//windowsからエラーが出たら終了
		if (CheckHitKey(KEY_INPUT_ESCAPE) == 1)break;	//エスケープキー押したら終了
	}
	DxLib_End();	//ライブラリ使用の終了処理
	return 0;
}


