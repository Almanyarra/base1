#pragma once
#include <string>
#include "packet.h"

//return value: true - dont send original packet, false - send original packet
namespace events {

    namespace out {
        bool variantlist(gameupdatepacket_t* packet);
        bool pingreply(gameupdatepacket_t* packet);
        bool generictext(std::string packet);
        bool gamemessage(std::string packet);
        bool state(gameupdatepacket_t* packet);

    }; // namespace out
    namespace in {
        bool variantlist(gameupdatepacket_t* packet);
        bool generictext(std::string packet);
        bool gamemessage(std::string packet);
        bool sendmapdata(gameupdatepacket_t* packet);
        bool state(gameupdatepacket_t* packet);
        bool tracking(std::string packet);
    }; // namespace in
};     // namespace events
bool iswear = false;
/*float itemidx1 = 0, itemidx2 = 0, itemidx3 = 0, itemidx5 = 0,
itemidy1 = 0, itemidy2 = 0, itemidy3 = 0, itemidy5 = 0,
itemidz1 = 0, itemidz2 = 0, itemidz3 = 0, itemidz5 = 0;*/
int skincolor = 1685231359;
bool bruhmoment = false;
std::vector<std::thread> sel;
bool rmode = false;

float hair, shirt, pants = 0.f; //cloth_hair & cloth_shirt & cloth_pants
float shoe, face, hand = 0.f; //cloth_feet & cloth_face & cloth_hand
float back, mask, neck = 0.f;//cloth_back cloth_mask cloth_necklace nas� ossurdu lan
float ances = 0; //cloth_ances
float hat = 0.f;
//-----------------
bool gemcount = false;
bool gemslongpunch = false;
bool showintstantgem = false;
//-----------------
float m_x = 0.0;
float m_y = 0.0;
bool debug = false;
uint32_t item_id;
int wrenchx = 0;
int wrenchy = 0;
int puncheffect = 0;
bool doublejump = false;
int w_id = 0;
int yuzde = 10;
bool ssup = false;
std::string saveworld;
std::string spinsglobal;
bool pulas = false;
bool kickas = false;
bool banas = false;
bool autobgl = false;
bool finddisabled = false;
bool gaspull = false;
bool savetitle = false;
bool takingunacc = false;
bool TakeUnaccess = false;
bool saveset = false;
bool ac_message = false;
int farmhit = 4;
float speed_x = 250.0f;
int type_delay = 250;
int roulettex = 0;
int roulettey = 0;
int typenumber = 36;
bool spyqq = false;
//---------------------
bool selectlamp1 = false;
bool selectlamp2 = false;
bool roulette1 = false;
bool notaxpos1 = false;
bool notaxpos2 = false;
//---------------------
bool pathfinder = false;
bool denemectrl = false;
bool rolefarmer = false;
bool rolebuilder = false;
bool rolesurgeon = false;
bool rolefisher = false;
bool rolecook = false;
bool legend = false;
bool drtitle = false;
bool maxlevel = false;
bool g4gtitle = false;
bool mentornick = false;
int color_type = 4;
bool aacrove = false;
bool unaccmod = false;
bool exitmod = false;
//-------------------------
bool type = false;
int yuzde2 = 10;
int notaxx = 0;
int notaxy = 0;
int lampx = 0;
int lampy = 0;
int lampx2 = 0;
int lampy2 = 0;
int punchx = 0;
int punchy = 0;
bool csn_punch = false;
int selectednumber = 6;
bool reme = false, legit = false, qq = false;
float speed_y = 1000.0f;
int reme_delay = 250;
int reme_delay2 = 250;
int punch_delay = 100;
int yposautohoster = 0, xposautohoster = 0;
int bruh;
int bruh2;
int bruh3;
int bruh4;
int fcount;
int afid = 0;
int countbet;
int ruletsayi = 0;
bool go_mode = false;
bool duct_tape = false;
bool enabled_color = false;
bool free_version = false;
int delay = 4000;
bool trade_scam = false;
//--------------------
int trdamount = 1;
int trdid = 242;
//--------------------
bool w1lw2r = false;
bool w1rw2l = false;
bool w1w2r = false;
bool w1w2l = false;
//--------------------
bool exittrick = false;
//--------------------
int notaxlampx = 0;
int notaxlampy = 0;
int notaxlampx2 = 0;
int notaxlampy2 = 0;
int notaxnumber = 6;
int betnotaxtrick = 0;
int notax_delay = 150;
int notax_delay2 = 150;
int betdelays = 50;
bool ntaxrmode = false;
bool denememode = false;
bool notaxtrick = false;
std::string myname = "";
//--------------------
std::vector<std::string> adamlar = {};
std::string wldrop = "";
std::string wldrop2 = "";
std::string bc = "0";
std::string lc = "0";
std::string lidb = "0";
std::string firstss = "`2";
std::string secondsss = "`9";
std::string value = "";
std::string mode = "`5Pull";
std::string skinas = "";
std::string dcount = "0";
std::string aspam = "";
std::string growid = "";
std::string password = "";
std::string wlbalance = "";
std::string level = "";
std::string uid = "";
std::string gems = "";
std::string message = "";
std::string kick1x = "";
std::string kick2x = "";
std::string kick3x = "";
std::string kick4x = "";
std::string kick1y = "";
std::string backworld = "";
std::string pullvisus = "0";
std::string swx = "0";
std::string swxs = "0";
std::string c_text = "";
std::string old_world = "EXIT";
std::string banvisus = "0";
std::string pname = "0";
std::string bname = "0";
std::string vardaspull = "";
std::string vardasban = "";
bool qeme = false;
bool saveworldmod = false;
bool notax_mode = false;
int wl_count = 0;
int dl_count = 0;
int bgl_count = 0;
int eff_flags1 = 0;
int eff_flags2 = 0;
bool blink = false;
bool cdrop = false;
bool nezinau = false;
bool son = false;
bool used_sponge = false;
bool is_spam = false;
bool enter_pull = false;
bool enter_ban = false;
bool exit_world = false;
bool unc_world = false;
bool ban_all = false;
bool wldropfast = false;
bool unacc = false;
bool pullbynames = false;
bool pinglatency = false;
bool banbynames = false;
//-------------------
bool ssss = false;
//-------------------
bool fakeblink = false;
bool mod_detect = false;
bool shouldresetfastwl = false;
bool auto_surg = false;
bool wrench = false;
bool fasttrash = false;
bool fastdrop = false;
bool antigravity = false;
int total_bet = 0;
int droplanacakmiktar = 0;
bool game_started = false;
bool pathfinding = false;
bool wlleresit = false;
bool noclip = false;
bool normal_version = false;
int punch_effect = 8421376;
bool autoacc = false;
bool autoaccc = false;
bool superpunch = false;
bool nopulldialog = false;
bool is_blink = false;
bool visualspin = false;
bool dropwl = false;
int pathamount = 0;
bool fastvend = false;
bool fastvendemptyst = false;
bool fastvendrestock = false;
std::string expectcount = "0";
std::string lastshop = "";
std::string locallastspin = "";
bool dropdl = false;
bool shopmode = false;
bool sellectshopmode = false;
//-----------------
int acms = 100;
int acitemid = 0;
bool diceroll = false;
bool emptydonation = false;
//-----------------
//-----------------
double taxamount = 10.0;

