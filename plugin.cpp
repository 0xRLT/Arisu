#include <Windows.h>
#include <string>
#include <fstream>

#include "plugin.h"
#include "patterns.h"

#include <dbg.h>

const char* MSG_ReadString{};
char MSG_WriteString[MAX_PATH];
char* PTR_MSG_WriteString = MSG_WriteString;
char Kei[MAX_PATH];

std::string bPath = SvenModAPI()->GetBaseDirectory();
std::string fPath = bPath + "\\arisu";
std::string aPath = fPath + "\\list.txt";
std::string isNull = "-1";

bool Exist (const char* MSG_ReadString)
{
	// awfull code ahead

	std::fstream Arisu;
	std::string line{};
	std::string cvar{ MSG_ReadString };

	Arisu.open(aPath, std::fstream::in | std::fstream::out | std::fstream::app);

	if (Arisu.is_open())
	{
		while (std::getline(Arisu, line))
		{
			if (line.find(cvar + " " + "=") != std::string::npos)
			{
				size_t start = 0; // :skull: ?
				while ((start = line.find(cvar)) != std::string::npos) {
					line.erase(start, cvar.length()+3);
				};

				if (line == isNull)
				{
					strcpy_s(Kei, "Bad CVAR request");
				}
				else
				{
					strcpy_s(Kei, line.c_str());
				}

				strncpy_s(MSG_WriteString, Kei, sizeof(Kei));
				Arisu.close();
				return true;
			}
		}
		Arisu.close();
		return false;
	}
	else
	{
		Warning("[Arisu] Something went wrong with opening list.txt\n");
		return false;
	}
}

DECLARE_CLASS_HOOK(void*, CL_Send_CvarValue2, void*);

DECLARE_CLASS_FUNC(void*, HOOKED_CL_Send_CvarValue2, void* thisptr)
{
	__asm
	{
		push esi
		mov MSG_ReadString, esi
		pop esi
	}
	
	//Msg("[DEBUG} Server is requesting value of: %s\n", MSG_ReadString);

	if (!Exist(MSG_ReadString))
	{

		//Msg("[DEBUG} Returned original value for: %s\n", MSG_ReadString);

		__asm
		{
			mov eax, [ebx+4]
		}

		return ORIG_CL_Send_CvarValue2(thisptr);
	}

	//Msg("[DEBUG} Successfully spoofed '%s' with value: %s\n", MSG_ReadString, Kei);

	__asm
	{
		push eax
		pop eax
		mov eax, [PTR_MSG_WriteString]
	}

	return ORIG_CL_Send_CvarValue2(thisptr);
}

api_version_s Arisu::GetAPIVersion()
{
	return SVENMOD_API_VER;
}

bool Arisu::Load(CreateInterfaceFn pfnSvenModFactory, ISvenModAPI* pSvenModAPI, IPluginHelpers* pPluginHelpers)
{
	BindApiToGlobals(pSvenModAPI);

	P_CL_Send_CvarValue2 = MemoryUtils()->FindPattern(SvenModAPI()->Modules()->Hardware, Patterns::Hardware::CL_Send_CvarValue2);
	return true;
}

void Arisu::PostLoad(bool bGlobalLoad)
{
	D_CL_Send_CvarValue2 = DetoursAPI()->DetourFunction(P_CL_Send_CvarValue2, HOOKED_CL_Send_CvarValue2, GET_FUNC_PTR(ORIG_CL_Send_CvarValue2));
}

void Arisu::Unload(void)
{
	DetoursAPI()->RemoveDetour(Arisu::D_CL_Send_CvarValue2);
}

bool Arisu::Pause(void)
{
	return true;
}

void Arisu::Unpause(void)
{

}

void Arisu::GameFrame(client_state_t state, double frametime, bool bPostRunCmd)
{
	if (bPostRunCmd)
	{
	}
	else
	{
	}
}

PLUGIN_RESULT Arisu::Draw(void)
{
	return PLUGIN_CONTINUE;
}

PLUGIN_RESULT Arisu::DrawHUD(float time, int intermission)
{
	return PLUGIN_CONTINUE;
}

const char* Arisu::GetName(void)
{
	return "Arisu";
}

const char* Arisu::GetAuthor(void)
{
	return "void";
}

const char* Arisu::GetVersion(void)
{
	return "1.0.0";
}

const char* Arisu::GetDescription(void)
{
	return "Change server cvars queries responses";
}

const char* Arisu::GetURL(void)
{
	return "...";
}

const char* Arisu::GetDate(void)
{
	return SVENMOD_BUILD_TIMESTAMP;
}

const char* Arisu::GetLogTag(void)
{
	return "ARISU";
}

EXPOSE_SINGLE_INTERFACE(Arisu, IClientPlugin, CLIENT_PLUGIN_INTERFACE_VERSION);