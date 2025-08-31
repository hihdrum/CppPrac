#include "msg.h"

enum State {
  D_S01,
  D_S02,
  D_S03,
};

enum State g_StateRepo = D_S01;

void trans_M01_S01(const struct Msg01* msg)
{
  g_StateRepo = D_S02;
}

void trans_M02_S02(const struct Msg02* msg)
{
  g_StateRepo = D_S03;
}

void trans_M03_S03(const struct Msg03* msg)
{
  g_StateRepo = D_S01;
}

void trans_no_proc(const struct MsgBase* msg)
{
}

void trans(const struct MsgBase* msg)
{
  const unsigned char msgId = msg->msgId;

  if(D_MSG_ID_01 == msgId)
  {
    if(D_S01 == g_StateRepo)
    {
      trans_M01_S01((const struct Msg01*)msg);
    }
    else
    {
      trans_no_proc(msg);
    }
  }
  else if(D_MSG_ID_02 == msgId)
  {
    if(D_S02 == g_StateRepo)
    {
      trans_M02_S02((const struct Msg02*)msg);
    }
    else
    {
      trans_no_proc(msg);
    }

  }
  else if(D_MSG_ID_03 == msgId)
  {
    if(D_S03 == g_StateRepo)
    {
      trans_M03_S03((const struct Msg03*)msg);
    }
    else
    {
      trans_no_proc(msg);
    }
  }
  else
  {
      trans_no_proc(msg);
  }
}

int main(void)
{
  return 0;
}
