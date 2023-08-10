#include <iostream>
#include "proc.h"

struct Player
{
	uintptr_t localPlayer{};
	uintptr_t moduleBase{}, moduleBase2;

	bool bStamina{ false }, bHealth{ false }, bAmmo{false};
	
	const float newHealth{1337}, newStamina{1337};
	const int newAmmo{ 1337 };
	/*const float newX{1.0f}, newY{1.0f}, newZ{1.0f};

	struct Vec3
	{
		float x{}, y{}, z{};
	};
	*/
};

namespace offsets
{
	uintptr_t localPlayerOffset{ 0x80E07C };
	std::vector<unsigned int>healthOffsets{0x7C, 0x4, 0x164 };
	std::vector<unsigned int>staminaOffsets{ 0x7C , 0x21F0 };
	std::vector<unsigned int>ammoOffsets{ 0x7C, 0x2530, 0x4, 0x20C, 0x7C, 0x1E4 };
	//std::vector<unsigned int>playerX{ 0x7c, 0x4, 0x8 };
	//std::vector<unsigned int>playerY{ 0x7c, 0x4, 0xC };
	//std::vector<unsigned int>playerZ{ 0x7c, 0x4, 0x10 };
	//std::vector<unsigned int>pitch{ 0x7c, 0x4, 0x74 };
	//std::vector<unsigned int>yaw{ 0x7c, 0x4, 0x78 };
};

int main()
{
	HANDLE h{ GetStdHandle(STD_OUTPUT_HANDLE) };
	Player plr{};
	DWORD procId{ GetProcId(L"cof.exe") };
	HANDLE hProc{ OpenProcess(PROCESS_ALL_ACCESS, NULL, procId) };
	
	SetConsoleTextAttribute(h, 13);

	if(procId)
	{
		plr.moduleBase = GetModuleBaseAddress(procId, L"hw.dll");
		plr.moduleBase2 = GetModuleBaseAddress(procId, L"hl.dll");

		plr.localPlayer = plr.moduleBase + offsets::localPlayerOffset;

		std::cout << "Process Found!:" << procId << std::endl;
	}
	else
	{
		std::cout << "Process not found, press enter to exit \n";
        getchar();
        return 0;
	}
	std::cout << "---------------------------------------" << std::endl;
	std::cout << "[F1] for Unlimited Health" << std::endl;
	std::cout << "[F2] for Unlimited Stamina" << std::endl;
	std::cout << "[F3] for Unlimited Ammo" << std::endl;
	std::cout << "---------------------------------------" << std::endl;
	std::cout << "Log: " << std::endl;

	DWORD dwExit{ 0 };
	
	uintptr_t healthAddr = FindDMAAddy(hProc, plr.localPlayer, offsets::healthOffsets);

	uintptr_t staminaAddr = {};

	uintptr_t ammoAddr = {};

	while (GetExitCodeProcess(hProc, &dwExit) && dwExit == STILL_ACTIVE)
	{

		ammoAddr = FindDMAAddy(hProc, plr.localPlayer, offsets::ammoOffsets);
		staminaAddr = FindDMAAddy(hProc, plr.localPlayer, offsets::staminaOffsets);

		if (GetAsyncKeyState(VK_F1) & 1)
		{
			plr.bHealth = !plr.bHealth;
			if (plr.bHealth == true)
			{
				SetConsoleTextAttribute(h, 2);
				std::cout << "Health Hack Activated!" << std::endl;
			}
			else
			{
				SetConsoleTextAttribute(h, 4);
				std::cout << "Health Hack Deactivated!" << std::endl;
			}
		}

		if (GetAsyncKeyState(VK_F2) & 1)
		{
			plr.bStamina = !plr.bStamina;
			if (plr.bStamina == true)
			{
				SetConsoleTextAttribute(h, 2);
				std::cout << "Stamina Hack Activated!" << std::endl;
			}
			else
			{
				SetConsoleTextAttribute(h, 4);
				std::cout << "Stamina Hack Deactivated!" << std::endl;
			}
		}

		//only once
		if (GetAsyncKeyState(VK_F3) & 1)
		{
			plr.bAmmo = !plr.bAmmo;
			if (plr.bAmmo == true)
			{
				SetConsoleTextAttribute(h, 2);
				std::cout << "Ammo Hack Activated" << std::endl;
			}
			else
			{
				SetConsoleTextAttribute(h, 4);
				std::cout << "Ammo Hack Deactivated" << std::endl;
			}
		}

		//continues write/freeze 
		if (plr.bHealth)
		{
			WriteProcessMemory(hProc, (BYTE*)healthAddr, &plr.newHealth, sizeof(plr.newHealth), 0);
		}

		if(plr.bStamina)
		{
			WriteProcessMemory(hProc, (BYTE*)staminaAddr, &plr.newStamina, sizeof(plr.newStamina), 0);
		}

		if(plr.bAmmo)
		{
			WriteProcessMemory(hProc, (BYTE*)ammoAddr, &plr.newAmmo, sizeof(plr.newAmmo), 0);
		}

		Sleep(1);
	}

	return 0;
}