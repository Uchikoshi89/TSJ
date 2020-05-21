#include <math.h >	
#include "main.h"
#include "player.h"
#include "shot.h"
#include "enemy.h"
#include "bullet.h"
#include "blast.h"
#include "key.h"
#include "effects.h"
#include "Score.h"
#include "DataSaveLoad.h"

typedef enum {
	GMODE_INIT,
	GMODE_TITLE,
	GMODE_GAME,
	GMODE_OVER,
	GMODE_CLEAR
}GAME_MODE;

GAME_MODE gameMode;

// title
int titleImage;
int titlecnt;

// bright
int bright;		// ���邳
int changeMode;	// 0:̪��ނ��Ă��Ȃ�,1: ̪��ޒ�

int pause;

// gameover
int overImage;
int overcnt;

// hiscore
int hiscore;


// stage
int stageLImage;
int stageRImage;
int backImage;

CHARACTER bzero;	// �^�񒆂̔w�i
CHARACTER bcMove;	// ���̔w�i
CHARACTER bcMove2;	// ����̔w�i



CHARACTER lstage;
CHARACTER rstage;
CHARACTER lstagemove;
CHARACTER rstagemove;


int  GameSystemInit(void);
void GameInit(void);
void GameMain(void);
void GameDraw(void);
void GameTitle(void);
void GameOver(void);
void BulletCtr(void);
void PlayerCtr(void);
void ShotCtr(void);
void BlastCtr(void);
void BlastCtr2(void);
//void BlastCtr3(void);
void BlastGenerator(xy pos);
void SparkCtr(void);

// ========== WinMain�֐� 
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	GameSystemInit();
	GameInit();

   // ---------- �ް�ٰ��  
	while (ProcessMessage() == 0 && CheckHitKey(KEY_INPUT_ESCAPE) == 0) 
	{
		ClsDrawScreen();

		KeyCheck();

		switch (gameMode)
		{
		case GMODE_INIT:
			GameInit();
			break;
		case GMODE_TITLE:
			GameTitle();
			break;
		case GMODE_GAME:
			GameMain();
			break;
		case GMODE_OVER:
			GameOver();
			break;
		case GMODE_CLEAR:
			break;
		default:
			break;
		}
		// Ҳݏ��� 

		ScreenFlip(); // ����ʂ�\��ʂɏu�Ժ�߰  
	}
	DxLib_End(); // DXײ���؂̏I������ 
	return 0;  // ������۸��т̏I�� 
}

// ---------- �O���t�B�b�N�̓o�^ ----------- 
int GameSystemInit(void)
{
	// ���я���  
	SetGraphMode(SCREEN_SIZE_X, SCREEN_SIZE_Y, 16); // 640�~480�ޯ�65536�FӰ�ނɐݒ�  
	ChangeWindowMode(true);     // true:window false:�ٽ�ذ�  
	SetWindowText("SPACE_TRAVELER");

	// DXײ���؏��������� 
	if (DxLib_Init() == -1)
	{
		return -1;
	}

	SetDrawScreen(DX_SCREEN_BACK);  // �ЂƂ܂��ޯ��ޯ̧�ɕ`�� 


	// title
	titleImage = LoadGraph("image/title.png");

	// over
	overImage = LoadGraph("image/GameOver.png");

	// spark
	ChargeEffectSystemInit();

	// player
	PlayerSystemInit();

	// shot
	ShotSystemInit();

	// enemy(2��)
	EnemySystemInit();

	// bullet(2��)
	BulletSystemInit();

	// blast
	BlastSystemInit();

	//number 
	NumberFirstInit();

	// hiscore 
	hiscore = 0;
	HighScoreSaveB();

	// back
	backImage = LoadGraph("image/haikei2.png");

	gameMode = GMODE_INIT;	// INIT�֍s��
}


// ----------- �ϐ������� ----------
void GameInit(void)
{
	if (hiscore < player.score)
	{
		hiscore = player.score;
		HighScoreSaveB();
	}

	// title
	titlecnt = 0;

	// bright 
	bright = 255;
	pause = 0;

	// over
	overcnt = 0;

	// �X�N���[�����W
	bzero.pos.x = 0;
	bzero.pos.y = 0;
	bcMove.pos.x = 0;
	bcMove.pos.y = -600;
	bcMove2.pos.x = 0;
	bcMove2.pos.y = 600;

	// spark
	ChargeEffectInit();

	// player 
	PlayerInit();

	// shot
	ShotInit();

	// enemy 
	EnemyInit();
	
	// bullet
	BulletInit();

	// blast
	BlastInit();

	gameMode = GMODE_TITLE;	// TITLE�֍s��
}

