#include <unistd.h>
#include <iostream>
#include <memory>
#include <map>
#include <functional>

#define D_MSG_ID_01 0x01
#define D_MSG_ID_02 0x02
#define D_MSG_ID_03 0x02

/* ここから外部提供のライブラリ(変更不可) */
extern "C" {
struct MsgBase {
  unsigned char from;
  unsigned char to;
  unsigned char msgId;
  unsigned char no_use;
  unsigned char data[12];
};

struct Msg01 {
  unsigned char from;
  unsigned char to;
  unsigned char msgId;
  unsigned char no_use;
  int comData1;
  int d2;
  int d3;
};

struct Msg02 {
  unsigned char from;
  unsigned char to;
  unsigned char msgId;
  unsigned char no_use;
  int comData1;
  int d[2];
};

struct Msg03 {
  unsigned char from;
  unsigned char to;
  unsigned char msgId;
  unsigned char no_use;
  char name[6];
  char data[6];
};

void read_msg(int fd, void *msg) {
  read(fd, msg, sizeof(struct MsgBase));
}
}
/* ここまで変更不可 */

/* ここから変更可 */

class Msg {
public:
    virtual ~Msg() {}
    virtual void proc() = 0;
};

class Msg01_impl : public Msg {
private:
    Msg01 data_;
public:
    Msg01_impl(const Msg01& data) : data_(data) {}
    void proc() override {
        std::cout << "Handling message ID 01" << std::endl;
        std::cout << "comData1: " << data_.comData1 << std::endl;
    }
};

class Msg02_impl : public Msg {
private:
    Msg02 data_;
public:
    Msg02_impl(const Msg02& data) : data_(data) {}
    void proc() override {
        std::cout << "Handling message ID 02" << std::endl;
        std::cout << "comData1: " << data_.comData1 << std::endl;
    }
};

class Msg03_impl : public Msg {
private:
    Msg03 data_;
public:
    Msg03_impl(const Msg03& data) : data_(data) {}
    void proc() override {
        std::cout << "Handling message ID 03" << std::endl;
        std::cout << "name: " << data_.name << std::endl;
    }
};

// メッセージハンドラファクトリ
class MsgFactory {
private:
    // メッセージIDと、対応するオブジェクトを生成する関数のマップ
    using CreateFn = std::function<std::unique_ptr<Msg>(const MsgBase&)>;
    static std::map<unsigned char, CreateFn> s_creators;

public:
    // ファクトリを初期化する関数
    static void initialize() {
        s_creators[D_MSG_ID_01] = [](const MsgBase& msg) {
            return std::make_unique<Msg01_impl>(*reinterpret_cast<const Msg01*>(&msg));
        };
        s_creators[D_MSG_ID_02] = [](const MsgBase& msg) {
            return std::make_unique<Msg02_impl>(*reinterpret_cast<const Msg02*>(&msg));
        };
        s_creators[D_MSG_ID_03] = [](const MsgBase& msg) {
            return std::make_unique<Msg03_impl>(*reinterpret_cast<const Msg03*>(&msg));
        };
    }

    // メッセージIDに基づいてオブジェクトを生成
    static std::unique_ptr<Msg> createMsg(const MsgBase& msg) {
        auto it = s_creators.find(msg.msgId);
        if (it != s_creators.end()) {
            return it->second(msg);
        }
        return nullptr;
    }
};

// 静的メンバー変数の定義
std::map<unsigned char, MsgFactory::CreateFn> MsgFactory::s_creators;

void event_proc(MsgBase *msg) {
    std::unique_ptr<Msg> handler = MsgFactory::createMsg(*msg);
    if (handler) {
        handler->proc();
    }
}

void process_message(std::unique_ptr<Msg> handler)
{
  if(handler)
  {
    handler->proc();
  }
}

int main(void) {
  // アプリケーション起動時にファクトリを初期化
  MsgFactory::initialize();

  MsgBase msg;
  // この例では、read_msgが常にmsgId=1のメッセージを読み込むと仮定
  Msg01 test_msg_01 = { 0, 0, D_MSG_ID_01, 0, 100, 200, 300 };
  msg = *reinterpret_cast<MsgBase*>(&test_msg_01);

  std::unique_ptr<Msg> handler = MsgFactory::createMsg(msg);
  process_message(std::move(handler));

  return 0;
}
