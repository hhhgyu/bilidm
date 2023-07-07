#ifndef BLiveDM_BLiveDMClient_H_
#define BLiveDM_BLiveDMClient_H_

#include "decode.h"
#include "MessageType.h"
#include "TypeJsonConverter.hpp"
#include <string>
#include <map>
#include <tuple>
#include <ArduinoJson.h>
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <WiFiClientSecure.h>
#include <WebSocketsClient.h>
#include <HTTPClient.h>

using std::tuple;

class BLiveDMClient
{
private:
  int _tmp_room_id;
  int _uid;
  int _heartbeat_interval;
  
  WebSocketsClient webSocket;
  int retry_count = 0;

  void LogPrintln(String);
  void LogPrintln(const char*);
public:
  bool print_log = false;
  
  bool init_room_success = false;
  bool connect_success = false;
  int room_id;
  int room_short_id;
  int room_owner_uid;
  std::vector<std::map<std::string, std::string>> _host_server_list;
  String _host_server_token;
  String error_state=""; 

  std::function<void(Operation, std::string&)> handler;

  BLiveDMClient(int room_id, int uid=0, int heartbeat_interval = 30, bool ssl = false);
  ~BLiveDMClient();

  std::vector<uint8_t> encode_packet(uint16_t protocol_version, Operation operation, const std::string &body);
  std::vector< tuple<Operation,std::string> > parse_packet(const uint8_t *data, size_t data_len);
  bool init_room();
  bool _init_room_id_and_owner();
  bool _init_host_server();
  void Start();
  void Loop();
  void send_auth();
  void send_heartbeat();
  void webSocketEventHandler(WStype_t type, uint8_t *payload, size_t length);
};

#endif // BLiveDM_BLiveDMClient_H_