void GameTitle(void)
{
	if (changeMode == 1)
	{
		bright -=4;		// ���邳�����炷
		if (bright < 0) {
			gameMode = GMODE_GAME;
			bright = 0;
		}
	}
	else {
		bright = 255;	// ���邳���ő�ɂ���
		if (trgkey[P1_CENTER]) {
			changeMode = 1;		// ̪��ނ��J�n����
		}
	}

	SetDrawBright(bright, bright, bright);

	DrawGraph(SCREEN_SIZE_X / 2 - 360 / 2, 100, titleImage, true);	// ���ى��

	titlecnt++;
	if (titlecnt / 35 % 2 == 0) {
		DrawString(400 - 8 * 6, 460, "PUSH_SPACE_KEY", 0xffffff);	// �����̏���
	}
	DrawString(0, 0, "TITLE", 0xffff00);
}

void GameMain(void)
{
	if(pause == 0){
	if (changeMode == 1)
	{
		bright += 5;
		if (bright > 255) {
			bright = 255;
			changeMode = 0;
		}
		SetDrawBright(bright, bright, bright);
	}

	else {
		PlayerUpData();
		ShotUpData();
		BulletUpData();
		BlastCtr();
		BlastCtr2();
	//	BlastCtr3();
		ChargeEffectUpData();
	}

		GameDraw();
	if ((trgkey[P1_PAUSE]) || (trgkey[P2_PAUSE]))
		{
		pause++;
		pause = pause % 2;
		}
	}

	for (int no = 0; no < 10; no++)
	{
		NumberDraw(800 - 32, 300, player.score);
	}

	NumberDraw(800-32, 0, hiscore);//�n�C�X�R�A


	if (playerLife < 0)
	{
		gameMode = GMODE_OVER;
	}
	
	if (playerLife < 0)
	{
		playerLife += 10;
	}
}

void GameDraw(void)
{

	int speed = 5;
	
	DrawString(0, 0, "MAIN", 0xffff00);

	// �w�i�\��
	DrawGraph(bzero.pos.x, bzero.pos.y, backImage, true);
	DrawGraph(bcMove.pos.x, bcMove.pos.y, backImage, true);
	DrawGraph(bcMove2.pos.x, bcMove2.pos.y, backImage, true);

	bzero.pos.y += 5;
	bcMove.pos.y += 5;
	bcMove2.pos.y += 5;

	//�w�i�X�N���[���\��
	if (bzero.pos.y >= SCREEN_SIZE_Y)	// Y���W����ʉ��ɂ�������
	{
		bzero.pos.y = bcMove.pos.y - 600;	// ��ʏ�ɂ����Ă���
	}

	if (bcMove.pos.y >= 600) // Y���W��0�ɂ�������
	{
		bcMove.pos.y = bzero.pos.y -600;	// �ꖇ�ڂ̏�ɂ����Ă���
	}

	if (bcMove2.pos.y >= 600)
	{
		if (bcMove.pos.y - 600)
		{
			bcMove2.pos.y = bzero.pos.y - 600;	// 
		}
	}

	// spark
	ChargeEffectDraw();

	// player
	PlayerDraw();

	// shot
	ShotDraw();

	// enemy
	EnemyDraw();

	// bullet
	BulletDraw();

	// blast 
	BlastDraw();
	
}

void SparkCtr(void)
{
	ChargeEffectUpData();
}


void PlayerCtr(void)
{
	PlayerUpData();
}

void ShotCtr(void)
{
	ShotUpData();
}

void BulletCtr(void)
{
	BulletUpData();
}

void GameOver(void)
{
	if (trgkey[P1_CENTER])
	{
		if (hiscore < player.score)
		{
			hiscore = player.score;
			HighScoreSaveB();
		}
			gameMode = GMODE_TITLE;	// TITLE�֍s��
	}

	DrawGraph(SCREEN_SIZE_X / 2 - 430 / 2, 100, overImage, true);

	overcnt++;
	if (overcnt / 15 % 2 == 0) {
		DrawString(400 - 8 * 6, 460, "PUSH_SPACE_KEY", 0xffffff);	// �����̏���
	}
	DrawString(0, 0, "GAMEOVER", 0xffff00);

}


}
