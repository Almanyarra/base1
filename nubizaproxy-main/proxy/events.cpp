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
bool blink = false;
std::string message = "";
std::string mode = "pull";
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
     else if (find_command(chat, "setmsg ")) {
       message = chat.substr(7);
       gt::send_log("Set Message to"+ message); 
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
        
        else if (find_command(chat, "uid ")) {
            std::string name = chat.substr(5);
            gt::send_log("resolving uid for " + name);
            g_server->send(false, "action|input\n|text|/ignore /" + name);
            g_server->send(false, "action|friends");
            gt::resolving_uid2 = true;
            return true;

        } else if (find_command(chat, "tp ")) {
            std::string name = chat.substr(4);
            std::transform(name.begin(), name.end(), name.begin(), ::tolower);
            for (auto& player : g_server->m_world.players) {
                auto name_2 = player.name.substr(2); //remove color
                std::transform(name_2.begin(), name_2.end(), name_2.begin(), ::tolower);
                if (name_2.find(name) == 0) {
                    gt::send_log("`#Teleporting to " + player.name);
                    variantlist_t varlist{ "OnSetPos" };
                    varlist[1] = player.pos;
                    g_server->m_world.local.pos = player.pos;
                    g_server->send(true, varlist, g_server->m_world.local.netid, -1);
                    break;
                }
            }
         

         
        } else if (find_command(chat, "warp ")) {
            std::string name = chat.substr(6);
            gt::send_log("`#Warping to " + name);
            g_server->send(false, "action|join_request\nname|" + name, 3);
            return true;
          
      } else if (find_command(chat, "door ")) {
            std::string worldname = g_server->m_world.name.c_str();
            std::string idkntl = chat.substr(6);
            g_server->send(false, "action|join_request\nname|" + worldname + "|" + idkntl, 3);
            return true;

           } else if (find_command(chat, "pullall")) {
            std::string username = chat.substr(6);
            for (auto& player : g_server->m_world.players) {
                auto name_2 = player.name.substr(2); //remove color
                if (name_2.find(username)) {
                    g_server->send(false, "action|wrench\n|netid|" + std::to_string(player.netid));
                    std::this_thread::sleep_for(std::chrono::milliseconds(5));
                    g_server->send(false, "action|dialog_return\ndialog_name|popup\nnetID|" + std::to_string(player.netid) + "|\nbuttonClicked|pull"); 
                    // You Can |kick |trade |worldban 
                    std::this_thread::sleep_for(std::chrono::milliseconds(5));
                    gt::send_log("`4Pulling all people");
                  
                }
            }
} else if (find_command(chat, "killall")) {
            std::string username = chat.substr(6);
            for (auto& player : g_server->m_world.players) {
                auto name_2 = player.name.substr(2); //remove color
                if (name_2.find(username)) {
                    g_server->send(false, "action|wrench\n|netid|" + std::to_string(player.netid));
                    std::this_thread::sleep_for(std::chrono::milliseconds(5));
                    g_server->send(false, "action|dialog_return\ndialog_name|popup\nnetID|" + std::to_string(player.netid) + "|\nbuttonClicked|kick"); 
                    // You Can |kick |trade |worldban 
                    std::this_thread::sleep_for(std::chrono::milliseconds(5));
                    gt::send_log("`4Kill All People in world");
                  
                }
            }
} else if (find_command(chat, "tradeall")) {
            std::string username = chat.substr(6);
            for (auto& player : g_server->m_world.players) {
                auto name_2 = player.name.substr(2); //remove color
                if (name_2.find(username)) {
                    g_server->send(false, "action|wrench\n|netid|" + std::to_string(player.netid));
                    std::this_thread::sleep_for(std::chrono::milliseconds(5));
                    g_server->send(false, "action|dialog_return\ndialog_name|popup\nnetID|" + std::to_string(player.netid) + "|\nbuttonClicked|trade"); 
                    // You Can |kick |trade |worldban 
                    std::this_thread::sleep_for(std::chrono::milliseconds(5));
                    gt::send_log("`4Trade all people in world");
                  
                }
            }
} else if (find_command(chat, "banall")) {
            std::string username = chat.substr(6);
            for (auto& player : g_server->m_world.players) {
                auto name_2 = player.name.substr(2); //remove color
                if (name_2.find(username)) {
                    g_server->send(false, "action|wrench\n|netid|" + std::to_string(player.netid));
                    std::this_thread::sleep_for(std::chrono::milliseconds(5));
                    g_server->send(false, "action|dialog_return\ndialog_name|popup\nnetID|" + std::to_string(player.netid) + "|\nbuttonClicked|worldban"); 
                    // You Can |kick |trade |worldban 
                    std::this_thread::sleep_for(std::chrono::milliseconds(5));
                    gt::send_log("`4Banned all people in world");
                  
                }
            }

} else if (find_command(chat, "msgall")) {
           std::string msgtext = "              Message from FakeModz YT";
            std::string username = chat.substr(6);
            for (auto& player : g_server->m_world.players) {
                auto name_2 = player.name.substr(2); //remove color
                if (name_2.find(username)) {
                  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                  g_server->send(false, "action|input\n|text|/msg "  +        player.name         +                   msgtext);
                 // std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                 // g_server->send(false, "action|input\n|text|/msg "  +        player2.name         +                   msgtext);
                  //std::this_thread::sleep_for(std::chrono::milliseconds(200));
                 // gt::send_log("`4Message all people in world");
                  
                }
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

        else if (find_command(chat, "blink")) {
            blink = !blink;
            if (blink) {
                gt::send_log("`2Enabled `9Blink Mode");
            }
            else gt::send_log("`4Disabled `9Blink Mode");

            std::thread([&]() {
                const int c_color[8] = { 1685231359, 1348237567, 2190853119, 2022356223, 2864971775, 2527912447, 3370516479, 3033464831 };
                while (blink) {
                    for (auto index = 0; index < 8; index++) {
                        std::this_thread::sleep_for(std::chrono::milliseconds(100));
                        string bsend = toString(c_color[index]);
                        g_server->send(false, "action|setSkin\ncolor|" + bsend);
                        std::this_thread::sleep_for(std::chrono::milliseconds(900));
                    }
                }
                }).detach();

                return true;

        
        } else if (find_command(chat, "proxy")) {
           // gt::send_log(
            //    "`2/tp [name] (teleports to a player in the world), /ghost (toggles ghost, you wont move for others when its enabled), /uid "
            //    "`2[name] (resolves name to uid), /flag [id] (sets flag to item id), /name [name] (sets name to name), /banall, /kickall, /tradeall"
            //    "`2/warp [world name] (warping world without SSUP), /skin [Id] (change skin colours), /wrenchmode (for wrench pull, kick, pull, ban, trade)"
           //     "`2/ft (fast trash), /fd (fast drop), /setcountry (bug), /wrenchset (for set wrenchmode : pull,kick,ban,trade,add friend),/msgall (bug), /pinfo"
            //    "`2/wrenchmsg (Auto Msg Wrench People), /setmsg (for set message text)");
           std::string paket1;
            paket1 =
                "\nadd_label_with_icon|big|Proxy Commands Gazette|left|20|"
                "\nadd_image_button|banner|interface/large/news_banner.rttex|bannerlayout|||"
                "\nadd_spacer|small"
                "\nadd_textbox|`2/tp [name] (teleports to a player in the world)|left|2480|"
                "\nadd_textbox|`2/ghost (toggles ghost, you wont move for others when its enabled)|left|2480|"
                "\nadd_textbox|`2/uid [name] (resolves name to uid)|left|2480|"
                "\nadd_textbox|`2/flag [id] (sets flag to item id)|left|2480|"
                "\nadd_textbox|`2/name [name] (Change Name Visual)|left|2480|"
                "\nadd_textbox|`2/banall (World Ban All People in world)|left|2480|"
                "\nadd_textbox|`2/killall (Kick all People in world)|left|2480|"
                "\nadd_textbox|`2/tradeall (trade all people in the world|left|2480|"
                "\nadd_textbox|`2/warp [world name] (warping world without SSUP)|left|2480|"
                "\nadd_textbox|`2/skin [Id] (change skin colours)|left|2480|"
                "\nadd_textbox|`2/wrenchmode (wrench modefor wrench pull, kick, pull, ban, trade, add)|left|2480|"
                "\nadd_textbox|`2/wrenchset (for set wrenchmode : pull,kick,ban,trade,add friend)|left|2480|"
                "\nadd_textbox|`2/ft (fast trash) |left|2480|"
                "\nadd_textbox|`2/fd (fast drop) |left|2480|"
                "\nadd_textbox|`2/wrenchmsg (Auto Msg when wrench people) |left|2480|"
                "\nadd_textbox|`2/setmsg (Costum Text for Wrenchmsg and wrenchspam) |left|2480|"
                "\nadd_textbox|`2/country (/countrylist for check list)|left|2480|"
                "\nadd_textbox|`2/msgall (not really worked because spam detected) |left|2480|"
                "\nadd_textbox|`2/wrenchspam (wrench spam like wrench msg do/setspam for set text) |left|2480|"
                "\nadd_textbox|`2/automsg (auto msg when people enter world) |left|2480|"
                "\nadd_textbox|`2/door (teleport to id door (you must know the id door)) |left|2480|"
                "\nadd_textbox|`2/pinfo (Proxy information) |left|2480|"
                "\nadd_textbox|`2/countrylist (List Country For /country) |left|2480|"
                "\nadd_textbox|`2/autopull (auto pull when people enter world) |left|2480|"
                "\nadd_textbox|`2/pullauto (auto pull for casino hoster) |left|2480|"
                "\nadd_spacer|small|\n\nadd_url_button||`$YouTube``|NOFLAGS|https://youtube.com/c/FakeModzGT|Open link?|0|0|"
                "\nadd_spacer|small|\n\nadd_url_button||`$Discord``|NOFLAGS|https://discord.com/invite/YfnMbjWjpP|Open link?|0|0|"
                "\nadd_spacer|small|\n\nadd_url_button||`$INSTAGRAM``|NOFLAGS|https://instagram.com/FakeModz.yt|Open link?|0|0|"
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
        auto hash_str = 
