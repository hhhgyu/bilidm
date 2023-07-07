#pragma once

#ifndef BLiveDM_MessageType_H_
#define BLiveDM_MessageType_H_

#include <ArduinoJson.h>
#include <string>
#include <vector>
enum class Operation
{
  HANDSHAKE = 0,
  HANDSHAKE_REPLY = 1,
  HEARTBEAT = 2,       // 心跳包
  HEARTBEAT_REPLY = 3, // 心跳包回复（人气值）
  SEND_MSG = 4,
  SEND_MSG_REPLY = 5, // 普通包（命令）
  DISCONNECT_REPLY = 6,
  AUTH = 7,       // 认证包
  AUTH_REPLY = 8, // 认证包（回复）
  RAW = 9,
  PROTO_READY = 10,
  PROTO_FINISH = 11,
  CHANGE_ROOM = 12,
  CHANGE_ROOM_REPLY = 13,
  REGISTER = 14,
  REGISTER_REPLY = 15,
  UNREGISTER = 16,
  UNREGISTER_REPLY = 17
};
enum class ProtocolVersion
{
  NORMAL = 0,
  HEARTBEAT = 1,
  DEFLATE = 2,
  BROTLI = 3
};
enum class AuthReplyCode
{
  OK = 0,
  TOKEN_ERROR = -101
};

/* 弹幕 */
struct DanmakuMessage
{
  // 弹幕显示模式（滚动、顶部、底部）
  int mode;
  // 字体尺寸
  int font_size;
  // 颜色
  int color;
  // 时间戳（毫秒）
  int timestamp;
  // 随机数，前端叫作弹幕ID，可能是去重用的
  int rnd;
  // 用户ID文本的CRC32
  String uid_crc32;
  // 是否礼物弹幕（节奏风暴）
  int msg_type;
  // 右侧评论栏气泡
  int bubble;
  // 弹幕类型，0文本，1表情，2语音
  int dm_type;
  // 表情参数
  String emoticon_options;
  // 语音参数
  String voice_config;
  // 一些附加参数
  String mode_info;

  // 弹幕内容
  String msg;

  // 用户ID
  int uid;
  // 用户名
  String uname;
  // 是否房管
  int admin;
  // 是否月费老爷
  int vip;
  // 是否年费老爷
  int svip;
  // 用户身份，用来判断是否正式会员，猜测非正式会员为5000，正式会员为10000
  int urank;
  // 是否绑定手机
  int mobile_verify;
  // 用户名颜色
  String uname_color;

  // 勋章等级
  String medal_level;
  // 勋章名
  String medal_name;
  // 勋章房间主播名
  String runame;
  // 勋章房间ID
  int medal_room_id;
  // 勋章颜色
  int mcolor;
  // 特殊勋章
  String special_medal;

  // 用户等级
  int user_level;
  // 用户等级颜色
  int ulevel_color;
  // 用户等级排名，>50000时为'>50000'
  String ulevel_rank;

  // 旧头衔
  String old_title;
  // 头衔
  String title;

  // 舰队类型，0非舰队，1总督，2提督，3舰长
  int privilege_type;

  static bool Check(JsonVariant);
  DanmakuMessage(JsonVariant);
};

/*
    礼物消息
  */
struct GiftMessage
{
  // 礼物名
  String gift_name;
  // 数量
  int num;
  // 用户名
  String uname;
  // 用户头像URL
  String face;
  // 舰队等级，0非舰队，1总督，2提督，3舰长
  int guard_level;
  // 用户ID
  int uid;
  // 时间戳
  int timestamp;
  // 礼物ID
  int gift_id;
  // 礼物类型（未知）
  int gift_type;
  // 目前遇到的有'喂食'、'赠送'
  String action;
  // 礼物单价瓜子数
  int price;
  // 随机数，可能是去重用的。有时是时间戳+去重ID，有时是UUID
  String rnd;
  // 瓜子类型，'silver'或'gold'，1000金瓜子 = 1元
  String coin_type;
  // 总瓜子数
  int total_coin;
  // 可能是事务ID，有时和rnd相同
  String tid;

  static bool Check(JsonVariant);
  GiftMessage(JsonVariant);
};

/*上舰消息*/
struct GuardBuyMessage
{

  // 用户ID
  int uid;
  // 用户名
  String username;
  // 舰队等级，0非舰队，1总督，2提督，3舰长
  int guard_level;
  // 数量
  int num;
  // 单价金瓜子数
  int price;
  // 礼物ID
  int gift_id;
  // 礼物名
  String gift_name;
  // 开始时间戳，和结束时间戳相同
  int start_time;
  // 结束时间戳，和开始时间戳相同
  int end_time;

  static bool Check(JsonVariant);
  GuardBuyMessage(JsonVariant);
};

  /*
    醒目留言消息
    */
struct SuperChatMessage
{

  // 价格（人民币）
  int price;
  // 消息
  String message;
  // 消息日文翻译（目前只出现在SUPER_CHAT_MESSAGE_JPN）
  String message_trans;
  // 开始时间戳
  int start_time;
  // 结束时间戳
  int end_time;
  // 剩余时间（约等于 结束时间戳 - 开始时间戳）
  int time;
  // 醒目留言ID，删除时用
  int id;
  // 礼物ID
  int gift_id;
  // 礼物名
  String gift_name;
  // 用户ID
  int uid;
  // 用户名
  String uname;
  // 用户头像URL
  String face;
  // 舰队等级，0非舰队，1总督，2提督，3舰长
  int guard_level;
  // 用户等级
  int user_level;
  // 底部背景色，'#rrggbb'
  String background_bottom_color;
  // 背景色，'#rrggbb'
  String background_color;
  // 背景图标
  String background_icon;
  // 背景图URL
  String background_image;
  // 背景价格颜色，'#rrggbb'
  String background_price_color;

  static bool Check(JsonVariant);
  SuperChatMessage(JsonVariant);
};

/*
删除醒目留言消息
*/
struct SuperChatDeleteMessage
{// 醒目留言ID数组
  std::vector<int>  ids;
  static bool Check(JsonVariant);
  SuperChatDeleteMessage(JsonVariant);
};

#endif // BLiveDM_MessageType_H_