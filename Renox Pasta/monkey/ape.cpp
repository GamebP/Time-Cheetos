#include "ape.h"
#include "rat.h"
Memory* KmDrv = nullptr;
#include "d3d9_x.h"
#include "xor.hpp"
#include <dwmapi.h>
#include <vector>
#include "Keybind.h"
#include <Windows.h>
#include "auth.hpp"
#include <string>
#include "imgui_help.h"
ImDrawList* draw;
#include "skStr.h"
#include <WinUser.h>


std::string tm_to_readable_time(tm ctx);
static std::time_t string_to_timet(std::string timestamp);
static std::tm timet_to_tm(time_t timestamp);

using namespace KeyAuth;

std::string name = "Test"; // application name. right above the blurred text aka the secret on the licenses tab among other tabs
std::string ownerid = "PuGZoqyGFu"; // ownerid, found in account settings. click your profile picture on top right of dashboard and then account settings.
std::string secret = "37a6c1c03b281185691d7e98df5bf90142bdc5ecdecf6f2ceaa4c0130dd56e47"; // app secret, the blurred text on licenses tab and other tabs
std::string version = "1.0"; // leave alone unless you've changed version on website
std::string url = "https://keyauth.win/api/1.2/"; // change if you're self-hosting

/*
	Video on what ownerid and secret are https://youtu.be/uJ0Umy_C6Fg

	Video on how to add KeyAuth to your own application https://youtu.be/GB4XW_TsHqA

	Video to use Web Loader (control loader from customer panel) https://youtu.be/9-qgmsUUCK4
*/

api KeyAuthApp(name, ownerid, secret, version, url);

bool fovcircle = true;
bool ShowMenu = true;
bool Esp = true;
bool Esp_box = false;
bool cornerbox = true;
bool Esp_info = true;
bool Spinbot = false;
bool distanceesp = true;
bool Esp_line = false;
bool Aimbot = true;
bool skeletonesp = true;
ImFont* m_pFont;
bool idc = false;
int smooth = 3.5;
static int VisDist = 270;
int AimFOV = 300;

static int aimkey = 0x02;
static int hitbox;
static int aimkeypos = 3;
static int hitboxpos = 0;
bool Filledfov = true;

bool aiminair = false;


static std::string ReadGetNameFromFName(int key) {
	uint32_t ChunkOffset = (uint32_t)((int)(key) >> 16);
	uint16_t NameOffset = (uint16_t)key;

	uint64_t NamePoolChunk = KmDrv->Rpm<uint64_t>(base_address + 0xDFE7980 + (8 * ChunkOffset) + 16) + (unsigned int)(4 * NameOffset);
	uint16_t nameEntry = KmDrv->Rpm<uint16_t>(NamePoolChunk);

	int nameLength = nameEntry >> 6;

	// SOURCE MADE BY Renox
	char buff[1024];
	if ((uint32_t)nameLength)
	{
		for (int x = 0; x < nameLength; ++x)
		{
			buff[x] = KmDrv->Rpm<char>(NamePoolChunk + 4 + x);
		}

		char* v2 = buff;
		signed int v4 = nameLength;
		uint64_t result;
		unsigned int v5;
		unsigned int v7;
		unsigned int v8;

		v5 = 0;
		result = 28;

		if (v4)
		{
			do
			{
				++v2;
				v7 = v5++ | 0xB000;
				v8 = v7 + result;
				v7 = v8 ^ ~*(BYTE*)(v2 - 1);
				result = v8 >> 2;
				*(BYTE*)(v2 - 1) = v7;
			} while (v5 < v4);
		}
		buff[nameLength] = '\0';
		return std::string(buff);
	}
	else {
		return "";
	}
}


static std::string GetNameFromFName(int key)
{
	uint32_t ChunkOffset = (uint32_t)((int)(key) >> 16);
	uint16_t NameOffset = (uint16_t)key;

	uint64_t NamePoolChunk = KmDrv->Rpm<uint64_t>(base_address + 0xDFE7980 + (8 * ChunkOffset) + 16) + (unsigned int)(4 * NameOffset); //((ChunkOffset + 2) * 8) ERROR_NAME_SIZE_EXCEEDED
	if (KmDrv->Rpm<uint16_t>(NamePoolChunk) < 64)
	{
		auto a1 = KmDrv->Rpm<DWORD>(NamePoolChunk + 4);
		return ReadGetNameFromFName(a1);
	}
	else
	{
		return ReadGetNameFromFName(key);
	}
}

DWORD_PTR Uworld;
DWORD_PTR LocalPawn;
DWORD_PTR PlayerState;
DWORD_PTR Localplayer;
DWORD_PTR Rootcomp;
DWORD_PTR PlayerController;
DWORD_PTR Persistentlevel;
uintptr_t PlayerCameraManager;
uintptr_t CurrentWeapon;
Vector3 localactorpos;

uint64_t TargetPawn;
int localplayerID;

RECT GameRect = { NULL };
D3DPRESENT_PARAMETERS d3dpp;

DWORD ScreenCenterX;
DWORD ScreenCenterY;
Vector3 LocalRelativeLocation; struct FBoxSphereBounds
{
	struct Vector3                                     Origin;                                                   // 0x0000(0x0018) (Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData)
	struct Vector3                                     BoxExtent;                                                // 0x0018(0x0018) (Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData)
	double                                             SphereRadius;                                             // 0x0030(0x0008) (Edit, BlueprintVisible, ZeroConstructor, SaveGame, IsPlainOldData)
};
static void xCreateWindow();
static void xInitD3d();
static void xMainLoop();
static void xShutdown();
static LRESULT CALLBACK WinProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

static HWND Window = NULL;
IDirect3D9Ex* p_Object = NULL;
static LPDIRECT3DDEVICE9 D3dDevice = NULL;
static LPDIRECT3DVERTEXBUFFER9 TriBuf = NULL;

DWORD Menuthread(LPVOID in)
{
	while (1)
	{
		if (MouseController::GetAsyncKeyState(VK_INSERT) & 1) {
			ShowMenu = !ShowMenu;
		}
		Sleep(2);
	}
}

FTransform GetBoneIndex(DWORD_PTR mesh, int index) {
	DWORD_PTR bonearray = KmDrv->Rpm<DWORD_PTR>(mesh + 0x5a0);
	if (bonearray == NULL) {
		bonearray = KmDrv->Rpm<DWORD_PTR>(mesh + 0x5a0 + 0x10);
	}
	return KmDrv->Rpm<FTransform>(bonearray + (index * 0x60));
}

Vector3 GetBoneWithRotation(DWORD_PTR mesh, int id)
{
	FTransform bone = GetBoneIndex(mesh, id);
	FTransform ComponentToWorld = KmDrv->Rpm<FTransform>(mesh + 0x240);

	D3DMATRIX Matrix;
	Matrix = MatrixMultiplication(bone.ToMatrixWithScale(), ComponentToWorld.ToMatrixWithScale());

	return Vector3(Matrix._41, Matrix._42, Matrix._43);
}

D3DXMATRIX Matrix(Vector3 rot, Vector3 origin = Vector3(0, 0, 0))
{
	float radPitch = (rot.x * float(M_PI) / 180.f);
	float radYaw = (rot.y * float(M_PI) / 180.f);
	float radRoll = (rot.z * float(M_PI) / 180.f);

	float SP = sinf(radPitch);
	float CP = cosf(radPitch);
	float SY = sinf(radYaw);
	float CY = cosf(radYaw);
	float SR = sinf(radRoll);
	float CR = cosf(radRoll);

	D3DMATRIX matrix;
	matrix.m[0][0] = CP * CY;
	matrix.m[0][1] = CP * SY;
	matrix.m[0][2] = SP;
	matrix.m[0][3] = 0.f;

	matrix.m[1][0] = SR * SP * CY - CR * SY;
	matrix.m[1][1] = SR * SP * SY + CR * CY;
	matrix.m[1][2] = -SR * CP;
	matrix.m[1][3] = 0.f;

	matrix.m[2][0] = -(CR * SP * CY + SR * SY);
	matrix.m[2][1] = CY * SR - CR * SP * SY;
	matrix.m[2][2] = CR * CP;
	matrix.m[2][3] = 0.f;

	matrix.m[3][0] = origin.x;
	matrix.m[3][1] = origin.y;
	matrix.m[3][2] = origin.z;
	matrix.m[3][3] = 1.f;

	return matrix;
}


