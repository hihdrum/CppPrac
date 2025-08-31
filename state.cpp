#include "msg.h"
#include <iostream>
#include <memory>

// 前方宣言
class State;
class StateS01;
class StateS02;
class StateS03;

// グローバルな状態リポジトリ
// std::unique_ptr を使うことでメモリ管理を自動化
std::unique_ptr<State> g_StateRepo;

// 状態とイベントを繋ぐためのイベント基底クラス
class Event {
public:
  // dispatchメソッドで、適切な状態の処理関数を呼び出す
  virtual void dispatch(StateS01& state) = 0;
  virtual void dispatch(StateS02& state) = 0;
  virtual void dispatch(StateS03& state) = 0;
  virtual ~Event() = default;
};

// 状態基底クラス
class State {
public:
  // イベントを受け取る仮想関数
  virtual void handleEvent(Event& event) = 0;
  virtual ~State() = default;
};

// 各状態に対応するクラス
class StateS01 : public State {
public:
  void handleEvent(Event& event) override {
    // Visitorパターンを利用して適切な dispatch メソッドを呼び出す
    event.dispatch(*this);
  }
};

class StateS02 : public State {
public:
  void handleEvent(Event& event) override {
    event.dispatch(*this);
  }
};

class StateS03 : public State {
public:
  void handleEvent(Event& event) override {
    event.dispatch(*this);
  }
};

// 各メッセージに対応するイベントクラス
class EventMsg01 : public Event {
private:
  const Msg01& m_msg;
public:
  EventMsg01(const Msg01& msg) : m_msg(msg) {}

  void dispatch(StateS01& state) override {
    std::cout << "Transition from S01 to S02 via Msg01" << std::endl;
    g_StateRepo.reset(new StateS02()); // 状態をS02に遷移
  }

  void dispatch(StateS02& state) override {
    std::cout << "No process for Msg01 in state S02" << std::endl;
    // 何もしない（遷移なし）
  }

  void dispatch(StateS03& state) override {
    std::cout << "No process for Msg01 in state S03" << std::endl;
    // 何もしない（遷移なし）
  }
};

class EventMsg02 : public Event {
private:
  const Msg02& m_msg;
public:
  EventMsg02(const Msg02& msg) : m_msg(msg) {}

  void dispatch(StateS01& state) override {
    std::cout << "No process for Msg02 in state S01" << std::endl;
  }

  void dispatch(StateS02& state) override {
    std::cout << "Transition from S02 to S03 via Msg02" << std::endl;
    g_StateRepo.reset(new StateS03());
  }

  void dispatch(StateS03& state) override {
    std::cout << "No process for Msg02 in state S03" << std::endl;
  }
};

class EventMsg03 : public Event {
private:
  const Msg03& m_msg;
public:
  EventMsg03(const Msg03& msg) : m_msg(msg) {}

  void dispatch(StateS01& state) override {
    std::cout << "No process for Msg03 in state S01" << std::endl;
  }

  void dispatch(StateS02& state) override {
    std::cout << "No process for Msg03 in state S02" << std::endl;
  }

  void dispatch(StateS03& state) override {
    std::cout << "Transition from S03 to S01 via Msg03" << std::endl;
    g_StateRepo.reset(new StateS01());
  }
};

// メッセージIDに対応するイベントオブジェクトを生成するファクトリ関数
std::unique_ptr<Event> createEvent(MsgBase* msg) {
  switch (msg->msgId) {
    case D_MSG_ID_01:
      return std::make_unique<EventMsg01>(*reinterpret_cast<const Msg01*>(msg));
    case D_MSG_ID_02:
      return std::make_unique<EventMsg02>(*reinterpret_cast<const Msg02*>(msg));
    case D_MSG_ID_03:
      return std::make_unique<EventMsg03>(*reinterpret_cast<const Msg03*>(msg));
    default:
      return nullptr;
  }
}

// イベント処理のメイン関数
void trans(MsgBase* msg) {
  auto event = createEvent(msg);
  if (event) {
    g_StateRepo->handleEvent(*event);
  }
}

int main(void) {
  // 初期状態をS01に設定
  g_StateRepo.reset(new StateS01());

  // テスト用のメッセージを作成
  Msg01 msg01 = {0, 0, D_MSG_ID_01, 0, 0, 0, 0};
  Msg02 msg02 = {0, 0, D_MSG_ID_02, 0, 0, {0, 0}};
  Msg03 msg03 = {0, 0, D_MSG_ID_03, 0, "", ""};

  // シーケンス
  trans(reinterpret_cast<MsgBase*>(&msg01)); // S01 -> S02
  trans(reinterpret_cast<MsgBase*>(&msg02)); // S02 -> S03
  trans(reinterpret_cast<MsgBase*>(&msg03)); // S03 -> S01

  // 不正なシーケンスのテスト
  trans(reinterpret_cast<MsgBase*>(&msg02)); // S01でMsg02を受信 -> No Process

  return 0;
}
