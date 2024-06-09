#include "events.h"
#include "gt.hpp"
#include "proton/hash.hpp"
#include "proton/rtparam.hpp"
#include "proton/variant.hpp"
#include "server.h"
#include "utils.h"
#include <thread>
#include <limits.h>
#include <stdio.h> /* printf, NULL */ 
#include <stdlib.h> /* srand, rand */ 
#include <time.h> /* time */
#include <curl/curl.h>
using namespace std;

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

bool events::out::variantlist(gameupdatepacket_t* packet) {
    variantlist_t varlist{};
    varlist.serialize_from_mem(utils::get_extended(packet));
    PRINTS("varlist: %s\n", varlist.print().c_str());
    return false;
}

bool events::out::pingreply(gameupdatepacket_t* packet) {
    //since this is a pointer we do not need to copy memory manually again
    packet->m_vec2_x = 1000.f;  //gravity
    packet->m_vec2_y = 250.f;   //move speed
    packet->m_vec_x = 64.f;     //punch range
    packet->m_vec_y = 64.f;     //build range
    packet->m_jump_amount = 0;  //for example unlim jumps set it to high which causes ban
    packet->m_player_flags = 0; //effect flags. good to have as 0 if using mod noclip, or etc.
    return false;
}

bool find_command(std::string chat, std::string name) {
    bool found = chat.find("/" + name) == 0;
    if (found)
        gt::send_log("`6" + chat);
    return found;
}
bool wrench = false;
bool fastdrop = false;
bool fasttrash = false;
bool wrenchmsg = false; 
bool wrenchspam = false; 
bool automessage = false; 
bool autopull = false;
bool pullauto = false; 
bool setmsg = false;
std::string message = "";
std::string mode = "pull";
// visual tittles bool
bool legend = false;
bool drtitle = false;
bool maxlevel = false;
bool g4gtitle = false;
bool mentornick = false;
// finish
bool events::out::generictext(std::string packet) {
    PRINTS("Generic text: %s\n", packet.c_str());
    auto& world = g_server->m_world;
    rtvar var = rtvar::parse(packet);
    if (!var.valid())
        return false;
        if (wrench == true) {
            if (packet.find("action|wrench") != -1) {
                g_server->send(false, packet);
                std::string sr = packet.substr(packet.find("netid|") + 6, packet.length() - packet.find("netid|") - 1);
                std::string motion = sr.substr(0, sr.find("|"));
                if (mode.find("pull") != -1) {
                    g_server->send(false, "action|dialog_return\ndialog_name|popup\nnetID|" + motion + "|\nnetID|" + motion + "|\nbuttonClicked|pull");
                }
                if (mode.find("kick") != -1) {
                    g_server->send(false, "action|dialog_return\ndialog_name|popup\nnetID|" + motion + "|\nnetID|" + motion + "|\nbuttonClicked|kick");
                }
                if (mode.find("ban") != -1) {
                    g_server->send(false, "action|dialog_return\ndialog_name|popup\nnetID|" + motion + "|\nnetID|" + motion + "|\nbuttonClicked|worldban");
                }
                if (mode.find("trade") != -1) {
                    g_server->send(false, "action|dialog_return\ndialog_name|popup\nnetID|" + motion + "|\nnetID|" + motion + "|\nbuttonClicked|trade");
                }
                if (mode.find("add") != -1) {
                    g_server->send(false, "action|dialog_return\ndialog_name|popup\nnetID|" + motion + "|\nnetID|" + motion + "|\nbuttonClicked|friend_add");
                }
                return true;
            }
        }
    if (var.get(0).m_key == "action" && var.get(0).m_value == "input") {
        if (var.size() < 2)
            return false;
        if (var.get(1).m_values.size() < 2)
            return false;

        if (!world.connected)
            return false;

        auto chat = var.get(1).m_values[1];
        if (find_command(chat, "name ")) { //ghetto solution, but too lazy to make a framework for commands.
            std::string name = "``" + chat.substr(6) + "``";
            variantlist_t va{ "OnNameChanged" };
            va[1] = name;
            g_server->send(true, va, world.local.netid, -1);
            gt::send_log("`#name set to: " + name);
            return true;
        } else if (find_command(chat, "flag ")) {
            int flag = atoi(chat.substr(6).c_str());
            variantlist_t va{ "OnGuildDataChanged" };
            va[1] = 1;
            va[2] = 2;
            va[3] = flag;
            va[4] = 3;
            g_server->send(true, va, world.local.netid, -1);
            gt::send_log("flag set to item id: " + std::to_string(flag));
            return true;
        } else if (find_command(chat, "ghost")) {
            gt::ghost = !gt::ghost;
            if (gt::ghost)
                gt::send_log("`#Ghost is now enabled.");
            else
                gt::send_log("`#Ghost is now disabled.");
            return true;
        } else if (find_command(chat, "country ")) {
            std::string cy = chat.substr(9);
            gt::flag = cy;
            gt::send_log("`#your country set to " + cy + ", (Relog to game to change it successfully!)");
            return true;
        }
        else if (find_command(chat, "fd")) {
            fastdrop = !fastdrop;
            if (fastdrop)
                gt::send_log("`#Fast Drop is now enabled.");
            else
                gt::send_log("`#Fast Drop is now disabled.");
            return true;
        }
        else if (find_command(chat, "ft")) {
            fasttrash = !fasttrash;
            if (fasttrash)
                gt::send_log("`#Fast Trash is now enabled.");
            else
                gt::send_log("`#Fast Trash is now disabled.");
            return true;
        }        
        else if (find_command(chat, "wrenchmsg")) {
            wrenchmsg = !wrenchmsg;
            if (wrenchmsg)
                gt::send_log("`#wrenchmsg is now enabled.");
            else
                gt::send_log("`#wrenchmsg is now disabled.");
            return true;
         } 
             else if (find_command(chat, "automsg")) {
            automessage = !automessage;
            if (automessage)
                gt::send_log("`#automsg is now enabled.");
            else
                gt::send_log("`#automsg is now disabled.");
            return true;
          } 
             else if (find_command(chat, "autopull")) {
            autopull = !autopull;
            if (autopull)
                gt::send_log("`#autopull is now enabled.");
            else
                gt::send_log("`#autopull is now disabled.");
            return true;
         } 
            else if (find_command(chat, "pullauto")) {
            pullauto = !pullauto;
            if (pullauto)
                gt::send_log("`#Pull Auto is now enabled.");
            else
                gt::send_log("`#Pull Auto is now disabled.");
            return true;
         }                  
        else if (find_command(chat, "wrenchspam")) {
            wrenchspam = !wrenchspam;
            if (wrenchspam)
                gt::send_log("`#wrenchspam is now enabled.");
            else
                gt::send_log("`#wrenchspam is now disabled.");
            return true;
        }  


        
        else if (find_command(chat, "wrenchset ")) {
            mode = chat.substr(10);
            gt::send_log("`#Wrench mode set to " + mode);
            return true;        
        }
        else if (find_command(chat, "wrenchmode")) {
            wrench = !wrench;
            if (wrench)
                gt::send_log("`#Wrench mode is on.");
            else
                gt::send_log("`#Wrench mode is off.");
            return true;
         }
        


         
            else if (find_command(chat, "warp ")) {
            std::string name = chat.substr(6);
            gt::send_log("`#Warping to " + name);
            g_server->send(false, "action|join_request\nname|" + name, 3);
            return true;
          
	}
            return true;
        } else if (find_command(chat, "skin ")) {
            int skin = atoi(chat.substr(6).c_str());
            variantlist_t va{ "OnChangeSkin" };
            va[1] = skin;
            g_server->send(true, va, world.local.netid, -1);
            return true;
        } else if (find_command(chat, "wrench ")) {
            std::string name = chat.substr(6);
            std::string username = ".";
            for (auto& player : g_server->m_world.players) {
                auto name_2 = player.name.substr(2);
                std::transform(name_2.begin(), name_2.end(), name_2.begin(), ::tolower);
                    g_server->send(false, "action|wrench\n|netid|" + std::to_string(player.netid));
            }
            return true;
    }
	    // visuallerin başlangıcı
	        else if (find_command(chat, "mentor")) {
            std::string mn = "|showGuild|master";

            variantlist_t mentor{ "OnCountryState" };
            mentor[1] = mn.c_str();
            g_server->send(true, mentor, g_server->m_world.local.netid, -1);
            return true;
        }
        else if (find_command(chat, "maxlevel")) {
            std::string packet125level = "us|showGuild|maxLevel";

            variantlist_t packet123{ "OnCountryState" };
            packet123[1] = packet125level.c_str();
            g_server->send(true, packet123, g_server->m_world.local.netid, -1);
            return true;
        }
        else if (find_command(chat, "g4g")) {
            std::string packet125level = "us|showGuild|donor";

            variantlist_t packet123{ "OnCountryState" };
            packet123[1] = packet125level.c_str();
            g_server->send(true, packet123, g_server->m_world.local.netid, -1);
            return true;
	}
	        if (find_command(chat, "legend")) {
            auto& visuals = g_server->m_world.local;
            variantlist_t va{ "OnNameChanged" };
            va[1] = "``" + visuals.name + " of Legend``";
            g_server->send(true, va, world.local.netid, -1);
            //gt::send_log("name set to: " + tittle + " of Legend");
            return true;
        }
        else if (find_command(chat, "doctor")) {
            auto& visuals = g_server->m_world.local;
            variantlist_t va{ "OnNameChanged" };
            va[1] = "`4Dr." + visuals.name;
            g_server->send(true, va, world.local.netid, -1);

            std::string packetdoctor = "mm|showGuild|doctor";
            variantlist_t doctor{ "OnCountryState" };
            doctor[1] = packetdoctor.c_str();
            g_server->send(true, doctor, g_server->m_world.local.netid, -1);
            return true;
        }
        else if (find_command(chat, "moderator")) {
            auto& visuals = g_server->m_world.local;
            variantlist_t va{ "OnNameChanged" };
            va[1] = "`#@" + visuals.name;
            g_server->send(true, va, world.local.netid, -1);

            std::string packetmod = "|showGuild|";
            variantlist_t mod{ "OnCountryState" };
            mod[1] = packetmod.c_str();
            g_server->send(true, mod, g_server->m_world.local.netid, -1);
            return true;
            //visuals end


        } else if (find_command(chat, "proxy")) {
           // gt::send_log(
            //    "`2/tp [name] (teleports to a player in the world), /ghost (toggles ghost, you wont move for others when its enabled), /uid "
            //    "`2[name] (resolves name to uid), /flag [id] (sets flag to item id), /name [name] (sets name to name), /banall, /kickall, /tradeall"
            //    "`2/warp [world name] (warping world without SSUP), /skin [Id] (change skin colours), /wrenchmode (for wrench pull, kick, pull, ban, trade)"
           //     "`2/ft (fast trash), /fd (fast drop), /setcountry (bug), /wrenchset (for set wrenchmode : pull,kick,ban,trade,add friend),/msgall (bug), /pinfo"
            //    "`2/wrenchmsg (Auto Msg Wrench People), /setmsg (for set message text)");
           std::string paket1;
            paket1 =
		"\nset_default_color|"
                "\nstart_custom_tabs|"
                "\ndisable_resize|"
                "\nreset_placement_x|"
                "\nend_custom_tabs|"
                "\nadd_textbox|`9 `8Welcome To SF Proxy 1.0 left|2480|"
                "\nadd_spacer|small|"
                "\nadd_label_with_icon|small|`wSF Proxy 1.0 Commands|left|828|"
                "\nadd_spacer|small"
                "\nadd_smalltext|`b >> `0: `#Proxy Founder: `0estoxd|left|"
                "\nadd_smalltext|`b >> `0: `#Proxy Co-Founder: `0osimaisback `#and `0sevmemseni |left|"
                "\nadd_spacer|small"
                "\nadd_spacer|small"
                "\nadd_label_with_icon|big|`2Main Commands|left|1900|"
                "\nadd_smalltext|`9Command :`0/options `w(toogles options page)|left|2480|"
                "\nadd_smalltext|`9Command :`0/logs `w(toogles logs page)|left|2480|"
                "\nadd_smalltext|`9Command :`0/ac `w(autoCollect)|left|2480|"
                "\nadd_smalltext|`9Command :`0/collect `w(collect 1 Times)|left|2480|"
                "\nadd_smalltext|`9Command :`0/wrench `w(toogles wrench page)|left|2480|"
                "\nadd_smalltext|`9Command :`0/autosurg `w(enables auto surgery)|left|2480|"
                "\nadd_smalltext|`9Command :`0/bgl `w(auto exchange blue gem lock)|left|2480|"
                "\nadd_smalltext|`9Command :`0/spam `w(toogle spam page)|left|2480|"
                "\nadd_smalltext|`9Command :`0// `w(start auto spam)|left|2480|"
                "\nadd_smalltext|`9Command :`0/pf `w(open/close pathfinder) (use with shift)|left|2480|"
                "\nadd_smalltext|`9Command :`0/world `w(toogles world page)|left|2480|"
                "\nadd_smalltext|`9Command :`0/lastspin `w(show last spin)|left|2480|"
                "\nadd_spacer|small"
                "\nadd_label_with_icon|small|`2Casino Commands|left|758|"
                "\nadd_smalltext|`9Command :`0/reme `w(enable/disable reme spin)|left|2480|"
                "\nadd_smalltext|`9Command :`0/qq `w(enable/disable qq spin)|left|2480|"
                "\nadd_smalltext|`9Command :`0/game [bet] `w(calculate tax)|left|2480|"
                "\nadd_smalltext|`9Command :`0/gd `w(drop wls with tax [/game [bet] ])|left|2480|"
                "\nadd_smalltext|`9Command :`0/tax `w(Set Tax)|left|2480|"
                "\nadd_smalltext|`9Command :`0/pos1 `w(set pos1 position)|left|2480|"
                "\nadd_smalltext|`9Command :`0/pos2 `w(set pos2 position)|left|2480|"
                "\nadd_smalltext|`9Command :`0/tp `w(collect pos1,pos2 wls&dls)|left|2480|"
                "\nadd_smalltext|`9Command :`0/win1 `w(teleport To 1. pos and drop taxed wls)|left|2480|"
                "\nadd_smalltext|`9Command :`0/win2 `w(teleport To 2. pos and drop taxed wls)|left|2480|"
                "\nadd_smalltext|`9Command :`0/mode `w(sellect game mode)|left|2480|"
                "\nadd_smalltext|`9Command :`0/cd & /cdrop `w(custom drop)|left|2480|"
                "\nadd_smalltext|`9Command :`0/dd & /ddrop `w(drop dls)|left|2480|"
                "\nadd_smalltext|`9Command :`0/dropbgl `w(drop bgl)|left|2480|"
                "\nadd_smalltext|`9Command :`0/dropwl `w(drop wl)|left|2480|"
                "\nadd_smalltext|`9Command :`0/daw `w(drop all locks)|left|2480|"
                "\nadd_smalltext|`9Command :`0/gp `w(enable/disable gass pul)|left|2480|"
                "\nadd_smalltext|`9Command :`0/setsave [world name] `w(Set Save World)|left|2480|"
                "\nadd_smalltext|`9Command :`0/save `w(Warp Save World)|left|2480|"
                "\nadd_smalltext|`9Command :`0/acces `w(enable/disable auto acc)|left|2480|"
                "\nadd_spacer|small"
                "\nadd_label_with_icon|small|`2Visual Commands|left|8996|"
                "\nadd_smalltext|`9Command :`0/find [item name] `w(Find Item Name For Visual Clothes)|left|2480|"
                "\nadd_smalltext|`9Command :`0/clothes `w(toogle clothes page)|left|2480|"
                "\nadd_smalltext|`9Command :`0/weather `w(toogle weather page)|left|2480|"
                "\nadd_smalltext|`9Command :`0/title `w(toogle titles page)|left|2480|"
                "\nadd_smalltext|`9Command :`0/g4g `w(G4G title)|left|2480|"
                "\nadd_smalltext|`9Command :`0/maxlevel `w(MaxLevel title)|left|2480|"
                "\nadd_smalltext|`9Command :`0/fakeres `w(fake respawn)|left|2480|"
                "\nadd_smalltext|`9Command :`0/mentor `w(Mentor Title)|left|2480|"
                "\nadd_smalltext|`9Command :`0/legend `w(Legend Title)|left|2480|"
                "\nadd_spacer|small"
                "\nadd_label_with_icon|small|`2Other Commands|left|32|"
                "\nadd_smalltext|`9Command :`0/name `w(change you name)|left|2480|"
                "\nadd_smalltext|`9Command :`0/dmove `w(enable/disable dance move)|left|2480|"
                "\nadd_smalltext|`9Command :`0/superpunch `w(enable/disable super punch)|left|2480|"
                "\nadd_smalltext|`9Command :`0/res `w(fast respawn)|left|2480|"
                "\nadd_smalltext|`9Command :`0/lastpull `w(pull last joined world)|left|2480|"
                "\nadd_smalltext|`9Command :`0/showxy `w(show your xy)|left|2480|"
                "\nadd_smalltext|`9Command :`0/speed `w(change speed and gravity)|left|2480|"
                "\nadd_smalltext|`9Command :`0/fastvend `w(open/close fastvend)|left|2480|"
                "\nadd_smalltext|`9Command :`0/antigravity `w(normal antigravity)|left|2480|"
                "\nadd_smalltext|`9Command :`0/vendcount `w(set vend count)|left|2480|"
                "\nadd_smalltext|`9Command :`0/debug `w(debug mode)|left|2480|"
                "\nadd_smalltext|`9Command :`0/split [%] `w(Split Your WLs)|left|2480|"
                "\nadd_smalltext|`9Command :`0/count `w(set count for fast drop & trash)|left|2480|"
                "\nadd_smalltext|`9Command :`0/growscan & /gscan `w(normal growscan)|left|2480|"
                "\nadd_smalltext|`9Command :`0/speed `w(set your speed)|left|2480|"
                "\nadd_smalltext|`9Command :`0/testmod `w(mod detect test)|left|2480|"
                "\nadd_smalltext|`9Command :`0/back `w(warp previously entered world)|left|2480|"
                "\nadd_smalltext|`9Command :`0/relog `w(relog world)|left|2480|"
                "\nadd_smalltext|`9Command :`0/warp [world name] `w(like super supporter)|left|2480|"
                "\nadd_smalltext|`9Command :`0/balance `w(see your wls&dls&bgls)|left|2480|"
                "\nadd_smalltext|`9Command :`0/auto `w(toogle autu page menu)|left|2480|"
                "\nadd_smalltext|`9Command :`0/blink `w(fast skin changer)|left|2480|"
                "\nadd_smalltext|`9Command :`0/autosurg `w(open/close autosurg)|left|2480|"
                "\nadd_smalltext|`9Command :`0/clist `w(see country list)|left|2480|"
                "\nadd_smalltext|`9Command :`0/country [id] `w(change your country)|left|2480|"
                "\nadd_smalltext|`9Command :`0/ptp [playername] `w(tp player)|left|2480|"
                "\nadd_smalltext|`9Command :`0/gems `w(See Gems in the World)|left|2480|"
                "\nadd_spacer|small"
                "\nadd_smalltext|`4SF Proxy in Beta For This You Can Get `bShadowban `9or `bBan `9For This Dont Use Your Main Account|left|828|"
                "\nadd_spacer|small|\n\nadd_url_button||`$Discord``|NOFLAGS|https://discord.com/invite/DYfwdqxXHx|Open link?|0|0|"
                "\nadd_quick_exit|"
                "\nend_dialog|end|Cancel|Okay|";
            variantlist_t liste{ "OnDialogRequest" };
            liste[1] = paket1;
            g_server->send(true, liste);
            
            return true;
        } 
        return false;
    }

    if (packet.find("game_version|") != -1) {
        rtvar var = rtvar::parse(packet);
        auto mac = utils::generate_mac();
        auto hash_str = mac + "RT";
        auto hash2 = utils::hash((uint8_t*)hash_str.c_str(), hash_str.length());
	    CURL *curl;
        std::string readBuffer;
        struct curl_slist *headers = NULL;
        curl = curl_easy_init();
        if(curl) {
            std::string postfields = string("version=") + gt::version + string("&platform=4&protocol=191");
            headers = curl_slist_append(headers, "User-Agent: UbiServices_SDK_2017.Final.21_ANDROID64_static");
            curl_easy_setopt(curl, CURLOPT_URL, "https://www.growtopia1.com/growtopia/server_data.php");
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postfields.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
            curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, 5000);
            curl_easy_perform(curl);
            curl_easy_cleanup(curl);
            rtvar var1 = rtvar::parse(readBuffer);

            if (var1.find("meta")) {
                g_server->meta = var1.get("meta");
                g_server->m_server = var1.get("server");
                g_server->m_port = var1.get_int("port");
                PRINTS("Meta: %s\n\n", g_server->meta.c_str());
            }
        }
        var.set("mac", mac);
        var.set("gid", utils::generate_rid());
        var.set("meta", g_server->meta);
        var.set("game_version", gt::version);
        var.set("country", gt::flag);
        packet = var.serialize();
        gt::in_game = false;
        PRINTS("Spoofing login info\n");
        g_server->send(false, packet);
        return true;
    }

    return false;
}