//CamewaDescwipsion
struct Camera
{
	float FieldOfView;
	Vector3 Rotation;
	Vector3 Location;
};
Camera GetCamera(int64 a1)
{
	Camera FGC_Camera;
	int64 v1;
	int64 v6;
	int64 v7;
	int64 v8;

	v1 = KmDrv->Rpm<int64>(Localplayer + 0xd0);
	int64 v9 = KmDrv->Rpm<int64>(v1 + 0x10); // 0x10

	FGC_Camera.FieldOfView = 80.0f / (KmDrv->Rpm<double>(v9 + 0x680) / 1.19f); // 0x600

	FGC_Camera.Rotation.x = KmDrv->Rpm<double>(v9 + 0x7D0);
	FGC_Camera.Rotation.y = KmDrv->Rpm<double>(a1 + 0x148);

	uint64_t FGC_Pointerloc = KmDrv->Rpm<uint64_t>(Uworld + 0x100);
	FGC_Camera.Location = KmDrv->Rpm<Vector3>(FGC_Pointerloc);


	//v6 = driver.read<int64>(Localplayer + 0x70);
	//v7 = driver.read<int64>(v6 + 0x98);
	//v8 = driver.read<__int64>(v7 + 0xF8);
	//FGC_Camera.Location = driver.read<Vector3>(Uworld + 0x100); // 0x20

	return FGC_Camera;
}



Vector3 ProjectWorldToScreen(Vector3 WorldLocation)
{
	Camera vCamera = GetCamera(Rootcomp);
	vCamera.Rotation.x = (asin(vCamera.Rotation.x)) * (180.0 / M_PI);


	D3DMATRIX tempMatrix = Matrix(vCamera.Rotation);

	Vector3 vAxisX = Vector3(tempMatrix.m[0][0], tempMatrix.m[0][1], tempMatrix.m[0][2]);
	Vector3 vAxisY = Vector3(tempMatrix.m[1][0], tempMatrix.m[1][1], tempMatrix.m[1][2]);
	Vector3 vAxisZ = Vector3(tempMatrix.m[2][0], tempMatrix.m[2][1], tempMatrix.m[2][2]);

	Vector3 vDelta = WorldLocation - vCamera.Location;
	Vector3 vTransformed = Vector3(vDelta.Dot(vAxisY), vDelta.Dot(vAxisZ), vDelta.Dot(vAxisX));

	if (vTransformed.z < 1.f)
		vTransformed.z = 1.f;

	return Vector3((Width / 2.0f) + vTransformed.x * (((Width / 2.0f) / tanf(vCamera.FieldOfView * (float)M_PI / 360.f))) / vTransformed.z, (Height / 2.0f) - vTransformed.y * (((Width / 2.0f) / tanf(vCamera.FieldOfView * (float)M_PI / 360.f))) / vTransformed.z, 0);
}

void DrawStrokeText(int x, int y, const char* str)
{
	ImFont a;
	std::string utf_8_1 = std::string(str);
	std::string utf_8_2 = string_To_UTF8(utf_8_1);

	ImGui::GetOverlayDrawList()->AddText(ImVec2(x, y - 1), ImGui::ColorConvertFloat4ToU32(ImVec4(1 / 255.0, 1 / 255.0, 1 / 255.0, 255 / 255.0)), utf_8_2.c_str());
	ImGui::GetOverlayDrawList()->AddText(ImVec2(x, y + 1), ImGui::ColorConvertFloat4ToU32(ImVec4(1 / 255.0, 1 / 255.0, 1 / 255.0, 255 / 255.0)), utf_8_2.c_str());
	ImGui::GetOverlayDrawList()->AddText(ImVec2(x - 1, y), ImGui::ColorConvertFloat4ToU32(ImVec4(1 / 255.0, 1 / 255.0, 1 / 255.0, 255 / 255.0)), utf_8_2.c_str());
	ImGui::GetOverlayDrawList()->AddText(ImVec2(x + 1, y), ImGui::ColorConvertFloat4ToU32(ImVec4(1 / 255.0, 1 / 255.0, 1 / 255.0, 255 / 255.0)), utf_8_2.c_str());
	ImGui::GetOverlayDrawList()->AddText(ImVec2(x, y), ImGui::ColorConvertFloat4ToU32(ImVec4(255, 255, 255, 255)), utf_8_2.c_str());
}

float DrawOutlinedText(ImFont* pFont, const std::string& text, const ImVec2& pos, float size, ImU32 color, bool center)
{
	std::stringstream stream(text);
	std::string line;

	float y = 0.0f;
	int i = 0;

	while (std::getline(stream, line))
	{
		ImVec2 textSize = pFont->CalcTextSizeA(size, FLT_MAX, 0.0f, line.c_str());

		if (center)
		{
			ImGui::GetOverlayDrawList()->AddText(pFont, size, ImVec2((pos.x - textSize.x / 2.0f) + 1, (pos.y + textSize.y * i) + 1), ImGui::GetColorU32(ImVec4(0, 0, 0, 255)), line.c_str());
			ImGui::GetOverlayDrawList()->AddText(pFont, size, ImVec2((pos.x - textSize.x / 2.0f) - 1, (pos.y + textSize.y * i) - 1), ImGui::GetColorU32(ImVec4(0, 0, 0, 255)), line.c_str());
			ImGui::GetOverlayDrawList()->AddText(pFont, size, ImVec2((pos.x - textSize.x / 2.0f) + 1, (pos.y + textSize.y * i) - 1), ImGui::GetColorU32(ImVec4(0, 0, 0, 255)), line.c_str());
			ImGui::GetOverlayDrawList()->AddText(pFont, size, ImVec2((pos.x - textSize.x / 2.0f) - 1, (pos.y + textSize.y * i) + 1), ImGui::GetColorU32(ImVec4(0, 0, 0, 255)), line.c_str());

			ImGui::GetOverlayDrawList()->AddText(pFont, size, ImVec2(pos.x - textSize.x / 2.0f, pos.y + textSize.y * i), ImGui::GetColorU32(color), line.c_str());
		}
		else
		{
			ImGui::GetOverlayDrawList()->AddText(pFont, size, ImVec2((pos.x) + 1, (pos.y + textSize.y * i) + 1), ImGui::GetColorU32(ImVec4(0, 0, 0, 255)), line.c_str());
			ImGui::GetOverlayDrawList()->AddText(pFont, size, ImVec2((pos.x) - 1, (pos.y + textSize.y * i) - 1), ImGui::GetColorU32(ImVec4(0, 0, 0, 255)), line.c_str());
			ImGui::GetOverlayDrawList()->AddText(pFont, size, ImVec2((pos.x) + 1, (pos.y + textSize.y * i) - 1), ImGui::GetColorU32(ImVec4(0, 0, 0, 255)), line.c_str());
			ImGui::GetOverlayDrawList()->AddText(pFont, size, ImVec2((pos.x) - 1, (pos.y + textSize.y * i) + 1), ImGui::GetColorU32(ImVec4(0, 0, 0, 255)), line.c_str());

			ImGui::GetOverlayDrawList()->AddText(pFont, size, ImVec2(pos.x, pos.y + textSize.y * i), ImGui::GetColorU32(color), line.c_str());
		}

		y = pos.y + textSize.y * (i + 1);
		i++;
	}
	return y;
}

