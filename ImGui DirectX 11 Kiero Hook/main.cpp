#define STB_IMAGE_IMPLEMENTATION
#define _CRT_SECURE_NO_WARNINGS

#include "includes.h"
#include <iostream>
#include <Windows.h>
#include "proc.h"
#include <minhook.h>
#include <string>
#include <vector>
#include <fstream>
#include "stb_image.h"
#include <Lmcons.h>
#include <winbase.h>
#include <ctime> 
#include <chrono>
#include "json/json/json.h"
#include <unordered_set>
#include "Replc/SDK.hpp"



HMODULE Safe = 0x0;
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

Present oPresent;
HWND window = NULL;
WNDPROC oWndProc;
ID3D11Device* pDevice = NULL;
ID3D11DeviceContext* pContext = NULL;
ID3D11RenderTargetView* mainRenderTargetView;

void InitImGui()
{
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;
	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX11_Init(pDevice, pContext);
}

bool init = false;


bool Active = true;

class initWindow {
public:
    const char* window_title = "Backwooms shit";
    ImVec2 window_size{ 550, 750 };
    ImVec2 WindowPos{ 0,0 };
    DWORD window_flags = ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoBackground;
} iw, iw2, iw4, Slendy1,Slendy2;


typedef struct
{
	DWORD R;
	DWORD G;
	DWORD B;
	DWORD A;

}RGBA;


struct ColorFr {
    RGBA Color = { 0,0,0,255 };
    int State = 0;
};


DWORD Delay = 0;

ULONGLONG CountActive = 0;
ULONGLONG CountBefore = 0;


//Old Version

/*



int Rainbowify(ColorFr* Col) {


    CountActive = GetTickCount64();

    if (CountActive >= CountBefore) {
        CountBefore = GetTickCount64() + Delay;
    }
    else
    {
        return 1;
    }


    if (Col->Color.B != 255 && Col->Color.G == 0) {
        Col->Color.B++;
    }

    if (Col->Color.B == 255 && Col->Color.R != 0) {
        Col->Color.R--;

    }


    if (Col->Color.B == 255 && Col->Color.G != 255 && Col->Color.R == 0) {
        Col->Color.G++;

    }


    if (Col->Color.G == 255 && Col->Color.B != 0) {
        Col->Color.B--;

    }

    if (Col->Color.B == 0 && Col->Color.R != 255) {
        Col->Color.R++;

    }

    if (Col->Color.R == 255 && Col->Color.G != 0) {
        Col->Color.G--;

    }
}

*/




int Rainbowify(RGBA* Color) {


    CountActive = GetTickCount64();

    if (CountActive >= CountBefore) {
        CountBefore = GetTickCount64() + Delay;
    }
    else
    {
        return 1;
    }


    if (Color->B != 255 && Color->G == 0) {
        Color->B++;      
    }

    if (Color->B == 255 && Color->R != 0) {
        Color->R--;
              
    }


    if (Color->B == 255 && Color->G != 255 && Color->R == 0) {
        Color->G++;
               
    }


    if (Color->G == 255 && Color->B != 0) {
        Color->B--;
               
    }

    if (Color->B == 0 && Color->R != 255) {
        Color->R++;
                
    }

    if (Color->R == 255 && Color->G != 0) {
        Color->G--;
                
    }
}





typedef struct
{
	float R;
	float G;
	float B;
	float A;
}color;

color Rainbow;



int ar;
bool iniRainBow = false;
float AnimationSpeedColorChanger = 0.5f;
color Colors;

color RainbowColorChanger(){
	if (iniRainBow == false) {
		Colors.R = 1.0f;
		ar = 1;
		iniRainBow = true;
	}
	switch (ar)
	{
	case 0:
		if (Colors.G <= 0.0f) {
			ar = 1;
		}
		else
		{
			Colors.G = Colors.G - AnimationSpeedColorChanger;
		}
		break;
	case 1:
		if (Colors.B >= 1.0f) {
			ar = 2;
		}
		else
		{
			Colors.B = Colors.B + AnimationSpeedColorChanger;
		}
		break;

	case 2:
		if (Colors.R <= 0.0f) {
			ar = 3;
		}
		else
		{
			Colors.R = Colors.R - AnimationSpeedColorChanger;
		}
		break;

	case 3:
		if (Colors.G >= 1.0f) {
			ar = 4;
		}
		else
		{
			Colors.G = Colors.G + AnimationSpeedColorChanger;
		}
		break;

	case 4:
		if (Colors.B <= 0.0f) {
			ar = 5;
		}
		else
		{
			Colors.B = Colors.B - AnimationSpeedColorChanger;
		}
		break;

	case 5:
		if (Colors.R >= 1.0f) {
			ar = 0;
		}
		else
		{
			Colors.R = Colors.R + AnimationSpeedColorChanger;
		}
		break;
	}
	return Colors;
}



std::string UserNamex = "";
std::string UserName() {
    if (UserNamex != "") {
        return UserNamex;
    }
	char username[UNLEN + 1];
	DWORD username_len = UNLEN + 1;
	GetUserName((LPWSTR)username, &username_len);
	std::string NameReal;
	char tmp2;
	uintptr_t startOfNameStringaddr = (uintptr_t)&username;
	for (size_t i = 0; i < username_len - 1; i++)
	{
		if (i == 0) {
			NameReal = *(char*)(startOfNameStringaddr);
			continue;
		}
		NameReal = NameReal + *(char*)(startOfNameStringaddr + 0x2 * i);
		continue;
	}
    UserNamex = NameReal;
	return NameReal;
}


uintptr_t testHandle = 0x0;

uintptr_t findShitByName(std::string UserName, const std::string& filename, const char* NameToFind, int SkipAble = 0, std::string TypeSignature = "", bool FindClassOnly = false, const char* BaseClassNot = "") {
	Json::Value root; // define a Json::Value object
	std::string Patht = "";
	Patht = "C:\\Users\\" + UserName + "\\Desktop\\Data\\Offsets n shit\\" + filename;
	std::ifstream file(Patht); // open the JSON file
	if (!file.is_open()) {
		throw std::runtime_error("Failed to open file: " + Patht);
	}
	try {
		file >> root; // read and parse the JSON data
	}
	catch (const Json::RuntimeError& e) {
		throw std::runtime_error("Failed to parse JSON data: " + std::string(e.what()));
	}
	int SkipAblesd = 0;
	for (Json::Value::ArrayIndex i = 0; i < root["ScriptMethod"].size(); ++i) {
		// Access the fields of the current object in the array
		uintptr_t address = root["ScriptMethod"][i]["Address"].asInt();
		std::string name = root["ScriptMethod"][i]["Name"].asString();
		std::string signature = root["ScriptMethod"][i]["Signature"].asString();
		std::string typeSignature = root["ScriptMethod"][i]["TypeSignature"].asString();

        if (FindClassOnly == true) {

            if (name.find(NameToFind) != std::string::npos ) {
                if (name.find(BaseClassNot) == std::string::npos) {
                    if (TypeSignature != typeSignature)continue;
                    return address + testHandle;
                }
                continue;
            }
            continue;
        }

        if (NameToFind[3] != name[3])continue;

		if (NameToFind == name) {
            if (TypeSignature != "") {
                if (TypeSignature != typeSignature)continue;
            }
			if (SkipAblesd >= SkipAble) {

				return address + testHandle;
			}
			else
			{
				SkipAblesd = SkipAblesd + 1;
			}
			
		}
	}

	std::cout << "Couldnt Find Information about: " << NameToFind << ". Well the cheat will be kinda broken but it will be prob ok" << "\n";
	std::cout << "Alternative you could try add the Name yourself if you can reverse it \n";

	return 0x0; // return a null value if the "foo" object was not found
}

ColorFr ColorRainbow;


RGBA MrWhite = { 255,255,255,255 };
RGBA Coolor = { 53, 0, 255, 255 };
RGBA RedColor = { 255,0,0,255 };
RGBA CoolColor = { 53,53,255,10 };
RGBA CoolColor1 = { 0,0,0,255 };
RGBA CoolColor2 = { 82,255,93,255 };
RGBA MrBlack = { 33,255,255,255 };
RGBA GreeenishColor = { 0, 255, 107, 255 };



ColorFr Colorcooler;



namespace imGuiDrawShit {
	////////////////////////////
	//Data////////////////////////


	class PackedGif
	{
	public:
		ID3D11ShaderResourceView* a1;

	private:

	}; PackedGif packedGif[36];


	const char* OldPath;
	unsigned char* image_data;
	ID3D11ShaderResourceView* my_texture;
	int my_image_width = 0;
	int my_image_height = 0;
	int Frames;
	bool IniGifs = false;
	bool ShowAnimation = false;

	static void DrawCircle(int x, int y, int radius, RGBA* color, int segments)
	{
		ImGui::GetOverlayDrawList()->AddCircle(ImVec2(x, y), radius, ImGui::ColorConvertFloat4ToU32(ImVec4(color->R / 255.0, color->G / 255.0, color->B / 255.0, color->A / 255.0)), segments);
	}

	static void DrawRect(int x, int y, int w, int h, RGBA* color, int thickness)
	{
		ImGui::GetOverlayDrawList()->AddRect(ImVec2(x, y), ImVec2(x + w, y + h), ImGui::ColorConvertFloat4ToU32(ImVec4(color->R / 255.0, color->G / 255.0, color->B / 255.0, color->A / 255.0)), 0, 0, thickness);
	}

	static void DrawFilledRect(int x, int y, int w, int h, RGBA* color)
	{
		ImGui::GetOverlayDrawList()->AddRectFilled(ImVec2(x, y - 1), ImVec2(x + w, y + h), ImGui::ColorConvertFloat4ToU32(ImVec4(1 / 255.0, 1 / 255.0, 1 / 255.0, color->A / 255.0)), 0, 0);
		ImGui::GetOverlayDrawList()->AddRectFilled(ImVec2(x, y + 1), ImVec2(x + w, y + h), ImGui::ColorConvertFloat4ToU32(ImVec4(1 / 255.0, 1 / 255.0, 1 / 255.0, color->A / 255.0)), 0, 0);
		ImGui::GetOverlayDrawList()->AddRectFilled(ImVec2(x - 1, y), ImVec2(x + w, y + h), ImGui::ColorConvertFloat4ToU32(ImVec4(1 / 255.0, 1 / 255.0, 1 / 255.0, color->A / 255.0)), 0, 0);
		ImGui::GetOverlayDrawList()->AddRectFilled(ImVec2(x + 1, y), ImVec2(x + w, y + h), ImGui::ColorConvertFloat4ToU32(ImVec4(1 / 255.0, 1 / 255.0, 1 / 255.0, color->A / 255.0)), 0, 0);
		ImGui::GetOverlayDrawList()->AddRectFilled(ImVec2(x, y), ImVec2(x + w, y + h), ImGui::ColorConvertFloat4ToU32(ImVec4(color->R / 255.0, color->G / 255.0, color->B / 255.0, color->A / 255.0)), 0, 0);
	}


	static void DrawFilledRectIMCol(int x, int y, int w, int h, ImU32 col)
	{
		ImGui::GetOverlayDrawList()->AddRectFilled(ImVec2(x, y), ImVec2(x + w, y + h), col, 0, 0);
	}

	static void DrawCircleFilled(int x, int y, int radius, RGBA* color, int segments)
	{
		ImGui::GetOverlayDrawList()->AddCircleFilled(ImVec2(x, y), radius, ImGui::ColorConvertFloat4ToU32(ImVec4(color->R / 255.0, color->G / 255.0, color->B / 255.0, color->A / 255.0)), segments);
	}


