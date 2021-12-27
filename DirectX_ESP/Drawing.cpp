#include "includes.h"

void Line(float x1, float y1, float x2, float y2, float width, bool antilias, DWORD color, LPDIRECT3DDEVICE9 d3d_devise) {
	ID3DXLine* line;
	D3DXCreateLine(d3d_devise, &line);
	D3DXVECTOR2 linePos[] = { D3DXVECTOR2(x1,y1),D3DXVECTOR2(x2,y2) };
	line->SetWidth(width);
	if (antilias) {
		line->SetAntialias(antilias);
	}
	line->Begin();
	line->Draw(linePos, 2, color);
	line->End();
	line->Release();
}
void DrawBox(float x, float y, float width, float height, float px, DWORD color, LPDIRECT3DDEVICE9 d3d_devise)
{
	ID3DXLine* p_Line;
	D3DXCreateLine(d3d_devise, &p_Line);
	D3DXVECTOR2 points[5];
	points[0] = D3DXVECTOR2(x, y);
	points[1] = D3DXVECTOR2(x + width, y);
	points[2] = D3DXVECTOR2(x + width, y + height);
	points[3] = D3DXVECTOR2(x, y + height);
	points[4] = D3DXVECTOR2(x, y);
	p_Line->SetWidth(px);
	p_Line->Draw(points, 5, color);
	p_Line->End();
	p_Line->Release();
}