void DrawText1(int x, int y, const char* str, RGBA* color)
{
	ImFont a;
	std::string utf_8_1 = std::string(str);
	std::string utf_8_2 = string_To_UTF8(utf_8_1);
	ImGui::GetOverlayDrawList()->AddText(ImVec2(x, y), ImGui::ColorConvertFloat4ToU32(ImVec4(color->R / 255.0, color->G / 255.0, color->B / 255.0, color->A / 255.0)), utf_8_2.c_str());
}

void DrawLine(int x1, int y1, int x2, int y2, RGBA* color, int thickness)
{
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(x1, y1), ImVec2(x2, y2), ImGui::ColorConvertFloat4ToU32(ImVec4(color->R / 255.0, color->G / 255.0, color->B / 255.0, color->A / 255.0)), thickness);
}

void DrawCircle(int x, int y, int radius, RGBA* color, int segments)
{
	ImGui::GetOverlayDrawList()->AddCircle(ImVec2(x, y), radius, ImGui::ColorConvertFloat4ToU32(ImVec4(color->R / 255.0, color->G / 255.0, color->B / 255.0, color->A / 255.0)), segments);
}

void DrawBox(float X, float Y, float W, float H, RGBA* color)
{
	ImGui::GetOverlayDrawList()->AddRect(ImVec2(X + 1, Y + 1), ImVec2(((X + W) - 1), ((Y + H) - 1)), ImGui::ColorConvertFloat4ToU32(ImVec4(color->R / 255.0, color->G / 255.0, color->B / 255.0, color->A / 255.0)));
	ImGui::GetOverlayDrawList()->AddRect(ImVec2(X, Y), ImVec2(X + W, Y + H), ImGui::ColorConvertFloat4ToU32(ImVec4(color->R / 255.0, color->G / 255.0, color->B / 255.0, color->A / 255.0)));
}
void DrawFilledRect(int x, int y, int w, int h, ImU32 color)
{
	ImGui::GetOverlayDrawList()->AddRectFilled(ImVec2(x, y), ImVec2(x + w, y + h), color, 0, 0);
}
void DrawNormalBox(int x, int y, int w, int h, int borderPx, ImU32 color)
{

	DrawFilledRect(x + borderPx, y, w, borderPx, color); //top 
	DrawFilledRect(x + w - w + borderPx, y, w, borderPx, color); //top 
	DrawFilledRect(x, y, borderPx, h, color); //left 
	DrawFilledRect(x, y + h - h + borderPx * 2, borderPx, h, color); //left 
	DrawFilledRect(x + borderPx, y + h + borderPx, w, borderPx, color); //bottom 
	DrawFilledRect(x + w - w + borderPx, y + h + borderPx, w, borderPx, color); //bottom 
	DrawFilledRect(x + w + borderPx, y, borderPx, h, color);//right 
	DrawFilledRect(x + w + borderPx, y + h - h + borderPx * 2, borderPx, h, color);//right 
}
void DrawCornerBox(int X, int Y, int W, int H, const ImU32& color, int thickness) {
	float lineW = (W / 3);
	float lineH = (H / 3);

	ImGui::GetOverlayDrawList()->AddLine(ImVec2(X, Y), ImVec2(X, Y + lineH), ImGui::ColorConvertFloat4ToU32(ImVec4(1 / 255.0, 1 / 255.0, 1 / 255.0, 255 / 255.0)), 3);
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(X, Y), ImVec2(X + lineW, Y), ImGui::ColorConvertFloat4ToU32(ImVec4(1 / 255.0, 1 / 255.0, 1 / 255.0, 255 / 255.0)), 3);
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(X + W - lineW, Y), ImVec2(X + W, Y), ImGui::ColorConvertFloat4ToU32(ImVec4(1 / 255.0, 1 / 255.0, 1 / 255.0, 255 / 255.0)), 3);
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(X + W, Y), ImVec2(X + W, Y + lineH), ImGui::ColorConvertFloat4ToU32(ImVec4(1 / 255.0, 1 / 255.0, 1 / 255.0, 255 / 255.0)), 3);
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(X, Y + H - lineH), ImVec2(X, Y + H), ImGui::ColorConvertFloat4ToU32(ImVec4(1 / 255.0, 1 / 255.0, 1 / 255.0, 255 / 255.0)), 3);
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(X, Y + H), ImVec2(X + lineW, Y + H), ImGui::ColorConvertFloat4ToU32(ImVec4(1 / 255.0, 1 / 255.0, 1 / 255.0, 255 / 255.0)), 3);
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(X + W - lineW, Y + H), ImVec2(X + W, Y + H), ImGui::ColorConvertFloat4ToU32(ImVec4(1 / 255.0, 1 / 255.0, 1 / 255.0, 255 / 255.0)), 3);
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(X + W, Y + H - lineH), ImVec2(X + W, Y + H), ImGui::ColorConvertFloat4ToU32(ImVec4(1 / 255.0, 1 / 255.0, 1 / 255.0, 255 / 255.0)), 3);


	ImGui::GetOverlayDrawList()->AddLine(ImVec2(X, Y), ImVec2(X, Y + lineH), ImGui::GetColorU32(color), thickness);
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(X, Y), ImVec2(X + lineW, Y), ImGui::GetColorU32(color), thickness);
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(X + W - lineW, Y), ImVec2(X + W, Y), ImGui::GetColorU32(color), thickness);
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(X + W, Y), ImVec2(X + W, Y + lineH), ImGui::GetColorU32(color), thickness);
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(X, Y + H - lineH), ImVec2(X, Y + H), ImGui::GetColorU32(color), thickness);
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(X, Y + H), ImVec2(X + lineW, Y + H), ImGui::GetColorU32(color), thickness);
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(X + W - lineW, Y + H), ImVec2(X + W, Y + H), ImGui::GetColorU32(color), thickness);
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(X + W, Y + H - lineH), ImVec2(X + W, Y + H), ImGui::GetColorU32(color), thickness);
}


   
	void DrawSkeleton(DWORD_PTR mesh)
	{
		Vector3 vHeadBone = GetBoneWithRotation(mesh, 68);
		Vector3 vHip = GetBoneWithRotation(mesh, 7);
		Vector3 vNeck = GetBoneWithRotation(mesh, 67);
		Vector3 vUpperArmLeft = GetBoneWithRotation(mesh, 9);
		Vector3 vUpperArmRight = GetBoneWithRotation(mesh, 38);
		Vector3 vLeftHand = GetBoneWithRotation(mesh, 30);
		Vector3 vRightHand = GetBoneWithRotation(mesh, 58);
		Vector3 vLeftHand1 = GetBoneWithRotation(mesh, 11);
		Vector3 vRightHand1 = GetBoneWithRotation(mesh, 40);
		Vector3 vRightThigh = GetBoneWithRotation(mesh, 78);
		Vector3 vLeftThigh = GetBoneWithRotation(mesh, 71);
		Vector3 vRightCalf = GetBoneWithRotation(mesh, 79);
		Vector3 vLeftCalf = GetBoneWithRotation(mesh, 72);
		Vector3 vLeftFoot = GetBoneWithRotation(mesh, 74);
		Vector3 vRightFoot = GetBoneWithRotation(mesh, 81);
		Vector3 vHeadBoneOut = ProjectWorldToScreen(vHeadBone);
		Vector3 vHipOut = ProjectWorldToScreen(vHip);
		Vector3 vNeckOut = ProjectWorldToScreen(vNeck);
		Vector3 vUpperArmLeftOut = ProjectWorldToScreen(vUpperArmLeft);
		Vector3 vUpperArmRightOut = ProjectWorldToScreen(vUpperArmRight);
		Vector3 vLeftHandOut = ProjectWorldToScreen(vLeftHand);
		Vector3 vRightHandOut = ProjectWorldToScreen(vRightHand);
		Vector3 vLeftHandOut1 = ProjectWorldToScreen(vLeftHand1);
		Vector3 vRightHandOut1 = ProjectWorldToScreen(vRightHand1);
		Vector3 vRightThighOut = ProjectWorldToScreen(vRightThigh);
		Vector3 vLeftThighOut = ProjectWorldToScreen(vLeftThigh);
		Vector3 vRightCalfOut = ProjectWorldToScreen(vRightCalf);
		Vector3 vLeftCalfOut = ProjectWorldToScreen(vLeftCalf);
		Vector3 vLeftFootOut = ProjectWorldToScreen(vLeftFoot);
		Vector3 vRightFootOut = ProjectWorldToScreen(vRightFoot);
		ImGui::GetOverlayDrawList()->AddLine(ImVec2(vHipOut.x, vHipOut.y), ImVec2(vNeckOut.x, vNeckOut.y), ImColor(0.92f, 0.10f, 0.14f), 2.0f);
		ImGui::GetOverlayDrawList()->AddLine(ImVec2(vUpperArmLeftOut.x, vUpperArmLeftOut.y), ImVec2(vNeckOut.x, vNeckOut.y), ImColor(0.92f, 0.10f, 0.14f), 2.0f);
		ImGui::GetOverlayDrawList()->AddLine(ImVec2(vUpperArmRightOut.x, vUpperArmRightOut.y), ImVec2(vNeckOut.x, vNeckOut.y), ImColor(0.92f, 0.10f, 0.14f), 2.0f);
		ImGui::GetOverlayDrawList()->AddLine(ImVec2(vLeftHandOut.x, vLeftHandOut.y), ImVec2(vUpperArmLeftOut.x, vUpperArmLeftOut.y), ImColor(0.92f, 0.10f, 0.14f), 2.0f);
		ImGui::GetOverlayDrawList()->AddLine(ImVec2(vRightHandOut.x, vRightHandOut.y), ImVec2(vUpperArmRightOut.x, vUpperArmRightOut.y), ImColor(0.92f, 0.10f, 0.14f), 2.0f);
		ImGui::GetOverlayDrawList()->AddLine(ImVec2(vLeftHandOut.x, vLeftHandOut.y), ImVec2(vLeftHandOut1.x, vLeftHandOut1.y), ImColor(0.92f, 0.10f, 0.14f), 2.0f);
		ImGui::GetOverlayDrawList()->AddLine(ImVec2(vRightHandOut.x, vRightHandOut.y), ImVec2(vRightHandOut1.x, vRightHandOut1.y), ImColor(0.92f, 0.10f, 0.14f), 2.0f);
		ImGui::GetOverlayDrawList()->AddLine(ImVec2(vLeftThighOut.x, vLeftThighOut.y), ImVec2(vHipOut.x, vHipOut.y), ImColor(0.92f, 0.10f, 0.14f), 2.0f);
		ImGui::GetOverlayDrawList()->AddLine(ImVec2(vRightThighOut.x, vRightThighOut.y), ImVec2(vHipOut.x, vHipOut.y), ImColor(0.92f, 0.10f, 0.14f), 2.0f);
		ImGui::GetOverlayDrawList()->AddLine(ImVec2(vLeftCalfOut.x, vLeftCalfOut.y), ImVec2(vLeftThighOut.x, vLeftThighOut.y), ImColor(0.92f, 0.10f, 0.14f), 2.0f);
		ImGui::GetOverlayDrawList()->AddLine(ImVec2(vRightCalfOut.x, vRightCalfOut.y), ImVec2(vRightThighOut.x, vRightThighOut.y), ImColor(0.92f, 0.10f, 0.14f), 2.0f);
		ImGui::GetOverlayDrawList()->AddLine(ImVec2(vLeftFootOut.x, vLeftFootOut.y), ImVec2(vLeftCalfOut.x, vLeftCalfOut.y), ImColor(0.92f, 0.10f, 0.14f), 2.0f);
		ImGui::GetOverlayDrawList()->AddLine(ImVec2(vRightFootOut.x, vRightFootOut.y), ImVec2(vRightCalfOut.x, vRightCalfOut.y), ImColor(0.92f, 0.10f, 0.14f), 2.0f);
	}



