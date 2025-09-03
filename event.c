#include "msg.h"

void proc_msg_01(struct Msg01 *msg) { }
void proc_msg_02(struct Msg02 *msg) { }
void proc_msg_03(struct Msg03 *msg) { }

void event_proc()
{
  struct MsgBase msgBase;
  read_msg(0, &msgBase);

  if(D_MSG_ID_01 == msgBase.msgId)
  {
    proc_msg_01((struct Msg01 *)&msgBase);
  }
  else if(D_MSG_ID_02 == msgBase.msgId)
  {
    proc_msg_02((struct Msg02 *)&msgBase);
  }
  else if(D_MSG_ID_03 == msgBase.msgId)
  {
    proc_msg_03((struct Msg03 *)&msgBase);
  }
}

int main(void)
{
  event_proc();
  return 0;
}
