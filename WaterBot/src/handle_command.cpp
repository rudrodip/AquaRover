#include "handle_command.h"

void handle_command(String cmd)
{
  char start = cmd.charAt(0);
  if (start == 'd')
  {
    int commaPos = cmd.indexOf(',');
    String disString = cmd.substring(1, commaPos);
    String angleString = cmd.substring(commaPos + 1);

    // Convert the string numbers to floats
    double dis = disString.toDouble();
    double angle = angleString.toDouble();
    drive(dis, angle);
  }
}