typedef struct _FNlEntity
{
	uint64_t Actor;
	int ID;
	uint64_t mesh;
}FNlEntity;

std::vector<FNlEntity> entityList;


struct HandleDisposer
{
	using pointer = HANDLE;
	void operator()(HANDLE handle) const
	{
		if (handle != NULL || handle != INVALID_HANDLE_VALUE)
		{
			CloseHandle(handle);
		}
	}
};
using unique_handle = std::unique_ptr<HANDLE, HandleDisposer>;

static std::uint32_t _GetProcessId(std::string process_name) {
	PROCESSENTRY32 processentry;
	const unique_handle snapshot_handle(CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0));

	if (snapshot_handle.get() == INVALID_HANDLE_VALUE)
		return 0;

	processentry.dwSize = sizeof(MODULEENTRY32);

	while (Process32Next(snapshot_handle.get(), &processentry) == TRUE) {
		if (process_name.compare(processentry.szExeFile) == 0)
			return processentry.th32ProcessID;
	}
	return 0;
}
int main(int argc, const char* argv[])
{



	

	
	
	SetConsoleTitleA(skCrypt("Poez Loader"));
	std::cout << skCrypt("\n\n Connecting..");
	KeyAuthApp.init();
	if (!KeyAuthApp.data.success)
	{
		std::cout << skCrypt("\n Status: ") << KeyAuthApp.data.message;
		Sleep(1500);
		exit(0);
	}

	if (KeyAuthApp.checkblack()) {
		abort();
	}
	

	

	std::cout << skCrypt("\n\n[1] License key only\n\n Choose option: ");

	int option;
	std::string username;
	std::string password;
	std::string key;

	std::cin >> option;
	switch (option)
	{
	case 1:
		std::cout << skCrypt("\n Enter license: ");
		std::cin >> key;
		KeyAuthApp.license(key);
		break;
	default:
		std::cout << skCrypt("\n\n Status: Failure: Invalid Selection");
		Sleep(3000);
		exit(0);
	}

	if (!KeyAuthApp.data.success)
	{
		std::cout << skCrypt("\n Status: ") << KeyAuthApp.data.message;
		void render();
		Sleep(1500);
		exit(0);
		
	}


	MouseController::Init();
	CreateThread(NULL, NULL, Menuthread, NULL, NULL, NULL);
	//AttachDriver();
	printf("waiting for fn");
	Beep(500, 300);
	while (hwnd == NULL)
	{
		XorS(wind, "Fortnite  ");
		hwnd = FindWindowA(0, wind.decrypt());
		Sleep(100);
	}

	processID = _GetProcessId("FortniteClient-Win64-Shipping.exe");
	KmDrv = new Memory(processID);
	base_address = KmDrv->GetModuleBase("FortniteClient-Win64-Shipping.exe");
	xCreateWindow();
	xInitD3d();

	xMainLoop();
	xShutdown();

	return 0;
}

void SetWindowToTarget()
{
	while (true)
	{
		if (hwnd)
		{
			ZeroMemory(&GameRect, sizeof(GameRect));
			GetWindowRect(hwnd, &GameRect);
			Width = GameRect.right - GameRect.left;
			Height = GameRect.bottom - GameRect.top;
			DWORD dwStyle = GetWindowLong(hwnd, GWL_STYLE);

			if (dwStyle & WS_BORDER)
			{
				GameRect.top += 32;
				Height -= 39;
			}
			ScreenCenterX = Width / 2;
			ScreenCenterY = Height / 2;
			MoveWindow(Window, GameRect.left, GameRect.top, Width, Height, true);
		}
		else
		{
			exit(0);
		}
	}
}

