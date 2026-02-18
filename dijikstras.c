#include "map.h"


                      //  GATE,       PATH,       SGRASS,     TGRASS,     ARCTIC,     LAKE,       FOREST,     MNTN,       PCNTR,      PMART,      BOULDER
int cost_table[] =      {{10         ,10         ,10         ,20         ,25         ,__INT_MAX__,__INT_MAX__,__INT_MAX__,10         ,10         ,__INT_MAX__}, //PC
                         {__INT_MAX__,10         ,10         ,15         ,20         ,__INT_MAX__,15         ,15         ,50         ,50         ,__INT_MAX__}, //HIKER
                         {__INT_MAX__,10         ,10         ,20         ,25         ,__INT_MAX__,__INT_MAX__,__INT_MAX__,50         ,50         ,__INT_MAX__}, //RIVAL
                         {__INT_MAX__,__INT_MAX__,__INT_MAX__,__INT_MAX__,__INT_MAX__,7          ,__INT_MAX__,__INT_MAX__,__INT_MAX__,__INT_MAX__,__INT_MAX__}, //SWIMMER
                         {__INT_MAX__,10         ,10         ,20         ,25         ,__INT_MAX__,__INT_MAX__,__INT_MAX__,50         ,50         ,__INT_MAX__}};//OTHER

/* We are just implementing the  */
int dijiksra(map *m, tile *pc_pos, enum npc_type npc) {
    return 0;
}
