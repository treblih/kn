/* please notice the sequence of the including list
 * we put the one who needs no other *.h at the beginning
 * follow the principle, const.h is 1st
 */

#include "type.h"
#include "protect.h"
#include "proc.h"
#include "tty.h"
#include "hd.h"
#include "proto.h"
#include "global.h"

void
fs()
{
        MESSAGE         m;

        /* notice that the m.source below is needed when u wanna send a msg, just like u name */
        m.source       =       FS;
        m.type         =       DEV_OPEN;
        send_rec(BOTH, &m, HD);
        printf("FS recv from %d\n", m.source);

        spin("FS");
}