const MARGINS Margin = { -1 };

void xCreateWindow()
{
	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)SetWindowToTarget, 0, 0, 0);

	WNDCLASS windowClass = { 0 };
	windowClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	windowClass.hInstance = NULL;
	windowClass.lpfnWndProc = WinProc;
	windowClass.lpszClassName = "SkyDash#4150";
	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	if (!RegisterClass(&windowClass))
		std::cout << "\n\n [-] overlay failed";

	Window = CreateWindow("SkyDash#4150",
		NULL,
		WS_POPUP,
		0,
		0,
		GetSystemMetrics(SM_CXSCREEN),
		GetSystemMetrics(SM_CYSCREEN),
		NULL,
		NULL,
		NULL,
		NULL);

	ShowWindow(Window, SW_SHOW);

	DwmExtendFrameIntoClientArea(Window, &Margin);
	SetWindowLong(Window, GWL_EXSTYLE, WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW | WS_EX_LAYERED);
	UpdateWindow(Window);
}

void xInitD3d()
{
	if (FAILED(Direct3DCreate9Ex(D3D_SDK_VERSION, &p_Object)))
		exit(3);

	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.BackBufferWidth = Width;
	d3dpp.BackBufferHeight = Height;
	d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;
	d3dpp.MultiSampleQuality = D3DMULTISAMPLE_NONE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.hDeviceWindow = Window;
	d3dpp.Windowed = TRUE;

	p_Object->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, Window, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &D3dDevice);

	IMGUI_CHECKVERSION();

	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	(void)io;

	io.IniFilename = NULL;

	ImGui_ImplWin32_Init(Window);
	ImGui_ImplDX9_Init(D3dDevice);

	ImGui::StyleColorsClassic();
	ImGuiStyle* style = &ImGui::GetStyle();

	style->WindowPadding = ImVec2(15, 15);
	style->WindowRounding = 0.0f;
	style->FramePadding = ImVec2(5, 5);
	style->FrameRounding = 0.0f;
	style->ItemSpacing = ImVec2(12, 8);
	style->ItemInnerSpacing = ImVec2(8, 6);
	style->IndentSpacing = 25.0f;
	style->ScrollbarSize = 15.0f;
	style->ScrollbarRounding = 0.0f;
	style->GrabMinSize = 5.0f;
	style->GrabRounding = 0.0f;

	style->Colors[ImGuiCol_Text] = ImColor(0, 0, 0, 255);
	style->Colors[ImGuiCol_TextDisabled] = ImColor(255, 255, 255, 255);
	style->Colors[ImGuiCol_WindowBg] = ImColor(255, 255, 255, 255);
	style->Colors[ImGuiCol_ChildWindowBg] = ImColor(255, 255, 255, 255);
	style->Colors[ImGuiCol_PopupBg] = ImColor(50, 50, 50, 255);
	style->Colors[ImGuiCol_Border] = ImColor(255, 0, 255, 255);
	style->Colors[ImGuiCol_BorderShadow] = ImColor(0, 0, 0, 255);
	style->Colors[ImGuiCol_FrameBg] = ImColor(50, 50, 50, 255);
	style->Colors[ImGuiCol_FrameBgHovered] = ImColor(70, 70, 70, 255);
	style->Colors[ImGuiCol_FrameBgActive] = ImColor(90, 90, 90, 255);
	style->Colors[ImGuiCol_TitleBg] = ImColor(30, 30, 30, 255);
	style->Colors[ImGuiCol_TitleBgCollapsed] = ImColor(30, 30, 30, 255);
	style->Colors[ImGuiCol_TitleBgActive] = ImColor(30, 30, 30, 255);
	style->Colors[ImGuiCol_MenuBarBg] = ImColor(255, 255, 255, 255);
	style->Colors[ImGuiCol_ScrollbarBg] = ImColor(100, 100, 100, 255);
	style->Colors[ImGuiCol_ScrollbarGrab] = ImColor(60, 60, 60, 255);
	style->Colors[ImGuiCol_ScrollbarGrabHovered] = ImColor(70, 70, 70, 255);
	style->Colors[ImGuiCol_ScrollbarGrabActive] = ImColor(80, 80, 80, 255);
	style->Colors[ImGuiCol_CheckMark] = ImColor(255, 0, 255, 255);
	style->Colors[ImGuiCol_SliderGrab] = ImColor(255, 0, 255, 255);
	style->Colors[ImGuiCol_SliderGrabActive] = ImColor(255, 255, 255, 255);
	style->Colors[ImGuiCol_Button] = ImColor(50, 50, 50, 0);
	style->Colors[ImGuiCol_ButtonHovered] = ImColor(70, 70, 70, 255);
	style->Colors[ImGuiCol_ButtonActive] = ImColor(90, 90, 90, 255);
	style->Colors[ImGuiCol_Header] = ImColor(50, 50, 50, 255);
	style->Colors[ImGuiCol_HeaderHovered] = ImColor(50, 50, 50, 255);
	style->Colors[ImGuiCol_HeaderActive] = ImColor(50, 50, 50, 255);
	style->Colors[ImGuiCol_Column] = ImColor(255, 255, 255, 255);
	style->Colors[ImGuiCol_ColumnHovered] = ImColor(255, 255, 255, 255);
	style->Colors[ImGuiCol_ColumnActive] = ImColor(255, 255, 255, 255);
	style->Colors[ImGuiCol_ResizeGrip] = ImColor(255, 255, 255, 255);
	style->Colors[ImGuiCol_ResizeGripHovered] = ImColor(255, 255, 255, 255);
	style->Colors[ImGuiCol_ResizeGripActive] = ImColor(255, 255, 255, 255);
	style->Colors[ImGuiCol_PlotLines] = ImColor(255, 255, 255, 255);
	style->Colors[ImGuiCol_PlotLinesHovered] = ImColor(255, 255, 255, 255);
	style->Colors[ImGuiCol_PlotHistogram] = ImColor(255, 255, 255, 255);
	style->Colors[ImGuiCol_PlotHistogramHovered] = ImColor(255, 255, 255, 255);
	style->Colors[ImGuiCol_TextSelectedBg] = ImColor(255, 255, 255, 255);
	style->Colors[ImGuiCol_ModalWindowDarkening] = ImColor(255, 255, 255, 255);

	style->WindowTitleAlign.x = 0.5f;
	style->FrameRounding = 0.0f;

	XorS(font, "C:\\Windows\\Fonts\\Arial.ttf");
	m_pFont = io.Fonts->AddFontFromFileTTF(font.decrypt(), 14.0f, nullptr, io.Fonts->GetGlyphRangesDefault());

}

