#include "includes.h"
#include "Memory.h"
#include <iostream>

Memory mem;

HANDLE game;
DWORD procces_id;
DWORD moduleBase_client;

float viewMatrix[4][4];
DWORD playerLocal;
DWORD iLocalHp;
DWORD localTeam;
DWORD entTeam;
DWORD pEntity;
DWORD entHp;
D3DXVECTOR3 entPos;
D3DXVECTOR3 entHeadPos;
D3DXVECTOR2 screenPos;
D3DXVECTOR2 screenHeadPos;
FLOAT boxHeight;
FLOAT boxWeight;

D3DXVECTOR3 getBonePos(int pTargetIn, int bone)
{
	const int matrix = mem.Read<int>(pTargetIn + hazedumper::netvars::m_dwBoneMatrix,game);
	return D3DXVECTOR3(
		mem.Read<float>(matrix + 0x30 * bone + 0xC,game),
		mem.Read<float>(matrix + 0x30 * bone + 0x1C,game),
		mem.Read<float>(matrix + 0x30 * bone + 0x2C,game)
	);
}

void StartCsGo() {
	if(mem.GetInfoWindow(L"Counter-Strike: Global Offensive", procces_id, game));
	moduleBase_client = mem.GetModuleBaseAddress(procces_id, L"client.dll");
}

bool WorldToScreen(D3DXVECTOR3 world, D3DXVECTOR2& screen, const RECT m_Rect, float flMatrix[4][4]) {
	float w = 0.f;

	screen.x = flMatrix[0][0] * world.x + flMatrix[0][1] * world.y + flMatrix[0][2] * world.z + flMatrix[0][3];
	screen.y = flMatrix[1][0] * world.x + flMatrix[1][1] * world.y + flMatrix[1][2] * world.z + flMatrix[1][3];
	w = flMatrix[3][0] * world.x + flMatrix[3][1] * world.y + flMatrix[3][2] * world.z + flMatrix[3][3];

	if (w < 0.01f) {
		return false;
	}

	float inw = 1.f / w;

	screen.x *= inw;
	screen.y *= inw;

	int weight = static_cast<int>(m_Rect.right - m_Rect.left);
	int height = static_cast<int>(m_Rect.bottom - m_Rect.top);

	float x = float(weight / 2);
	float y = float(height / 2);

	x += float(screen.x * x);
	y -= float(screen.y * y);

	screen.x = x + m_Rect.left;
	screen.y = y + m_Rect.top;

	return true;
}

void ESP(RECT desk_r, LPDIRECT3DDEVICE9 dev) {
	playerLocal = mem.Read<DWORD>(moduleBase_client + hazedumper::signatures::dwLocalPlayer, game);
	iLocalHp = mem.Read<DWORD>(playerLocal + 0x100, game);
	if (!iLocalHp) {
		return;
	}
	localTeam = mem.Read<DWORD>(playerLocal + 0xF4, game);
	mem.Read(moduleBase_client + hazedumper::signatures::dwViewMatrix, game, viewMatrix, 64);

	for (int i = 0; i <= 32; i++) {
		pEntity = mem.Read<DWORD>(moduleBase_client + hazedumper::signatures::dwEntityList + (i * 0x10),game);
		if (!pEntity) {
			continue;
		}
		entHp = mem.Read<DWORD>(pEntity + hazedumper::netvars::m_iHealth, game);
		if (!entHp) {
			continue;
		}
		entTeam = mem.Read<DWORD>(pEntity + hazedumper::netvars::m_iTeamNum, game);
		if (entTeam == localTeam) {
			continue;
		}
		if (mem.Read<bool>(pEntity + hazedumper::signatures::m_bDormant, game)) {
			continue;
		}
		entPos = mem.Read<D3DXVECTOR3>(pEntity + hazedumper::netvars::m_vecOrigin, game);
		entHeadPos = getBonePos(pEntity, 8);
		if (!WorldToScreen(entPos, screenPos, desk_r, viewMatrix)) {
			continue;
		}
		if (!WorldToScreen(entHeadPos, screenHeadPos, desk_r, viewMatrix)) {
			continue;
		}
		boxHeight = abs(screenHeadPos.y - screenPos.y);
		boxWeight = boxHeight / 2;
		DrawBox(screenPos.x - boxWeight / 2, screenHeadPos.y-10, boxWeight, boxHeight+10, 3, D3DCOLOR_ARGB(255, 255, 255, 0),dev);
		Line(1440/2, 900/2, screenPos.x, screenPos.y,3,true, D3DCOLOR_ARGB(255, 255, 255, 0), dev);
	}
}

void CloseCs() {
	mem.Exit(game);
}


