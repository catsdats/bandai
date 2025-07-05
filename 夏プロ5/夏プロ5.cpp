#include "DxLib.h"
#include<stdlib.h>
#include<time.h>


//�\����������Ԃ̕ϐ�
enum { RED, YELLOW, BLUE, TRUCK };	//�Ԏ�gg
const int CAR_MAX = 4;	//�Ԑ�
int imgCar[CAR_MAX];	//���ꂼ��̎Ԃ̎ʐ^
const int CAR_W[CAR_MAX] = { 32,26,26,40 };	//�Ԃ̉���
const int CAR_H[CAR_MAX] = { 48,48,48,100 };//�Ԃ̏c��


//�\�����鐔���̕ϐ�
int score=0;
int highscore=2000;
int fuel=200;


//�Ԃ�\������֐�
void drawCar(int x, int y, int type)
{
	DrawGraph(x - CAR_W[type] / 2, y - CAR_H[type] / 2, imgCar[type], TRUE);	//x,y�𒆐S�ɉ摜��\��
}

//�������\������֐�
void drawText(int x, int y, int col, const char* txt, int val, int siz)
{
	SetFontSize(siz);
	DrawFormatString(x+2,y+2,0x000000,txt,val);	//�����̉e
	DrawFormatString(x,y,col,txt,val);			//����
}


