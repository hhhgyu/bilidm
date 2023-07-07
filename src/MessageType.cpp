#include "MessageType.h"
bool DanmakuMessage::Check(JsonVariant json)
{
    String cmd = json["cmd"].as<String>();
    return cmd.indexOf("DANMU_MSG") > -1;
}
DanmakuMessage::DanmakuMessage(JsonVariant json)
{
    auto info = json["info"];
    int medal_level = 0;
    String medal_name = "";
    String runame = "";
    int room_id = 0;
    int mcolor = 0;
    int special_medal = 0;
    auto info3 = info[3].as<JsonArray>();
    if (!info3.isNull() && info3.size() > 0)
    {
        medal_level = info3[0];
        medal_name = info3[1].as<String>();
        runame = info3[2].as<String>();
        room_id = info3[3];
        mcolor = info3[4];
        special_medal = info3[5];
    }
    mode = info[0][1];
    font_size = info[0][2];
    color = info[0][3];
    timestamp = info[0][4];
    rnd = info[0][5];
    uid_crc32 = info[0][7].as<String>();
    msg_type = info[0][9];
    bubble = info[0][10];
    dm_type = info[0][12];
    emoticon_options = info[0][13].as<String>();
    voice_config = info[0][14].as<String>();
    mode_info = info[0][15].as<String>();

    msg = info[1].as<String>();

    uid = info[2][0];
    uname = info[2][1].as<String>();
    admin = info[2][2];
    vip = info[2][3];
    svip = info[2][4];
    urank = info[2][5];
    mobile_verify = info[2][6];
    uname_color = info[2][7].as<String>();

    this->medal_level = medal_level;
    this->medal_name = medal_name;
    this->runame = runame;
    this->medal_room_id = room_id;
    this->mcolor = mcolor;
    this->special_medal = special_medal;

    user_level = info[4][0];
    ulevel_color = info[4][2];
    ulevel_rank = info[4][3].as<String>();

    old_title = info[5][0].as<String>();
    title = info[5][1].as<String>();

    privilege_type = info[7];
}

bool GiftMessage::Check(JsonVariant json)
{
    String cmd = json["cmd"].as<String>();
    return cmd.indexOf("SEND_GIFT") > -1;
}

GiftMessage::GiftMessage(JsonVariant json)
{
    JsonObject data = json["data"];
    gift_name = data["giftName"].as<String>();
    num = data["num"];
    uname = data["uname"].as<String>();
    face = data["face"].as<String>();
    guard_level = data["guard_level"];
    uid = data["uid"];
    timestamp = data["timestamp"];
    gift_id = data["giftId"];
    gift_type = data["giftType"];
    action = data["action"].as<String>();
    price = data["price"];
    rnd = data["rnd"].as<String>();
    coin_type = data["coin_type"].as<String>();
    total_coin = data["total_coin"];
    tid = data["tid"].as<String>();
}

bool GuardBuyMessage::Check(JsonVariant json)
{
    String cmd = json["cmd"].as<String>();
    return cmd.indexOf("GUARD_BUY") > -1;
}
GuardBuyMessage::GuardBuyMessage(JsonVariant json)
{
    JsonObject data = json["data"];
    uid = data["uid"];
    username = data["username"].as<String>();
    guard_level = data["guard_level"];
    num = data["num"];
    price = data["price"];
    gift_id = data["gift_id"];
    gift_name = data["gift_name"].as<String>();
    start_time = data["start_time"];
    end_time = data["end_time"];
}

bool SuperChatMessage::Check(JsonVariant json)
{
    String cmd = json["cmd"].as<String>();
    return cmd.indexOf("SUPER_CHAT_MESSAGE") > -1;
}
SuperChatMessage::SuperChatMessage(JsonVariant json)
{
    JsonObject data = json["data"];
    price = data["price"];
    message = data["message"].as<String>();
    message_trans = data["message_trans"].as<String>();
    start_time = data["start_time"];
    end_time = data["end_time"];
    time = data["time"];
    id = data["id"];
    gift_id = data["gift"]["gift_id"];
    gift_name = data["gift"]["gift_name"].as<String>();
    uid = data["uid"];
    uname = data["user_info"]["uname"].as<String>();
    face = data["user_info"]["face"].as<String>();
    guard_level = data["user_info"]["guard_level"];
    user_level = data["user_info"]["user_level"];
    background_bottom_color = data["background_bottom_color"].as<String>();
    background_color = data["background_color"].as<String>();
    background_icon = data["background_icon"].as<String>();
    background_image = data["background_image"].as<String>();
    background_price_color = data["background_price_color"].as<String>();
}

bool SuperChatDeleteMessage::Check(JsonVariant json)
{
    String cmd = json["cmd"].as<String>();
    return cmd.indexOf("SUPER_CHAT_MESSAGE_DELETE") > -1;
}
SuperChatDeleteMessage::SuperChatDeleteMessage(JsonVariant json)
{
    JsonObject data = json["data"];
    JsonArray arr = data["ids"];
    for (auto &&i : arr)
    {
        ids.push_back(i);
    }
}