void aimbot(float x, float y)
{
	float ScreenCenterX = (Width / 2);
	float ScreenCenterY = (Height / 2);
	int AimSpeed = smooth;
	float TargetX = 0;
	float TargetY = 0;

	if (x != 0)
	{
		if (x > ScreenCenterX)
		{
			TargetX = -(ScreenCenterX - x);
			TargetX /= AimSpeed;
			if (TargetX + ScreenCenterX > ScreenCenterX * 2) TargetX = 0;
		}

		if (x < ScreenCenterX)
		{
			TargetX = x - ScreenCenterX;
			TargetX /= AimSpeed;
			if (TargetX + ScreenCenterX < 0) TargetX = 0;
		}
	}

	if (y != 0)
	{
		if (y > ScreenCenterY)
		{
			TargetY = -(ScreenCenterY - y);
			TargetY /= AimSpeed;
			if (TargetY + ScreenCenterY > ScreenCenterY * 2) TargetY = 0;
		}

		if (y < ScreenCenterY)
		{
			TargetY = y - ScreenCenterY;
			TargetY /= AimSpeed;
			if (TargetY + ScreenCenterY < 0) TargetY = 0;
		}
	}

	mouse_event(MOUSEEVENTF_MOVE, static_cast<DWORD>(TargetX), static_cast<DWORD>(TargetY), NULL, NULL);
	return;
}
void AimAt(DWORD_PTR entity)
{
	uint64_t currentactormesh = KmDrv->Rpm<uint64_t>(entity + 0x310);
	auto rootHead = GetBoneWithRotation(currentactormesh, hitbox);
	Vector3 rootHeadOut = ProjectWorldToScreen(rootHead);

	if (rootHeadOut.y != 0 || rootHeadOut.y != 0)
	{
		aimbot(rootHeadOut.x, rootHeadOut.y);
	}
}
bool isVisible(uint64_t mesh)
{
	float bing = KmDrv->Rpm<float>(mesh + 0x330);
	float bong = KmDrv->Rpm<float>(mesh + 0x338);
	const float tick = 0.06f;
	return bong + tick >= bing;
}
static auto DrawCircleFilled(int x, int y, int radius, RGBA* color) -> void
{
	ImGui::GetOverlayDrawList()->AddCircleFilled(ImVec2(x, y), radius, ImGui::ColorConvertFloat4ToU32(ImVec4(color->R / 255.0, color->G / 255.0, color->B / 255.0, color->A / 255.0)));
}
namespace cumera
{
	Vector3 Location;
};
double GetCrossDistance(double x1, double y1, double x2, double y2) {
	return sqrt(pow((x2 - x1), 2) + pow((y2 - y1), 2));
}
void OutlinedText(int x, int y, ImColor Color, const char* text)
{
	ImGui::GetOverlayDrawList()->AddText(ImVec2(x - 1, y), ImColor(0, 0, 0), text);
	ImGui::GetOverlayDrawList()->AddText(ImVec2(x + 1, y), ImColor(0, 0, 0), text);
	ImGui::GetOverlayDrawList()->AddText(ImVec2(x, y - 1), ImColor(0, 0, 0), text);
	ImGui::GetOverlayDrawList()->AddText(ImVec2(x, y + 1), ImColor(0, 0, 0), text);
	ImGui::GetOverlayDrawList()->AddText(ImVec2(x, y), Color, text);
}