bool events::out::gamemessage(std::string packet) {
    PRINTS("Game message: %s\n", packet.c_str());
    if (packet == "action|quit") {
        g_server->quit();
        return true;
    }

    return false;
}

bool events::out::state(gameupdatepacket_t* packet) {
    if (!g_server->m_world.connected)
        return false;

    g_server->m_world.local.pos = vector2_t{ packet->m_vec_x, packet->m_vec_y };
    PRINTS("local pos: %.0f %.0f\n", packet->m_vec_x, packet->m_vec_y);

    if (gt::ghost)
        return true;
    return false;
}

bool events::in::variantlist(gameupdatepacket_t* packet) {
    variantlist_t varlist{};
    auto extended = utils::get_extended(packet);
    extended += 4; //since it casts to data size not data but too lazy to fix this
    varlist.serialize_from_mem(extended);
    PRINTC("varlist: %s\n", varlist.print().c_str());
    auto func = varlist[0].get_string();

    //probably subject to change, so not including in switch statement.
    if (func.find("OnSuperMainStartAcceptLogon") != -1)
        gt::in_game = true;

    switch (hs::hash32(func.c_str())) {

        case fnv32("OnRequestWorldSelectMenu"): {
            auto& world = g_server->m_world;
            world.players.clear();
            world.local = {};
            world.connected = false;
            world.name = "EXIT";
        } break;
        case fnv32("OnSendToServer"): g_server->redirect_server(varlist); return true;

        case fnv32("OnConsoleMessage"): {
            varlist[1] = " `b[`9SF Proxy`b]```$ " + varlist[1].get_string();
            auto cnsl = varlist[1].get_string();
          g_server->send(true, varlist);
       return true;
//std::string console = varlist[1].get_string();
//std::transform(console.begin(),console.end(),console.begin(),::tolower);

    if(pullauto == true) {
              g_server->send(true, varlist);
                if (cnsl.find("Skem") != -1) {
                    if (cnsl.find("`$") != -1) {
                        std::string nasmasma = cnsl.substr(cnsl.find("[W]_ `6<`") + 10, cnsl.length() - cnsl.find("[W]_ `6<`") - 1);
                        nasmasma.erase(nasmasma.begin() + nasmasma.find("``>``"), nasmasma.end());
                        g_server->send(false, "action|input\n|text|bapak kau " + nasmasma);
                    }
                } else if (cnsl.find("scam") != -1) {
                    if (cnsl.find("`w") != -1) {
                        std::string nasmasma = cnsl.substr(cnsl.find("[W]_ `6<`w") + 10, cnsl.length() - cnsl.find("[W]_ `6<`w") - 1);
                        nasmasma.erase(nasmasma.begin() + nasmasma.find("``>``"), nasmasma.end());
                        g_server->send(false, "action|input\n|text|/ban " + nasmasma);
                    }
                } else if (cnsl.find("dont") != -1) {
                    if (cnsl.find("`w") != -1) {
                        std::string nasmasma = cnsl.substr(cnsl.find("[W]_ `6<`w") + 10, cnsl.length() - cnsl.find("[W]_ `6<`w") - 1);
                        nasmasma.erase(nasmasma.begin() + nasmasma.find("``>``"), nasmasma.end());
                        g_server->send(false, "action|input\n|text|/ban " + nasmasma);
                    }
                } else if (cnsl.find("Scam") != -1) {
                    if (cnsl.find("`w") != -1) {
                        std::string nasmasma = cnsl.substr(cnsl.find("[W]_ `6<`w") + 10, cnsl.length() - cnsl.find("[W]_ `6<`w") - 1);
                        nasmasma.erase(nasmasma.begin() + nasmasma.find("``>``"), nasmasma.end());
                        g_server->send(false, "action|input\n|text|/ban " + nasmasma);
                    }
                } else if (cnsl.find("SCam") != -1) {
                    if (cnsl.find("`w") != -1) {
                        std::string nasmasma = cnsl.substr(cnsl.find("[W]_ `6<`w") + 10, cnsl.length() - cnsl.find("[W]_ `6<`w") - 1);
                        nasmasma.erase(nasmasma.begin() + nasmasma.find("``>``"), nasmasma.end());
                        g_server->send(false, "action|input\n|text|/ban " + nasmasma);
                    }
                } else if (cnsl.find("noob") != -1) {
                    if (cnsl.find("`w") != -1) {
                        std::string nasmasma = cnsl.substr(cnsl.find("[W]_ `6<`w") + 10, cnsl.length() - cnsl.find("[W]_ `6<`w") - 1);
                        nasmasma.erase(nasmasma.begin() + nasmasma.find("``>``"), nasmasma.end());
                        g_server->send(false, "action|input\n|text|/ban " + nasmasma);
                    }
                } else if (cnsl.find("lol") != -1) {
                    if (cnsl.find("`w") != -1) {
                        std::string nasmasma = cnsl.substr(cnsl.find("[W]_ `6<`w") + 10, cnsl.length() - cnsl.find("[W]_ `6<`w") - 1);
                        nasmasma.erase(nasmasma.begin() + nasmasma.find("``>``"), nasmasma.end());
                        g_server->send(false, "action|input\n|text|/ban " + nasmasma);
                    }
                } else if (cnsl.find("LOL") != -1) {
                    if (cnsl.find("`w") != -1) {
                        std::string nasmasma = cnsl.substr(cnsl.find("[W]_ `6<`w") + 10, cnsl.length() - cnsl.find("[W]_ `6<`w") - 1);
                        nasmasma.erase(nasmasma.begin() + nasmasma.find("``>``"), nasmasma.end());
                        g_server->send(false, "action|input\n|text|/ban " + nasmasma);
                    }
                }           
        return true;
}
        } break;

        case fnv32("OnDialogRequest"): {
            auto content = varlist[1].get_string();
        if (wrench == true) {
            if (content.find("add_button|report_player|`wReport Player``|noflags|0|0|") != -1) {
                if (content.find("embed_data|netID") != -1) {
                    return true; // block wrench dialog
                }
            }
        }
        if (fastdrop == true) {
            std::string itemid = content.substr(content.find("embed_data|itemID|") + 18, content.length() - content.find("embed_data|itemID|") - 1);
            std::string count = content.substr(content.find("count||") + 7, content.length() - content.find("count||") - 1);
            if (content.find("embed_data|itemID|") != -1) {
                if (content.find("Drop") != -1) {
                    g_server->send(false, "action|dialog_return\ndialog_name|drop_item\nitemID|" + itemid + "|\ncount|" + count);
                    return true;
                }
            }
        }
        if (fasttrash == true) {
            std::string itemid = content.substr(content.find("embed_data|itemID|") + 18, content.length() - content.find("embed_data|itemID|") - 1);
            std::string count = content.substr(content.find("you have ") + 9, content.length() - content.find("you have ") - 1);
            std::string delimiter = ")";
            std::string token = count.substr(0, count.find(delimiter));
            if (content.find("embed_data|itemID|") != -1) {
                if (content.find("Trash") != -1) {
                    g_server->send(false, "action|dialog_return\ndialog_name|trash_item\nitemID|" + itemid + "|\ncount|" + token);
                    return true;
                }
            }
        }
       

            //hide unneeded ui when resolving
            //for the /uid command
            if (gt::resolving_uid2 && (content.find("friend_all|Show offline") != -1 || content.find("Social Portal") != -1) ||
                content.find("Are you sure you wish to stop ignoring") != -1) {
                return true;
            } else if (gt::resolving_uid2 && content.find("Ok, you will now be able to see chat") != -1) {
                gt::resolving_uid2 = false;
                return true;
            } else if (gt::resolving_uid2 && content.find("`4Stop ignoring") != -1) {
                int pos = content.rfind("|`4Stop ignoring");
                auto ignore_substring = content.substr(0, pos);
                auto uid = ignore_substring.substr(ignore_substring.rfind("add_button|") + 11);
                auto uid_int = atoi(uid.c_str());
                if (uid_int == 0) {
                    gt::resolving_uid2 = false;
                    gt::send_log("name resolving seems to have failed.");
                } else {
                    gt::send_log("Target UID: " + uid);
                    g_server->send(false, "action|dialog_return\ndialog_name|friends\nbuttonClicked|" + uid);
                    g_server->send(false, "action|dialog_return\ndialog_name|friends_remove\nfriendID|" + uid + "|\nbuttonClicked|remove");
                }
                return true;
            }
        } break;
        case fnv32("OnRemove"): {
            auto text = varlist.get(1).get_string();
            if (text.find("netID|") == 0) {
                auto netid = atoi(text.substr(6).c_str());

                if (netid == g_server->m_world.local.netid)
                    g_server->m_world.local = {};

                auto& players = g_server->m_world.players;
                for (size_t i = 0; i < players.size(); i++) {
                    auto& player = players[i];
                    if (player.netid == netid) {
                        players.erase(std::remove(players.begin(), players.end(), player), players.end());
                        break;
                    }
                }
            }
        } break;
        case fnv32("OnSpawn"): {
            std::string meme = varlist.get(1).get_string();
            rtvar var = rtvar::parse(meme);
            auto name = var.find("name");
            auto netid = var.find("netID");
            auto onlineid = var.find("onlineID");

            if (name && netid && onlineid) {
                player ply{};

                if (var.find("invis")->m_value != "1") {
                    ply.name = name->m_value;
                    ply.country = var.get("country");
                    name->m_values[0] += " `4[" + netid->m_value + "]``";
                    auto pos = var.find("posXY");
                    if (pos && pos->m_values.size() >= 2) {
                        auto x = atoi(pos->m_values[0].c_str());
                        auto y = atoi(pos->m_values[1].c_str());
                        ply.pos = vector2_t{ float(x), float(y) };
                    }
                } else {
                    ply.mod = true;
                    ply.invis = true;
                }
                if (var.get("mstate") == "1" || var.get("smstate") == "1")
                    ply.mod = true;
                ply.userid = var.get_int("userID");
                ply.netid = var.get_int("netID");
                if (meme.find("type|local") != -1) {
                    //set mod state to 1 (allows infinite zooming, this doesnt ban cuz its only the zoom not the actual long punch)
                    var.find("mstate")->m_values[0] = "1";
                    g_server->m_world.local = ply;
                }
                g_server->m_world.players.push_back(ply);
                auto str = var.serialize();
                utils::replace(str, "onlineID", "onlineID|");
                varlist[1] = str;
                PRINTC("new: %s\n", varlist.print().c_str());
                g_server->send(true, varlist, -1, -1);
                return true;
            }

            
        } break;
    }
    return false;
}