//���C���֐�
int WINAPI WinMain(HINSTANCE hIntance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	printf("printf");
	//�萔
	const int WIDTH = 720, HEIGHT = 640;//��ʃT�C�Y

	//�����ݒ�
	SetWindowText("�J�[���[�X");		//�E�B���h�E�^�C�g��
	SetGraphMode(WIDTH, HEIGHT, 32);	//�i���A�c�A�J���[�r�b�g�j�ŉ�ʂ�ݒ�
	ChangeWindowMode(TRUE);				//�E�B���h�E���[�h�ŋN��
	if (DxLib_Init() == -1)return -1;	//���C�u�����̏������A�G���[���N������C��
	SetBackgroundColor(0, 0, 0);		//�w�i�F�ݒ�
	SetDrawScreen(DX_SCREEN_BACK);		//�`��ʂ𗠉�ʂ�
	srand((unsigned int)time(NULL));
	
	//�Q�[���i�s�n
	enum{TITLE,PLAY,OVER};	//��ʃ��[�h
	int scene = TITLE;		//�͂��߂̓^�C�g������
	int timer = 0;			//timer������

	//�T�E���h
	int bgm = LoadSoundMem("sound/bgm.mp3");		//bgm
	int gmov = LoadSoundMem("sound/gameover.mp3");	//�Q�[���I�[�o�[
	int seFuel = LoadSoundMem("sound/fuel.mp3");	//�R��
	int seCrash = LoadSoundMem("sound/crash.mp3");	//�Ԃ����Ă�Ƃ�
	ChangeVolumeSoundMem(128, bgm);					
	ChangeVolumeSoundMem(128, gmov);				

	//�w�i
	int bgY = 0;							//�w�i��ʂ𓮂����ϐ�
	int imgBG = LoadGraph("image/bg.png");	//�w�i���


	//�ԉ摜
	imgCar[RED] = LoadGraph("image/car_red.png");			//�Ԃ���
	imgCar[YELLOW] = LoadGraph("image/car_yellow.png");		//���F����
	imgCar[BLUE] = LoadGraph("image/car_blue.png");			//����
	imgCar[TRUCK] = LoadGraph("image/truck.png");			//�g���b�N

	//�R��
	int imgFuel = LoadGraph("image/fuel.png");
	int fuelX = WIDTH / 2;
	int fuelY = 0;

	//�v���C���[
	int playerX = WIDTH / 2;	//���삷��Ԃ̕ϐ��A�����͐^�񒆂ɂ���
	int playerY = HEIGHT / 2;
	int playerType = RED;		//���삷��Ԃ̐F
	
	//�R���s���[�^�[�����
	const int COM_MAX = 8;//�R���s���[�^�̐�
	int computerX[COM_MAX];//�R���s���[�^�̂�����W
	int computerY[COM_MAX];
	int computerType[COM_MAX];//�Ԃ̎�ނƂʂ����ꔻ��
	int computerFlag[COM_MAX];
	int computerV[COM_MAX];//����
	int crash[COM_MAX];//�Ԃ��蔻��

	//��Q�ԏ����z�u
	for (int i = 0; i < COM_MAX; i++)
	{
		computerX[i] = rand() % 180 + 270;
		computerY[i] = -100;
		computerType[i] = YELLOW + rand() % 3;
		computerFlag[i] = 0;
		computerV[i] = i;
		crash[i] = 0;
	}

	//�Q�[�����̃��C��
	while (1)
	{
		ClearDrawScreen();	//��ʏ�����

		//�w�i����
		if (scene == PLAY)bgY = (bgY + 10) % HEIGHT;//PLAY���͔w�i����
		DrawGraph(0, bgY-HEIGHT, imgBG, FALSE);		//�����l�͉�ʂ̐^��ŉ摜�̓��߂͖����Ŕw�i�\��
		DrawGraph(0, bgY, imgBG, FALSE);			//�����l�͉�ʃs�b�^���ŉ摜�̓��߂͖����Ŕw�i�\��

		//������Q��
		if (scene == PLAY)
		
		{
			for (int i = 0; i < COM_MAX; i++)
			{


				computerY[i] = computerY[i] + 1 + computerV[i];

				//�����Ԃ��n��؂�����
				if (computerY[i] >= HEIGHT + 100)
				{
					computerY[i] = -100;
					computerX[i] = rand() % 180 + 270;
					computerType[i] = YELLOW + rand() % 3;
					computerFlag[i] = 0;
					computerV[i] = rand() % 8+2;
				}
				//��Q�ԂƎ��Ԃ̋���
				int dx = abs(computerX[i] - playerX);
				int dy = abs(computerY[i] - playerY);
				//��Q�ԂƎ������d�Ȃ钷��
				int wid = abs(CAR_W[playerType] / 2 + CAR_W[computerType[i]] / 2) - 4;
				int hei = abs(CAR_H[playerType] / 2 + CAR_H[computerType[i]] / 2) - 4;
				//�����d�Ȃ�����
				if (dx <= wid && dy <= hei)
				{
					//�����Ԃ������u�ԂȂ�
					if (crash[i] == 0)
					{
						score -= 10;//�X�R�A�A�R���̌��������߂ɂȂ�
						fuel -= 10;
						PlaySoundMem(seCrash, DX_PLAYTYPE_BACK);
						crash[i] = 1;//�Ԃ����Ă��
					}
					score -= 1;
					fuel -= 5;
					int col = GetColor(rand() % 256, rand() % 256, rand() % 256);
					SetDrawBlendMode(DX_BLENDMODE_ADD, 255);
					DrawBox(playerX - CAR_W[playerType] / 2, playerY - CAR_H[playerType] / 2, playerX + CAR_W[playerType] / 2, playerY + CAR_H[playerType] / 2, col, TRUE);
					SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
					
				}
				//�������Ԃɔ������ꂽ��
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
				drawCar(computerX[i], computerY[i], computerType[i]);	//�e��Q�ԕ\��
			}	
		}

		//�R���̏���
		if (scene == PLAY)
		{
			fuelY += 4;//�R�����ォ�痎���Ă���
			if (fuelY > HEIGHT)fuelY = -100;//���܂ōs�������ɖ߂�
			if (abs(fuelX - playerX) < CAR_W[playerType] / 2 + 12 && abs(fuelY - playerY) < CAR_H[playerType] / 2 + 12)//�����d�Ȃ��
			{
				fuelX = rand() % 180 + 270;
				fuelY = -500;
				fuel += 200;
				PlaySoundMem(seFuel, DX_PLAYTYPE_BACK);
			}
			DrawGraph(fuelX - 12, fuelY - 12, imgFuel, TRUE);

			//�v���C���[�ԏ���
			GetMousePoint(&playerX, &playerY);		//�}�E�X�̈ʒu��ϐ��֓���
			if (playerX < 260)playerX = 260;		//�}�E�X�͍�260�ȉ��ɍs���Ȃ�
			if (playerX > 460)playerX = 460;		//�}�E�X�͉E460�ȏ�ɍs���Ȃ�
			if (playerY < 40)playerY = 40;			//�}�E�X�͉�40�ȉ��ɍs���Ȃ�
			if (playerY > 600)playerY = 600;		//�}�E�X�͏�600�ȏ�ɍs���Ȃ�
			drawCar(playerX, playerY, playerType);	//�}�E�X�̈ʒu�Ɏԕ\��
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
		//������\��
		drawText(10,10,0x00ffff,"SCORE %d",score,30);
		drawText(WIDTH-200,10,0xffff00,"HI-SC %d",highscore,30);
		int col = 0x00ff00;
		if (fuel < 400)col = 0xffc000;
		if (fuel < 200)col = 0xff0000;
		drawText(10, HEIGHT-40,col, "FUEL %d", fuel, 30);

		//��ʂ�\������
		ScreenFlip();									//����ʂ�\��ʂ�
		WaitTimer(16);									//��莞�ԑ҂�
		if (ProcessMessage() == -1)break;				//windows����G���[���o����I��
		if (CheckHitKey(KEY_INPUT_ESCAPE) == 1)break;	//�G�X�P�[�v�L�[��������I��
	}
	DxLib_End();	//���C�u�����g�p�̏I������
	return 0;
}


