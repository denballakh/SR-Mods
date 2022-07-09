#include <algorithm>
#include <cctype>
#include <codecvt>
#include <ctime>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <locale>
#include <math.h>
#include <shlobj.h>
#include <string>
#include <vector>
#include <windows.h>
#include <stddef.h>
#include <string>

#include "structs/include.h"
#include "wrapped-classes/include.h"
#include "TList_methods.cpp"

#define DLLEXPORT extern "C" __declspec(dllexport)

constexpr auto t_Kling = 0;
constexpr auto t_Ranger = 1;
constexpr auto t_Transport = 2;
constexpr auto t_Pirate = 3;
constexpr auto t_Warrior = 4;
constexpr auto t_Tranclucator = 5;
constexpr auto t_RC = 6;
constexpr auto t_PB = 7;
constexpr auto t_WB = 8;
constexpr auto t_SB = 9;
constexpr auto t_BK = 10;
constexpr auto t_MC = 11;
constexpr auto t_CB = 12;
constexpr auto t_UB = 13;
constexpr auto t_ObjNone = 0;
constexpr auto t_ObjStar = 1;
constexpr auto t_ObjHole = 2;
constexpr auto t_ObjPlanet = 3;
constexpr auto t_ObjStation = 4;
constexpr auto t_ObjShip = 5;
constexpr auto t_ObjItem = 6;
constexpr auto t_ObjMissile = 7;
constexpr auto t_ObjAsteroid = 8;
constexpr auto t_ObjSector = 9;
constexpr auto DBL_EPSILON = 1e-10;

using namespace std;


CHAR system_folder[MAX_PATH];
CHAR log_path[MAX_PATH];
struct shipNearby {
public:
    TShip *Ship;
    int Dist;
};
static vector <shipNearby> ShipsNearby;
void ShipsNearbySort(vector<shipNearby> &vector, int, int);
int ShipsNearbyPartition(vector<shipNearby> &vector, int, int);
void SFT(string);
int Rnd(int, int);
int ObjectType(void*);
byte ShipTypeN(TShip *);
string ShipType(TShip *);
int StarShips(TStar* );
TShip* StarShips(TStar*, int);
TStar* ShipStar(TShip*);
bool ShipInNormalSpace(TShip*);
bool ShipInHyperSpace(TShip*);
TPlanet* GetShipPlanet(TShip*);
TShip* GetShipRuins(TShip*);
TShip* ShipGetBad(TShip*);
string ShipName(TShip*);
string GetParFromStringLocal(wchar_t*, wchar_t*, int, int);


BOOL APIENTRY DllMain(
    HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
) {
    switch (ul_reason_for_call) {
        case DLL_PROCESS_ATTACH:
            srand(time(0));
            // SHGetSpecialFolderPath(0, system_folder, CSIDL_PERSONAL, true);
            SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL, 0, system_folder);
            strcpy(log_path, system_folder);
            strcat(log_path, "\\SpaceRangersHD\\########.log");
            SFT("UtilityFunctions.dll version 1.5.016 was linked");
            break;
        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
        case DLL_PROCESS_DETACH:
            break;
    }
    return TRUE;
}
static wstring_convert<codecvt_utf8<wchar_t>> convert;
static wstring to_game;
static TGalaxy* Galaxy = nullptr;
static TPlayer* Player = nullptr;
static VMT vmt_TObject = nullptr;
static VMT vmt_TObjectEx = nullptr;
static VMT vmt_TConstellation = nullptr;
static VMT vmt_TStar = nullptr;
static VMT vmt_THole = nullptr;
static VMT vmt_TPlanet = nullptr;
static VMT vmt_TRanger = nullptr;
static VMT vmt_TShip = nullptr;
static VMT vmt_TPlayer = nullptr;
static VMT vmt_TMissile = nullptr;
static VMT vmt_TAsteroid = nullptr;
static VMT vmt_TItem = nullptr;
static VMT vmt_TEquipment = nullptr;
static VMT vmt_TGoods = nullptr;
static VMT vmt_TScript = nullptr;
static VMT_TScriptShip *vmt_TScriptShip = nullptr;
TScriptShip* (__fastcall* TScriptShip_Create) (VMT_TScriptShip*, int8_t flag);
int(__fastcall* TScriptShip_Destroy) (TScriptShip* list, int8_t flag);
TScriptShip* (__fastcall* TScriptShip_New) (VMT_TScriptShip*);
void* (__fastcall* GetMem) (int);
void(__fastcall* FreeMem) (void*);
void SFT(string str_out) {
    const char* out = str_out.c_str();
    FILE* file = fopen(log_path, "a");
    fwrite(out, strlen(out), 1, file);
    fwrite("\n", 1, 1, file);
    fclose(file);
}
DLLEXPORT
void UtilityFunctionsLibInit(TGalaxy* galaxy) {
    Galaxy = galaxy;
    Player = (TPlayer*)Galaxy->rangers->items[Galaxy->player_index];
    vmt_TPlayer = Player->cls;
    VMT prev_prev_prev_parent = vmt_TPlayer;
    VMT prev_prev_parent = vmt_TPlayer;
    VMT prev_parent = vmt_TPlayer;
    VMT parent = vmt_struct(vmt_TPlayer)->parent;
    while (parent) {
        prev_prev_prev_parent = prev_prev_parent;
        prev_prev_parent = prev_parent;
        prev_parent = parent;
        parent = ((VMT_helper*)parent)->parent;
    }
    vmt_TObject = prev_parent;
    vmt_TObjectEx = prev_prev_parent;
    vmt_TShip = prev_prev_prev_parent;
    parent = vmt_struct(((THull*)Player->items->items[0])->cls)->parent;
    while (parent != vmt_TObjectEx) {
        prev_prev_parent = prev_parent;
        prev_parent = parent;
        parent = ((VMT_helper*)parent)->parent;
    }
    vmt_TItem = prev_parent;
    vmt_TEquipment = prev_prev_parent;
    TConstellation* sector = (TConstellation*)Galaxy->constellations->items[0];
    vmt_TConstellation = sector->cls;
    TStar* star = (TStar*)Galaxy->stars->items[0];
    vmt_TStar = star->cls;
    vmt_TAsteroid = ((TAsteroid*)(star->asteroids->items[0]))->cls;
    vmt_TPlanet = ((TPlanet*)(Galaxy->planets->items[0]))->cls;
    vmt_TRanger = ((TRanger*)(Galaxy->rangers->items[0]))->cls;
    vmt_TScript = ((TScript*)(Galaxy->scripts->items[0]))->cls;
    TScriptShip* first_obj = (TScriptShip*)Player->script_ship_objects->items[0];
    vmt_TScriptShip = (VMT_TScriptShip*)first_obj->cls;
    TList_methods_init(Galaxy->scripts);
    *(void**)&TScriptShip_Destroy = *((void**)vmt_TScriptShip - 1);
    *(void**)&TScriptShip_New = *((void**)vmt_TScriptShip - 3);
    SET_VAR_WITH_OFFSET(TScriptShip_Create, TScriptShip_Destroy, 0x52C - 0x570);
    *(void**)&GetMem = (void*)0x402488;
    *(void**)&FreeMem = (void*)0x402B0C;
}
void ShipsNearbySort(vector<shipNearby>& vector, int lo, int hi) {
    if (hi <= lo) return;
    int j = ShipsNearbyPartition(vector, lo, hi);
    ShipsNearbySort(vector, lo, j - 1);
    ShipsNearbySort(vector, j + 1, hi);
}
int ShipsNearbyPartition(vector<shipNearby>& vector, int lo, int hi) {
    int i = lo;
    int j = hi + 1;
    while (true) {
        while (vector[++i].Dist < vector[lo].Dist) {
            if (i == hi) break;
        }
        while (vector[--j].Dist > vector[lo].Dist) {
            if (j == lo) {
                break;
            }
        }
        if (i >= j) {
            break;
        }
        swap(vector[i], vector[j]);
    }
    swap(vector[lo], vector[j]);
    return j;
}
void TListAdd(void* list, void* object) {
    ASM_CALL(TList_Add, list, object);
}
void TListDelete(void* list, int el_num) {
    ASM_CALL(TList_Delete, list, el_num);
}

