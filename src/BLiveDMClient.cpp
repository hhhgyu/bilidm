#include "BLiveDMClient.h"

String ROOM_INIT_URL = "https://api.live.bilibili.com/xlive/web-room/v1/index/getInfoByRoom";
String DANMAKU_SERVER_CONF_URL = "https://api.live.bilibili.com/xlive/web-room/v1/index/getDanmuInfo";
std::vector<std::map<std::string, std::string>> DEFAULT_DANMAKU_SERVERS = {
    {
      {"host", "broadcastlv.chat.bilibili.com"},
      {"port", "2243"},
      {"wss_port", "443"},
      {"ws_port", "2244"}
    }
};

void BLiveDMClient::LogPrintln(String str)
{
  if (this->print_log)
  {
    Serial.println(str.c_str());
  }
}
void BLiveDMClient::LogPrintln(const char* str)
{
  if (this->print_log)
  {
    Serial.println(str);
  }
}

std::vector<uint8_t> BLiveDMClient::encode_packet(uint16_t protocol_version, Operation operation, const std::string &body)
{
  std::vector<uint8_t> packet;
  uint32_t packet_length = 16 + body.size();
  uint16_t header_length = 16;
  uint32_t sequence_id = 1;

  // Packet Length
  uint32_t packet_length_n = htonl(packet_length);
  packet.insert(packet.end(), reinterpret_cast<uint8_t *>(&packet_length_n), reinterpret_cast<uint8_t *>(&packet_length_n) + 4);

  // Header Length
  uint16_t header_length_n = htons(header_length);
  packet.insert(packet.end(), reinterpret_cast<uint8_t *>(&header_length_n), reinterpret_cast<uint8_t *>(&header_length_n) + 2);

  // Protocol Version
  uint16_t protocol_version_n = htons(protocol_version);
  packet.insert(packet.end(), reinterpret_cast<uint8_t *>(&protocol_version_n), reinterpret_cast<uint8_t *>(&protocol_version_n) + 2);

  // Operation
  uint32_t operation_n = htonl((uint32_t)operation);
  packet.insert(packet.end(), reinterpret_cast<uint8_t *>(&operation_n), reinterpret_cast<uint8_t *>(&operation_n) + 4);

  // Sequence Id
  uint32_t sequence_id_n = htonl(sequence_id);
  packet.insert(packet.end(), reinterpret_cast<uint8_t *>(&sequence_id_n), reinterpret_cast<uint8_t *>(&sequence_id_n) + 4);

  // Body
  packet.insert(packet.end(), body.begin(), body.end());

  return packet;
}
std::vector<tuple<Operation, std::string>> BLiveDMClient::parse_packet(const uint8_t *data, size_t data_len)
{
  std::vector<tuple<Operation, std::string>> messages;
  size_t offset = 0;
  while (offset < data_len)
  {
    uint32_t packet_length = ntohl(*reinterpret_cast<const uint32_t *>(data + offset));
    uint16_t header_length = ntohs(*reinterpret_cast<const uint16_t *>(data + offset + 4));
    uint16_t protocol_version = ntohs(*reinterpret_cast<const uint16_t *>(data + offset + 6));
    uint32_t operation = ntohl(*reinterpret_cast<const uint32_t *>(data + offset + 8));
    offset += header_length;
    LogPrintln("op:" + String(operation));

    //| 心跳回应 | Body 内容为房间人气值 |
    if (operation == (uint32_t)Operation::HEARTBEAT_REPLY)
    {
      uint32_t body = ntohl(*reinterpret_cast<const uint32_t *>(data + offset));
      messages.push_back(make_tuple(Operation::HEARTBEAT_REPLY, std::to_string(body)));
      break;
    }
    //| 数据类型为JSON纯文本 | 通知 | 弹幕、广播等全部信息 |
    else if (operation == (uint32_t)Operation::SEND_MSG_REPLY)
    {
      std::string body(reinterpret_cast<const char *>(data + offset), packet_length - header_length);

      switch (protocol_version)
      {
      case (uint32_t)ProtocolVersion::NORMAL:
      {
        messages.push_back(make_tuple(Operation::SEND_MSG_REPLY, body));
        break;
      }
      case (uint32_t)ProtocolVersion::BROTLI:
      {
        size_t output_length = 10 * data_len;
        uint8_t *output_buffer = new uint8_t[output_length];
        // BrotliDecoderState *brotli_state = BrotliDecoderCreateInstance(nullptr, nullptr, nullptr);
        auto result = BrotliDecoderDecompress(
            body.size(),
            reinterpret_cast<const uint8_t *>(body.data()),
            &output_length,
            output_buffer);
        // BrotliDecoderDestroyInstance(brotli_state);
        auto nested_messages = parse_packet(output_buffer, output_length);
        messages.insert(messages.end(), nested_messages.begin(), nested_messages.end());
        delete[] output_buffer;
        break;
      }
      default:
      {
        LogPrintln("Unknown protocol version: "+String(protocol_version));
        break;
      }
      }
    }
    else if (operation == (uint32_t)Operation::AUTH_REPLY)
    {
      DynamicJsonDocument bodyjson(2*data_len);
      std::string body(reinterpret_cast<const char *>(data + offset), packet_length - header_length);
      deserializeJson(bodyjson, body);
      if (bodyjson["code"] != (int)AuthReplyCode::OK)
      {
        this->error_state = "AuthError";
      }
    }
    else
    {
      LogPrintln("unknwon operation");
      return messages;
    }
    offset += (packet_length - header_length);
  }

  return messages;
}
bool BLiveDMClient::init_room()
{
  auto res = true;
  if (this->_init_room_id_and_owner())
  {
    res = false;
    this->room_id = this->room_short_id = this->_tmp_room_id;
  }
  if (this->_init_host_server())
  {
    res = false;
    this->_host_server_list = DEFAULT_DANMAKU_SERVERS;
  }
  init_room_success = true;
  return res;
}
bool BLiveDMClient::_init_room_id_and_owner()
{
  HTTPClient http;
  String URL = ROOM_INIT_URL + "?room_id=" + String(this->_tmp_room_id);
  http.begin(URL.c_str());
  http.addHeader("User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/102.0.0.0 Safari/537.36");
  int httpCode = http.GET();
  if (httpCode != 200)
  {
    LogPrintln("_init_room_id_and_owner : http get failed " + String(this->_tmp_room_id));
    return false;
  }
  String response = http.getString();
  http.end();
  DynamicJsonDocument doc(response.length()*2);
  deserializeJson(doc, response);
  JsonVariant code = doc["code"];
  if (code.isNull() || !code.is<int>() || code.as<int>() != 0)
  {
    LogPrintln("_init_room_id_and_owner : read code failed " + String(this->_tmp_room_id));
    return false;
  }
  JsonVariant room_info = doc["data"]["room_info"];
  if (room_info.isNull())
  {
    LogPrintln("_init_room_id_and_owner : read room_info failed " + String(this->_tmp_room_id));
    return false;
  }
  if (!room_info["room_id"].isNull())
  {
    this->room_id = room_info["room_id"].as<int>();
  }
  if (!room_info["short_id"].isNull())
  {
    this->room_short_id = room_info["short_id"].as<int>();
  }
  if (!room_info["uid"].isNull())
  {
    this->room_owner_uid = room_info["uid"].as<int>();
  }
  return true;
}
bool BLiveDMClient::_init_host_server()
{
  HTTPClient http;
  String URL = DANMAKU_SERVER_CONF_URL + "?id=" + String(this->room_id) + "&type=0";
  http.begin(URL.c_str());
  http.addHeader("User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/102.0.0.0 Safari/537.36");
  int httpCode = http.GET();
  if (httpCode != 200)
  {
    LogPrintln("_init_host_server : http get failed " + String(this->room_id));
    return false;
  }
  String response = http.getString();
  http.end();
  DynamicJsonDocument doc(response.length()*2);
  deserializeJson(doc, response);
  JsonVariant code = doc["code"];
  if (code.isNull() || !code.is<int>() || code.as<int>() != 0)
  {
    LogPrintln("_init_host_server : read code failed " + String(this->room_id));
    return false;
  }
  JsonVariant data = doc["data"];
  if (data.isNull())
  {
    LogPrintln("_init_host_server : read data failed " + String(this->room_id));
    return false;
  }
  if (data["host_list"].isNull())
  {
    LogPrintln("_init_host_server : read host_list failed " + String(this->room_id));
    return false;
  }
  this->_host_server_list = data["host_list"].as<std::vector<std::map<std::string, std::string>>>();
  if (!data.containsKey("token"))
  {
    LogPrintln("_init_host_server : read token failed " + String(this->room_id));
    return false;
  }
  this->_host_server_token = data["token"].as<String>();
  return true;
}
BLiveDMClient::BLiveDMClient(int room_id, int uid, int heartbeat_interval, bool ssl)
{
  this->_tmp_room_id = room_id;
  this->_uid = uid;
  this->_heartbeat_interval = heartbeat_interval;
}
void BLiveDMClient::Start()
{
  if (!init_room_success)
  {
    init_room();
  }
}
void BLiveDMClient::Loop()
{
  static bool need_try_connect = true;
  static ulong last_heartbeat_time = 0;
  if (!init_room_success)
  {
    return;
  }
  if (need_try_connect)
  {
    retry_count = retry_count % _host_server_list.size();
    auto host_server = _host_server_list[retry_count];
    webSocket.beginSSL(host_server["host"].c_str(), atoi(host_server["wss_port"].c_str()), "/sub");
    webSocket.setExtraHeaders("User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/102.0.0.0 Safari/537.36");
    auto wshandler = [this](WStype_t type, uint8_t *payload, size_t length)
    { this->webSocketEventHandler(type, payload, length); };
    webSocket.onEvent(wshandler);
    webSocket.disableHeartbeat();
    last_heartbeat_time = millis();
    need_try_connect = false;
  }
  webSocket.loop();
  if (this->error_state == "")
  {
    ulong t1 = millis();
    if ((t1 < last_heartbeat_time) || (t1 - last_heartbeat_time) >= this->_heartbeat_interval * 1000)
    {
      send_heartbeat();
      last_heartbeat_time = t1;
    }
  }
  else
  {
    if (this->error_state == "AuthError")
    {
      this->init_room();
    }
    this->error_state = "";
    retry_count++;
  }
}
void BLiveDMClient::send_auth()
{
  DynamicJsonDocument json(6000);
  // json["clientver"] = "1.6.3";
  json["platform"] = "web";
  json["protover"] = 3;
  json["roomid"] = this->room_id;
  json["uid"] = this->_uid;
  json["type"] = 2;
  if (!this->_host_server_token.isEmpty())
  {
    json["key"] = this->_host_server_token;
  }
  int packet_ver = 1;
  std::string join_request_str;
  serializeJson(json, join_request_str);
  std::vector<uint8_t> packet = encode_packet(packet_ver, Operation::AUTH, join_request_str);
  webSocket.sendBIN(packet.data(), packet.size());
  LogPrintln("send auth");
}
void BLiveDMClient::send_heartbeat()
{
  int packet_ver = 1;
  std::vector<uint8_t> packet = encode_packet(packet_ver, Operation::HEARTBEAT, "");
  webSocket.sendBIN(packet.data(), packet.size());
  LogPrintln("send_heartbeat");
}
void BLiveDMClient::webSocketEventHandler(WStype_t type, uint8_t *payload, size_t length)
{
  switch (type)
  {
  case WStype_DISCONNECTED:
    // Serial.printf("[WSc] Disconnected!\n");
    break;
  case WStype_CONNECTED:
    // Serial.printf("[WSc] Connected to url: %s\n", payload);
    this->send_auth();
    delay(200);
    break;
  case WStype_TEXT:
    // Serial.printf("[WSc] get text: %s\n", payload);
    break;
  case WStype_BIN:
  {
    LogPrintln("[WSc] get bin:" + String(length));
    auto messages = this->parse_packet(payload, length);
    for (size_t i = 0; i < messages.size(); i++)
    {
      this->handler(std::get<0>(messages[i]), std::get<1>(messages[i]));
    }
    break;
  }
  case WStype_ERROR:
  case WStype_FRAGMENT_TEXT_START:
  case WStype_FRAGMENT_BIN_START:
  case WStype_FRAGMENT:
  case WStype_FRAGMENT_FIN:
    break;
  }
  this->retry_count = 0;
}

BLiveDMClient::~BLiveDMClient()
{
  this->webSocket.disconnect();
}