void DrawESP() {
	if (fovcircle) {
		ImGui::GetOverlayDrawList()->AddCircle(ImVec2(ScreenCenterX, ScreenCenterY), float(AimFOV), ImColor(255, 0, 255), 100.0f, 1.5f);

	}
	if (hitboxpos == 0)
	{
		hitbox = 68; //head
	}
	else if (hitboxpos == 1)
	{
		hitbox = 67; //neck
	}
	else if (hitboxpos == 2)
	{
		hitbox = 7; //chest
	}
	else if (hitboxpos == 3)
	{
		hitbox = 2; //pelvis
	}

	if (aimkeypos == 0)
	{
		aimkey = 0x01;//left mouse button
	}
	else if (aimkeypos == 1)
	{
		aimkey = 0x02;//right mouse button
	}
	else if (aimkeypos == 2)
	{
		aimkey = 0x04;//middle mouse button
	}
	else if (aimkeypos == 3)
	{
		aimkey = 0x05;//x1 mouse button
	}
	else if (aimkeypos == 4)
	{
		aimkey = 0x06;//x2 mouse button
	}
	else if (aimkeypos == 5)
	{
		aimkey = 0x03;//control break processing
	}
	else if (aimkeypos == 6)
	{
		aimkey = 0x08;//backspace
	}
	else if (aimkeypos == 7)
	{
		aimkey = 0x09;//tab
	}
	else if (aimkeypos == 8)
	{
		aimkey = 0x0c;//clear
	}
	else if (aimkeypos == 9)
	{
		aimkey == 0x0D;//enter
	}
	else if (aimkeypos == 10)
	{
		aimkey = 0x10;//shift
	}
	else if (aimkeypos == 11)
	{
		aimkey = 0x11;//ctrl
	}
	else if (aimkeypos == 12)
	{
		aimkey == 0x12;//alt
	}
	else if (aimkeypos == 13)
	{
		aimkey == 0x14;//caps lock
	}
	else if (aimkeypos == 14)
	{
		aimkey == 0x1B;//esc
	}
	else if (aimkeypos == 15)
	{
		aimkey == 0x20;//space
	}
	else if (aimkeypos == 16)
	{
		aimkey == 0x30;//0
	}
	else if (aimkeypos == 17)
	{
		aimkey == 0x31;//1
	}
	else if (aimkeypos == 18)
	{
		aimkey == 0x32;//2
	}
	else if (aimkeypos == 19)
	{
		aimkey == 0x33;//3
	}
	else if (aimkeypos == 20)
	{
		aimkey == 0x34;//4
	}
	else if (aimkeypos == 21)
	{
		aimkey == 0x35;//5
	}
	else if (aimkeypos == 22)
	{
		aimkey == 0x36;//6
	}
	else if (aimkeypos == 23)
	{
		aimkey == 0x37;//7
	}
	else if (aimkeypos == 24)
	{
		aimkey == 0x38;//8
	}
	else if (aimkeypos == 25)
	{

		aimkey == 0x39;//9
	}
	else if (aimkeypos == 26)
	{
		aimkey == 0x41;//a
	}
	else if (aimkeypos == 27)
	{
		aimkey == 0x42;//b
	}
	else if (aimkeypos == 28)
	{
		aimkey == 0x43;//c
	}
	else if (aimkeypos == 29)
	{
		aimkey == 0x44;//d
	}
	else if (aimkeypos == 30)
	{
		aimkey == 0x45;//e
	}
	else if (aimkeypos == 31)
	{
		aimkey == 0x46;//f
	}
	else if (aimkeypos == 32)
	{
		aimkey == 0x47;//g
	}
	else if (aimkeypos == 33)
	{
		aimkey == 0x48;//h
	}
	else if (aimkeypos == 34)
	{
		aimkey == 0x49;//i
	}
	else if (aimkeypos == 35)
	{
		aimkey == 0x4A;//j
	}
	else if (aimkeypos == 36)
	{
		aimkey == 0x4B;//k

	}
	else if (aimkeypos == 37)
	{
		aimkey == 0x4C;//L
	}
	else if (aimkeypos == 38)
	{
		aimkey == 0x4D;//m
	}
	else if (aimkeypos == 39)
	{
		aimkey == 0x4E;//n
	}
	else if (aimkeypos == 40)
	{
		aimkey == 0x4F;//o
	}
	else if (aimkeypos == 41)
	{
		aimkey == 0x50;//p
	}
	else if (aimkeypos == 42)
	{
		aimkey == 0x51;//q
	}
	else if (aimkeypos == 43)
	{
		aimkey == 0x52;//r
	}
	else if (aimkeypos == 44)
	{
		aimkey == 0x53;//s
	}
	else if (aimkeypos == 45)
	{
		aimkey == 0x54;//t
	}
	else if (aimkeypos == 46)
	{
		aimkey == 0x55;//u
	}
	else if (aimkeypos == 47)
	{
		aimkey == 0x56;//v
	}
	else if (aimkeypos == 48)
	{
		aimkey == 0x57;//w
	}
	else if (aimkeypos == 49)
	{
		aimkey == 0x58;//x
	}
	else if (aimkeypos == 50)
	{
		aimkey == 0x59;//y
	}
	else if (aimkeypos == 51)
	{
		aimkey == 0x5A;//z
	}
	else if (aimkeypos == 52)
	{
		aimkey == 0x60;//numpad 0
	}
	else if (aimkeypos == 53)
	{
		aimkey == 0x61;//numpad 1
	}
	else if (aimkeypos == 54)
	{
		aimkey == 0x62;//numpad 2

	}
	else if (aimkeypos == 55)
	{
		aimkey == 0x63;//numpad 3
	}
	else if (aimkeypos == 56)
	{
		aimkey == 0x64;//numpad 4
	}
	else if (aimkeypos == 57)
	{
		aimkey == 0x65;//numpad 5
	}
	else if (aimkeypos == 58)
	{
		aimkey == 0x66;//numpad 6
	}
	else if (aimkeypos == 59)
	{
		aimkey == 0x67;//numpad 7
	}
	else if (aimkeypos == 60)
	{
		aimkey == 0x68;//numpad 8
	}
	else if (aimkeypos == 61)
	{
		aimkey == 0x69;//numpad 9
	}
	else if (aimkeypos == 62)
	{
		aimkey == 0x6A;//multiply
	}
	auto entityListCopy = entityList;
	float closestDistance = FLT_MAX;
	DWORD_PTR closestPawn = NULL;
	Uworld = KmDrv->Rpm<DWORD_PTR>(base_address + 0xDF0FE00);
	DWORD_PTR Gameinstance = KmDrv->Rpm<DWORD_PTR>(Uworld + 0x1A8);
	DWORD_PTR LocalPlayers = KmDrv->Rpm<DWORD_PTR>(Gameinstance + 0x38);
	Localplayer = KmDrv->Rpm<DWORD_PTR>(LocalPlayers);
	PlayerController = KmDrv->Rpm<DWORD_PTR>(Localplayer + 0x30);
	LocalPawn = KmDrv->Rpm<DWORD_PTR>(PlayerController + 0x328);
	PlayerCameraManager = KmDrv->Rpm<uintptr_t >(PlayerController + 0x328);
	PlayerState = KmDrv->Rpm<DWORD_PTR>(LocalPawn + 0x2a8);
	Rootcomp = KmDrv->Rpm<DWORD_PTR>(LocalPawn + 0x190);
	Persistentlevel = KmDrv->Rpm<DWORD_PTR>(Uworld + 0x30);
	DWORD ActorCount = KmDrv->Rpm<DWORD>(Persistentlevel + 0xA0);
	DWORD_PTR AActors = KmDrv->Rpm<DWORD_PTR>(Persistentlevel + 0x98);

	for (int i = 0; i < ActorCount; i++)
	{
		uint64_t CurrentActor = KmDrv->Rpm<uint64_t>(AActors + i * 0x8);

		if (KmDrv->Rpm<float>(CurrentActor + 0x4170) != 10) continue;
		uint64_t CurrentActorMesh = KmDrv->Rpm<uint64_t>(CurrentActor + 0x310);
		int MyTeamId = KmDrv->Rpm<int>(PlayerState + 0x1098);
		DWORD64 otherPlayerState = KmDrv->Rpm<uint64_t>(CurrentActor + 0x2a8);
		int ActorTeamId = KmDrv->Rpm<int>(otherPlayerState + 0x1080);
		auto isDBNO = (KmDrv->Rpm<char>(CurrentActor + 0x782) >> 4) & 1;
		if (MyTeamId == ActorTeamId) continue;
		if (CurrentActor == LocalPawn) continue;
		Vector3 Headpos = GetBoneWithRotation(CurrentActorMesh, 68);
		Vector3 footpos = GetBoneWithRotation(CurrentActorMesh, 83);
		localactorpos = KmDrv->Rpm<Vector3>(Rootcomp + 0x128);
		auto normal_head = ProjectWorldToScreen(Headpos);
		float distance = localactorpos.Distance(Headpos) / 80.f;

		CurrentWeapon = KmDrv->Rpm<uintptr_t>(LocalPawn + 0x828);

		Vector3 bone0 = GetBoneWithRotation(CurrentActorMesh, 0);
		Vector3 bottom = ProjectWorldToScreen(bone0);
		Vector3 Headbox = ProjectWorldToScreen(Vector3(Headpos.x, Headpos.y, Headpos.z + 15));
		Vector3 w2shead = ProjectWorldToScreen(Headpos);
		auto player_screen = ProjectWorldToScreen(bone0);
		float BoxHeight = (float)(Headbox.y - bottom.y);
		float BoxWidth = BoxHeight * 0.380f;

		float LeftX = (float)Headbox.x - (BoxWidth / 1);
		float LeftY = (float)bottom.y;
		//Paste made by SkyDash#4150
		float CornerHeight = abs(Headbox.y - bottom.y);
		float CornerWidth = CornerHeight * 0.75;

		if (aiminair)
		{
			KmDrv->Wpm(LocalPawn + 0x4a05, true);
		}
		int while_rotating;
		if (Spinbot)
		{

			if (GetAsyncKeyState(VK_RBUTTON))
			{
				while_rotating = true;

				if (LocalPawn)
				{
					uint64_t MeshA = KmDrv->Rpm<uint64_t>(LocalPawn + 0x310);
					KmDrv->Wpm<Vector3>(MeshA + 0x140, Vector3(1, 5, 1));
				}
			}
			else if (!GetAsyncKeyState(VK_RBUTTON))
			{
				if (LocalPawn)
				{
					if (while_rotating)
					{
						uint64_t MeshA = KmDrv->Rpm<uint64_t>(LocalPawn + 0x310);
						KmDrv->Wpm<Vector3>(MeshA + 0x140, Vector3(0, -90, 0));
						while_rotating = false;
					}
				}
			}
		}

		if (distance < VisDist)
		{
			if (Esp)
			{
				if (Esp_box)
				{
					if (isVisible(CurrentActorMesh)) {
						ImGui::GetOverlayDrawList()->AddRect(ImVec2(Headbox.x - (CornerWidth / 2), Headbox.y), ImVec2(bottom.x + (CornerWidth / 2), bottom.y), ImColor(255, 0, 255)), ImGui::GetOverlayDrawList()->AddRectFilled(ImVec2(Headbox.x - (CornerWidth / 2), Headbox.y), ImVec2(bottom.x + (CornerWidth / 2), bottom.y), ImColor(0, 0, 0, 175), 0.75f);
						//	DrawCorneredBox(Headbox.x - (CornerWidth / 2), Headbox.y, CornerWidth, CornerHeight, IM_COL32(255, 0, 0, 255), 1.5);

					}
					else if (!isVisible(CurrentActorMesh)) {
						//DrawCorneredBox(Headbox.x - (CornerWidth / 2), Headbox.y, CornerWidth, CornerHeight, IM_COL32(255, 255, 255, 255), 1.5);
						ImGui::GetOverlayDrawList()->AddRect(ImVec2(Headbox.x - (CornerWidth / 2), Headbox.y), ImVec2(bottom.x + (CornerWidth / 2), bottom.y), ImColor(255, 0, 255)), ImGui::GetOverlayDrawList()->AddRectFilled(ImVec2(Headbox.x - (CornerWidth / 2), Headbox.y), ImVec2(bottom.x + (CornerWidth / 2), bottom.y), ImColor(0, 0, 0, 175), 0.75f);
					}

				}

				if (distanceesp)
				{

					if (isVisible(CurrentActorMesh)) {
						XorS(dst, "[%.fm]\n");
						char dist[64];
						sprintf_s(dist, dst.decrypt(), distance);
						DrawOutlinedText(m_pFont, dist, ImVec2(Headbox.x, Headbox.y - 35), 17.0f, ImColor(0, 255, 0), true);
					}
					else if (!isVisible(CurrentActorMesh)) {
						XorS(dst, "[%.fm]\n");
						char dist[64];
						sprintf_s(dist, dst.decrypt(), distance);
						DrawOutlinedText(m_pFont, dist, ImVec2(Headbox.x, Headbox.y - 35), 17.0f, ImColor(0, 255, 0), true);
					}
				}

				
				{





					if (cornerbox)
					{
						if (isVisible(CurrentActorMesh)) {
							DrawCornerBox(Headbox.x - (CornerWidth / 2), Headbox.y, CornerWidth, CornerHeight, ImColor(255, 0, 255), 1.5);
						}
						else if (!isVisible(CurrentActorMesh)) {
							DrawCornerBox(Headbox.x - (CornerWidth / 2), Headbox.y, CornerWidth, CornerHeight, ImColor(255, 0, 255), 1.5);
						}
					}
				}


				if (Esp_line)
				{
					ImGui::GetOverlayDrawList()->AddLine(ImVec2(Width / 2, Height - Height), ImVec2(bottom.x, bottom.y), ImColor(255, 0, 255), 0.5);
				}

				if (Filledfov)
				{
					//ImGui::GetOverlayDrawList()->AddCircleFilled(ImVec2(ScreenCenterX, ScreenCenterY), float(AimFOV), ImColor(24, 24, 24, 200), 100.0f);
				}
			}
		}

		auto dx = w2shead.x - (Width / 2);
		auto dy = w2shead.y - (Height / 2);
		auto dist = sqrtf(dx * dx + dy * dy);


		if (isVisible(CurrentActorMesh)) {

			if (dist < AimFOV && dist < closestDistance) {
				closestDistance = dist;
				closestPawn = CurrentActor;

			}
		}
	}

	if (Aimbot)
	{
		if (Aimbot && closestPawn && GetAsyncKeyState(hotkeys::aimkey) < 0) {
			AimAt(closestPawn);
		}
	}

	Sleep(0.9);
}



