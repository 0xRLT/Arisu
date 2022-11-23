#include <Windows.h>
#include <string>

#include "plugin.h"
#include "patterns.h"

#include <convar.h>
#include <dbg.h>
#include <messagebuffer.h>

#include <hl_sdk/common/protocol.h>

CMessageBuffer CvarValueBuffer;
sizebuf_t* clc_buffer;
void* m_pclc_buffer;
FILE* list = NULL;
char line[999][MAX_PATH];
char cvar[999][MAX_PATH];
char value[999][MAX_PATH];
char s_string[MAX_PATH];
int lines = 0;

std::string bPath = (SvenModAPI()->GetBaseDirectory());
std::string fPath = "\\arisu\\list.txt";
std::string aPath = bPath + fPath;

bool InList(char* pszCvarName)
{
	for (int n = 0; n <= lines - 1; n++)
	{
		if (strcmp(cvar[n], pszCvarName) == 0)
		{
			if (strcmp(value[n], "-1") == 0)
			{
				strncpy_s(s_string, "Bad CVAR request", 17);
				return true;
			}
			else
			{
				strncpy_s(s_string, value[n], sizeof(value[n]));
				return true;
			}
		}
	}
	return false;
}

bool LoadList()
{
	fopen_s(&list, aPath.c_str(), "r");

	if (list == NULL)
	{
		return false;
	}
	else
	{
		int n = 0;
		while (fgets(line[n], sizeof(line[n]), list))
		{
			bool IsValue = false;
			int v_i = 0;
			line[n][strcspn(line[n], "\n")] = 0;
			for (int i = 0; i <= strlen(line[n]); i++)
			{
				if (!IsValue)
				{
					if (line[n][i] == ' ')
					{
						IsValue = true;
						i = i + 2;
					}
					else
					{
						cvar[n][i] = line[n][i];
					}
				}
				else
				{
					value[n][v_i] = line[n][i];
					v_i++;
				}
			}
			n++;
		}
		lines = n;
		fclose(list);
		return true;
	}
}

CON_COMMAND(arisu_reload, "Reloads list file")
{
	if (!LoadList())
	{
		Warning("[Arisu] Failed to reload file list.txt\n");
		ConColorMsg({ 85, 150, 255, 255 }, "[Arisu] Using latest loaded list\n");
	}
	else
	{
		ConColorMsg({ 85, 150, 255, 255 }, "[Arisu] list.txt reloaded successfully\n");
	}
}

DECLARE_HOOK(void, __cdecl, CL_Send_CvarValue2);

DECLARE_FUNC(void, __cdecl, HOOKED_CL_Send_CvarValue2)
{
	CNetMessageParams* params = Utils()->GetNetMessageParams();
	CvarValueBuffer.Init(params->buffer, params->readcount, params->badread);

	CMessageBuffer ClientToServerBuffer;
	ClientToServerBuffer.Init(clc_buffer);

	int iRequestID = CvarValueBuffer.ReadLong();
	char* pszCvarName = CvarValueBuffer.ReadString();

	if (strlen(pszCvarName) >= 0xFF)
	{
		ClientToServerBuffer.WriteByte(CLC_REQUESTCVARVALUE2);
		ClientToServerBuffer.WriteLong(iRequestID);
		ClientToServerBuffer.WriteString(pszCvarName);
		ClientToServerBuffer.WriteString((char*)"Bad CVAR request");
	}
	else
	{
		cvar_t* pCvar = CVar()->FindCvar(pszCvarName);

		ClientToServerBuffer.WriteByte(CLC_REQUESTCVARVALUE2);
		ClientToServerBuffer.WriteLong(iRequestID);
		ClientToServerBuffer.WriteString(pszCvarName);

		//ConColorMsg({ 40, 255, 40, 255 }, "[Arisu] Server is requesting value of: %s\n", pszCvarName);

		if (!InList(pszCvarName))
		{
			if (pCvar != NULL)
			{
				ClientToServerBuffer.WriteString((char*)pCvar->string);
			}
			else
			{
				ClientToServerBuffer.WriteString((char*)"Bad CVAR request");
			}
		}
		else
		{
			ClientToServerBuffer.WriteString((char*)s_string);
			ConColorMsg({ 85, 150, 255, 255 }, "[Arisu] Returned value: '%s' for query '%s'\n", s_string, pszCvarName);
		}
	}

	Utils()->ApplyReadToNetMessageBuffer(&CvarValueBuffer);
}

api_version_s Arisu::GetAPIVersion()
{
	return SVENMOD_API_VER;
}

bool Arisu::Load(CreateInterfaceFn pfnSvenModFactory, ISvenModAPI* pSvenModAPI, IPluginHelpers* pPluginHelpers)
{
	BindApiToGlobals(pSvenModAPI);

	P_CL_Send_CvarValue2 = MemoryUtils()->FindPattern(SvenModAPI()->Modules()->Hardware, Patterns::Hardware::CL_Send_CvarValue2);
	m_pclc_buffer = MemoryUtils()->FindPattern(SvenModAPI()->Modules()->Hardware, Patterns::Hardware::clc_buffer);

	if (m_pclc_buffer == NULL)
	{
		Warning("[Arisu] Failed to locate \"clc_buffer\"\n");
		return false;
	}

	clc_buffer = *reinterpret_cast<sizebuf_t**>((unsigned char*)m_pclc_buffer + 1);

	if (!LoadList()) 
	{
		Warning("[Arisu] Failed to open list.txt\n");
		return false;
	}

	ConVar_Register();

	return true;
}

void Arisu::PostLoad(bool bGlobalLoad)
{
	D_CL_Send_CvarValue2 = DetoursAPI()->DetourFunction(P_CL_Send_CvarValue2, HOOKED_CL_Send_CvarValue2, GET_FUNC_PTR(ORIG_CL_Send_CvarValue2));
}

void Arisu::Unload(void)
{
	DetoursAPI()->RemoveDetour(Arisu::D_CL_Send_CvarValue2);
	ConVar_Unregister();
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

void Arisu::Draw(void)
{
}

void Arisu::DrawHUD(float time, int intermission)
{
}

void Arisu::OnFirstClientdataReceived(client_data_t* pcldata, float flTime)
{
}

void Arisu::OnBeginLoading(void)
{
}

void Arisu::OnEndLoading(void)
{
}

void Arisu::OnDisconnect(void)
{
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
	return "1.0.1";
}

const char* Arisu::GetDescription(void)
{
	return "Change server cvars queries responses";
}

const char* Arisu::GetURL(void)
{
	return "https://github.com/Keizawa/Arisu";
}

const char* Arisu::GetDate(void)
{
	return SVENMOD_BUILD_TIMESTAMP;
}

const char* Arisu::GetLogTag(void)
{
	return "ARISU";
}

Arisu g_Arisu;

EXPOSE_SINGLE_INTERFACE(Arisu, IClientPlugin, CLIENT_PLUGIN_INTERFACE_VERSION, g_Arisu);
