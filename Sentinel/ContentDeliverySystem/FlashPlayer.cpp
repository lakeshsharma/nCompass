#include "stdafx.h"

#include "FlashPlayer.h"

using namespace std;

static const DWORD END_FLASHPLAYER_TASK_MAXIMUM_TICKS = 1000;
static const UINT TERMINATE_FLASHPLAYER_TASK_EXIT_CODE = 0;

CFlashPlayer::CFlashPlayer(const string& windowName)
    :   m_flashPlayerWindowName(windowName)
{	
	StartFlashPlayer(m_flashPlayerWindowName);
}

CFlashPlayer::~CFlashPlayer()
{
	StopFlashPlayer();
}

void CFlashPlayer::StopFlashPlayer()
{
	//TODO
}

void CFlashPlayer::StartFlashPlayer(const string& filename)
{
//TODO
	string pth(filename);

	//if the window is not up yet, then give it a chance
	int count = 0;
	while (!IsFlashPlayerRunning() && count++ < 20)
	{
//		Sleep(10);
	}
}

bool CFlashPlayer::IsFlashPlayerRunning()
{
	//TODO
	return true;
}
