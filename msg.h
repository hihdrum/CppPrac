#include <unistd.h>

/* ここから外部提供のライブラリ(変更不可) */
#ifdef __cplusplus
extern "C" {
#endif

#define D_MSG_ID_01 0x01
#define D_MSG_ID_02 0x02
#define D_MSG_ID_03 0x03

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
#ifdef __cplusplus
}
#endif
/* ここまで変更不可 */