int Rnd(int min, int max) {
    unsigned int seed;
    if (Galaxy->AA_enabled && Galaxy->AA_rnd_chaotic) seed = rand();
    else {
        seed = Galaxy->rnd_seed;
        unsigned int prev_seed = seed;
        seed = seed * 7981 + 567 + seed / 7981;
        if (prev_seed == seed) {
            seed = seed * 7281 + 517 + seed / 7181;
        }
        Galaxy->rnd_seed = seed;
    }
    if (min < max) return min + int(seed % (max - min + 1));
    return max + int(seed % (min - max + 1));
}
int BankRound(double value) {
    float fShift = (value >= 0 ? 0.5 : -0.5);
    if (fabs(fabs(value) - fabs(double(int(value))) - 0.5) < DBL_EPSILON) {
        return (double(nearbyint(value * 0.5f) * 2.0f));
    }
    return (double(int(value + fShift)));
}
int NearestOddRound(double value) {
    float fShift = (value >= 0 ? 0.5 : -0.5);
    if (fabs(fabs(value) - fabs(double(int(value))) - 0.5) < DBL_EPSILON) {
        return (double(2 * floor(value * 0.5f) + 1));
    }
    return (double(int(value + fShift)));
}
static inline string& ltrim(string& s) {
    s.erase(s.begin(), find_if (s.begin(), s.end(), not1(ptr_fun<int, int>(isspace))));
    return s;
}
static inline string& rtrim(string& s) {
    s.erase(find_if (s.rbegin(), s.rend(), not1(ptr_fun<int, int>(isspace))).base(), s.end());
    return s;
}
static inline string& trim(string &s) {
    return ltrim(rtrim(s));
}
byte GalaxyTechLevel() {
    return Galaxy->GTL;
}
byte GalaxyDiffLevels(int diff_num) {
    return Galaxy->diff_levels[diff_num];
}
int ObjectType(void* obj) {
    VMT vmt = ((TShip*)obj)->cls;
    VMT prev_parent = vmt;
    VMT parent = vmt_struct(vmt)->parent;
    while (parent != vmt_TObjectEx && parent != vmt_TObject) {
        prev_parent = parent;
        parent = ((VMT_helper*)parent)->parent;
    }
    vmt = prev_parent;
    if (vmt == vmt_TShip) {
        if (ShipTypeN((TShip*)obj) < t_RC) return t_ObjShip;
        else return t_ObjStation;
    } else if (vmt == vmt_TItem) return t_ObjItem;
    else if (vmt == vmt_TPlanet) return t_ObjPlanet;
    else if (vmt == vmt_TStar) return t_ObjStar;
    else if (vmt == vmt_TAsteroid) return t_ObjAsteroid;
    else if (vmt == vmt_TConstellation) return t_ObjSector;
    else return t_ObjNone;
}
int StarShips(TStar* star) {
    return star->ships->count;
}
TShip* StarShips(TStar* star, int index) {
    return (TShip*)star->ships->items[index];
}
TStar* ShipStar(TShip* ship) {
    return ship->cur_star;
}
bool ShipInNormalSpace(TShip* ship) {
    return (!GetShipPlanet(ship) && !GetShipRuins(ship) && !ShipInHyperSpace(ship));
}
bool ShipInHyperSpace(TShip* ship) {
    return ship->in_hyper_space;
}
TPlanet* GetShipPlanet(TShip* ship) {
    if (ship == Player) {
        if (Player->bridge_num) return Player->bridge_cur_planet;
    }
    return ship->cur_planet;
}
TShip* GetShipRuins(TShip* ship) {
    if (ship == Player) {
        if (Player->bridge_num) return Player->bridge_cur_ship;
    }
    return ship->cur_ship;
}
TShip* ShipGetBad(TShip* ship) {
    return ship->ship_bad;
}
byte ShipTypeN(TShip* ship) {
    return ship->type;
}
string ShipName(TShip* ship) {
    string name = convert.to_bytes(ship->name);
    return name;
}
DLLEXPORT
int GetGalaxyGenerationSeed() {
    return Galaxy->gen_seed;
}
DLLEXPORT
unsigned int GetSectorAdjacentToDicea() {
    for (int i = 0; i < Galaxy->constellations->count; ++i) {
        TConstellation* sector = (TConstellation*)Galaxy->constellations->items[i];
        if (sector->id == 20) continue;
        if (sector->p_bound_hidden->count) return (unsigned int)sector;
    }
    return 0;
}
DLLEXPORT
int AdvancedAdjustmentSet(
    int option,
    int val
) {
    switch (option) {
    case 0:
        if (val != -1) {
            if (val < 0 || val > 1) {
                SFT("UtilityFunctions.dll AdvancedAdjustmentSet function error. Wrong value for AA_enabled was received.");
                throw;
            }
            int old_val = Galaxy->AA_enabled;
            Galaxy->AA_enabled = val;
            return old_val;
        }
        return Galaxy->AA_enabled;
    case 1:
        if (val != -1) {
            if (val < 50 || val > 500) {
                SFT("UtilityFunctions.dll AdvancedAdjustmentSet function error. Wrong value for AA_kling_strength was received.");
                throw;
            }
            int old_val = Galaxy->AA_kling_strength;
            Galaxy->AA_kling_strength = BankRound(0.16 * val) - 8;
            return 50 + BankRound(old_val * 6.25);
        }
        return 50 + BankRound(Galaxy->AA_kling_strength * 6.25);
    case 2:
        if (val != -1) {
            if (val < 50 || val > 500) {
                SFT("UtilityFunctions.dll AdvancedAdjustmentSet function error. Wrong value for AA_kling_aggro was received.");
                throw;
            }
            int old_val = Galaxy->AA_kling_aggro;
            Galaxy->AA_kling_aggro = BankRound(0.16 * val) - 8;
            return 50 + BankRound(old_val * 6.25);
        }
        return 50 + BankRound(Galaxy->AA_kling_aggro * 6.25);
    case 3:
        if (val != -1) {
            if (val < 50 || val > 500) {
                SFT("UtilityFunctions.dll AdvancedAdjustmentSet function error. Wrong value for AA_kling_spawn was received.");
                throw;
            }
            int old_val = Galaxy->AA_kling_spawn;
            Galaxy->AA_kling_spawn = BankRound(0.16 * val) - 8;
            return 50 + BankRound(old_val * 6.25);
        }
        return 50 + BankRound(Galaxy->AA_kling_spawn * 6.25);
    case 4:
        if (val != -1) {
            if (val < 50 || val > 500) {
                SFT("UtilityFunctions.dll AdvancedAdjustmentSet function error. Wrong value for AA_pirate_aggro was received.");
                throw;
            }
            int old_val = Galaxy->AA_pirate_aggro;
            Galaxy->AA_pirate_aggro = BankRound(0.16 * val) - 8;
            return 50 + BankRound(old_val * 6.25);
        }
        return 50 + BankRound(Galaxy->AA_pirate_aggro * 6.25);
    case 5:
        if (val != -1) {
            if (val < 50 || val > 200) {
                SFT("UtilityFunctions.dll AdvancedAdjustmentSet function error. Wrong value for AA_coal_aggro was received.");
                throw;
            }
            int old_val = Galaxy->AA_coal_aggro;
            Galaxy->AA_coal_aggro = BankRound(0.16 * val) - 8;
            return 50 + BankRound(old_val * 6.25);
        }
        return 50 + BankRound(Galaxy->AA_coal_aggro * 6.25);
    case 6:
        if (val != -1) {
            if (val < 50 || val > 200) {
                SFT("UtilityFunctions.dll AdvancedAdjustmentSet function error. Wrong value for AA_asteroid_mod was received.");
                throw;
            }
            int old_val = Galaxy->AA_asteroid_mod;
            Galaxy->AA_asteroid_mod = BankRound(0.16 * val) - 8;
            return 50 + BankRound(old_val * 6.25);
        }
        return 50 + BankRound(Galaxy->AA_asteroid_mod * 6.25);
    case 7:
        if (val != -1) {
            if (val < 50 || val > 200) {
                SFT("UtilityFunctions.dll AdvancedAdjustmentSet function error. Wrong value for AA_sun_damage_mod was received.");
                throw;
            }
            int old_val = Galaxy->AA_sun_damage_mod;
            Galaxy->AA_sun_damage_mod = BankRound(0.16 * val) - 8;
            return 50 + BankRound(old_val * 6.25);
        }
        return 50 + BankRound(Galaxy->AA_sun_damage_mod * 6.25);
    case 8:
        if (val != -1) {
            if (val < 0 || val > 255) {
                SFT("UtilityFunctions.dll AdvancedAdjustmentSet function error. Wrong value for AA_extra_inventions was received.");
                throw;
            }
            int old_val = Galaxy->AA_extra_inventions;
            Galaxy->AA_extra_inventions = val;
            return old_val;
        }
        return Galaxy->AA_extra_inventions;
    case 9:
        if (val != -1) {
            if (val < 0 || val > 100) {
                SFT("UtilityFunctions.dll AdvancedAdjustmentSet function error. Wrong value for AA_akrin_mod was received.");
                throw;
            }
            int old_val = Galaxy->AA_akrin_mod;
            Galaxy->AA_akrin_mod = val;
            return old_val;
        }
        return Galaxy->AA_akrin_mod;
    case 10:
        if (val != -1) {
            if (val < 50 || val > 200) {
                SFT("UtilityFunctions.dll AdvancedAdjustmentSet function error. Wrong value for AA_node_drop_mod was received.");
                throw;
            }
            int old_val = Galaxy->AA_node_drop_mod;
            Galaxy->AA_node_drop_mod = BankRound(0.16 * val) - 8;
            return 50 + BankRound(old_val * 6.25);
        }
        return 50 + BankRound(Galaxy->AA_node_drop_mod * 6.25);
    case 11:
        if (val != -1) {
            if (val < 50 || val > 200) {
                SFT("UtilityFunctions.dll AdvancedAdjustmentSet function error. Wrong value for AA_AB_drop_value_mod was received.");
                throw;
            }
            int old_val = Galaxy->AA_AB_drop_value_mod;
            Galaxy->AA_AB_drop_value_mod = BankRound(0.16 * val) - 8;
            return 50 + BankRound(old_val * 6.25);
        }
        return 50 + BankRound(Galaxy->AA_AB_drop_value_mod * 6.25);
    case 12:
        if (val != -1) {
            if (val < 50 || val > 200) {
                SFT("UtilityFunctions.dll AdvancedAdjustmentSet function error. Wrong value for AA_drop_value_mod was received.");
                throw;
            }
            int old_val = Galaxy->AA_drop_value_mod;
            Galaxy->AA_drop_value_mod = BankRound(0.16 * val) - 8;
            return 50 + BankRound(old_val * 6.25);
        }
        return 50 + BankRound(Galaxy->AA_drop_value_mod * 6.25);
    case 13:
        if (val != -1) {
            if (val < 0 || val > 10) {
                SFT("UtilityFunctions.dll AdvancedAdjustmentSet function error. Wrong value for AA_ag_planets was received.");
                throw;
            }
            int old_val = Galaxy->AA_ag_planets;
            Galaxy->AA_ag_planets = val;
            return old_val;
        }
        return Galaxy->AA_ag_planets;
    case 14:
        if (val != -1) {
            if (val < 0 || val > 10) {
                SFT("UtilityFunctions.dll AdvancedAdjustmentSet function error. Wrong value for AA_mi_planets was received.");
                throw;
            }
            int old_val = Galaxy->AA_mi_planets;
            Galaxy->AA_mi_planets = val;
            return old_val;
        }
        return Galaxy->AA_mi_planets;
    case 15:
        if (val != -1) {
            if (val < 0 || val > 10) {
                SFT("UtilityFunctions.dll AdvancedAdjustmentSet function error. Wrong value for AA_in_planets was received.");
                throw;
            }
            int old_val = Galaxy->AA_in_planets;
            Galaxy->AA_in_planets = val;
            return old_val;
        }
        return Galaxy->AA_in_planets;
    case 16:
        if (val != -1) {
            if (val < 0 || val > 50) {
                SFT("UtilityFunctions.dll AdvancedAdjustmentSet function error. Wrong value for AA_extra_rangers was received.");
                throw;
            }
            int old_val = Galaxy->AA_extra_rangers;
            Galaxy->AA_extra_rangers = val;
            return old_val;
        }
        return Galaxy->AA_extra_rangers;
    case 17:
        if (val != -1) {
            if (val < 50 || val > 200) {
                SFT("UtilityFunctions.dll AdvancedAdjustmentSet function error. Wrong value for AA_AB_hitpoints_mod was received.");
                throw;
            }
            int old_val = Galaxy->AA_AB_hitpoints_mod;
            Galaxy->AA_AB_hitpoints_mod = BankRound(0.16 * val) - 8;
            return 50 + BankRound(old_val * 6.25);
        }
        return 50 + BankRound(Galaxy->AA_AB_hitpoints_mod * 6.25);
    case 18:
        if (val != -1) {
            if (val < 50 || val > 200) {
                SFT("UtilityFunctions.dll AdvancedAdjustmentSet function error. Wrong value for AA_AB_damage_mod was received.");
                throw;
            }
            int old_val = Galaxy->AA_AB_damage_mod;
            Galaxy->AA_AB_damage_mod = BankRound(0.16 * val) - 8;
            return 50 + BankRound(old_val * 6.25);
        }
        return 50 + BankRound(Galaxy->AA_AB_damage_mod * 6.25);
    case 19:
        if (val != -1) {
            if (val < 0 || val > 50) {
                SFT("UtilityFunctions.dll AdvancedAdjustmentSet function error. Wrong value for AA_AI_tolerate_junk was received.");
                throw;
            }
            int old_val = Galaxy->AA_AI_tolerate_junk;
            Galaxy->AA_AI_tolerate_junk = val;
            return old_val;
        }
        return Galaxy->AA_AI_tolerate_junk;
    case 20:
        if (val != -1) {
            if (val < 0 || val > 1) {
                SFT("UtilityFunctions.dll AdvancedAdjustmentSet function error. Wrong value for AA_rnd_chaotic was received.");
                throw;
            }
            int old_val = Galaxy->AA_rnd_chaotic;
            Galaxy->AA_rnd_chaotic = val;
            return old_val;
        }
        return Galaxy->AA_rnd_chaotic;
    case 21:
        if (val != -1) {
            if (val < 0 || val > 1) {
                SFT("UtilityFunctions.dll AdvancedAdjustmentSet function error. Wrong value for AA_eq_knowledge_restricted was received.");
                throw;
            }
            int old_val = Galaxy->AA_eq_knowledge_restricted;
            Galaxy->AA_eq_knowledge_restricted = val;
            return old_val;
        }
        return Galaxy->AA_eq_knowledge_restricted;
    case 22:
        if (val != -1) {
            if (val < 0 || val > 1) {
                SFT("UtilityFunctions.dll AdvancedAdjustmentSet function error. Wrong value for AA_ruins_near_stars was received.");
                throw;
            }
            int old_val = Galaxy->AA_ruins_near_stars;
            Galaxy->AA_ruins_near_stars = val;
            return old_val;
        }
        return Galaxy->AA_ruins_near_stars;
    case 23:
        if (val != -1) {
            if (val < 0 || val > 1) {
                SFT("UtilityFunctions.dll AdvancedAdjustmentSet function error. Wrong value for AA_ruins_targetting_full was received.");
                throw;
            }
            int old_val = Galaxy->AA_ruins_targetting_full;
            Galaxy->AA_ruins_targetting_full = val;
            return old_val;
        }
        return Galaxy->AA_ruins_targetting_full;
    case 24:
        if (val != -1) {
            if (val < 0 || val > 1) {
                SFT("UtilityFunctions.dll AdvancedAdjustmentSet function error. Wrong value for AA_special_ships_in_game was received.");
                throw;
            }
            int old_val = Galaxy->AA_special_ships_in_game;
            Galaxy->AA_special_ships_in_game = val;
            return old_val;
        }
        return Galaxy->AA_special_ships_in_game;
    case 25:
        if (val != -1) {
            if (val < 0 || val > 1) {
                SFT("UtilityFunctions.dll AdvancedAdjustmentSet function error. Wrong value for AA_zero_start_exp was received.");
                throw;
            }
            int old_val = Galaxy->AA_zero_start_exp;
            Galaxy->AA_zero_start_exp = val;
            return old_val;
        }
        return Galaxy->AA_zero_start_exp;
    case 26:
        if (val != -1) {
            if (val < 0 || val > 1) {
                SFT("UtilityFunctions.dll AdvancedAdjustmentSet function error. Wrong value for AA_AB_battle_royale was received.");
                throw;
            }
            int old_val = Galaxy->AA_AB_battle_royale;
            Galaxy->AA_AB_battle_royale = val;
            return old_val;
        }
        return Galaxy->AA_AB_battle_royale;
    case 27:
        if (val != -1) {
            if (val < 0 || val > 1) {
                SFT("UtilityFunctions.dll AdvancedAdjustmentSet function error. Wrong value for AA_kling_racial_weapons was received.");
                throw;
            }
            int old_val = Galaxy->AA_kling_racial_weapons;
            Galaxy->AA_kling_racial_weapons = val;
            return old_val;
        }
        return Galaxy->AA_kling_racial_weapons;
    case 28:
        if (val != -1) {
            if (val < 0 || val > 1) {
                SFT("UtilityFunctions.dll AdvancedAdjustmentSet function error. Wrong value for AA_start_center was received.");
                throw;
            }
            int old_val = Galaxy->AA_start_center;
            Galaxy->AA_start_center = val;
            return old_val;
        }
        return Galaxy->AA_start_center;
    case 29:
        if (val != -1) {
            if (val < 0 || val > 1) {
                SFT("UtilityFunctions.dll AdvancedAdjustmentSet function error. Wrong value for AA_max_range_missiles was received.");
                throw;
            }
            int old_val = Galaxy->AA_max_range_missiles;
            Galaxy->AA_max_range_missiles = val;
            return old_val;
        }
        return Galaxy->AA_max_range_missiles;
    case 30:
        if (val != -1) {
            if (val < 0 || val > 1) {
                SFT("UtilityFunctions.dll AdvancedAdjustmentSet function error. Wrong value for AA_old_hyper was received.");
                throw;
            }
            int old_val = Galaxy->AA_old_hyper;
            Galaxy->AA_old_hyper = val;
            return old_val;
        }
        return Galaxy->AA_old_hyper;
    case 31:
        if (val != -1) {
            if (val < 0 || val > 1) {
                SFT("UtilityFunctions.dll AdvancedAdjustmentSet function error. Wrong value for AA_pirate_nodes was received.");
                throw;
            }
            int old_val = Galaxy->AA_pirate_nodes;
            Galaxy->AA_pirate_nodes = val;
            return old_val;
        }
        return Galaxy->AA_pirate_nodes;
    case 32:
        if (val != -1) {
            if (val < 0 || val > 1) {
                SFT("UtilityFunctions.dll AdvancedAdjustmentSet function error. Wrong value for AA_AI_use_shops was received.");
                throw;
            }
            int old_val = Galaxy->AA_AI_use_shops;
            Galaxy->AA_AI_use_shops = val;
            return old_val;
        }
        return Galaxy->AA_AI_use_shops;
    case 33:
        if (val != -1) {
            if (val < 0 || val > 1) {
                SFT("UtilityFunctions.dll AdvancedAdjustmentSet function error. Wrong value for AA_ruins_use_shops was received.");
                throw;
            }
            int old_val = Galaxy->AA_ruins_use_shops;
            Galaxy->AA_ruins_use_shops = val;
            return old_val;
        }
        return Galaxy->AA_ruins_use_shops;
    case 34:
        if (val != -1) {
            if (val < 0 || val > 1) {
                SFT("UtilityFunctions.dll AdvancedAdjustmentSet function error. Wrong value for AA_duplicate_arts was received.");
                throw;
            }
            int old_val = Galaxy->AA_duplicate_arts;
            Galaxy->AA_duplicate_arts = val;
            return old_val;
        }
        return Galaxy->AA_duplicate_arts;
    case 35:
        if (val != -1) {
            if (val < 0 || val > 2) {
                SFT("UtilityFunctions.dll AdvancedAdjustmentSet function error. Wrong value for AA_hull_growth was received.");
                throw;
            }
            int old_val = Galaxy->AA_hull_growth;
            Galaxy->AA_hull_growth = val;
            return old_val;
        }
        return Galaxy->AA_hull_growth;
    case 36:
        if (val != -1) {
            if (val < 0 || val > 1) {
                SFT("UtilityFunctions.dll AdvancedAdjustmentSet function error. Wrong value for AA_AB_equip_change was received.");
                throw;
            }
            int old_val = Galaxy->AA_AB_equip_change;
            Galaxy->AA_AB_equip_change = val;
            return old_val;
        }
        return Galaxy->AA_AB_equip_change;
    case 37:
        if (val != -1) {
            if (val < 0 || val > 1) {
                SFT("UtilityFunctions.dll AdvancedAdjustmentSet function error. Wrong value for AA_linear_speed_formula was received.");
                throw;
            }
            int old_val = Galaxy->AA_linear_speed_formula;
            Galaxy->AA_linear_speed_formula = val;
            return old_val;
        }
        return Galaxy->AA_linear_speed_formula;
    case 38:
        if (val != -1) {
            if (val < 0 || val > 1) {
                SFT("UtilityFunctions.dll AdvancedAdjustmentSet function error. Wrong value for AA_missiles_bonus_not_split was received.");
                throw;
            }
            int old_val = Galaxy->AA_missiles_bonus_not_split;
            Galaxy->AA_missiles_bonus_not_split = val;
            return old_val;
        }
        return Galaxy->AA_missiles_bonus_not_split;
    default:
        return -1;
    }
}
DLLEXPORT
int ParsCountFromString(
    wchar_t* g_par_string,
    wchar_t* g_sep
) {
    if (g_par_string == nullptr) return 0;
    string par_string = convert.to_bytes(g_par_string);
    string sep = convert.to_bytes(g_sep);
    int count = 1;
    size_t start = 0;
    size_t end = par_string.find(sep);
    if (end == string::npos) return 1;
    while (true) {
        start = end + sep.length();
        end = par_string.find(sep, start);
        if (end == string::npos) return ++count;
        ++count;
    }
}
DLLEXPORT
const wchar_t* GetParFromString(
    wchar_t* g_par_string,
    wchar_t* g_sep,
    int g_par_num,
    int clear_spaces
) {
    to_game = convert.from_bytes(GetParFromStringLocal(g_par_string, g_sep, g_par_num, clear_spaces));
    return to_game.c_str();
}
string GetParFromStringLocal(
    wchar_t* g_par_string,
    wchar_t* g_sep,
    int g_par_num,
    int clear_spaces
) {
    if (g_par_string == nullptr) return "";
    string par_string = convert.to_bytes(g_par_string);
    string sep;
    if (g_sep == nullptr) sep = "";
    else sep = convert.to_bytes(g_sep);
    int count = abs(g_par_num);
    size_t start = 0;
    size_t end = par_string.find(sep);
    if (end == string::npos) {
        if (!count) {
            if (clear_spaces) {
                if (clear_spaces == 1) trim(par_string);
                else {
                    par_string.erase(remove(par_string.begin(), par_string.end(), ' '), par_string.end());
                    par_string.erase(remove(par_string.begin(), par_string.end(), '\t'), par_string.end());
                }
            }
            return par_string;
        } else return "";
    }
    while (true) {
        if (!count) {
            par_string = par_string.substr(start, end - start);
            break;
        }
        start = end + sep.length();
        end = par_string.find(sep, start);
        if (end == string::npos) {
            end = par_string.length();
            par_string = par_string.substr(start, end - start);
            --count;
            break;
        }
        --count;
    }
    if (!count) {
        if (clear_spaces) {
            if (clear_spaces == 1) trim(par_string);
            else {
                par_string.erase(remove(par_string.begin(), par_string.end(), ' '), par_string.end());
                par_string.erase(remove(par_string.begin(), par_string.end(), '\t'), par_string.end());
            }
        }
        return par_string;
    } else return "";
}
DLLEXPORT
const wchar_t* SetParFromString(
    wchar_t* g_par_string,
    wchar_t* g_sep,
    int g_par_num,
    wchar_t* g_par
) {
    if (g_par_string == nullptr) return g_par;
    string par_string = convert.to_bytes(g_par_string);
    string sep;
    if (g_sep == nullptr) sep = "";
    else sep = convert.to_bytes(g_sep);
    string par;
    if (g_par == nullptr) par = "";
    else par = convert.to_bytes(g_par);
    int count = abs(g_par_num);
    if (count < 0) return L"";
    size_t start = 0;
    size_t end = par_string.find(sep);
    if (end == string::npos) {
        if (!count) {
            to_game = convert.from_bytes(par + sep + par_string);
            return to_game.c_str();
        } else {
            to_game = convert.from_bytes(par_string + sep + par);
            return to_game.c_str();
        }
    }
    while (true) {
        if (!count) {
            par_string = par_string.substr(0, start) + par + par_string.substr(end, par_string.length());
            break;
        }
        start = end + sep.length();
        end = par_string.find(sep, start);
        if (end == string::npos) {
            par_string = par_string.substr(0, start) + par;
            --count;
            break;
        }
        --count;
    }
    to_game = convert.from_bytes(par_string);
    return to_game.c_str();
}
DLLEXPORT
const wchar_t* TrimNumbers(wchar_t *from_game) {
    string trim_str = "";
    for (; *from_game; ++from_game) {
        if (!isdigit(*from_game)) {

            trim_str += convert.to_bytes(*from_game);
        }
    }
    to_game = convert.from_bytes(trim_str);
    return to_game.c_str();
}
DLLEXPORT
float PortionInDiapason(
    float cur,
    float a,
    float b,
    float fromA,
    float toB
) {
    if (a < b) {
        if (cur <= a) return fromA;
        else if (cur >= b) return toB;
        else return (cur - a) / (b - a) * (toB - fromA) + fromA;
    } else {
        if (cur >= a) return fromA;
        else if (cur <= b) return toB;
        else return (a - cur) / (a - b) * (toB - fromA) + fromA;
    }
}
DLLEXPORT
float RoundTo(
    float a,
    float round_to,
    int mark
) {
    if (!mark) a = round_to * floor(a / round_to + 0.5);
    else if (mark < 0) a = round_to * floor(a / round_to);
    else if (mark == 1) a = round_to * ceil(a / round_to);
    else if (mark == 2) a = round_to * BankRound(a / round_to);
    else a = round_to * NearestOddRound(a / round_to);
    return a;
}
DLLEXPORT
float Exponent(
    float a,
    float b
) {
    return pow(a, b);
}
DLLEXPORT
int DistCoords(
    int Xa,
    int Ya,
    int Xb,
    int Yb
) {
    int distX = Xa - Xb;
    int distY = Ya - Yb;
    return BankRound(sqrt(distX * distX + distY * distY));
}
int ShipCoordX(TShip* ship) {
    return ship->pos.x;
}
int ShipCoordY(TShip* ship) {
    return ship->pos.y;
}
int DistShips(TShip* ship1, TShip* ship2) {
    return DistCoords(ShipCoordX(ship1), ShipCoordY(ship1), ShipCoordX(ship2), ShipCoordY(ship2));
}
bool ShipInScript(TShip* ship) {
    if (ship == Player) return true;
    TScriptShip* script_object = ship->script_ship;
    if (script_object) return true;
    return false;
}
TScript* GetScriptByName(string script_name) {
    if (script_name == "") return 0;
    wstring w_script_name = L"Script.";
    w_script_name += convert.from_bytes(script_name);
    TScript* target_script = 0;
    TList* scripts_list = Galaxy->scripts;
    for (int i = 0; i < scripts_list->count; ++i) {
        TScript* script = (TScript*)scripts_list->items[i];
        if (wcscmp(script->name, w_script_name.c_str())) continue;
        target_script = script;
        break;
    }
    return target_script;
}
TScriptGroup* GetScriptGroupByNum(TScript* script, int group_num) {
    return (TScriptGroup*)script->groups->items[group_num];
}
TScriptGroup* GetScriptGroupByName(TScript* script, string group_name) {
    if (group_name == "") return 0;
    wstring w_group_name = convert.from_bytes(group_name);
    TScriptGroup* target_group = 0;
    TList* groups_list = script->groups;
    for (int i = 0; i < groups_list->count; ++i) {
        TScriptGroup* group = (TScriptGroup*)groups_list->items[i];
        if (wcscmp(group->name, w_group_name.c_str())) continue;
        target_group = group;
        break;
    }
    return target_group;
}
TScriptState* GetScriptStateByName(TScript* script, string state_name) {
    if (state_name == "") return 0;
    wstring w_state_name = convert.from_bytes(state_name);
    TScriptState* target_state = 0;
    TList* state_list = script->states;
    for (int i = 0; i < state_list->count; ++i) {
        TScriptState* state = (TScriptState*)state_list->items[i];
        if (wcscmp(state->name, w_state_name.c_str())) continue;
        target_state = state;
        break;
    }
    return target_state;
}
int GetScriptGroupNum(TScript* script, TScriptGroup* group) {
    TScriptGroup* target_group = 0;
    TList* groups_list = script->groups;
    for (int i = 0; i < groups_list->count; ++i) {
        TScriptGroup *cur_group = (TScriptGroup*)groups_list->items[i];
        if (cur_group != group) continue;
        return i;
    }
    return -1;
}
TScriptState* GetScriptStateByNum(TScript *script, int state_num) {
    TList *states_list = script->states;
    return (TScriptState*)states_list->items[state_num];
}
bool IsSpecialAgent(TShip* ship) {
    if (ship == Player) return false;
    TScriptShip* ship_script_object = ship->script_ship;
    if (!ship_script_object) return false;
    TScript* script = ship_script_object->script;
    if (wcscmp(script->name, L"Script.PC_fem_rangers")) return false;
    TScriptGroup* group = (TScriptGroup*)script->groups->items[ship_script_object->group];
    if (wcscmp(group->name, L"GroupFem")) return false;
    return true;
}
byte ShipSubType(TShip* ship) {
    switch (ShipTypeN(ship)) {
        case(t_Kling):
            return ((TKling*)ship)->sub_type;
        case(t_Transport):
            return ((TTransport*)ship)->sub_type;
        case(t_Pirate):
            return ((TPirate*)ship)->sub_type;
        case(t_Warrior):
            return ((TWarrior*)ship)->sub_type;
    }
    return 0;
}
string ShipType(TShip *ship) {
    wchar_t *type = ship->custom_type_name;
    if (type) return convert.to_bytes(type);
    else {
        switch (ShipTypeN(ship)) {
            case(t_Kling):
                return "Kling";
            case(t_Ranger):
                if (IsSpecialAgent(ship)) return "FemRanger";
                return "Ranger";
            case(t_Transport):
                switch (ShipSubType(ship)) {
                    case(0):
                        return "Transport";
                    case(1):
                        return "Liner";
                    case(2):
                        return "Diplomat";
                }
            case(t_Pirate):
                return "Pirate";
            case(t_Warrior):
                switch (ShipSubType(ship)) {
                    case(0):
                        return "Warrior";
                    case(1):
                        return "WarriorBig";
                }
            case(t_Tranclucator):
                return "Tranclucator";
            case(t_RC):
                return "RC";
            case(t_PB):
                return "PB";
            case(t_WB):
                if (ship == Player->bridge) return "PlayerBridge";
                else return "WB";
            case(t_SB):
                return "SB";
            case(t_BK):
                return "BK";
            case(t_MC):
                return "MC";
            case(t_CB):
                return "CB";
            case(t_UB):
                SFT("UtilityFunctions.dll ShipType function error. Wrong ruin t_UB ShipType determination.");
                throw;
        }
        return "";
    }
}
string ShipCustomFaction(TShip* ship) {
    TScriptShip* script_object = ship->script_ship;
    if (!script_object) return "";
    wchar_t* faction_str = script_object->custom_faction;
    if (faction_str) return convert.to_bytes(faction_str);
    else return "";
}
DLLEXPORT
int CustomArtCostCalc(
    int cost
) {
    cost = Rnd(cost - 500, cost + 500);
    double TL_coef = -4.0 + GalaxyTechLevel();
    if (TL_coef < 0) TL_coef = 0;
    TL_coef = 0.5 * TL_coef + 1.0;
    double Diff_coef = 0.85 + 0.15 * trunc(GalaxyDiffLevels(1) / 50 - 1);
    cost = BankRound(TL_coef * Diff_coef * cost);
    cost = cost + Rnd(-50, 100);
    if (cost <= 0) cost = 1;
    return cost;
}
DLLEXPORT
int CustomArtSizeCalc(
    int size
) {
    size = BankRound(0.01 * GalaxyDiffLevels(4) * size) + Rnd(0, 7);
    if (size <= 0) size = 1;
    return size;
}
DLLEXPORT
int RndObject(int min, int max, void* obj) {
    int obj_type = ObjectType(obj);
    int* rnd_seed = nullptr;
    switch (obj_type) {
        case(t_ObjShip):
        case(t_ObjStation):
            rnd_seed = &((TShip*)obj)->rnd_seed;
            break;
        case(t_ObjPlanet):
            rnd_seed = &((TPlanet*)obj)->rnd_seed;
            break;
        case(t_ObjStar):
            rnd_seed = &((TStar*)obj)->rnd_seed;
            break;
        default:
            SFT("UtilityFunctions.dll RndObject Warning! Wrong type of object was received!");
            return 0;
    }
    unsigned int seed;
    if (Galaxy->AA_enabled && Galaxy->AA_rnd_chaotic) seed = rand();
    else {
        seed = *rnd_seed;
        unsigned int prev_seed = seed;
        seed = seed * 7981 + 567 + seed / 7981;
        if (prev_seed == seed) {
            seed = seed * 7281 + 517 + seed / 7181;
        }
        *rnd_seed = seed;
    }
    if (min < max) return min + int(seed % (max - min + 1));
    else return max + int(seed % (min - max + 1));
}
DLLEXPORT
uint32_t GetPlanetOrbitProbe(TPlanet* planet, int orbit_num, int probe_get) {
    if (orbit_num > -1) {
        if (orbit_num >= planet->orbit_cnt) return 0;
        for (int i = 0; i < Player->satellites->count; ++i) {
            TSatellite* probe = (TSatellite*)Player->satellites->items[i];
            if (probe->planet == planet && probe->orbit_num == orbit_num) {
                if (probe_get) {
                    probe->planet = 0;
                    TListDelete(Player->satellites, i);
                }
                return (uint32_t)probe;
            }
        }
        return 0;
    }
    return planet->orbit_cnt;
}
DLLEXPORT
uint32_t ShipNearbyShips(
    TShip* ship,
    int dist_num,
    int req_type,
    int types_set,
    wchar_t* types_str,
    wchar_t* custom_faction
) {
    if (dist_num < -1) return 0;
    int obj_type = ObjectType(ship);
    if (obj_type != t_ObjShip && obj_type != t_ObjStation) {
        SFT("UtilityFunctions.dll ShipNearbyShips function error. First argument is not a ship or station.");
        throw;
    }
    if (!ShipInNormalSpace(ship)) return 0;
    if (!req_type) {
        switch (types_set) {
            case 0:
                types_set = 16383;
                break;
            case -1:
                types_set = 63;
                break;
            case -2:
                types_set = 16320;
                break;
        }
        ShipsNearby.clear();
        ShipsNearby.push_back({ ship, 0 });
        TStar* star = ShipStar(ship);
        for (int i = 0; i < StarShips(star); ++i) {
            TShip* cur_ship = StarShips(star, i);
            if (cur_ship == ship || !ShipInNormalSpace(cur_ship)) continue;
            if (!(types_set & (1 << ShipTypeN(cur_ship)))) continue;
            if (types_str != nullptr) {
                string types_filt = "," + convert.to_bytes(types_str) + ",";
                if (types_filt.find("," + ShipType(cur_ship) + ",") == string::npos) continue;
            }
            if (custom_faction != nullptr) {
                string faction_filt = "," + convert.to_bytes(custom_faction) + ",";
                string faction = "," + ShipCustomFaction(cur_ship) + ",";
                if (faction == ",," || faction == ",SubFactionFixedStanding,") faction = ",!,";
                if (faction_filt.find(faction) == string::npos) continue;
            }
            ShipsNearby.push_back({ cur_ship, DistShips(ship, cur_ship) });
        }
        if (ShipsNearby.size() > 1) ShipsNearbySort(ShipsNearby, 0, ShipsNearby.size() - 1);
    }
    if (dist_num == -1) return ShipsNearby.size() - 1;
    if (dist_num > ShipsNearby.size() - 1) return 0;
    return (uint32_t)ShipsNearby[dist_num].Ship;
}
unsigned int GetShipScriptData(TShip* ship, int data_num) {
    if (data_num < 0 || data_num > 3) return 0;
    if (ship == Player) return 0;
    TScriptShip* ship_script_obj = ship->script_ship;
    if (!ship_script_obj) return 0;
    return ship_script_obj->data[data_num];
}
void SetShipScriptData(TShip* ship, int data_num, unsigned int new_val) {
    if (data_num < 0 || data_num > 3) return;
    if (ship == Player) return;
    TScriptShip* ship_script_obj = ship->script_ship;
    if (!ship_script_obj) return;
    ship_script_obj->data[data_num] = new_val;
}
int GetScriptGroupBaseState(TScriptGroup* group) {
    return group->state;
}
DLLEXPORT
const wchar_t* GetShipScriptName(
    TShip* ship,
    int to_return
) {
    if (ship == Player) return L"";
    int obj_type = ObjectType(ship);
    if (obj_type != t_ObjShip && obj_type != t_ObjStation) {
        SFT("UtilityFunctions.dll GetShipScriptName function error. First argument is not a ship or station.");
        throw;
    }
    TScriptShip* ship_script_obj = ship->script_ship;
    if (ship_script_obj) {
        TScript* script = ship_script_obj->script;
        if (!to_return) return script->name + 7;
        TScriptGroup* group = (TScriptGroup*)script->groups->items[ship_script_obj->group];
        if (to_return == 1) return group->name;
        TScriptState* state = ship_script_obj->state;
        if (to_return == 2) return state->name;
        static wchar_t* out = nullptr;
        if (out) delete[] out;
        out = new wchar_t[wcslen(script->name + 7) + wcslen(group->name) + wcslen(state->name) + 3];
        wcscpy(out, script->name + 7);
        wcscat(out, L".");
        wcscat(out, group->name);
        wcscat(out, L".");
        wcscat(out, state->name);
        return out;
    } else return L"";
}
DLLEXPORT
void ShipJoinToScript(
    TShip* ship,
    wchar_t* path
) {
    if (ship == Player) return;
    int obj_type = ObjectType(ship);
    if (obj_type != t_ObjShip && obj_type != t_ObjStation) {
        SFT("UtilityFunctions.dll ShipJoinToScript function error. First argument is not a ship or station.");
        throw;
    }
    wchar_t sep[] = L".";
    TScript* script = GetScriptByName(GetParFromStringLocal(path, sep, 0, 0));
    if (!script) {
        SFT("UtilityFunctions.dll Warning! ShipJoinToScript cannot find script by it's name. Path is: " + convert.to_bytes(path));
        return;
    }
    TScriptGroup* group = GetScriptGroupByName(script, GetParFromStringLocal(path, sep, 1, 0));
    if (!group) {
        SFT("UtilityFunctions.dll Warning! ShipJoinToScript cannot find script group by it's name. Path is: " + convert.to_bytes(path));
        return;
    }
    TScriptState* state;
    string state_name = GetParFromStringLocal(path, sep, 2, 0);
    if (state_name != "") {
        state = GetScriptStateByName(script, state_name);
        if (!state) {
            state = GetScriptStateByNum(script, GetScriptGroupBaseState(group));
            SFT("UtilityFunctions.dll Warning! ShipJoinToScript cannot find script state by it's name. Ship will be added in base group state. Path is: " + convert.to_bytes(path));
        }
    } else state = GetScriptStateByNum(script, GetScriptGroupBaseState(group));
    TScriptShip* ship_script_obj = ship->script_ship;
    TScript* old_script = 0;
    if (ship_script_obj) {
        old_script = ship_script_obj->script;
        if (script != old_script) {
            TList* ship_script_objs_list = old_script->ships;
            for (int i = 0; i < ship_script_objs_list->count; ++i) {
                TScriptShip* cur_obj = (TScriptShip*)ship_script_objs_list->items[i];
                if (cur_obj == ship_script_obj) {
                    TListDelete(ship_script_objs_list, i);
                    break;
                }
                if (i == ship_script_objs_list->count - 1) {
                    SFT("UtilityFunctions.dll ShipJoinToScript Warning! Can't find ship script object in it's script. Path is: " + convert.to_bytes(path));
                    return;
                }
            }
        } else {
            if (group == GetScriptGroupByNum(script, ship_script_obj->group) && state == ship_script_obj->state) {
                SFT("UtilityFunctions.dll ShipJoinToScript Warning! This ship is already in target script group and target script state. Path is: " + convert.to_bytes(path));
                return;
            }
        }
        if (group != GetScriptGroupByNum(script, ship_script_obj->group)) {
            for (int i = 0; i <= 3; ++i) SetShipScriptData(ship, i, 0);
            ship_script_obj->custom_faction = 0;
            ship_script_obj->end_order = 0;
            ship_script_obj->hit = 0;
            ship_script_obj->hit_player = 0;
        }
    } else {
        int sz = *(int*)(int(vmt_TScriptShip) - 40);
        char *tstr = *(char**)(int(vmt_TScriptShip) - 44);
        ship_script_obj = (TScriptShip*)GetMem(sz);
        *(VMT_TScriptShip**)ship_script_obj = vmt_TScriptShip;
        for (int i = 4; i < sz; ++i) ((char*)ship_script_obj)[i] = 0;
        ship_script_obj->ship = ship;
    }
    if (script != old_script) TListAdd(script->ships, ship_script_obj);
    ship_script_obj->script = script;
    ship_script_obj->group = GetScriptGroupNum(script, group);
    ship_script_obj->state = state;
    ship->script_ship = ship_script_obj;
}
DLLEXPORT
const wchar_t* GetScriptNoKlingMarksFromStar(
    TStar* star
) {
    if (!star) {
        SFT("UtilityFunctions.dll GetScriptNoKlingMarksFromStar function error. Argument is a null.");
        throw;
    }
    if (ObjectType(star) != t_ObjStar) {
        SFT("UtilityFunctions.dll GetScriptNoKlingMarksFromStar function error. Argument is not a star.");
        throw;
    }
    string scripts_list = "";
    for (int i = 0; i < Galaxy->scripts->count; ++i) {
        TScript* script = (TScript*)Galaxy->scripts->items[i];
        for (int j = 0; j < script->script_stars->count; ++j) {
            TScriptStar* script_star = (TScriptStar*)script->script_stars->items[j];
            if (script_star->star != star) continue;
            int mark_type = 0;
            if (script_star->no_kling) mark_type += 1;
            if (script_star->no_come_kling) mark_type += 2;
            if (!mark_type) continue;
            if (scripts_list != "") scripts_list += ",";
            scripts_list += convert.to_bytes(script->name + 7) + "." + to_string(mark_type);
        }
    }
    if (scripts_list != "") {
        to_game = convert.from_bytes(scripts_list);
        return to_game.c_str();
    } else return L"";
}
DLLEXPORT
int ShipSubrace(
    TShip* ship,
    int sub_race
) {
    int obj_type = ObjectType(ship);
    if (obj_type != t_ObjShip && obj_type != t_ObjStation) {
        SFT("UtilityFunctions.dll ShipSubrace function error. First argument is not a ship or station.");
        throw;
    }
    if (ShipTypeN(ship) != t_Kling) {
        return -1;
    }
    if (sub_race <= -1) return ((TKling*)ship)->sub_race;
    int old_sub_race = ((TKling*)ship)->sub_race;
    ((TKling*)ship)->sub_race = sub_race;
    return old_sub_race;
}
DLLEXPORT
uint32_t IdToHole(
    int id
) {
    if (id <= 0) {
        SFT("UtilityFunctions.dll IdToHole function error. Invalid Id number was received.");
        throw;
    }
    for (int i = 0; i < Galaxy->holes->count; ++i) {
        THole* hole = (THole*)Galaxy->holes->items[i];
        if (hole->id == id) return (uint32_t)hole;
    }
    return 0;
}
DLLEXPORT
const wchar_t* GetParFromTxt(
    wchar_t* txt_path,
    wchar_t* s_param_name
) {
    string param_name;
    string cur_line;
    if (s_param_name == nullptr) param_name = "";
    else param_name = convert.to_bytes(s_param_name);
    string path = system_folder;
    path += "\\" + convert.to_bytes(txt_path);
    ifstream file(path, ios::in);
    if (file) {
        int check = 0;
        while (getline(file, cur_line)) {
            int sep = cur_line.find("=");
            if (cur_line.substr(0, sep) == param_name) {
                cur_line = cur_line.substr(sep + 1);
                to_game = convert.from_bytes(cur_line);
                check = 1;
                return to_game.c_str();
            }
        }
        if (!check) return L"";
        file.close();
    } else return 0;
}
DLLEXPORT
const wchar_t* SetParFromTxt(
    wchar_t* txt_path,
    wchar_t* s_param_name,
    wchar_t* value
) {
    string param_name;
    string cur_line;
    string add_line;
    if (s_param_name == nullptr) param_name = "";
    else param_name = convert.to_bytes(s_param_name);
    string path = system_folder;
    path += "\\" + convert.to_bytes(txt_path);
    ifstream file(path, ios::in);
    string str_value = convert.to_bytes(value);
    string old_value = "";
    if (file) {
        int check = 0;
        while (getline(file, cur_line)) {
            int sep = cur_line.find("=");
            if (cur_line.substr(0, sep) == param_name) {
                old_value = cur_line.substr(sep + 1);
                to_game = convert.from_bytes(old_value);
                if (add_line != "") add_line += "\n";
                add_line += param_name + "=" + str_value;
                check = 1;
            } else {
                if (add_line != "") add_line += "\n";
                add_line += cur_line;
            }
        }
        if (!check) {
            if (add_line != "") add_line += "\n";
            add_line += param_name + "=" + str_value;
        }
        file.close();
    } else add_line = param_name + "=" + str_value;
    ofstream file_add(path, ios::out);
    file_add << add_line;
    file_add.close();
    if (old_value != "") return to_game.c_str();
    else return L"";
}
DLLEXPORT
int IsShiftCtrlPressed() {
    if ((GetAsyncKeyState(VK_SHIFT) & 0x8000) && (GetAsyncKeyState(VK_CONTROL) & 0x8000)) {
        return 1;
    }
    return 0;
}
DLLEXPORT
void SendStringToClipboard(wchar_t* from_game) {
    // wstring to_clipboard = from_game;
    // if (OpenClipboard(GetActiveWindow())) {
    //     HGLOBAL hgBuffer;
    //     char* chBuffer;
    //     EmptyClipboard();
    //     hgBuffer = GlobalAlloc(GMEM_DDESHARE, to_clipboard.length() + 1);
    //     chBuffer = (char*)GlobalLock(hgBuffer);
    //     strcpy_s(chBuffer, to_clipboard.length() + 1, LPCSTR(to_clipboard.c_str()));
    //     GlobalUnlock(hgBuffer);
    //     SetClipboardData(CF_TEXT, hgBuffer);
    //     CloseClipboard();
    // }
}
DLLEXPORT
const wchar_t* GetStringFromClipboard() {
    // wstring from_clipboard;
    // if (OpenClipboard(GetActiveWindow())) {
    //     HANDLE hData = GetClipboardData(CF_TEXT);
    //     char* chBuffer = (char*)GlobalLock(hData);
    //     from_clipboard = chBuffer;
    //     GlobalUnlock(hData);
    //     CloseClipboard();
    // }
    // to_game = convert.from_bytes(from_clipboard);
    // return to_game.c_str();
    return nullptr;
}
