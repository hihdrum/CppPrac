#include "msg.h"
#include <iostream>
#include <memory>

// 前方宣言
class Event;
class State;
class StateS01;
class StateS02;
class StateS03;

enum StateType {
  S01,
  S02,
  S03,
};

// グローバルな状態リポジトリ
// std::unique_ptr を使うことでメモリ管理を自動化
std::unique_ptr<State> g_StateRepo;

// 状態とイベントを繋ぐためのイベント基底クラス
class Event {
public:
  // dispatchメソッドで、適切な状態の処理関数を呼び出す
  virtual StateType dispatch(StateS01& state) = 0;
  virtual StateType dispatch(StateS02& state) = 0;
  virtual StateType dispatch(StateS03& state) = 0;
  virtual ~Event() = default;
};

// 状態基底クラス
class State {
public:
  // イベントを受け取る仮想関数
  virtual StateType handleEvent(Event& event) = 0;
  virtual ~State() = default;
};

// 各状態に対応するクラス
class StateS01 : public State {
public:
  StateType handleEvent(Event& event) override {
    // Visitorパターンを利用して適切な dispatch メソッドを呼び出す
    return event.dispatch(*this);
  }
};

class StateS02 : public State {
public:
  StateType handleEvent(Event& event) override {
    return event.dispatch(*this);
  }
};

class StateS03 : public State {
public:
  StateType handleEvent(Event& event) override {
    return event.dispatch(*this);
  }
};

// 各メッセージに対応するイベントクラス
class EventMsg01 : public Event {
private:
  const Msg01& m_msg;
public:
  EventMsg01(const Msg01& msg) : m_msg(msg) {}

  StateType dispatch(StateS01& state) override {
    std::cout << "Transition from S01 to S02 via Msg01" << std::endl;
    return S02;
  }

  StateType dispatch(StateS02& state) override {
    std::cout << "No process for Msg01 in state S02" << std::endl;
    return S02;
  }

  StateType dispatch(StateS03& state) override {
    std::cout << "No process for Msg01 in state S03" << std::endl;
    return S03;
  }
};

class EventMsg02 : public Event {
private:
  const Msg02& m_msg;
public:
  EventMsg02(const Msg02& msg) : m_msg(msg) {}

  StateType dispatch(StateS01& state) override {
    std::cout << "No process for Msg02 in state S01" << std::endl;
    return S01;
  }

  StateType dispatch(StateS02& state) override {
    std::cout << "Transition from S02 to S03 via Msg02" << std::endl;
    return S03;
  }

  StateType dispatch(StateS03& state) override {
    std::cout << "No process for Msg02 in state S03" << std::endl;
    return S03;
  }
};

class EventMsg03 : public Event {
private:
  const Msg03& m_msg;
public:
  EventMsg03(const Msg03& msg) : m_msg(msg) {}

  StateType dispatch(StateS01& state) override {
    std::cout << "No process for Msg03 in state S01" << std::endl;
    return S01;
  }

  StateType dispatch(StateS02& state) override {
    std::cout << "No process for Msg03 in state S02" << std::endl;
    return S02;
  }

  StateType dispatch(StateS03& state) override {
    std::cout << "Transition from S03 to S01 via Msg03" << std::endl;
    return S01;
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

class StateMachineService {
private:
  std::unique_ptr<State> m_currentState;

  std::unique_ptr<State> createState(StateType type)
  {
    switch(type) {
      case S01: return std::make_unique<StateS01>();
      case S02: return std::make_unique<StateS02>();
      case S03: return std::make_unique<StateS03>();
      default: return nullptr;
    }
  }

public:
  StateMachineService() : m_currentState(std::make_unique<StateS01>()) { }

  void processMessage(MsgBase* msg)
  {
    auto event = createEvent(msg);
    if(event && m_currentState)
    {
      StateType nextState = m_currentState->handleEvent(*event);

      if(nextState != S01 && nextState != S02 && nextState != S03)
      {
        return;
      }

      if(nextState != getCurrentStateType())
      {
        m_currentState = createState(nextState);
      }
    }
  }

  StateType getCurrentStateType() const {
    if (dynamic_cast<StateS01*>(m_currentState.get())) return S01;
    if (dynamic_cast<StateS02*>(m_currentState.get())) return S02;
    if (dynamic_cast<StateS03*>(m_currentState.get())) return S03;

    return S01;
  }
};

int main(void) {

  StateMachineService service;

  Msg01 msg01 = {0, 0, D_MSG_ID_01, 0, 0, 0, 0};
  Msg02 msg02 = {0, 0, D_MSG_ID_02, 0, 0, {0, 0}};
  Msg03 msg03 = {0, 0, D_MSG_ID_03, 0, "", ""};

  // シーケンス
  service.processMessage(reinterpret_cast<MsgBase*>(&msg01));
  service.processMessage(reinterpret_cast<MsgBase*>(&msg02));
  service.processMessage(reinterpret_cast<MsgBase*>(&msg03));

  // 不正なシーケンスのテスト
  service.processMessage(reinterpret_cast<MsgBase*>(&msg02));

  return 0;
}