//Pasta by nigger
void render() {
	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	static int maintabs = 1;


	ImGui::GetOverlayDrawList()->AddLine(ImVec2(Width / 2.01f - 14, Height / 2), ImVec2(Width / 2.01f + 1, Height / 2), ImColor(255, 0, 0, 255), 0.5f);
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(Width / 1.99f + 14, Height / 2), ImVec2(Width / 1.99f + 1, Height / 2), ImColor(255, 0, 0, 255), 0.5f);
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(Width / 2, Height / 2.01f - 14), ImVec2(Width / 2, Height / 2.01f), ImColor(255, 0, 0, 255), 0.5f);
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(Width / 2, Height / 1.99f + 14), ImVec2(Width / 2, Height / 1.99f), ImColor(255, 0, 0, 255), 0.5f);
	//char name[64];

	if (ShowMenu)
	{

	
		
			ImGui::SetNextWindowSize(ImVec2(400.000f, 700.000f), ImGuiCond_Once);
			ImGui::Begin("Peoz Menu", NULL, 0);
			ImGui::Text("Niggas Paste.");
			ImGui::Spacing();
			ImGui::Checkbox("Aimbot", &Aimbot);
			ImGui::Spacing();
			ImGui::SliderInt("fov", &AimFOV, 50, 1920);
			ImGui::Spacing();
			ImGui::SliderInt("Smoothness", &smooth, 3, 10);
			ImGui::Spacing();
			ImGui::Checkbox("Box Esp", &Esp_box);
			ImGui::Spacing();
			ImGui::Checkbox("corner esp", &cornerbox);
			ImGui::Spacing();
			ImGui::Checkbox("distance ESp", &distanceesp);
			ImGui::Spacing();
			ImGui::Text("By SkyDash#4150");

			ImGui::End();
		}



	


	DrawESP();

	ImGui::EndFrame();
	D3dDevice->SetRenderState(D3DRS_ZENABLE, false);
	D3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
	D3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, false);
	D3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);

	if (D3dDevice->BeginScene() >= 0)
	{
		ImGui::Render();
		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
		D3dDevice->EndScene();
	}
	HRESULT result = D3dDevice->Present(NULL, NULL, NULL, NULL);

	if (result == D3DERR_DEVICELOST && D3dDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
	{
		ImGui_ImplDX9_InvalidateDeviceObjects();
		D3dDevice->Reset(&d3dpp);
		ImGui_ImplDX9_CreateDeviceObjects();
	}
}

MSG Message = { NULL };
int Loop = 0;
void xMainLoop()
{
	static RECT old_rc;
	ZeroMemory(&Message, sizeof(MSG));

	while (Message.message != WM_QUIT)
	{
		if (PeekMessage(&Message, Window, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&Message);
			DispatchMessage(&Message);
		}

		HWND hwnd_active = GetForegroundWindow();

		if (hwnd_active == hwnd) {
			HWND hwndtest = GetWindow(hwnd_active, GW_HWNDPREV);
			SetWindowPos(Window, hwndtest, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		}

		if (GetAsyncKeyState(0x23) & 1)
			exit(8);

		RECT rc;
		POINT xy;

		ZeroMemory(&rc, sizeof(RECT));
		ZeroMemory(&xy, sizeof(POINT));
		GetClientRect(hwnd, &rc);
		ClientToScreen(hwnd, &xy);
		rc.left = xy.x;
		rc.top = xy.y;

		ImGuiIO& io = ImGui::GetIO();
		io.IniFilename = NULL;
		io.ImeWindowHandle = hwnd;
		io.DeltaTime = 1.0f / 60.0f;

		POINT p;
		GetCursorPos(&p);
		io.MousePos.x = p.x - xy.x;
		io.MousePos.y = p.y - xy.y;

		if (GetAsyncKeyState(VK_LBUTTON)) {
			io.MouseDown[0] = true;
			io.MouseClicked[0] = true;
			io.MouseClickedPos[0].x = io.MousePos.x;
			io.MouseClickedPos[0].x = io.MousePos.y;
		}
		else
			io.MouseDown[0] = false;

		if (rc.left != old_rc.left || rc.right != old_rc.right || rc.top != old_rc.top || rc.bottom != old_rc.bottom)
		{
			old_rc = rc;

			Width = rc.right;
			Height = rc.bottom;

			d3dpp.BackBufferWidth = Width;
			d3dpp.BackBufferHeight = Height;
			SetWindowPos(Window, (HWND)0, xy.x, xy.y, Width, Height, SWP_NOREDRAW);
			D3dDevice->Reset(&d3dpp);
		}
		render();
		if (Loop == 0) {
			if (Uworld) {
				printf("\n");
			}
			else if (!Uworld)
			{
				printf("\n");
			}
			if (PlayerController) {
				printf("\n");
			}
			else if (!PlayerController)
			{
				printf("\n");
			}
			if (Rootcomp) {
				printf("\n");
			}
			else if (!Rootcomp)
			{
				printf("\n");
			}
			if (PlayerState) {
				printf("\n");
			}
			else if (!PlayerState)
			{
				printf("\n\n");
			}
		}
		Loop = Loop + 1;
	}
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	DestroyWindow(Window);
}

LRESULT CALLBACK WinProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, Message, wParam, lParam))
		return true;

	switch (Message)
	{
	case WM_DESTROY:
		xShutdown();
		PostQuitMessage(0);
		exit(4);
		break;
	case WM_SIZE:
		if (D3dDevice != NULL && wParam != SIZE_MINIMIZED)
		{
			ImGui_ImplDX9_InvalidateDeviceObjects();
			d3dpp.BackBufferWidth = LOWORD(lParam);
			d3dpp.BackBufferHeight = HIWORD(lParam);
			HRESULT hr = D3dDevice->Reset(&d3dpp);
			if (hr == D3DERR_INVALIDCALL)
				IM_ASSERT(0);
			ImGui_ImplDX9_CreateDeviceObjects();
		}
		break;
	default:
		return DefWindowProc(hWnd, Message, wParam, lParam);
		break;
	}
	return 0;
}

void xShutdown()
{
	TriBuf->Release();
	D3dDevice->Release();
	p_Object->Release();

	DestroyWindow(Window);
	UnregisterClass("fgers", NULL);
}