	static void DrawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, RGBA* color, float thickne)
	{
		ImGui::GetOverlayDrawList()->AddTriangle(ImVec2(x1, y1), ImVec2(x2, y2), ImVec2(x3, y3), ImGui::ColorConvertFloat4ToU32(ImVec4(color->R / 255.0, color->G / 255.0, color->B / 255.0, color->A / 255.0)), thickne);
	}

	static void DrawTriangleFilled(int x1, int y1, int x2, int y2, int x3, int y3, RGBA* color)
	{
		ImGui::GetOverlayDrawList()->AddTriangleFilled(ImVec2(x1, y1), ImVec2(x2, y2), ImVec2(x3, y3), ImGui::ColorConvertFloat4ToU32(ImVec4(color->R / 255.0, color->G / 255.0, color->B / 255.0, color->A / 255.0)));
	}

	static void DrawLine(int x1, int y1, int x2, int y2, RGBA* color, int thickness)
	{
		ImGui::GetOverlayDrawList()->AddLine(ImVec2(x1, y1), ImVec2(x2, y2), ImGui::ColorConvertFloat4ToU32(ImVec4(color->R / 255.0, color->G / 255.0, color->B / 255.0, color->A / 255.0)), thickness);
	}

	static void DrawCornerBox(int x, int y, int w, int h, int borderPx, RGBA* color)
	{
		DrawFilledRect(x + borderPx, y, w / 3, borderPx, color); //top 
		DrawFilledRect(x + w - w / 3 + borderPx, y, w / 3, borderPx, color); //top 
		DrawFilledRect(x, y, borderPx, h / 3, color); //left 
		DrawFilledRect(x, y + h - h / 3 + borderPx * 2, borderPx, h / 3, color); //left 
		DrawFilledRect(x + borderPx, y + h + borderPx, w / 3, borderPx, color); //bottom 
		DrawFilledRect(x + w - w / 3 + borderPx, y + h + borderPx, w / 3, borderPx, color); //bottom 
		DrawFilledRect(x + w + borderPx, y, borderPx, h / 3, color);//right 
		DrawFilledRect(x + w + borderPx, y + h - h / 3 + borderPx * 2, borderPx, h / 3, color);//right 
	}


	static void BlackMamba() {
		ImGuiStyle* style = &ImGui::GetStyle();
		style->Colors[ImGuiCol_Text] = ImVec4(0.80f, 0.80f, 0.83f, 1.00f);
		style->Colors[ImGuiCol_TextDisabled] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
		style->Colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
		style->Colors[ImGuiCol_PopupBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
		style->Colors[ImGuiCol_Border] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
		style->Colors[ImGuiCol_BorderShadow] = ImVec4(0.92f, 0.91f, 0.88f, 0.00f);
		style->Colors[ImGuiCol_FrameBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
		style->Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
		style->Colors[ImGuiCol_FrameBgActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
		style->Colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
		style->Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 0.98f, 0.95f, 0.75f);
		style->Colors[ImGuiCol_TitleBgActive] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
		style->Colors[ImGuiCol_MenuBarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
		style->Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
		style->Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
		style->Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
		style->Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
		style->Colors[ImGuiCol_CheckMark] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
		style->Colors[ImGuiCol_SliderGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
		style->Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
		style->Colors[ImGuiCol_Button] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
		style->Colors[ImGuiCol_ButtonHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
		style->Colors[ImGuiCol_ButtonActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
		style->Colors[ImGuiCol_Header] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
		style->Colors[ImGuiCol_HeaderHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
		style->Colors[ImGuiCol_HeaderActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
		style->Colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		style->Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
		style->Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
		style->Colors[ImGuiCol_PlotLines] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
		style->Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
		style->Colors[ImGuiCol_PlotHistogram] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
		style->Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
		style->Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.25f, 1.00f, 0.00f, 0.43f);
	}



	static void DrawNewText(int x, int y, RGBA* color, const char* str)
	{
		ImGui::GetOverlayDrawList()->AddText(ImVec2(x, y), ImGui::ColorConvertFloat4ToU32(ImVec4(color->R / 255.0, color->G / 255.0, color->B / 255.0, color->A / 255.0)), str);
	}

	static void DrawInSideText(int x, int y, RGBA* color, const char* str)
	{
		ImGui::GetBackgroundDrawList()->AddText(ImVec2(x, y), ImGui::ColorConvertFloat4ToU32(ImVec4(color->R / 255.0, color->G / 255.0, color->B / 255.0, color->A / 255.0)), str);
	}


	// Simple helper function to load an image into a DX11 texture with common settings




    #include "D3DX11tex.h"
    #pragma comment(lib, "D3DX11.lib")

    bool LoadImageByMemory(unsigned char* image, size_t image_size, ID3D11ShaderResourceView** result) {
        D3DX11_IMAGE_LOAD_INFO information;
        ID3DX11ThreadPump* thread = nullptr;

        auto hres = D3DX11CreateShaderResourceViewFromMemory(pDevice, image, image_size, &information, thread, result, 0);
        return (hres == S_OK);
    }


    ImVec2 MessageBoxPos(500, 500);
    bool ShowMessageBox = false;


    void DrawMessageBox(int Index, const char* Text, const char* TextMain = "TextBox") {
        ImGui::SetNextWindowSize(ImVec2(500, 280));
        ImGui::Begin(TextMain, &ShowMessageBox, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoResize); {
            ImGui::Text(TextMain);
            ImGui::Separator();
            ImGui::Spacing();
            ImGui::Text(Text);
            ImGui::Spacing();
            ImGui::Spacing();
            ImGui::Spacing();
            ImGui::Spacing();
            if (ImGui::Button("Show me ya Github")) {
                ShowMessageBox = false;
                system(R"(start https://github.com/DumbDev69420)");
            }
            ImGui::SameLine(0.0f, -10.0f);
            if (ImGui::Button("Oki"))ShowMessageBox = false;
        }
        ImGui::End();
    }




    const char* Textd = "About Me";
    float textSize = 0.5;

    void DrawAboutMe() {



        // Calculate the size of the button based on text length and text size
        ImVec2 buttonSize(strlen(Textd) * 10.0 + 2, 20.0f);

        // Calculate the position for the button in the lower-right corner
        ImVec2 windowSize = ImGui::GetWindowSize();
        ImVec2 buttonPosition(windowSize.x - buttonSize.x + 6.0f, windowSize.y - buttonSize.y);

        // Create the button using ImGui::Button and set its position
        ImVec2 CursorPosCurrent = ImGui::GetCursorPos();

        ImGui::SetCursorPos(buttonPosition);
        if (ImGui::Button(Textd, buttonSize))
        {
            ShowMessageBox = true;
        }

        ImGui::SetCursorPos(CursorPosCurrent);


        if (ShowMessageBox) {
            DrawMessageBox(0, "Hi im a Student from Germany, and i like to Create Trainers", "About me");
        }
    }
	
	
	bool LoadTextureFromFile(const char* filename, ID3D11ShaderResourceView** out_srv, int* out_width, int* out_height)
    {
	// Load from disk into a raw RGBA buffer
	int image_width = 0;
	int image_height = 0;
	unsigned char* image_data = stbi_load(filename, &image_width, &image_height, NULL, 4);
	if (image_data == NULL)
		return false;

	// Create texture
	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Width = image_width;
	desc.Height = image_height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;

	ID3D11Texture2D* pTexture = NULL;
	D3D11_SUBRESOURCE_DATA subResource;
	subResource.pSysMem = image_data;
	subResource.SysMemPitch = desc.Width * 4;
	subResource.SysMemSlicePitch = 0;
	pDevice->CreateTexture2D(&desc, &subResource, &pTexture);

	// Create texture view
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = desc.MipLevels;
	srvDesc.Texture2D.MostDetailedMip = 0;
	pDevice->CreateShaderResourceView(pTexture, &srvDesc, out_srv);

	// Release memory
	pTexture->Release();
	stbi_image_free(image_data);
	image_data = 0;

	// Output dimensions
	*out_width = image_width;
	*out_height = image_height;

	return true;
    }
	


   


	static void DrawPic(int x, int y,int w, int h, const char* Path) {

		if (OldPath != Path) {
			OldPath = Path;
			LoadTextureFromFile(Path, &my_texture, &my_image_width, &my_image_height);
		}

		ImGui::GetOverlayDrawList()->AddImage(my_texture, ImVec2(x, y), ImVec2(x + w, y + h));
	}
	//frame_00_delay-0.03s.jpg
    const bool UseMemoryImage = true;
	std::string Patht = "";
	const char* Path;
	char username[UNLEN + 1];
	int T1ickcCF;
	int T1ickcCL;
	int Part = NULL;
	int AnimationSpeed = 100;
	
#include "Graphics.h"
	static void DrawBackGroundGif(int x, int y, int w, int h) {
		if (ShowAnimation == true) {
			if (iw.window_flags == ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBringToFrontOnFocus) {
				iw.window_flags = ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoBackground;
				iw2.window_flags = ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoBackground;
			}
			if (IniGifs == false) {
                if (UseMemoryImage) {
                    LoadImageByMemory(rawData0, sizeof(rawData0), &packedGif[0].a1);
                    LoadImageByMemory(rawData1, sizeof(rawData1), &packedGif[1].a1);
                    LoadImageByMemory(rawData2, sizeof(rawData2), &packedGif[2].a1);
                    LoadImageByMemory(rawData3, sizeof(rawData3), &packedGif[3].a1);
                    LoadImageByMemory(rawData4, sizeof(rawData4), &packedGif[4].a1);
                    LoadImageByMemory(rawData5, sizeof(rawData5), &packedGif[5].a1);
                    LoadImageByMemory(rawData6, sizeof(rawData6), &packedGif[6].a1);
                    LoadImageByMemory(rawData7, sizeof(rawData7), &packedGif[7].a1);
                    LoadImageByMemory(rawData8, sizeof(rawData8), &packedGif[8].a1);
                    LoadImageByMemory(rawData9, sizeof(rawData9), &packedGif[9].a1);
                    LoadImageByMemory(rawData10, sizeof(rawData10), &packedGif[10].a1);
                    LoadImageByMemory(rawData11, sizeof(rawData11), &packedGif[11].a1);
                    LoadImageByMemory(rawData12, sizeof(rawData12), &packedGif[12].a1);
                    LoadImageByMemory(rawData13, sizeof(rawData13), &packedGif[13].a1);
                    LoadImageByMemory(rawData14, sizeof(rawData14), &packedGif[14].a1);
                    LoadImageByMemory(rawData15, sizeof(rawData15), &packedGif[15].a1);
                    LoadImageByMemory(rawData16, sizeof(rawData16), &packedGif[16].a1);
                    LoadImageByMemory(rawData17, sizeof(rawData17), &packedGif[17].a1);
                    LoadImageByMemory(rawData18, sizeof(rawData18), &packedGif[18].a1);
                    LoadImageByMemory(rawData19, sizeof(rawData19), &packedGif[19].a1);
                    LoadImageByMemory(rawData20, sizeof(rawData20), &packedGif[20].a1);
                    LoadImageByMemory(rawData21, sizeof(rawData21), &packedGif[21].a1);
                    LoadImageByMemory(rawData22, sizeof(rawData22), &packedGif[22].a1);
                    LoadImageByMemory(rawData23, sizeof(rawData23), &packedGif[23].a1);
                    LoadImageByMemory(rawData24, sizeof(rawData24), &packedGif[24].a1);
                    LoadImageByMemory(rawData25, sizeof(rawData25), &packedGif[25].a1);
                    LoadImageByMemory(rawData26, sizeof(rawData26), &packedGif[26].a1);
                    LoadImageByMemory(rawData27, sizeof(rawData27), &packedGif[27].a1);
                    LoadImageByMemory(rawData28, sizeof(rawData28), &packedGif[28].a1);
                    LoadImageByMemory(rawData29, sizeof(rawData29), &packedGif[29].a1);
                    LoadImageByMemory(rawData30, sizeof(rawData30), &packedGif[30].a1);
                    LoadImageByMemory(rawData31, sizeof(rawData31), &packedGif[31].a1);
                    LoadImageByMemory(rawData32, sizeof(rawData32), &packedGif[32].a1);
                    LoadImageByMemory(rawData33, sizeof(rawData33), &packedGif[33].a1);
                    LoadImageByMemory(rawData34, sizeof(rawData34), &packedGif[34].a1);
                    LoadImageByMemory(rawData35, sizeof(rawData35), &packedGif[35].a1);
                    IniGifs = true;
                }
                else
                {
                    DWORD username_len = UNLEN + 1;
                    GetUserName((LPWSTR)username, &username_len);
                    std::string NameReal;
                    char tmp2;
                    uintptr_t startOfNameStringaddr = (uintptr_t)&username;
                    for (size_t i = 0; i < username_len - 1; i++)
                    {
                        if (i == 0) {
                            NameReal = *(char*)(startOfNameStringaddr);
                            continue;
                        }
                        NameReal = NameReal + *(char*)(startOfNameStringaddr + 0x2 * i);
                        continue;
                    }
                    Patht = "C:\\Users\\";
                    Patht = Patht + NameReal + "\\Desktop\\Data\\Animation\\ffffff-ezgif-4-0b5934b623-gif-jpg\\frame_";
                    Path = NULL;
                    std::ifstream File;
                    for (size_t i = 0; i < 37; i++)
                    {
                        Patht = "C:\\Users\\" + NameReal;
                        Patht = Patht + "\\Desktop\\Data\\Animation\\ffffff-ezgif-4-0b5934b623-gif-jpg\\frame_";
                        Path = NULL;
                        if (i == 0) {
                            Patht = Patht + "00" + "_delay-0.03s.jpg";
                            Path = Patht.c_str();
                            File.open(Path);
                            if (!File.is_open()) {
                                ShowAnimation = false;
                                std::cout << "Couldnt find Animation stuff damn\n";
                                return;
                            }
                            LoadTextureFromFile(Path, &packedGif[i].a1, &my_image_width, &my_image_height);
                            continue;
                        }

                        if (i >= 10) {
                            Patht = Patht + std::to_string(i) + "_delay-0.03s.jpg";
                            Path = Patht.c_str();
                            LoadTextureFromFile(Path, &packedGif[i].a1, &my_image_width, &my_image_height);
                            continue;
                        }
                        else
                        {
                            Patht = Patht + "0" + std::to_string(i) + "_delay-0.03s.jpg";
                            Path = Patht.c_str();
                            LoadTextureFromFile(Path, &packedGif[i].a1, &my_image_width, &my_image_height);
                            continue;
                        }
                    }
                    Frames = 0;
                    IniGifs = true;
                }
			}
			else
			{
				if (Part > 35)Part = 0;
				T1ickcCF = GetTickCount64();
				if (T1ickcCF >= T1ickcCL) {
					T1ickcCL = T1ickcCF + AnimationSpeed;
					Part = Part + 1;
				}
				if (Part > 35)Part = 0;
				//ImGui::GetBackgroundDrawList()->AddImage(packedGif[Part].a1, ImVec2(x, y), ImVec2(x + w, y + h));
				ImGui::GetBackgroundDrawList()->AddImageRounded(packedGif[Part].a1, ImVec2(x, y), ImVec2(x + w, y + h), ImVec2(0, 0), ImVec2(1, 1), ImU32(IM_COL32_WHITE), 10.0f);
			}
		}
		else
		{
			if (iw.window_flags == ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoBackground) {
              
				iw.window_flags = ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBringToFrontOnFocus;
				iw2.window_flags = ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBringToFrontOnFocus;
			}
		}
		return;
	}
	

    std::vector<ID3D11ShaderResourceView*> srvArrayL;

	std::wstring stringToWideString(const std::string& str)
	{
		std::wstring wstr(str.length(), L' ');
		std::copy(str.begin(), str.end(), wstr.begin());
		return wstr;
	}
    /*
    const int start = IDB_BITMAP1;
    const char* CustomGif = "giphybetter.gif";
    int TotalFrames = 0;
	static void DrawBackGroundGif(int x, int y, int w, int h) {
		if (ShowAnimation == true) {
            if (CustomGif == "") {
                ShowAnimation = false;
                return;
            }

			if (iw.window_flags == ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBringToFrontOnFocus) {
				iw.window_flags = ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoBackground;
				iw2.window_flags = ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoBackground;
			}
			if (IniGifs == false) {
                DWORD username_len = UNLEN + 1;
                GetUserName((LPWSTR)username, &username_len);
                std::string NameReal;
                char tmp2;
                uintptr_t startOfNameStringaddr = (uintptr_t)&username;
                for (size_t i = 0; i < username_len - 1; i++)
                {
                    if (i == 0) {
                        NameReal = *(char*)(startOfNameStringaddr);
                        continue;
                    }
                    NameReal = NameReal + *(char*)(startOfNameStringaddr + 0x2 * i);
                    continue;
                }

                Patht = "C:\\Users\\" + NameReal + "\\Desktop\\Data\\Animation\\" + CustomGif;

                if (LoadGifFromPath(Patht.c_str(), pDevice, &srvArrayL, &TotalFrames) == false) {
                    std::cout << "Upsi ging was falsch\n";
                    ShowAnimation = false; return;
                }

				Frames = 0;
				IniGifs = true;
			}
			else
			{

                if (srvArrayL.at(0) == 0x0) { ShowAnimation = false; return; }
				if (Part > TotalFrames)Part = 0;
				T1ickcCF = GetTickCount64();
				if (T1ickcCF >= T1ickcCL) {
					T1ickcCL = T1ickcCF + AnimationSpeed;
					Part = Part + 1;
				}
				if (Part > TotalFrames)Part = 0;
				//ImGui::GetBackgroundDrawList()->AddImage(packedGif[Part].a1, ImVec2(x, y), ImVec2(x + w, y + h));
				ImGui::GetBackgroundDrawList()->AddImageRounded(srvArrayL.at(Part), ImVec2(x, y), ImVec2(x + w, y + h), ImVec2(0, 0), ImVec2(1, 1), ImU32(IM_COL32_WHITE), 10.0f);
			}
		}
		else
		{
			if (iw.window_flags == ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoBackground) {
				iw.window_flags = ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBringToFrontOnFocus;
				iw2.window_flags = ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBringToFrontOnFocus;
			}
		}
		return;
	}*/

}



namespace Saver {
    int TypesCreated = 0;

}


size_t stringLength(const char* str) {
    size_t length = 0;
    while (str[length] != '\0') {
        if (length == 1600)break;

        length++;
    }
    return length;
}

class ParameterShit {
public:

    HANDLE TargetProcess; DWORD start; DWORD size; const char* sig; const char* mask;
    DWORD* returnaddrs;
    bool ReadMem = false;
    bool MinusBase = false;
    DWORD EditAddrs = 0x0;
    bool WasRead = false;



    void DefineObjects(HANDLE TargetProcess1, DWORD Start1, DWORD Size, const char* Signature, const char* Mask, DWORD* AddressToReturnTo, bool ReadMemory = false, bool ReturnOffsetMinusBaseAddress = false, DWORD EditAddressBy = 0x0) {
        TargetProcess = TargetProcess1;
        start = Start1;
        size = Size;
        sig = Signature;
        mask = Mask;
        returnaddrs = AddressToReturnTo;
        ReadMem = ReadMemory;
        MinusBase = ReturnOffsetMinusBaseAddress;
        EditAddrs = EditAddressBy;
    }
};


class ParameterShit64 {
public:


    HANDLE TargetProcess; uintptr_t start; uintptr_t size; const char* sig; const char* mask;

    uintptr_t* returnaddrs;
    bool ReadMem = false;
    bool MinusBase = false;
    uintptr_t EditAddrs = 0x0;
    bool WasRead = false;
    std::string ModuleName = "No Module Name :(";
    bool SkipFirstFind = false;
    bool GoToAddressFromStart = false;
    uintptr_t EditAddrdif = 0x0;

    void DefineObjects(HANDLE TargetProcess1, uintptr_t Start1, uintptr_t Size, const char* Signature, const char* Mask, uintptr_t* AddressToReturnTo, bool ReadMemory = false, bool ReturnOffsetMinusBaseAddress = false, uintptr_t EditAddressBy = 0x0) {
        TargetProcess = TargetProcess1;
        start = Start1;
        size = Size;
        sig = Signature;
        mask = Mask;
        returnaddrs = AddressToReturnTo;
        ReadMem = ReadMemory;
        MinusBase = ReturnOffsetMinusBaseAddress;
        EditAddrs = EditAddressBy;
    }
};

ParameterShit NewBugati;
ParameterShit64 NewBugatti1;
//External sig scanning functions where written by Zer0Mem0ry on github
namespace SigShit {

    void GetProcessMemoryInfoe(HANDLE Proc, DWORD* dwSize) {
        HANDLE process = GetCurrentProcess();
        PROCESS_MEMORY_COUNTERS_EX pmc;
        if (GetProcessMemoryInfo(process, (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc))) {
            *dwSize = (DWORD)pmc.PeakWorkingSetSize;
        }
        else {
            // If the function fails, set the memory info to zero
            *dwSize = 0;
        }
        CloseHandle(process);
    }


    void GetProcessMemoryInfoeX64(HANDLE Proc, uintptr_t* dwSize) {
        HANDLE process = GetCurrentProcess();
        PROCESS_MEMORY_COUNTERS_EX pmc;
        if (GetProcessMemoryInfo(process, (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc))) {
            *dwSize = (uintptr_t)pmc.PeakWorkingSetSize;
        }
        else {
            // If the function fails, set the memory info to zero
            *dwSize = 0;
        }
        CloseHandle(process);
    }

    // for comparing a region in memory, needed in finding a signature
    bool MemoryCompare(const BYTE* bData, const BYTE* bMask, const char* szMask) {
        for (; *szMask; ++szMask, ++bData, ++bMask) {
            if (*szMask == 'x' && *bData != *bMask) {
                return false;
            }
        }
        return (*szMask == NULL);
    }


    // for finding a signature/pattern in memory of another process
    DWORD FindSignature(HANDLE TargetProcess, DWORD start, DWORD size, const char* sig, const char* mask)
    {
        BYTE* data = new BYTE[size];
        SIZE_T bytesRead;

        ReadProcessMemory(TargetProcess, (LPVOID)start, data, size, &bytesRead);

        for (DWORD i = 0; i < size; i++)
        {
            if (MemoryCompare((const BYTE*)(data + i), (const BYTE*)sig, mask)) {
                return start + i;
            }
        }
        delete[] data;
        return NULL;
    }

    // for finding a signature/pattern in memory of another process
    uintptr_t FindSignature64(HANDLE TargetProcess, uintptr_t start, uintptr_t size, const char* sig, const char* mask, bool SkipFirstFind = false)
    {
        BYTE* data = new BYTE[size];
        SIZE_T bytesRead;

        ReadProcessMemory(TargetProcess, (LPVOID)start, data, size, &bytesRead);

        for (DWORD i = 0; i < size; i++)
        {
            if (MemoryCompare((const BYTE*)(data + i), (const BYTE*)sig, mask)) {
                if (SkipFirstFind == true) {
                    SkipFirstFind = false;
                    continue;
                }
                return start + i;
            }
        }
        delete[] data;
        return NULL;
    }

    void DebugPrint(ParameterShit& Amk);
    void DebugPrint64(ParameterShit64& Amk);

    bool WrapSig(ParameterShit& Doe) {
        bool Execute = true;

        if (Doe.start == 0 || Doe.TargetProcess == 0) {
            std::cout << "Invalid start address or Handle of Target proc!\n";
            Execute = false;
        }

        if (Doe.sig == "" || Doe.mask == "") {
            std::cout << "Invalid Sig or mask!\n";
            Execute = false;
        }

        if (Doe.size == 0) {
            std::cout << "Invalid size!\n";
        }

        CreateThread(nullptr, 0, LPTHREAD_START_ROUTINE(SigShit::DebugPrint), &Doe, 0, nullptr);
        return true;
    }

    bool WrapSig64(ParameterShit64& Doe) {
        bool Execute = true;

        if (Doe.start == 0 || Doe.TargetProcess == 0) {
            std::cout << "Invalid start address or Handle of Target proc!\n";
            Execute = false;
        }

        if (Doe.sig == "" || Doe.mask == "") {
            std::cout << "Invalid Sig or mask!\n";
            Execute = false;
        }

        if (Doe.size == 0) {
            std::cout << "Invalid size!\n";
        }

        CreateThread(nullptr, 0, LPTHREAD_START_ROUTINE(SigShit::DebugPrint64), &Doe, 0, nullptr);
        return true;
    }

    bool ReadAddrbackwards = false;

    void DebugPrint(ParameterShit& Amk)
    {
        DWORD Dot = 0x0;

        Dot = SigShit::FindSignature64(Amk.TargetProcess, Amk.start, Amk.size, Amk.sig, Amk.mask) + Amk.EditAddrs;
        if (Amk.ReadMem == false) {
            if (Amk.MinusBase == true) {
                *Amk.returnaddrs = (Dot - Amk.start);
                std::cout << "Base: " << std::hex << *Amk.returnaddrs << "\n";
                Amk.WasRead = true;
                return;
            }
            std::cout << "Base: " << std::hex << *Amk.returnaddrs << "\n";
            *Amk.returnaddrs = Dot;
            Amk.WasRead = true;
            return;
        }



        if (ReadAddrbackwards == true) {
            BYTE Dot2 = 0x0;
            DWORD Dot3 = 0x0;
            ReadProcessMemory(Amk.TargetProcess, (BYTE*)Dot, &Dot2, sizeof(Dot2), 0);
            for (size_t i = 0; i < 4; i++)
            {
                ReadProcessMemory(Amk.TargetProcess, (BYTE*)Dot - i + 3, &Dot2, sizeof(Dot2), 0);
                switch (i)
                {
                case 0:
                    Dot3 = Dot3 + (Dot2 * 0x1000000);
                    break;


                case 1:

                    Dot3 = Dot3 + (Dot2 * 0x10000);
                    break;


                case 2:

                    Dot3 = Dot3 + (Dot2 * 0x100);
                    break;



                case 3:

                    Dot3 = Dot3 + (Dot2 * 0x1);
                    break;
                }
            }
            Dot = Dot3;
        }
        else
        {
            ReadProcessMemory(Amk.TargetProcess, (BYTE*)Dot, &Dot, sizeof(Dot), 0);
        }

        if (Amk.MinusBase == true) {
            *Amk.returnaddrs = (Dot - Amk.start);
            Amk.WasRead = true;
            return;
        }

        *Amk.returnaddrs = Dot;
        Amk.WasRead = true;
    }

    void DebugPrint64(ParameterShit64& Amk)
    {
        uintptr_t Dot = 0x0;

        Dot = SigShit::FindSignature64(Amk.TargetProcess, Amk.start, Amk.size, Amk.sig, Amk.mask, Amk.SkipFirstFind) + Amk.EditAddrs;

        if (Dot - Amk.EditAddrs == 0x0) {
            std::cout << "Couldnt Find Address for {" + Amk.ModuleName + "}\n";
            *Amk.returnaddrs = 0x0;
            return;
        }

        if (Amk.GoToAddressFromStart == true) {
            volatile uintptr_t Sfd;
            Sfd = stringLength(Amk.mask) * 0x8;
            Dot = (Dot + Sfd);
        }

        if (Amk.ReadMem == false) {
            if (Amk.MinusBase == true) {
                *Amk.returnaddrs = (Dot - Amk.start);
                std::cout << "Base: " << std::hex << *Amk.returnaddrs << "\n";
                Amk.WasRead = true;
                return;
            }
            std::cout << "Base: " << std::hex << *Amk.returnaddrs << "\n";
            *Amk.returnaddrs = Dot;
            Amk.WasRead = true;
            return;
        }



        if (ReadAddrbackwards == true) {
            volatile uintptr_t Dot3 = 0x0;
            uintptr_t Dot6 = 0x0;

            for (size_t i = 0; i < 4; i++)
            {
                BYTE Dot2 = 0x0;
                ReadProcessMemory(Amk.TargetProcess, (BYTE*)Dot - i + 4, &Dot2, sizeof(Dot2), nullptr);

                switch (i)
                {
                case 0:
                    Dot6 = Dot2;
                    Dot3 = (0x10000000 * Dot6);
                    break;

                case 1:
                    Dot6 = Dot2;
                    Dot3 = Dot3 + (0x100000 * Dot6);
                    break;

                case 2:
                    Dot6 = Dot2;
                    Dot3 = Dot3 + (0x1000 * Dot6);
                    break;

                case 3:
                    Dot6 = Dot2;
                    Dot3 = Dot3 + (0x10 * Dot6);
                    break;
                }
            }

            Dot3 = static_cast<uintptr_t>(Dot3) >> 4;
            Dot = Dot - Amk.EditAddrs + Dot3;
            Dot = Dot + 0x7;


        }
        else
        {
            ReadProcessMemory(Amk.TargetProcess, (BYTE*)Dot, &Dot, sizeof(Dot), 0);
        }

        if (Amk.MinusBase == true) {
            if (Amk.EditAddrdif != 0x0) {
                *Amk.returnaddrs = (Dot - Amk.start + (Amk.EditAddrs - Amk.EditAddrdif));
            }
            else
            {
                *Amk.returnaddrs = (Dot - Amk.start);
            }

            Amk.WasRead = true;
            return;
        }

        *Amk.returnaddrs = Dot;
        Amk.WasRead = true;
    }
}










namespace Spotify {


	int TickFirst42;
	int TickLast42;
	int TickFirst41;
	int TickLast41;

	uintptr_t ModuleBaseSpotify = 0;
	HANDLE hSpotify = 0;
	uintptr_t SpotifyTextAddress;
	uintptr_t firstAddr;
	uintptr_t SpotifySongAddress;
	int SongNameLenght = 0;
	std::string SongFirst;
	std::string Songname;
	std::string SongCreator;
	DWORD SProcID;

	bool DebugMode = true;
	bool SpotifyMode = false;
	bool AlwaysShowName = false;
	bool RainbowMode = true;
	bool Biggers = false;
	bool Spotifyini = false;
	bool EmptyCharacter = false;
	bool OnlyName = false;
	bool SpotifyTb = false;

	const char* CurrentSong;

	//std::vector<unsigned int>JoeBidden = { 0x34, 0x2c, 0x20, 0x18 };
	std::vector<unsigned int>JoeBidden1 = { 0x34, 0x2c, 0x20 };
	std::vector<unsigned int>Lyrics = { 0x2c, 0x3e0,0x10, 0x24 , 0x20 };
	std::vector<unsigned int>Lyrics1 = { 0x2c, 0x3e0 };

    bool AddressChanged = false;
    int SongLemgthLast = 0;
    const char* LastCharacters = "";
	void SameAddress() {
        char rnd = 0;
        std::string Characters = "";

		DWORD OverDupe = FindDMAAddy32(hSpotify, firstAddr, JoeBidden1);
		ReadProcessMemory(hSpotify, (BYTE*)OverDupe + 0x18, &SpotifySongAddress, sizeof(OverDupe), 0);
		ReadProcessMemory(hSpotify, (BYTE*)OverDupe + 0x1c, &SongNameLenght, sizeof(SongNameLenght), 0);
        ReadProcessMemory(hSpotify, (BYTE*)SpotifySongAddress, &rnd, sizeof(rnd), 0);
        Characters = rnd;
        ReadProcessMemory(hSpotify, (BYTE*)SpotifySongAddress + 0x5, &rnd, sizeof(rnd), 0);
        Characters = Characters + rnd;

        if (LastCharacters != Characters.c_str()) {
            LastCharacters = Characters.c_str();
            AddressChanged = true;
        }

        if (SongLemgthLast != SongNameLenght) {
            SongLemgthLast = SongNameLenght;
            AddressChanged = true;
        }

		if (SpotifySongAddress != SpotifyTextAddress) {
			SpotifyTextAddress = SpotifySongAddress;
            AddressChanged = true;
		}

	}






    DWORD Spotify4 = 0x0;
    HANDLE hspotifyxy = 0x0;
    DWORD ModulehSpotif = 0x0;


    bool AddrFound = false;
    DWORD Pointer = 0x99748D4;// 0x99748D4;
    void AutoFixPtr() {
        if (AddrFound == true)return;
        const char* Sig = "\xa1\x00\x00\x00\x00\x5d\xc3\xcc\xcc\xcc\xcc\xcc\xcc\x55\x89\xe5\x53\x57\x56\x83\xec\x00\x89\xce\x8b\x7d\x00\x8b\x5d\x00\xa1";
        const char* Mask = "x????xxxxxxxxxxxxxxxx?xxxx?xx?x";
        DWORD SizeOfSpotify = 0x0;
        SigShit::GetProcessMemoryInfoe(hSpotify, &SizeOfSpotify);
        ParameterShit Shit = { hSpotify, ModulehSpotif, SizeOfSpotify,Sig, Mask, &Pointer ,true, true, 0x1 };
        SigShit::DebugPrint(Shit);
        if (*Shit.returnaddrs != 0x0) {
            std::cout << "Pointer of Spotify Found!\n";
        }
        AddrFound = true;
    }

	void cod();


	void GetCurrentSong() {
		if (SpotifyMode == true) {
			if (Spotifyini == false) {
				SProcID = GetProcId(L"Spotify.exe");

				if (SProcID != 0x0) {
					Spotifyini = true;
				}
				else
				{
					SongFirst = "Spotify Closed";
					return;
				}
				hSpotify = 0;
				Spotify4 = 0x0;
				hSpotify = OpenProcess(PROCESS_ALL_ACCESS, 0, SProcID);
				if (Spotify4 == 0x0) {
					Spotify4 = GetNthChildProcessId(SProcID, 5);
					if (Spotify4 != 0x0) {
						hspotifyxy = OpenProcess(PROCESS_ALL_ACCESS, 0, Spotify4);
						ModulehSpotif = GetModuleBaseAddress(Spotify4, L"libcef.dll");
					}


				}
				ModuleBaseSpotify = GetModuleBaseAddress(SProcID, L"libcef.dll");
				firstAddr = ModuleBaseSpotify + Pointer;
				DWORD OverDupe = FindDMAAddy32(hSpotify, firstAddr, JoeBidden1);
                if (ReadProcessMemory(hSpotify, (BYTE*)OverDupe + 0x18, &SpotifySongAddress, sizeof(OverDupe), 0) == false) {
                    AutoFixPtr();
                    Spotifyini = false;
                    return;
                }
				ReadProcessMemory(hSpotify, (BYTE*)OverDupe + 0x1c, &SongNameLenght, sizeof(SongNameLenght), 0);
				if (DebugMode == true) {

					std::cout << "[DEBUG] SongNameAddr: 0x" << std::hex << SpotifyTextAddress << "\n";
					std::cout << "[DEBUG] ModuleBase: 0x" << std::hex << ModuleBaseSpotify << "\n";
					std::cout << "[DEBUG] ProcID: 0x" << std::hex << SProcID << "\n";
				}
				TickFirst41 = 0;
				TickLast41 = 0;
			}
			else
			{
				SProcID = GetProcId(L"Spotify.exe");
				if (SProcID == 0x0) {
					SongFirst = "Spotify Closed";
					Spotifyini = false;
					return;
				}


				if (SProcID != 0x0 || hSpotify != INVALID_HANDLE_VALUE) {
					SameAddress();
                    if (AddressChanged == true) {
                        AddressChanged = false;
                        cod();
                    }

					if (SongFirst != Songname) {
						if (DebugMode == true) {
							std::cout << "Now playing: " << Songname << "\n";
						}
						SpotifyTb = true;
						SongFirst = Songname;
					}
				}
				else
				{
					SongFirst = "Spotify Closed";
					Spotifyini = false;
				}
			}
		}
	}

    


	bool ShowLyrics = true;
	DWORD LyricsAddr, TestAddr1, TestAddr;
	std::string LyricText, TestText;
	int TextLength;
	uintptr_t LyricsBro = 0x0;
	DWORD ptr = 0x0;
	DWORD Lyrptr = 0x0;
	DWORD lyriscs = 0x0;


	void GetLyrics() {
		if (ShowLyrics == false)
			if (SProcID == 0x0)return;


		if (Spotify4 == 0x0) {
			Spotify4 = GetNthChildProcessId(SProcID, 5);
			if (Spotify4 != 0x0) {
				hspotifyxy = OpenProcess(PROCESS_ALL_ACCESS, 0, Spotify4);
				ModulehSpotif = GetModuleBaseAddress(Spotify4, L"libcef.dll");
				Lyrptr = ModulehSpotif + 0x99ECE4C;
			}


		}
		else
		{
			SIZE_T size = 0;
			LyricsBro = FindDMAAddy32(hspotifyxy, Lyrptr, Lyrics);

			ReadProcessMemory(hspotifyxy, (BYTE*)LyricsBro + 0x4, &TextLength, sizeof(TextLength), 0);
			size = TextLength;
			if (TextLength > 120)return;
			LyricsAddr = LyricsBro + 0xc;

			LyricText = "";

			char csd;
			for (size_t i = 0; i < TextLength; i++)
			{
				if (i == 0) {
					ReadProcessMemory(hspotifyxy, (BYTE*)LyricsAddr, &csd, sizeof(csd), 0);
					LyricText = LyricText + csd;
					continue;
				}
				ReadProcessMemory(hspotifyxy, (BYTE*)LyricsAddr + 0x1 * i, &csd, sizeof(csd), 0);
				LyricText = LyricText + csd;
			}




			if (TestText != LyricText) {
				TestText = LyricText;
			}
		}



	}

	std::string wstringToString(const std::wstring& wstr)
	{
		std::string str;
		for (wchar_t c : wstr) {
			str += static_cast<char>(c);
		}
		return str;
	}



    bool Unicode(std::wstring wstr) {
        std::string Cd = "";
        Cd = wstringToString(wstr);

        if (IsTextUnicode(Cd.c_str(), Cd.length(), 0) == 1) {
            wstr.pop_back();
            wstr = wstr + L"!";
        }
    }



	void cod() {
		Songname = "";
		WCHAR Empttts = 0;
		std::wstring Emptsio = L"";
        std::wstring CreatorName = L"";
        int TestFly = 0;
        bool CreatorName_b = false;
		if (SongNameLenght < 100) {

				for (size_t i = 0; i < SongNameLenght; i++)
				{
                    if (CreatorName_b == true) {

                        if (Empttts == 0) {
                            ReadProcessMemory(hSpotify, (BYTE*)SpotifyTextAddress + i + 1, &Empttts, sizeof(Empttts), NULL);
                            if (Empttts == 0) {
                                Spotifyini = false;
                            }
                        }


                        ReadProcessMemory(hSpotify, (BYTE*)SpotifyTextAddress + i, &Empttts, sizeof(Empttts), NULL);

                        CreatorName = CreatorName + Empttts;
                    }
                    else
                    {

                        if (i == 0) {
                            ReadProcessMemory(hSpotify, (BYTE*)SpotifyTextAddress + i, &Empttts, sizeof(Empttts), NULL);
                            Emptsio = Emptsio + Empttts;
                            continue;
                        }

                        ReadProcessMemory(hSpotify, (BYTE*)SpotifyTextAddress + i, &Empttts, sizeof(Empttts), NULL);
                        ReadProcessMemory(hSpotify, (BYTE*)SpotifyTextAddress + i, &TestFly, sizeof(TestFly), NULL);

                        if (TestFly == 547520738) {
                            i = i + 3;
                             CreatorName_b = true;
                             Emptsio = Emptsio + L"by ";
                             continue;
                        }


                        if (Empttts == 0) {
                            ReadProcessMemory(hSpotify, (BYTE*)SpotifyTextAddress + i + 1, &Empttts, sizeof(Empttts), NULL);
                            if (Empttts == 0) {
                                Spotifyini = false;
                            }
                        }



                        Emptsio = Emptsio + Empttts;
                    }
					
				}
				Songname = wstringToString(Emptsio) +  wstringToString(CreatorName);
				Emptsio = L"";
				Empttts = 0;
		}
		else
		{
			Songname = "";
		}
	}



	void NoAds() {

	}

}







namespace Cheat {

    POINT p;

    namespace Vals {
        HWND hWnd; UINT uMsg; WPARAM wParam; LPARAM lParam;
    }

    namespace Rendershit {
        bool ShowRenderStuff = false;

    }



    std::string tmp1s1;
    ImVec2 WindowSizePlylist(700, 300);

    bool Active4 = false;

    void RenderLyrics() {
        imGuiDrawShit::BlackMamba();
        if (iw4.window_title != "Lyrics")iw4.window_title = "Lyrics";
        ImGui::SetNextWindowSize(WindowSizePlylist);
        ImGui::Begin(iw4.window_title, &Active4, iw4.window_flags);
        {
            tmp1s1 = "Lyric: " + Spotify::LyricText;
            const char* Data11 = tmp1s1.data();
            ImGui::Text(Data11);
        }
        ImGui::End();
        ImGui::StyleColorsDark();
        MrWhite = { 255,255,255,255 };


    }




    std::string overflow0;
    std::string overflow1;
    std::string overflow2;
    std::string overflow3;

    int TickcC, Tickcb;
    int TickcC1, Tickcb1;
    int res;
    int tesrt;
    int height2;
    char tmpi;
    DWORD colorfa = 0;
    bool inif = false;
    int OverflowingParts = 0;
    bool part1 = false;
    bool Start = false;
    bool ShouldChangeColor = false;
    bool Debuga;
    RECT Dawg;
    RGBA OverwriteColor = { 53,53,255,colorfa };
    int Size2;

    bool* CurrentBool = 0x0;
    //Duration in ms
    bool NotifyBox(BYTE Flag, std::string Dawg2, int Duration, bool* Tmp) {
        Duration /= 2;

        if (CurrentBool != 0x0) {
            if (CurrentBool != Tmp) {
                return false;
            }
        }
        else
        {
            CurrentBool = Tmp;
            ShouldChangeColor = false;
        }
        if (inif == false) {
            int Characters = strlen(Dawg2.c_str());
            if (Characters > 56) {
                OverflowingParts = 1;
                for (size_t i = 0; i < 54; i++)
                {
                    overflow0 = overflow0 + Dawg2[i];
                }
                for (size_t i = 0; i < 54; i++)
                {
                    if (i >= Characters) {
                        break;
                    }
                    overflow1 = overflow1 + Dawg2[54 + i];
                }
                if (Characters > 112)
                {
                    OverflowingParts = 2;
                    for (size_t i = 0; i < 54; i++)
                    {
                        if (i >= Characters) {
                            break;
                        }
                        overflow2 = overflow2 + Dawg2[110 + i];
                    }
                }
                if (Characters > 168) {
                    OverflowingParts = 3;
                    for (size_t i = 0; i < 54; i++)
                    {
                        if (i >= Characters) {
                            break;
                        }
                        overflow3 = overflow3 + Dawg2[166 + i];
                    }
                }
            }
            Tickcb = TickcC + 1;
            inif = true;
            res = Dawg.bottom - 200;
            height2 = Dawg.bottom;

            if (Flag == 0) {

                OverwriteColor.A = colorfa;
                CoolColor = OverwriteColor;
            }
            else
            {
                CoolColor2 = { 82,255,93,colorfa };
            }
        }
        if (Flag == 0) {
            imGuiDrawShit::DrawFilledRect(20, height2, 400, 150, &CoolColor);
        }
        else
        {
            imGuiDrawShit::DrawFilledRect(20, height2, 400, 150, &CoolColor2);
        }
        if (OverflowingParts >= 1) {
            if (Flag == 0) {
                imGuiDrawShit::DrawNewText(30, height2 + 10, &MrWhite, overflow0.c_str());
                imGuiDrawShit::DrawNewText(30, height2 + 20, &MrWhite, overflow1.c_str());
                if (OverflowingParts >= 2) {
                    imGuiDrawShit::DrawNewText(30, height2 + 30, &MrWhite, overflow2.c_str());
                    if (OverflowingParts >= 3) {
                        imGuiDrawShit::DrawNewText(30, height2 + 30, &MrWhite, overflow3.c_str());
                    }
                }
            }
            else
            {
                imGuiDrawShit::DrawNewText(30, height2 + 10, &MrBlack, overflow0.c_str());
                imGuiDrawShit::DrawNewText(30, height2 + 20, &MrBlack, overflow1.c_str());
                if (OverflowingParts >= 2) {
                    imGuiDrawShit::DrawNewText(30, height2 + 30, &MrBlack, overflow2.c_str());
                    if (OverflowingParts >= 3) {
                        imGuiDrawShit::DrawNewText(30, height2 + 30, &MrBlack, overflow3.c_str());
                    }
                }
            }

        }
        else
        {
            imGuiDrawShit::DrawNewText(30, height2 + 10, &MrWhite, Dawg2.c_str());
        }
        TickcC = GetTickCount64();
        TickcC1 = GetTickCount64();
        if (TickcC >= Tickcb) {
            Tickcb = TickcC + 1;
            GetWindowRect(Vals::hWnd, &Dawg);
            if (part1 == false) {
                if (height2 >= res) {
                    if (Flag == 0) {
                        OverwriteColor.A = colorfa;
                        CoolColor = OverwriteColor;
                    }
                    else
                    {
                        CoolColor2 = { 82,255,93,colorfa };
                    }
                    //imGuiDrawShit::DrawFilledRect(0, (Dawg.bottom - height2) , 400, 200, &CoolColor);
                    if (colorfa <= 150) {
                        colorfa = colorfa + 2;
                    }
                    height2 = height2 - 4;
                }
                else
                {
                    part1 = true;
                    res = res + 200;
                    Tickcb1 = TickcC1 + Duration;
                }
            }
            else
            {
                if (TickcC1 >= Tickcb1) {
                    if (height2 <= res) {
                        if (Flag == 0) {
                            OverwriteColor.A = colorfa;
                            CoolColor = OverwriteColor;
                        }
                        else
                        {
                            CoolColor2 = { 82,255,93,colorfa };
                        }
                        //imGuiDrawShit::DrawFilledRect(0, (Dawg.bottom + height2), 400, 200, &CoolColor);
                        colorfa = colorfa - 2;
                        height2 = height2 + 4;
                    }
                    else
                    {
                        colorfa = 0;
                        part1 = false;
                        inif = false;
                        ShouldChangeColor = true;
                        *Tmp = false;
                        CurrentBool = 0x0;
                        overflow0 = "";
                        overflow1 = "";
                        overflow2 = "";
                        overflow3 = "";
                        OverflowingParts = 0;
                        CoolColor = { 53,53,255,colorfa };
                        return true;
                    }
                }
            }
        }
        return false;
    }





    class Vector3
    {
    public:
        float x, z, y;
    };

    class Vector4
    {
    public:
        float x, z, y, w;
    };


    std::string Tempd2;
    const char* TempBus23;
    std::string Tempd21;
    std::string Tempd215;
    const char* TempBus213;
    const char* TempBus231;

    bool StopCheat = false;
    bool IniCheat = false;
    bool GetVals = false;
    bool ShowCheat = true;
    bool StopDaCheat = false;
    bool Cheatclosed = false;
    bool LightSkin = false;
    bool LightSkinini = false;
    bool ShowConWin = false;

    HWND POSk;

    Vector3 Pos = { 0.0f,0.0f,0.0f };
    Vector3 Rotation = { 0.0f,0.0f,0.0f };


    int TickFirst;
    int TickLast;
    int TickFirst9;
    int TickLast9;
    int TimeFunctionsCalled = 500;
    int HeightofMouse = 0;
    int Tabs = 0;
    int Te2[4];
    int height24;

    DWORD GetMemoryProtection(LPVOID address)
    {
        MEMORY_BASIC_INFORMATION memInfo;
        VirtualQuery(address, &memInfo, sizeof(memInfo));
        return memInfo.Protect;
    }


    bool ShouldUsePointer64(uintptr_t ptr) {
        if (ptr == 0x0 || ptr < 0x10000000 || ptr > 0x7fffffffffff || GetMemoryProtection(reinterpret_cast<LPVOID>(ptr)) == PAGE_NOACCESS) return false;
        return true;
    }


    bool MemoryUsable(uintptr_t ptr) {
        DWORD Protection = GetMemoryProtection(reinterpret_cast<LPVOID>(ptr));
        if (Protection == PAGE_NOACCESS || Protection == PAGE_READONLY || Protection != PAGE_READWRITE)return false;
        return true;
    }

    void PrintMemoryProtection(uintptr_t ptr) {
        std::cout << "Memory protection of Object: " << GetMemoryProtection(reinterpret_cast<LPVOID>(ptr)) << "\n";
    }


    template<typename T>
    //32bit
    bool ReadMemSafe(DWORD ptr, T& Value) {
        if (ptr >= 0 || ptr <= 0x10000 || GetMemoryProtection(reinterpret_cast<LPVOID>(ptr)) == PAGE_NOACCESS)return false;
        DWORD d, ds;
        int val2;
        VirtualProtect((LPVOID)ptr, 4000, PAGE_EXECUTE_READWRITE, &d);
        Value = *(T*)(ptr);
        VirtualProtect((LPVOID)ptr, 4000, d, &ds);
        return true;
    }

    template<typename T1>
    bool WriteMemSafe(DWORD ptr, T1 const& Value) {
        if (ptr >= 0 || ptr <= 0x10000 || GetMemoryProtection(reinterpret_cast<LPVOID>(ptr)) == PAGE_NOACCESS)return false;
        DWORD d, ds;
        int val2;
        VirtualProtect((LPVOID)ptr, 4000, PAGE_EXECUTE_READWRITE, &d);
        *(T1*)(ptr) = Value;
        VirtualProtect((LPVOID)ptr, 4000, d, &ds);
        return true;
    }




    template<typename T3>

    bool ReadMemSafe64(uintptr_t ptr, T3& Value) {
        if (ptr < 0x10000000 || ptr > 0x7fffffffffff || GetMemoryProtection(reinterpret_cast<LPVOID>(ptr)) == PAGE_NOACCESS) return false;
        DWORD d, ds;
        int val2;
        VirtualProtect((LPVOID)ptr, sizeof(Value), PAGE_EXECUTE_READWRITE, &d);
        Value = *reinterpret_cast<T3*>(ptr);
        VirtualProtect((LPVOID)ptr, sizeof(Value), d, &ds);
        return true;
    }


    template<typename T4>

    bool WriteMemSafe64(uintptr_t ptr, T4 const& Value) {
        if (ptr < 0x10000000 || ptr > 0x7fffffffffff || GetMemoryProtection(reinterpret_cast<LPVOID>(ptr)) == PAGE_NOACCESS) return false;
        DWORD d, ds;
        int val2;
        VirtualProtect((LPVOID)ptr, sizeof(Value), PAGE_EXECUTE_READWRITE, &d);
        *reinterpret_cast<T4*>(ptr) = Value;
        VirtualProtect((LPVOID)ptr, sizeof(Value), d, &ds);
        return true;
    }


    template<typename T8>
    void SetMem(uintptr_t address, T8 const& Value) {
        DWORD d, ds;
        int val2;
        VirtualProtect((LPVOID)address, 4000, PAGE_EXECUTE_READWRITE, &d);
        //val2 = (T8)(Value);
        //memset((LPVOID)address, val2, sizeof(T8));
        *(T8*)(address) = Value;
        VirtualProtect((LPVOID)address, 4000, d, &ds);
    }



    class CheatOptions
    {

    public:
        bool CheatActivated = true;
        volatile bool InGame = false;
        bool InfiniteStamina = false;
        bool SuperSpeed = false;
        bool StopServerSidedLocation = true;
        bool IniStopServerSidedLocation = false;
        bool InfSanity = true;
        bool Noclip = false;
        bool NoclipIni = false;
        bool DupeItems = false;
        bool DupeItemWindow = false;
        bool RenderItemLocations = true;
        bool RenderEnemies = true;
        bool ShowAllActors = false;
        bool DeleteActor = false;
        bool UnDoActor = false;
        bool ActionCall = false;
        bool ActorTpTO = false;
        bool ActorBringTo = false;
        bool isFlyHackEnabled = false;
        bool FlyHack = false;
        bool PickUpItem = false;
        std::string Collision = "Collision unknown";
        bool GetItemList = false;
        bool WaterMark = true;
        bool WasFlying = false;
        bool SuperFlashlight = false;
        bool Box = false;
        bool MainStuff = false;
        std::string CurrentLevelName = "NONE";
        std::string FilterFr = "NONE";
        std::string Filter = "NONE";
        std::string ItemName = "";
        int DupeItem = 0;
        float TickPlayer = 0.0f;
        float FlyHigh = 5.0f;
        float SpeedBf = 0.0f;
    };
    CheatOptions Cheato;


    namespace SpecialFunctions {


        bool isWithinRadius(float x1, float y1, float x2, float y2, float radius) {
            if (x2 < 0.0f || y2 < 0.0f)return false;
            float distance = sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
            return distance <= radius;
        }



        std::string GetString(uintptr_t AddrOfString, byte type) {

            if (AddrOfString == 0)return "";

            uintptr_t NameAddr;
            int StringLength = 0;
            char tf1;
            std::string Name;


            if (type == 1) {
                ReadMemSafe64((AddrOfString + 0xE0), AddrOfString);
                if (AddrOfString == 0)return "";
            }
            //
            ReadMemSafe64((AddrOfString + 0x10), StringLength);

            NameAddr = AddrOfString + 0x14;


            for (size_t i = 0; i < StringLength; i++)
            {
                if (i == 0) {
                    ReadMemSafe64(NameAddr, tf1);
                    Name = tf1;
                    continue;
                }

                ReadMemSafe64(NameAddr + 0x2 * i, tf1);

                if (tf1 == 0) {
                    ReadMemSafe64(NameAddr + 0x2 * i + 1, tf1);
                    if (tf1 == 0)break;
                }

                Name = Name + tf1;
                continue;
            }



            return Name;
        }


        void setString(uintptr_t AddrOfString, byte type, std::string Beta = "") {
            if (AddrOfString == 0)return;

            uintptr_t NameAddr;
            int StringLength = 0;
            char tf1;
            std::string Name;


            if (type == 1) {
                ReadMemSafe64((AddrOfString + 0xE0), AddrOfString);
                if (AddrOfString == 0)return;
            }
            //

            int Legtnth = Beta.length();
            WriteMemSafe64((AddrOfString + 0x10), Legtnth);

            NameAddr = AddrOfString + 0x14;
            uintptr_t drf = (uintptr_t)Beta.c_str();
            for (size_t i = 0; i < Legtnth; i++)
            {
                char tem = 0;

                if (i == 0) {
                    ReadMemSafe64((drf), tem);
                    WriteMemSafe64((NameAddr), tem);
                }


                ReadMemSafe64((drf + i), tem);
                WriteMemSafe64((NameAddr + 0x2 * i), tem);
            }


        }

        std::string GetString2(uintptr_t AddrOfString, byte type, int Lenght) {

            if (AddrOfString == 0)return "";

            uintptr_t NameAddr;
            char tf1;
            std::string Name;
            if (type == 1) {
                ReadMemSafe64((AddrOfString + 0xE0), AddrOfString);
                if (AddrOfString == 0)return "";
            }
            //

            NameAddr = AddrOfString + 0x14;


            for (size_t i = 0; i < Lenght; i++)
            {
                if (i == 0) {
                    ReadMemSafe64(NameAddr, tf1);
                    Name = tf1;
                    continue;
                }

                ReadMemSafe64(NameAddr + 0x2 * i, tf1);

                if (tf1 == 0) {
                    ReadMemSafe64(NameAddr + 0x2 * i + 1, tf1);
                    if (tf1 == 0)break;
                }

                Name = Name + tf1;
                continue;
            }



            return Name;
        }

        std::string getIP(uintptr_t UdpEndPoint1) {
            if (UdpEndPoint1 == 0)return "";
            uintptr_t tmpad;
            std::string ip1;
            UINT PackedIP;
            BYTE bytes[5];
            ReadMemSafe64(UdpEndPoint1, bytes[0]);
            ReadMemSafe64((UdpEndPoint1 + 0x1), bytes[1]);
            ReadMemSafe64((UdpEndPoint1 + 0x2), bytes[2]);
            ReadMemSafe64((UdpEndPoint1 + 0x3), bytes[3]);
            //ReadMemSafe64((tmpad), ip);


            ip1 = "IP:" + std::to_string(bytes[0]) + "." + std::to_string(bytes[1]) + "." + std::to_string(bytes[2]) + "." + std::to_string(bytes[3]);
            return ip1;
        }

    }




    ///////////////////////////////////////////////////////////

    float windowWidth, windowHeight;


    namespace Functions {

        LRESULT(__fastcall* WndProcfunc)(const HWND, UINT, WPARAM, LPARAM);
        LRESULT __stdcall WndProcfunc_hook(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

            Cheat::Vals::hWnd = hWnd;
            Cheat::Vals::uMsg = uMsg;
            Cheat::Vals::wParam = wParam;
            Cheat::Vals::lParam = lParam;

            if (Cheat::GetVals == false) {
                Cheat::GetVals = true;

                GetWindowRect(hWnd, &Cheat::Dawg);
                //std::cout << "Window Pos: y: " << Cheat::Dawg.bottom << " x: " << Cheat::Dawg.left << Cheat::Dawg.right << " y1: " << Cheat::Dawg.top << " x1: " <<  " \n";
                Cheat::Start = true;

                //place = GameHandle + 0x14967B0;
                //place1 = *(uintptr_t*)(GameHandle + 0x27F9AA0);
            }

            ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);
            std::cout << "Function called \n";

            return CallWindowProc(oWndProc, Cheat::Vals::hWnd, Cheat::Vals::uMsg, Cheat::Vals::wParam, Cheat::Vals::lParam);
        }


        //////////////////////////////////////////////////////////////

    }



    namespace ImportantShit {

        int ActorsCurrent = 0x0;
        SDK::ABPCharacter_Demo_C* FoPtr = {};
        SDK::AMP_PS_C* playerStatePremium = {};
        SDK::FVector LocalPlayerPos = { 0,0,0 };
        volatile float SanityMaier = 0.0f;
        float hight = 0.0f;
        float heightZ = 0.0f;

        std::string Vector3DToString(float x, float y, float z) {
            return std::to_string((double)x) + "," + std::to_string((double)z) + "," + std::to_string((double)y);
        }
    }



    class ShitNeeded
    {
    public:
        SDK::UEngine* Engine = 0x0;
        SDK::UKismetMathLibrary* MathLib = 0x0;
        SDK::UKismetSystemLibrary* SystemLib = 0x0;
        SDK::UGameplayStatics* GPStatics = 0x0;
        SDK::APlayerController* PlayerController = 0x0;

        uintptr_t FName;
    }; ShitNeeded ShitNeeded1;



    uintptr_t GameHandle;

    uintptr_t ErrorCheck = 0x0;

/*

    bool SDK::UObject::HasTypeFlag(EClassCastFlags TypeFlag) const {
        if (TypeFlag == EClassCastFlags::None)return true;
        return TypeFlag == Class->CastFlags;
    }




    std::string SDK::UObject::GetName() const {
        return  Name.ToString();
    }

    std::string SDK::UObject::GetFullName() const {
        return Name.GetRawString();
    }
    */

    void InitGObjects()
    {
        SDK::UObject::GObjects = reinterpret_cast<SDK::TUObjectArray*>(uintptr_t(GetModuleHandle(0)) + Offsets::GObjects);
    }
        

    void DefineHooks();
    

    uintptr_t GetAdressStatic(const char* Sig, const char* Mask) {
        DWORD SizeOfProc = 0x0;
        HANDLE ProcHandle = OpenProcess(PROCESS_ALL_ACCESS, 0, GetCurrentProcessId());
        SigShit::GetProcessMemoryInfoe(ProcHandle, &SizeOfProc);



        uintptr_t Pointer = 0x0;
        ParameterShit64 Shit = { ProcHandle, (uintptr_t)GetModuleHandle(0), SizeOfProc,Sig, Mask, &Pointer};
        Shit.EditAddrs = 0x2;
        Shit.ReadMem = true;
        Shit.MinusBase = false;
        SigShit::ReadAddrbackwards = true;
        SigShit::DebugPrint64(Shit);
        SigShit::ReadAddrbackwards = false;
        return Pointer;
    }







	void IniValues() {
        ::ShowWindow(GetConsoleWindow(), SW_SHOW);
		GameHandle = (uintptr_t)GetModuleHandle(0);
        SDK::FName::InitGNames();
        InitGObjects();


        //UEngine  \x48\x8b\x05\x00\x00\x00\x00\x48\x8b\x88\x00\x00\x00\x00\x48\x85\xc9\x74\x00\x48\x8b\x01\xff\x50\x00\x84\xc0\x74\x00\xb3 xxx????xxx????xxxx?xxxxx?xxx?x v4.27
        ReadMemSafe64(GetAdressStatic("\x48\x8b\x05\x00\x00\x00\x00\x48\x8b\x88\x00\x00\x00\x00\x48\x85\xc9\x74\x00\x48\x8b\x01\xff\x50\x00\x84\xc0\x74\x00\xb3", "xxx????xxx????xxxx?xxxxx?xxx?x"), ShitNeeded1.Engine);


        ShitNeeded1.MathLib = (SDK::UKismetMathLibrary*)SDK::UObject::FindClassFast("KismetMathLibrary")->DefaultObject;

        ShitNeeded1.SystemLib = (SDK::UKismetSystemLibrary*)SDK::UObject::FindClassFast("KismetSystemLibrary")->DefaultObject;


        ShitNeeded1.GPStatics = (SDK::UGameplayStatics*)SDK::UObject::FindClassFast("GameplayStatics")->DefaultObject;




        if (ShitNeeded1.MathLib == nullptr) {
            std::cout << "Couldnt Find " << "KismetMathLibrary" << "\n";
        }
        else
        {
            std::cout << "KismetMathLibrary: " << std::hex << ShitNeeded1.MathLib << "\n";
        }

        if (ShitNeeded1.SystemLib == nullptr) {
            std::cout << "Couldnt Find " << "KismetSystemLibrary" << "\n";
        }
        else
        {
            std::cout << "SystemLib: " << std::hex << ShitNeeded1.SystemLib << "\n";
        }


        if (ShitNeeded1.GPStatics == nullptr) {
            std::cout << "Couldnt Find " << "GameplayStatics" << "\n";
        }
        else
        {
            std::cout << "GameplayStatics: " << std::hex << ShitNeeded1.GPStatics << "\n";
        }

        if (!ShouldUsePointer64((uintptr_t)ShitNeeded1.Engine))std::cout << "ptr not found of Engine -_-\n";

        //ReadMemSafe64((GameHandle + ), ShitNeeded1.Engine);
        ReadMemSafe64((GameHandle + Offsets::GNames), ShitNeeded1.FName);

		DefineHooks();

        std::cout << "Look at the Console when turning stuff on since there will be standing controls in the Console when turning stuff on\n";
	}




using namespace Cheat::ImportantShit;



SDK::FName Empty;
SDK::FName Use;
bool ItemShow = false;
bool GeneratedList = true;
bool IniList = false;



class BufferSize {
public:
    char s[128]; // Increased buffer size
};

BufferSize BufferSize1;

class Items {
public:
    const char* ItemName = "";
    SDK::FName ItemFname;
};

Items Thing[3];

int Index = 0;
int FNameNumber = 0;
HANDLE CurrentThreadf = nullptr;

//NOT FINISHED FINISH LATER OR SO NEGRO
int FindFName(const char* Name) {
    uintptr_t FNameAddress = GameHandle + Offsets::GNames;
    uintptr_t Offsets = 0x0;
    SDK::FName Current;

    while (ShouldUsePointer64(FNameAddress + Offsets))
    {
        ReadMemSafe64((FNameAddress + Offsets), Current);

        Offsets += 0x8;
    }
}


void ItemListPick() {

    /*
    if (GeneratedList == true && CurrentThreadf != 0x0) {
        
        CloseHandle(CurrentThreadf);
        CurrentThreadf = 0x0;
    }

    if (IniList == false) {
        IniList = true;
        CurrentThreadf = CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)GenerateList, 0, 0, nullptr);
    }
    */
    if (Cheato.InGame == false)return;
    
        imGuiDrawShit::BlackMamba();
        ImGui::SetNextWindowSize(ImVec2(WindowSizePlylist.x, WindowSizePlylist.y));
        ImGui::Begin("Item List", &Cheat::Cheato.DupeItemWindow, iw4.window_flags);
        {
            
            if (GeneratedList == true) {
                if (ImGui::Button("Empty Inv") == true) {
                    for (size_t i = 1; i < 12; i++)
                    {
                        playerStatePremium->Items_Rep[i] = Empty;
                    }
                }
                ImGui::Spacing();
                if (ImGui::Button("Show/Hide Items giveable") == true) {
                    ItemShow = !ItemShow;
                }
                if (ItemShow == true) {
                    if (ImGui::Button("Give EnergyBar in Chosen spot") == true) {
                        
                    }
                    ImGui::Spacing();
                    if (ImGui::Button("Give Jelly in Chosen spot") == true) {

                    }
                    ImGui::Spacing();
                    if (ImGui::Button("Give Juice in Chosen spot") == true) {
                        for (size_t i = 0; i < 3; i++)
                        {
                            if (Thing[Index].ItemName == "Juice") {
                                Use.ComparisonIndex = Thing[Index].ItemFname.ComparisonIndex;
                                break;
                            }
                        }
                    }
                    ImGui::Spacing();
                    if (ImGui::Button("Give Thermometer in Chosen spot") == true) {

                    }
                    ImGui::Spacing();
                    if (ImGui::Button("Give Almondwater in Chosen spot") == true) {
                        for (size_t i = 0; i < 3; i++)
                        {
                            if (Thing[Index].ItemName == "Almondwater") {
                                Use.ComparisonIndex = Thing[Index].ItemFname.ComparisonIndex;
                                break;
                            }
                        }
                    }
                    ImGui::Spacing();
                    if (ImGui::Button("Give Bugspray in Chosen spot") == true) {

                    }
                    ImGui::Spacing();
                    if (ImGui::Button("Give Flashlight in Chosen spot") == true) {
                        for (size_t i = 0; i < 3; i++)
                        {
                            if (Thing[Index].ItemName == "Flashlight") {
                                Use.ComparisonIndex = Thing[Index].ItemFname.ComparisonIndex;
                                break;
                            }
                        }
                    }
                    ImGui::Spacing();
                    if (ImGui::Button("Give LiquidPain in Chosen spot") == true) {

                    }
                }


                ImGui::Spacing();
                for (size_t i = 0; i < 12; i++)
                {
                    std::string Item = playerStatePremium->Items_Rep[i].ToString() + " Index:" + std::to_string(playerStatePremium->Items_Rep[i].ComparisonIndex) + " Number:" + std::to_string(playerStatePremium->Items_Rep[i].Number);
                    if (Item != "") {
                        std::string nd = "Dupe Item:" + std::to_string(i);
                        if (ImGui::Button(nd.c_str()) == true) {
                            Cheato.DupeItems = true;
                            Cheato.DupeItem = i;
                        }
                        if (ItemShow == true) {
                            std::string nd = "Give Item picked " + std::to_string(i);
                            if (ImGui::Button(nd.c_str()) == true) {
                                std::cout << "Index given?" << std::to_string(Use.ComparisonIndex) << "\n";
                                playerStatePremium->Items_Rep[i].ComparisonIndex = Use.ComparisonIndex;
                            }
                        }
                        ImGui::SameLine();
                        ImGui::Text(Item.c_str());
                    }
                }
            }
            else
            {
                GeneratedList = true;
                std::string Li = "Waiting for List to end:" + std::to_string(FNameNumber);
                ImGui::Text(Li.c_str());
            }

            
        }
        ImGui::End();
        ImGui::StyleColorsDark();
        MrWhite = { 255,255,255,255 };
       
}


template<typename TF>
void EmptyArray(TF *Arrayptr) {

    SDK::TArray<uintptr_t> Array;

    if (!ReadMemSafe64((uintptr_t)Arrayptr, Array))return;

    for (size_t i = 0; i < Array.Num(); i++)
    {
        Array[i] = 0x0;
    }
}




SDK::TArray<SDK::AActor*> Items_Down;
SDK::TArray<SDK::AActor*> Enemies_There;
SDK::TArray<SDK::AActor*> Flashlights;
SDK::TArray<SDK::AActor*> EmptyActors;
SDK::TArray<SDK::AActor*> Actorinas;

namespace HookeFunctions {
    typedef void(__stdcall* AFancyCharacterKillPlayer)(uintptr_t AFancyCharacter);
    AFancyCharacterKillPlayer AFancyCharacterKillPlayerTarget;
    AFancyCharacterKillPlayer AFancyCharacterKillPlayerOriginal;
    
    bool invincible = true;

    void __stdcall AFancyCharacterKillPlayer_Hook(uintptr_t AFancyCharacter) {
       
        if (invincible) {
            std::cout << "Tried to kill Player!\n";
            return;
        }


        return AFancyCharacterKillPlayerOriginal(AFancyCharacter);
    }




    typedef void (__stdcall* ABPCharacter_Demo_CPlayJumpScare)(uintptr_t ABPCharacter_Demo_C, class SDK::ULevelSequence* Sequence, class SDK::AActor* Entity, const struct SDK::FMovieSceneObjectBindingID& EntityBinding, const struct SDK::FMovieSceneObjectBindingID& CameraBinding);
    ABPCharacter_Demo_CPlayJumpScare ABPCharacter_Demo_CPlayJumpScareTarget;
    ABPCharacter_Demo_CPlayJumpScare ABPCharacter_Demo_CPlayJumpScareOriginal;



    void __stdcall ABPCharacter_Demo_CPlayJumpScare_Hook(uintptr_t ABPCharacter_Demo_C, class SDK::ULevelSequence* Sequence, class SDK::AActor* Entity, const struct SDK::FMovieSceneObjectBindingID& EntityBinding, const struct SDK::FMovieSceneObjectBindingID& CameraBinding) {

        /*bool EntityJumpScare = false;
        

        for (size_t i = 0; i < Enemies_There->Num(); i++)
        {
            if (Enemies_There[0][i] == Entity && Enemies_There[0][i] != Cheat::ShitNeeded1.PlayerController->Character) {
                EntityJumpScare = true;
                break;
            }
        }

        if (invincible && EntityJumpScare) {
            std::cout << "Tried to PlayJumpScare\n";
            return;
        }
        */

        
        return ABPCharacter_Demo_CPlayJumpScareOriginal(ABPCharacter_Demo_C, Sequence, Entity, EntityBinding, CameraBinding);
    }



}

   void DefineHooks() {


     //MH_CreateHook(HookeFunctions::AFancyCharacterKillPlayerTarget, )

       SDK::ABPCharacter_Demo_C* PrePointer = (SDK::ABPCharacter_Demo_C*)SDK::UObject::FindClassFast("BPCharacter_Demo_C")->DefaultObject;

       /*
       FoPtr->Class->GetFunction("BPCharacter_Demo_C", "PlayJumpScare")->ExecFunction;
       FoPtr->Class->GetFunction("FancyCharacter", "KillPlayer")->ExecFunction;
       */



     HookeFunctions::AFancyCharacterKillPlayerTarget = (HookeFunctions::AFancyCharacterKillPlayer)((uintptr_t)PrePointer->Class->GetFunction("FancyCharacter", "KillPlayer")->ExecFunction);

     MH_CreateHook(reinterpret_cast<LPVOID*>(HookeFunctions::AFancyCharacterKillPlayerTarget), &HookeFunctions::AFancyCharacterKillPlayer_Hook, reinterpret_cast<LPVOID*>(&HookeFunctions::AFancyCharacterKillPlayerOriginal));

    // MH_EnableHook(reinterpret_cast<LPVOID*>(HookeFunctions::AFancyCharacterKillPlayerTarget));


     

     HookeFunctions::ABPCharacter_Demo_CPlayJumpScareTarget = (HookeFunctions::ABPCharacter_Demo_CPlayJumpScare)((uintptr_t)PrePointer->Class->GetFunction("BPCharacter_Demo_C", "PlayJumpScare")->ExecFunction);

     MH_CreateHook(reinterpret_cast<LPVOID*>(HookeFunctions::ABPCharacter_Demo_CPlayJumpScareTarget), &HookeFunctions::ABPCharacter_Demo_CPlayJumpScare_Hook, reinterpret_cast<LPVOID*>(&HookeFunctions::ABPCharacter_Demo_CPlayJumpScareOriginal));

    // MH_EnableHook(reinterpret_cast<LPVOID*>(HookeFunctions::ABPCharacter_Demo_CPlayJumpScareTarget));


    

     std::cout << "Project shit to shit: " << std::hex << ShitNeeded1.GPStatics->Class->GetFunction("GameplayStatics", "ProjectWorldToScreen")->ExecFunction << "\n";
     
     //std::cout << "OpenLevel shit: " << std::hex << ShitNeeded1.GPStatics->Class->GetFunction("GameplayStatics", "OpenLevel")->ExecFunction << "\n";

     std::cout << "Hooked all Functions\n";

   }



SDK::FName SkinNegro;



bool HookedFF = false;
bool IngameFr = false;
SDK::ULevel* LastLevel = 0x0;

uintptr_t Levelbefore = 0x0;


	std::time_t end_time;
	auto end = std::chrono::system_clock::now();
	std::string Timea;
	HWND ConsoleHandle;
	void CheatMainRun() {
		if (IniCheat == false) {
			IniCheat = true;
			TickFirst = GetTickCount64();
			TickLast = TickFirst + TimeFunctionsCalled;
			TickFirst9 = GetTickCount64();
			TickLast9 = TickFirst9 + 500;
			Spotify::GetCurrentSong();
			Spotify::DebugMode = false;


			
		}

		if (ConsoleHandle == 0) {
			ConsoleHandle = GetConsoleWindow();
		}
		else
		{
			SetWindowPos(ConsoleHandle, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
		}




		TickFirst9 = GetTickCount64();
		if (TickFirst9 >= TickLast9) {
			TickLast9 = TickFirst9 + 500;
			Spotify::GetCurrentSong();
			end = std::chrono::system_clock::now();
			end_time = std::chrono::system_clock::to_time_t(end);
			Timea = (std::string)std::ctime(&end_time);
	
		}

		TickFirst = GetTickCount64();
        if (TickFirst >= TickLast) {
            TickLast = TickFirst + TimeFunctionsCalled;
           // Limit the Times the Functions get Called Performance Reasons and shit
            SDK::UGameViewportClient* GameViewPortShit = 0x0;

            if (ShouldUsePointer64((uintptr_t)ShitNeeded1.Engine)) {
                GameViewPortShit = ShitNeeded1.Engine->GameViewport;

                Cheato.InGame = false;
                if (GameViewPortShit->World != nullptr) {
                    Cheat::Cheato.CurrentLevelName = ShitNeeded1.GPStatics->GetCurrentLevelName(GameViewPortShit->World, true).ToString();
                }


                LocalPlayerPos = { 0,0,0 };

                if ((uintptr_t)GameViewPortShit->GameInstance != 0x0) {


                    if ((uintptr_t)GameViewPortShit->GameInstance->LocalPlayers[0] != 0x0) {



                        if ((uintptr_t)GameViewPortShit->GameInstance->LocalPlayers[0]->PlayerController != 0x0) {




                            if ((uintptr_t)GameViewPortShit->GameInstance->LocalPlayers[0]->PlayerController->AcknowledgedPawn != 0x0) {

                                Cheato.InGame = true;

                                LocalPlayerPos = GameViewPortShit->GameInstance->LocalPlayers[0]->PlayerController->AcknowledgedPawn->RootComponent->RelativeLocation;
                            }
                        }
                    }
                }
            }
            

           


          
            if (Cheato.InGame == true) {
                

                 bool UseStamina = false;
                SDK::ACharacter* Character1 = GameViewPortShit->GameInstance->LocalPlayers[0]->PlayerController->Character;
                ShitNeeded1.PlayerController = GameViewPortShit->GameInstance->LocalPlayers[0]->PlayerController;
                if (Character1 != 0x0) {

                    Cheato.Collision = "[*] Collision ";
                    if (Character1->GetActorEnableCollision()) {
                        Cheato.Collision += "On";
                    }
                    else
                    {
                        Cheato.Collision += "Off";
                    }
                    


                    SDK::APlayerState* ppstate = Character1->PlayerState;


                    FoPtr = (SDK::ABPCharacter_Demo_C*)Character1;

                    playerStatePremium = (SDK::AMP_PS_C*)ppstate;


                    if (Cheato.InfiniteStamina == true) {
                        FoPtr->ShouldUseStamina = 0;
                    }
                    else
                    {
                        FoPtr->ShouldUseStamina = 1;
                    }

                    LastLevel = ShitNeeded1.Engine->GameViewport->World->PersistentLevel;
                    

                    if (Cheato.StopServerSidedLocation == true) {
                        if (Cheato.IniStopServerSidedLocation == false) {
                            Cheato.IniStopServerSidedLocation = true;
                            Cheato.TickPlayer = playerStatePremium->PrimaryActorTick.TickInterval;
                        }
                        playerStatePremium->PrimaryActorTick.TickInterval = 1000.0f;
                    }
                    else
                    {
                        if (Cheato.IniStopServerSidedLocation == true) {
                            Cheato.IniStopServerSidedLocation = false;
                            playerStatePremium->PrimaryActorTick.TickInterval = Cheato.TickPlayer;
                        }
                    }


                    Cheat::ShitNeeded1.GPStatics->GetAllActorsOfClass(Cheat::ShitNeeded1.Engine->GameViewport->World, SDK::AActor::StaticClass(), &Actorinas);

                    Cheat::ShitNeeded1.GPStatics->GetAllActorsOfClass(Cheat::ShitNeeded1.Engine->GameViewport->World, SDK::ABP_DroppedItem_C::StaticClass(), &Cheat::Items_Down);

                    Cheat::ShitNeeded1.GPStatics->GetAllActorsOfClass(Cheat::ShitNeeded1.Engine->GameViewport->World, SDK::ACharacter::StaticClass(), &Cheat::Enemies_There);

                    //FoPtr->SetSprintSpeedServer(1000.0f);

                    //FoPtr->SprintSpeed = 1000.0f;
                   

                    
                    Cheat::ShitNeeded1.GPStatics->GetAllActorsOfClass(Cheat::ShitNeeded1.Engine->GameViewport->World, SDK::ABP_Item_Flashlight_C::StaticClass(), &Cheat::Flashlights);


                   
                    if (Cheato.SuperFlashlight && ShouldUsePointer64((uintptr_t)&Cheat::Flashlights)) {

                            for (size_t i = 0; i < Cheat::Flashlights.Num(); i++)
                            {
                                SDK::ABP_Item_Flashlight_C* CurrentActor = (SDK::ABP_Item_Flashlight_C*)Cheat::Flashlights[i]; 
                                if (Cheat::ShouldUsePointer64((uintptr_t)CurrentActor) || Cheat::ShouldUsePointer64((uintptr_t)CurrentActor->Vft)) {
                                    CurrentActor->Flashlight->SetIntensity(10000.0f);
                                    CurrentActor->Flashlight->SetOuterConeAngle(10000.0f);
                                    CurrentActor->Flashlight->SetInnerConeAngle(10000.0f);
                                    CurrentActor->Flashlight->SetSourceRadius(10000.0f);
                                    CurrentActor->Flashlight->SetLightFalloffExponent(0.0001f);
                                    CurrentActor->Flashlight->SetSourceLength(10000.0f);
                                }

                            }
                    }


                    

                    SanityMaier = playerStatePremium->Sanity;


                    if (Cheato.InfSanity == true) {
                        if (playerStatePremium->Sanity <= 20.0f)playerStatePremium->Sanity = 100.0f;
                        playerStatePremium->ShouldLowerSanity = 0;
                        playerStatePremium->MaxSanity = 1000.0f;

                        
                    }
                    else
                    {
                        playerStatePremium->ShouldLowerSanity = 1;
                    }


                    if (Cheato.DupeItems == true) {
                        Cheato.DupeItems = false;

                        for (size_t i = 0; i < 12; i++)
                        {
                            playerStatePremium->Items_Rep[i] = playerStatePremium->Items_Rep[Cheato.DupeItem];
                        }
                    }
                   


                    //FoPtr->CharacterMovement->ServerCorrectionRootMotion = FoPtr->CharacterMovement->CurrentRootMotion;
                }


            }



			if (GetAsyncKeyState(VK_NUMPAD1) & 1) {
				HWND tmp = GetConsoleWindow();
				ShowConWin = !ShowConWin;
				switch (ShowConWin)
				{
				case 0:
					::ShowWindow(tmp, SW_HIDE);
					ConsoleHandle = GetConsoleWindow();
					break;


				case 1:
					::ShowWindow(tmp, SW_SHOW);
					ConsoleHandle = GetConsoleWindow();
					break;
				default:
					break;
				}
			}

            

			if (LightSkin == true) {
				if (LightSkinini == false) {
					ImGui::StyleColorsLight();
					MrWhite = { 0,0,0,255 };
					LightSkinini = true;
				}
			}
			else
			{
				if (LightSkinini == true) {
					ImGui::StyleColorsDark();
					MrWhite = { 255,255,255,255 };
					LightSkinini = false;
				}
			}
		}
	}


#define NOP 0x90
	void nop_(PVOID address, int bytes) {
		DWORD d, ds;
		VirtualProtect(address, bytes, PAGE_EXECUTE_READWRITE, &d);
		memset(address, NOP, bytes); VirtualProtect(address, bytes, d, &ds);
	}

	DWORD place;
	DWORD place1;

}


RGBA BoxColor = RedColor;
RGBA TextColor = MrWhite;

void DrawBoxOnActor(SDK::AActor* Actor, const char* Name) {

    SDK::FVector xLocation;
    SDK::FVector xBoxExtent;

    Actor->GetActorBounds(true, &xLocation, &xBoxExtent, false);


    SDK::FVector HeadLocation = { xLocation.X, xLocation.Y, xLocation.Z + (xBoxExtent.Z) };
    SDK::FVector FootLocation = { xLocation.X, xLocation.Y, xLocation.Z - (xBoxExtent.Z) };


    SDK::FVector2D HeadScreen;
    SDK::FVector2D FootScreen;

    Cheat::ShitNeeded1.GPStatics->ProjectWorldToScreen(Cheat::ShitNeeded1.PlayerController, HeadLocation, &HeadScreen, true);
    Cheat::ShitNeeded1.GPStatics->ProjectWorldToScreen(Cheat::ShitNeeded1.PlayerController, FootLocation, &FootScreen, true);

    const float height = abs(HeadScreen.Y - FootScreen.Y);

    const float width = height * 0.6f;



    imGuiDrawShit::DrawRect(HeadScreen.X - width / 2, HeadScreen.Y, width, height, &BoxColor, 2.0f);
    imGuiDrawShit::DrawNewText(HeadScreen.X, HeadScreen.Y + 15, &TextColor, Name);
}




DWORD TickFirst2, TickLast2, TickLast3;
LRESULT __stdcall WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);



DXGI_SWAP_CHAIN_DESC sdf;

HRESULT __stdcall hkPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
{
	if (!init)
	{
		if (SUCCEEDED(pSwapChain->GetDevice(__uuidof(ID3D11Device), (void**)&pDevice)))
		{
			MH_Initialize();
			pDevice->GetImmediateContext(&pContext);
			DXGI_SWAP_CHAIN_DESC sd;
			pSwapChain->GetDesc(&sd);
			window = sd.OutputWindow;
			ID3D11Texture2D* pBackBuffer;
			pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
			pDevice->CreateRenderTargetView(pBackBuffer, NULL, &mainRenderTargetView);
			pBackBuffer->Release();
			oWndProc = (WNDPROC)SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)WndProc);
			InitImGui();
			AllocConsole();
			HWND tmp = GetConsoleWindow();
		//	::ShowWindow(tmp, SW_HIDE);
			FILE* f = new FILE();
			freopen_s(&f, "CONOUT$", "w", stdout);
			freopen_s(&f, "CONIN$", "r", stdin);
			SetConsoleTitle(L"Escape The backwrooooms");
			::ShowWindow(tmp, SW_HIDE);
			Cheat::IniValues();
			iw4.window_flags = ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBringToFrontOnFocus;
			init = true;
		}

		else
			return oPresent(pSwapChain, SyncInterval, Flags);
	}

    pSwapChain->GetDesc(&sdf);
    GetWindowRect(sdf.OutputWindow, &Cheat::Dawg);
    Cheat::windowWidth = Cheat::Dawg.right;
    Cheat::windowHeight = Cheat::Dawg.bottom;

	if (Cheat::Cheatclosed == false) {
		if (GetAsyncKeyState(VK_INSERT) & 1) {
			if (Cheat::Tabs == 0)Cheat::Tabs = 1;
			else
			{
				Cheat::Tabs = 0;
			}
			//Cheat::ShowCheat = !Cheat::ShowCheat;
		}
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
		if (Cheat::ShowCheat == true) {
			if (Active == true) {
			Ce2:
				if (Cheat::Start == true) {
					Cheat::NotifyBox(0, "Successfully Injected                                      Press insert to Open Menu", 8000, &Cheat::Start);
				}

                if (GetAsyncKeyState(VK_DELETE) & 1)Cheat::StopDaCheat = true;

                if (Cheat::StopDaCheat) {
                    

                    if (Cheat::ShouldChangeColor)Cheat::OverwriteColor = RedColor;

                    if (Cheat::NotifyBox(0, "Unhooking stuff and cleaning up", 4000, &Cheat::Cheatclosed)) {
                        Active = false;
                        Cheat::Cheatclosed = true;
                    }
                }


				if (Spotify::SpotifyTb == true) {
					Cheat::NotifyBox(1, "Now Playing: " + Spotify::SongFirst, 1000, &Spotify::SpotifyTb);
				}

                bool RenderShit = false;
               

               


               



                //PlayerController
                


                if (Cheat::Cheato.MainStuff && Cheat::Cheato.InGame && Cheat::ShouldUsePointer64((uintptr_t)Cheat::ShitNeeded1.PlayerController) && Cheat::ShitNeeded1.GPStatics != nullptr && Cheat::ShouldUsePointer64((uintptr_t)Cheat::ShitNeeded1.Engine->GameViewport->World) && Cheat::ShitNeeded1.Engine->GameViewport->World->PersistentLevel == Cheat::LastLevel)
                {
                   

                    
                                            

                                            if (Cheat::Cheato.isFlyHackEnabled) {


                                                // Checks if the 'F' key is pressed if so turn shit on

                                                if (GetAsyncKeyState('G') & 1) {
                                                    Cheat::Cheato.FlyHack = !Cheat::Cheato.FlyHack;

                                                    if (!Cheat::Cheato.FlyHack) {
                                                        SDK::APawn* Pawnf = Cheat::ShitNeeded1.PlayerController->K2_GetPawn();
                                                        if (Pawnf != nullptr) {
                                                          Pawnf->SetActorEnableCollision(true);
                                                          Cheat::ShitNeeded1.PlayerController->Character->CharacterMovement->MovementMode = SDK::EMovementMode::MOVE_Falling;
                                                        }
                                                    }
                                                }


                                                // Check if the player controller and pawn are valid
                                                if (Cheat::Cheato.FlyHack == true && Cheat::ShitNeeded1.PlayerController && Cheat::ShitNeeded1.PlayerController->Class && Cheat::ShitNeeded1.PlayerController->K2_GetPawn() != nullptr && Cheat::ShitNeeded1.PlayerController->Character != nullptr && Cheat::ShitNeeded1.PlayerController->Character->CharacterMovement != nullptr) {
                                                    SDK::APawn* Pawnf = Cheat::ShitNeeded1.PlayerController->K2_GetPawn();



                                                    Cheat::Cheato.WasFlying = true;
                                                    Cheat::ShitNeeded1.PlayerController->Character->CharacterMovement->MovementMode = SDK::EMovementMode::MOVE_Flying;
                                                    SDK::FVector ForwardPos = Pawnf->GetActorForwardVector();
                                                    float MovementStep = 10.2f;



                                                    if (GetAsyncKeyState(VK_TAB) & 1) {
                                                        if (Pawnf->GetActorEnableCollision()) {
                                                            Pawnf->SetActorEnableCollision(false);
                                                        }
                                                        else
                                                        {
                                                            Pawnf->SetActorEnableCollision(true);
                                                        }
                                                    }



                                                    // Check if the 'W' key is pressed
                                                    if (GetAsyncKeyState(0x57) & 0x8000) {
                                                        
                                                        

                                                        SDK::FVector NewLocation = Pawnf->K2_GetActorLocation() + ForwardPos * MovementStep;


                                                        Pawnf->K2_SetActorLocation(NewLocation, false, 0, true);
                                                    }



                                                    


                                                    


                                                    if (GetAsyncKeyState(VK_SPACE)) {
                                                        SDK::FVector NewLocation = Pawnf->K2_GetActorLocation();
                                                        NewLocation.Z += Cheat::Cheato.FlyHigh;
                                                        Pawnf->K2_SetActorLocation(NewLocation, false, 0, true);

                                                    }


                                                    if (GetAsyncKeyState(VK_SHIFT)) {
                                                        SDK::FVector NewLocation = Pawnf->K2_GetActorLocation();
                                                        NewLocation.Z -= Cheat::Cheato.FlyHigh;
                                                        Pawnf->K2_SetActorLocation(NewLocation, false, 0, true);
                                                    }
                                                }
                                            }
                                            else
                                            {
                                                if (Cheat::Cheato.WasFlying) {
                                                    Cheat::Cheato.WasFlying = false;
                                                    if (Cheat::ShitNeeded1.PlayerController->K2_GetPawn() != nullptr && !Cheat::ShitNeeded1.PlayerController->K2_GetPawn()->GetActorEnableCollision()) {
                                                        Cheat::ShitNeeded1.PlayerController->K2_GetPawn()->SetActorEnableCollision(true);
                                                    }
                                                    Cheat::ShitNeeded1.PlayerController->Character->CharacterMovement->MovementMode = SDK::EMovementMode::MOVE_Falling;
                                                }
                                            }



                                            



                                            if (Cheat::Cheato.PickUpItem || Cheat::Cheato.GetItemList) {
                                                if (Cheat::ShouldUsePointer64((uintptr_t)&Cheat::Items_Down))
                                                {

                                                    if (Cheat::Cheato.GetItemList) {
                                                        std::vector<std::string> Items;

                                                        Items.reserve(1);

                                                        for (size_t i = 0; i < Cheat::Items_Down.Num(); i++)
                                                        {
                                                            SDK::ABP_DroppedItem_C* CurrentActor = (SDK::ABP_DroppedItem_C*)Cheat::Items_Down[i];
                                                            if (!Cheat::ShouldUsePointer64((uintptr_t)CurrentActor->Class->DefaultObject))break;
                                                            //CurrentActor->Children
                                                            if (!Cheat::ShouldUsePointer64((uintptr_t)CurrentActor) || !Cheat::ShouldUsePointer64((uintptr_t)CurrentActor->Class) || !Cheat::ShouldUsePointer64((uintptr_t)CurrentActor->Vft)) {
                                                                Cheat::Cheato.PickUpItem = false;
                                                                Cheat::Cheato.GetItemList = false;
                                                                for (size_t i = 0; i < Items.size(); i++)
                                                                {
                                                                    std::cout << "Item: " << Items[i] << "\n";
                                                                }
                                                                break;
                                                            }

                                                            if (CurrentActor != nullptr) {
                                                                bool FoundItemName = false;

                                                                std::string Name = CurrentActor->ID.ToString();

                                                                for (size_t i = 0; i < Items.size(); i++)
                                                                {

                                                                    if (Items[i] == Name) {
                                                                        FoundItemName = true;
                                                                        break;
                                                                    }
                                                                }

                                                                if (!FoundItemName)Items.push_back(Name);
                                                            }
                                                        }
                                                        for (size_t i = 0; i < Items.size(); i++)
                                                        {
                                                            std::cout << "Item: " << Items[i] << "\n";
                                                        }
                                                    }
                                                    else
                                                    {
                                                        for (size_t i = 0; i < Cheat::Items_Down.Num(); i++)
                                                        {
                                                            SDK::ABP_DroppedItem_C* CurrentActor = (SDK::ABP_DroppedItem_C*)Cheat::Items_Down[i];
                                                            //CurrentActor->Children
                                                            if (!Cheat::ShouldUsePointer64((uintptr_t)CurrentActor) || !Cheat::ShouldUsePointer64((uintptr_t)CurrentActor->Class) || !Cheat::ShouldUsePointer64((uintptr_t)CurrentActor->Vft)) {
                                                                break;
                                                            }
                                                            if (CurrentActor != nullptr) {



                                                                std::string Itemname = CurrentActor->ID.ToString();

                                                                if (Itemname.find(Cheat::Cheato.ItemName.c_str()) != std::string::npos) {
                                                                    SDK::ABPCharacter_Demo_C* FoPtr1 = (SDK::ABPCharacter_Demo_C*)Cheat::ShitNeeded1.PlayerController->Character;
                                                                    SDK::AMP_PS_C* ppstate = (SDK::AMP_PS_C*)Cheat::ShitNeeded1.PlayerController->PlayerState;


                                                                    
                                                                    //ppstate->Items_Re
                                                                    for (size_t i = 0; i < ppstate->Items_Rep.Num(); i++)
                                                                    {
                                                                        if (ppstate->Items_Rep[i].ComparisonIndex == 0) {
                                                                            FoPtr1->PickUp_SERVER(CurrentActor);
                                                                            ppstate->Items_Rep[i] = CurrentActor->ID;
                                                                            break;
                                                                        }
                                                                    }
                                                                }

                                                            }

                                                        }
                                                    }
                                                }
                                                Cheat::Cheato.PickUpItem = false;
                                                Cheat::Cheato.GetItemList = false;
                                            }






                                            if (Cheat::Cheato.RenderItemLocations) {
                                                // SDK::ABP_DroppedItem_C
                                               //SDK::TSubclassOf <SDK::AActor> Hanuutta = (SDK::UObject::FindClassFast("BP_DroppedItem_C"));
                                               //Items

                                                SDK::FVector2D Loc2d;
                                                if (Cheat::ShouldUsePointer64((uintptr_t)&Cheat::Items_Down))
                                                {

                                                    try // Pretty much unsafe so gotta handle shit
                                                    {
                                                        for (size_t i = 0; i < Cheat::Items_Down.Num(); i++)
                                                        {
                                                            SDK::ABP_DroppedItem_C* CurrentActor = (SDK::ABP_DroppedItem_C*)Cheat::Items_Down[i];
                                                            //CurrentActor->Children
                                                            if (!Cheat::ShouldUsePointer64((uintptr_t)CurrentActor) || !Cheat::ShouldUsePointer64((uintptr_t)CurrentActor->Class) || !Cheat::ShouldUsePointer64((uintptr_t)CurrentActor->Vft) || !Cheat::ShouldUsePointer64((uintptr_t)&CurrentActor->Class->DefaultObject)) {
                                                                break;
                                                            }
                                                            SDK::FVector WorldPos;
                                                            if (CurrentActor != nullptr) {
                                                                WorldPos = CurrentActor->K2_GetActorLocation();
                                                                Cheat::ShitNeeded1.GPStatics->ProjectWorldToScreen(Cheat::ShitNeeded1.PlayerController, WorldPos, &Loc2d, true);
                                                                if (Loc2d.X == 0.0f && Loc2d.Y == 0.0f)continue;
                                                                imGuiDrawShit::DrawCircle(Loc2d.X, Loc2d.Y, 10, &RedColor, 12);
                                                                std::string Itemname = CurrentActor->ID.ToString();
                                                                imGuiDrawShit::DrawNewText(Loc2d.X, Loc2d.Y + 10, &MrWhite, Itemname.c_str());
                                                            }

                                                        }
                                                    }
                                                    catch (const std::exception& msg)
                                                    {
                                                        std::cout << "Caught exception: " << msg.what() << "\n";
                                                        Cheat::Items_Down[0] = 0x0;
                                                    }

                                                }
                                            }




                                            bool WallTrace = true;

                                            if (Cheat::Cheato.RenderEnemies) {

                                                //Enemies Ai    SDK::AAIController



                                                SDK::FVector2D Loc2d; 
                                                if (Cheat::ShouldUsePointer64((uintptr_t)&Cheat::Enemies_There))
                                                {

                                                    //  SDK::AAIC_SkinStealer_C
                                                    try // Pretty much unsafe so gotta handle shit
                                                    {
                                                        for (size_t i = 0; i < Cheat::Enemies_There.Num(); i++)
                                                        {

                                                            SDK::ACharacter* CurrentActor = (SDK::ACharacter*)Cheat::Enemies_There[i];
                                                            if (CurrentActor == Cheat::ShitNeeded1.PlayerController->Character)continue;


                                                            if (!Cheat::ShouldUsePointer64((uintptr_t)CurrentActor) || !Cheat::ShouldUsePointer64((uintptr_t)CurrentActor->Class) || !Cheat::ShouldUsePointer64((uintptr_t)CurrentActor->Vft)) {
                                                                break;
                                                            }
                                                            if (!Cheat::ShouldUsePointer64((uintptr_t)CurrentActor->Class->DefaultObject))break;

                                                            SDK::FVector WorldPos;
                                                            if (CurrentActor != nullptr) {

                                                                std::string NameActor = "";
                                                                NameActor = CurrentActor->Name.ToString();
                                                                if (NameActor.find("BPCharacter_Demo_C") != std::string::npos) {
                                                                    SDK::ABPCharacter_Demo_C* Player = (SDK::ABPCharacter_Demo_C*)CurrentActor;
                                                                    SDK::AMP_PS_C* PlayerInfos = (SDK::AMP_PS_C*)CurrentActor->PlayerState;
                                                                    if(PlayerInfos != nullptr)NameActor = PlayerInfos->PlayerNamePrivate.ToString();
                                                                }


                                                                WorldPos = CurrentActor->K2_GetActorLocation();
                                                                Cheat::ShitNeeded1.GPStatics->ProjectWorldToScreen(Cheat::ShitNeeded1.PlayerController, WorldPos, &Loc2d, true);

                                                                if (Loc2d.X == 0.0f && Loc2d.Y == 0.0f)continue;

                                                                if (WallTrace) {
                                                                    CurrentActor->Mesh->SetRenderCustomDepth(true);
                                                                    CurrentActor->Mesh->CustomDepthStencilValue = 255;
                                                                }

                                                                if (Cheat::Cheato.Box) {
                                                                    DrawBoxOnActor(CurrentActor, NameActor.c_str());

                                                                }
                                                                else
                                                                {
                                                                    imGuiDrawShit::DrawCircle(Loc2d.X, Loc2d.Y, 10, &RedColor, 12);
                                                                    imGuiDrawShit::DrawNewText(Loc2d.X, Loc2d.Y + 10, &MrWhite, NameActor.c_str());
                                                                }

                                                            }
                                                        }
                                                    }
                                                    catch (const std::exception& msg)
                                                    {
                                                        std::cout << "Caught exception: " << msg.what() << "\n";
                                                        Cheat::Enemies_There[0] = 0x0;
                                                    }

                                                }

                                            }


                                            



                                            if (Cheat::Cheato.ShowAllActors) {
                                                /*bool DeleteStuff = false;
                                                if (GetAsyncKeyState(VK_LBUTTON) & 1) {
                                                    DeleteStuff = true;
                                                }*/
                                                SDK::FVector2D Loc2d;



                                                // Hanuutta = (SDK::UObject::FindClassFast("Actor"));
                                                
                                                 // Not any more slow :)
                                                if (Cheat::ShouldUsePointer64((uintptr_t)&Cheat::Actorinas))
                                                {

                                                    try // also unsafe asf
                                                    {
                                                        if (Cheat::Actorinas.IsValid()) {
                                                            for (size_t i = 0; i < Cheat::Actorinas.Num(); i++)
                                                            {
                                                                SDK::AActor* CurrentActor = (SDK::AActor*)Cheat::Actorinas[i];
                                                                SDK::FVector WorldPos;
                                                               if (Cheat::ShouldUsePointer64((uintptr_t)CurrentActor) && CurrentActor->Outer != nullptr) {// && Cheat::ShouldUsePointer64((uintptr_t)CurrentActor->Vft)) {

                                                                   if (CurrentActor->Name.ComparisonIndex <= 0)break;
                                                                    std::string NameActor = CurrentActor->Name.ToString();
                                                                    if (NameActor.find(Cheat::Cheato.Filter.c_str()) == std::string::npos)continue;

                                                                    WorldPos = CurrentActor->K2_GetActorLocation();
                                                                    Cheat::ShitNeeded1.GPStatics->ProjectWorldToScreen(Cheat::ShitNeeded1.PlayerController, WorldPos, &Loc2d, true);
                                                                    if (Cheat::Cheato.ActionCall) {
                                                                        if (Cheat::Cheato.DeleteActor) {
                                                                            CurrentActor->SetActorHiddenInGame(true);
                                                                            CurrentActor->SetActorEnableCollision(false);
                                                                        }
                                                                        else
                                                                        {
                                                                            if (Cheat::Cheato.UnDoActor) {
                                                                                CurrentActor->SetActorHiddenInGame(false);
                                                                                CurrentActor->SetActorEnableCollision(true);
                                                                            }
                                                                            else
                                                                            {
                                                                                if (Cheat::Cheato.ActorTpTO) {
                                                                                    Cheat::Cheato.ActorTpTO = false;
                                                                                    Cheat::ShitNeeded1.PlayerController->K2_GetPawn()->K2_SetActorLocation(WorldPos, false, 0, true);
                                                                                }
                                                                                else
                                                                                {
                                                                                    if (Cheat::Cheato.ActorBringTo) {
                                                                                        CurrentActor->K2_SetActorLocation(Cheat::ShitNeeded1.PlayerController->K2_GetPawn()->K2_GetActorLocation(), false, 0, true);
                                                                                    }
                                                                                }
                                                                            }
                                                                        }
                                                                    }

                                                                    if (Loc2d.X == 0.0f && Loc2d.Y == 0.0f)continue;




                                                                    imGuiDrawShit::DrawCircle(Loc2d.X, Loc2d.Y, 10, &RedColor, 12);

                                                                    imGuiDrawShit::DrawNewText(Loc2d.X, Loc2d.Y + 10, &MrWhite, NameActor.c_str());

                                                                }

                                                            }
                                                        }
                                                        
                                                    }
                                                    catch (const std::exception& msg)
                                                    {
                                                        std::cout << "Caught exception: " << msg.what() << "\n";
                                                        Cheat::Actorinas[0] = 0x0;
                                                    }

                                                    if (Cheat::Cheato.ActionCall) {
                                                        Cheat::Cheato.ActionCall = false;
                                                        Cheat::Cheato.DeleteActor = false;
                                                        Cheat::Cheato.UnDoActor = false;
                                                    }
                                                }
                                            }

                }

                

                if (Cheat::Cheato.WaterMark) {
                    if (Rainbowify(&Colorcooler.Color) == 0);

                    Cheat::Tempd215 = "Made by Senpai42";
                    int lf = Cheat::Tempd215.size() * 10;
                    Cheat::TempBus213 = Cheat::Tempd215.data();
                    imGuiDrawShit::DrawInSideText(Cheat::Dawg.right - lf, Cheat::height24, &Colorcooler.Color, Cheat::TempBus213);
                }





				if (Cheat::Rendershit::ShowRenderStuff == true) {


                    //TestHit
                    

					Cheat::Tempd215 = "Made by Senpai42";
					Cheat::TempBus213 = Cheat::Tempd215.data();
					imGuiDrawShit::DrawInSideText(80, Cheat::height24 + 150, &MrWhite, Cheat::TempBus213);
					Cheat::Size2 = 1;
					Cheat::Tempd215 = Cheat::Timea;
					Cheat::TempBus213 = Cheat::Tempd215.data();
					imGuiDrawShit::DrawInSideText(Cheat::windowWidth /2, 100, &MrWhite, Cheat::TempBus213);

					Cheat::Tempd215 = "Song Playing: " + Spotify::SongFirst ;
					Cheat::TempBus213 = Cheat::Tempd215.data();
					imGuiDrawShit::DrawInSideText(80, Cheat::height24 + 150 + 15 * Cheat::Size2, &MrWhite, Cheat::TempBus213);
					Cheat::Size2 = Cheat::Size2 + 1;

                    Cheat::Tempd215 = "Current Pos x,y,z: " ;
                    Cheat::TempBus213 = Cheat::Tempd215.data();
                    imGuiDrawShit::DrawInSideText(80, Cheat::height24 + 150 + 15 * Cheat::Size2, &MrWhite, Cheat::TempBus213);
                    Cheat::Size2 = Cheat::Size2 + 1;

                    Cheat::Tempd215 = Cheat::Cheato.Collision;
                    Cheat::TempBus213 = Cheat::Tempd215.data();
                    imGuiDrawShit::DrawInSideText(80, Cheat::height24 + 150 + 15 * Cheat::Size2, &MrWhite, Cheat::TempBus213);
                    Cheat::Size2 = Cheat::Size2 + 1;

                    
					Cheat::Tempd215 = "Pos x,y,z: " + Cheat::ImportantShit::Vector3DToString(Cheat::ImportantShit::LocalPlayerPos.X, Cheat::ImportantShit::LocalPlayerPos.Y, Cheat::ImportantShit::LocalPlayerPos.Z);
					Cheat::TempBus213 = Cheat::Tempd215.data();
					imGuiDrawShit::DrawInSideText(80, Cheat::height24 + 150 + 15 * Cheat::Size2, &MrWhite, Cheat::TempBus213);
					Cheat::Size2 = Cheat::Size2 + 1;


                    Cheat::Tempd215 = "Sanity: " + std::to_string(double(Cheat::ImportantShit::SanityMaier));
                    Cheat::TempBus213 = Cheat::Tempd215.data();
                    imGuiDrawShit::DrawInSideText(80, Cheat::height24 + 150 + 15 * Cheat::Size2, &MrWhite, Cheat::TempBus213);
                    Cheat::Size2 = Cheat::Size2 + 1;

                   
                    Cheat::Tempd215 = "Current Level: " + Cheat::Cheato.CurrentLevelName;
                    Cheat::TempBus213 = Cheat::Tempd215.data();
                    imGuiDrawShit::DrawInSideText(80, Cheat::height24 + 150 + 15 * Cheat::Size2, &MrWhite, Cheat::TempBus213);
                    Cheat::Size2 = Cheat::Size2 + 1;


				}

                if (Cheat::Cheato.DupeItemWindow == true) {
                    Cheat::ItemListPick();
                }

				//DrawCircle(int x, int y, int radius, RGBA * color, int segments)
				GetCursorPos(&Cheat::p);
				Cheat::p.y = Cheat::p.y + Cheat::HeightofMouse;
				if (Cheat::Tabs != 0) {
					imGuiDrawShit::DrawCircle((int)Cheat::p.x, (int)Cheat::p.y, 5.0f, &MrWhite, 12);

                    ImGui::SetNextWindowSize(iw.window_size);
                    ImGui::Begin(iw.window_title, &Active, iw.window_flags);
                    {
                        iw.WindowPos = ImGui::GetWindowPos();
                        imGuiDrawShit::DrawBackGroundGif(iw.WindowPos.x, iw.WindowPos.y, iw.window_size.x, iw.window_size.y);
                        if (ImGui::Button("Home page") == true) {
                            Cheat::Tabs = 1;
                        }
                        ImGui::SameLine();
                        if (ImGui::Button("Misc page") == true) {
                            Cheat::Tabs = 2;
                        }
                        ImGui::SameLine();
                        if (ImGui::Button("Settings page") == true) {
                            Cheat::Tabs = 3;
                        }
                        ImGui::SameLine();
                        if (ImGui::Button("Performance page") == true) {
                            Cheat::Tabs = 4;
                        }
                        ImGui::SameLine();
                        if (ImGui::Button("Aimbot and shit") == true) {
                            Cheat::Tabs = 5;
                        }
                        if (ImGui::Button("Spotify Shit (Not working)") == true) { // Spotify switched 2 to 3 months ago to 64bit and im to lazy to get the new code i wrote from another project and paste it here :)
                            Cheat::Tabs = 6;
                        }
                        ImGui::Separator();
                        imGuiDrawShit::DrawAboutMe();
                    }
				}
				if (GetAsyncKeyState(VK_F3) & 1) {
					Cheat::p.y = Cheat::p.y + Cheat::HeightofMouse;
					ImGui::SetNextWindowPos(ImVec2(Cheat::p.x, Cheat::p.y));
				}
				//imGuiDrawShit::DrawFilledRect(0, Cheat::Dawg.bottom - 150,400, 200, &CoolColor);
				switch (Cheat::Tabs) {
				case 1:
						ImGui::Spacing();
						ImGui::Checkbox("Debug Stuff", &Cheat::Rendershit::ShowRenderStuff);
						ImGui::Spacing();
                        ImGui::Checkbox("inf Stamina", &Cheat::Cheato.InfiniteStamina);
                        ImGui::Spacing();
                        ImGui::Checkbox("inf Sanity", &Cheat::Cheato.InfSanity);
                        ImGui::Spacing();
                        if (ImGui::Checkbox("FlyHack (Main has to be on)", &Cheat::Cheato.isFlyHackEnabled)) {
                            if (Cheat::Cheato.isFlyHackEnabled)std::cout << "[*] Flyhack on!\n Control: G = Turn on/off Flyhack, W = Forward, Space = Up, Shift = Down, Shift = Noclip on/off (Quick info: Collision resets when a new level is loaded)\n";
                        }
                        ImGui::Spacing();
                        ImGui::Checkbox("Super Flashlights", &Cheat::Cheato.SuperFlashlight);
                        ImGui::Spacing();
                        if (ImGui::Button("Snatch item from da World")) {
                            Cheat::Cheato.PickUpItem = true;
                        }
                        ImGui::Spacing();
                        if (ImGui::SmallButton("Get ItemList")) {
                            Cheat::Cheato.GetItemList = true;
                        }
                        ImGui::Spacing();
                        if (ImGui::Button("Dupe Item in your Hand") == true)Cheat::Cheato.DupeItemWindow = true;
                        ImGui::Spacing();
                        ImGui::InputText("Item Name:", (char*)Cheat::Cheato.ItemName.c_str(), sizeof(Cheat::Cheato.ItemName));
                        ImGui::Spacing();
                        if (Cheat::Cheato.isFlyHackEnabled) {
                            ImGui::SliderFloat("Fly hight", &Cheat::Cheato.FlyHigh, 0.01f, 10000.0f);
                            ImGui::Spacing();
                        }


						break;
				case 2:
                        ImGui::Checkbox("Activate Main (for Esp and Flyhack)", &Cheat::Cheato.MainStuff);
                        ImGui::Spacing();
                        ImGui::Checkbox("Item Esp", &Cheat::Cheato.RenderItemLocations);
                        ImGui::Spacing();
                        ImGui::Checkbox("Enemy Esp", &Cheat::Cheato.RenderEnemies);
                        ImGui::Spacing();
                        ImGui::Checkbox("Box Esp for Enemies", &Cheat::Cheato.Box);
                        ImGui::Spacing();
                        ImGui::Checkbox("Actors Esp (Unsafe may crash game ;) )", &Cheat::Cheato.ShowAllActors);               
                        ImGui::Spacing();
                        if (Cheat::Cheato.ShowAllActors)
                        {
                            ImGui::InputText("Actor Filter ", (char*)Cheat::Cheato.FilterFr.c_str(), sizeof(Cheat::Cheato.FilterFr));
                            if (ImGui::SmallButton("Trigger Filter")) {
                                Cheat::Cheato.Filter = Cheat::Cheato.FilterFr.c_str();
                            }
                            if (ImGui::SmallButton("Delete Actors")) {
                                Cheat::Cheato.DeleteActor = true;
                                Cheat::Cheato.ActionCall = true;
                            }
                            if (ImGui::SmallButton("Undelete Actors")) {
                                Cheat::Cheato.UnDoActor = true;
                                Cheat::Cheato.ActionCall = true;
                            }
                            if (ImGui::SmallButton("Goto Actor")) {
                                Cheat::Cheato.ActorTpTO = true;
                                Cheat::Cheato.ActionCall = true;
                            }
                            if (ImGui::SmallButton("Bring Actors")) {
                                Cheat::Cheato.ActorBringTo = true;
                                Cheat::Cheato.ActionCall = true;
                            }
                        }

					
					break;

				case 3:
                
						ImGui::SliderInt("Hight of Mouse Circle", &Cheat::HeightofMouse, -200, 2000, 0);
						ImGui::Spacing();
						ImGui::Checkbox("LightSkin?", &Cheat::LightSkin); //ShowAnimation
						ImGui::Spacing();
                        ImGui::Checkbox("Watermark", &Cheat::Cheato.WaterMark);
                        ImGui::Spacing();
						ImGui::Checkbox("Animation", &imGuiDrawShit::ShowAnimation);
						ImGui::Spacing();
					
					break;

				case 4:

						ImGui::SliderInt("Times Functions Called ( in ms )", &Cheat::TimeFunctionsCalled, 1, 2000, 0);
                        ImGui::Spacing();
						if (ImGui::Button("Test Spotify msg box") == true) {
							Spotify::SpotifyTb = true;
						}
						ImGui::Spacing();
					
					break;

				case 5:

					break;


				case 6:

						ImGui::Checkbox("Spotify mode", &Spotify::SpotifyMode);
						ImGui::Spacing();/*
						ImGui::Checkbox("Lyrics And shit", &Cheat::Active4);
						ImGui::Spacing();
						if (ImGui::Button("No Ads (coming soon)") == true) {
							Spotify::NoAds();
						}*/

					break;
				}

				if (Cheat::Tabs != 0) {
					ImGui::End();
				}
			}
			else
			{
				if (MessageBoxW(NULL, L"Quit Cheat?", L"?????????", MB_YESNO | MB_SYSTEMMODAL) == 7) {
					Active = true;
					goto Ce2;
				}
				Active = false;
				Cheat::Cheatclosed = true;
				ImGui::Render();
				pContext->OMSetRenderTargets(1, &mainRenderTargetView, NULL);
				ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
				return oPresent(pSwapChain, SyncInterval, Flags);
			}

		}

	}
	ImGui::Render();
	pContext->OMSetRenderTargets(1, &mainRenderTargetView, NULL);
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	return oPresent(pSwapChain, SyncInterval, Flags);
}

HMODULE Modulestored = NULL;

void UnloadCmd() {
    if (Cheat::ConsoleHandle) {
        FreeConsole();
        SendMessage(Cheat::ConsoleHandle, WM_CLOSE, 0, 0);
    }
}

DWORD WINAPI MainThread(LPVOID lpReserved)
{
	bool init_hook = false;
	do
	{

		if (kiero::init(kiero::RenderType::D3D11) == kiero::Status::Success)
		{
			kiero::bind(8, (void**)&oPresent, hkPresent);
			init_hook = true;
		}
	} while (!init_hook);
	while (Cheat::Cheatclosed == false)
	{
		Cheat::CheatMainRun();
		Sleep(1);
	}

		MH_DisableHook(MH_ALL_HOOKS); 
        UnloadCmd();
		MH_Uninitialize();
		kiero::unbind(8);
	return TRUE;
}



bool IsMouseInsideImGuiMenu()
{
    ImGuiIO& io = ImGui::GetIO();
    return ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow) || io.WantCaptureMouse;
}


LRESULT __stdcall WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {


    Cheat::Vals::hWnd = hWnd;
    Cheat::Vals::uMsg = uMsg;
    Cheat::Vals::wParam = wParam;
    Cheat::Vals::lParam = lParam;
    if (Cheat::GetVals == false) {
        Cheat::GetVals = true;
        GetWindowRect(hWnd, &Cheat::Dawg);
        Cheat::windowWidth = Cheat::Dawg.right;
        Cheat::windowHeight = Cheat::Dawg.bottom;
        Cheat::Start = true;
    }

    if (Cheat::Cheatclosed == false && init) {
        if (IsMouseInsideImGuiMenu() == true) {
            ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);
            return true;
        }
        else
        {
            return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
        }
    }


    return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
}



BOOL WINAPI DllMain(HMODULE hMod, DWORD dwReason, LPVOID lpReserved)
{
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls(hMod);
		Modulestored = hMod;
        Safe = hMod;
		CreateThread(nullptr, 0, MainThread, hMod, 0, nullptr);
		break;
	case DLL_PROCESS_DETACH:
		kiero::shutdown();
		FreeLibraryAndExitThread(hMod, 1);
		break;
	}
	return TRUE;
}