bool events::in::generictext(std::string packet) {
    PRINTC("Generic text: %s\n", packet.c_str());
    return false;
}

bool events::in::gamemessage(std::string packet) {
    PRINTC("Game message: %s\n", packet.c_str());

    if (gt::resolving_uid2) {
        if (packet.find("PERSON IGNORED") != -1) {
            g_server->send(false, "action|dialog_return\ndialog_name|friends_guilds\nbuttonClicked|showfriend");
            g_server->send(false, "action|dialog_return\ndialog_name|friends\nbuttonClicked|friend_all");
        } else if (packet.find("Nobody is currently online with the name") != -1) {
            gt::resolving_uid2 = false;
            gt::send_log("Target is offline, cant find uid.");
        } else if (packet.find("Clever perhaps") != -1) {
            gt::resolving_uid2 = false;
            gt::send_log("Target is a moderator, can't ignore them.");
        }
    }
    return false;
}

bool events::in::sendmapdata(gameupdatepacket_t* packet) {
    g_server->m_world = {};
    auto extended = utils::get_extended(packet);
    extended += 4;
    auto data = extended + 6;
    auto name_length = *(short*)data;

    char* name = new char[name_length + 1];
    memcpy(name, data + sizeof(short), name_length);
    char none = '\0';
    memcpy(name + name_length, &none, 1);

    g_server->m_world.name = std::string(name);
    g_server->m_world.connected = true;
    delete[] name;
    PRINTC("world name is %s\n", g_server->m_world.name.c_str());
    return false;
}

bool events::in::state(gameupdatepacket_t* packet) {
    if (!g_server->m_world.connected)
        return false;
    if (packet->m_player_flags == -1)
        return false;

    auto& players = g_server->m_world.players;
    for (auto& player : players) {
        if (player.netid == packet->m_player_flags) {
            player.pos = vector2_t{ packet->m_vec_x, packet->m_vec_y };
            PRINTC("player %s position is %.0f %.0f\n", player.name.c_str(), player.pos.m_x, player.pos.m_y);
            break;
        }
    }
    return false;
}

bool events::in::tracking(std::string packet) {
    PRINTC("Tracking packet: %s\n", packet.c_str());
    return true;
}