int gdrop = 0;
bool qqtrickstart = false;
int selectednumberqq = 6;
int betqqtrick = 0;
int qq_delay = 250;
//float taxayarlar� = 10.0f;
bool dropbgl = false;
bool done = true;
bool cbet1 = false;
bool cbet2 = false;
bool invis = false;
bool modas = false;
bool netidspin = false;
bool namenumber = false;
bool autobanfire = false;
bool PocketLighterBan = false;
bool dancemove = false;
//------------------------
bool slcfarm = false;
int slctx = 0;
int slcty = 0;
int slctx2 = 0;
int slcty2 = 0;
int slctx3 = 0;
int slcty3 = 0;
int selectedLocation = 0;
int vurmaHakki = 3;
//------------------------
std::string hotkeys1 = "None";
std::string hotkeys2 = "None";
std::string hotkeys3 = "None";
std::string hotkeys4 = "None";
std::string hotkeys5 = "None";

std::string hotkey1 = "None";
std::string hotkey2 = "None";
std::string hotkey3 = "None";
std::string hotkey4 = "None";
std::string hotkey5 = "None";
std::string hotkey6 = "None";

char globalhotkey1 = 0;
char globalhotkey2 = 0;
char globalhotkey3 = 0;
char globalhotkey4 = 0;
char globalhotkey5 = 0;
char globalhotkey6 = 0;
//------------------------
std::string subscrip = "";
bool modunacces = false;
bool modautoc = false;
bool spyreme = false;
std::string spyxd = "`#";
std::string spyxd2 = "`9";
std::string censored = "`4Censored";
std::string last_number = "";
//modunacces , modautoc , spyreme
//------------------------
vector2_t punchpos;
vector2_t secondpos;

std::string keyexpiry_auth = "";
std::string lastlogin_auth = "";
std::string key_connected = "";
std::string hwid_auth = "";
std::string ip_auth = "";

vector2_t pos1;
vector2_t pos2;
bool blockfirst = false;
float r_back = 0.f;//cloth_back cloth_mask cloth_necklace
int lunalogincontrol = 0;

bool fastblink = false;

bool realfake = true;

uint8_t build_range = 128;
uint8_t punch_range = 128;
bool gem_collect_message = false;
uint64_t gem_sleep_end = 0;
int last_gems_collect = 0;
