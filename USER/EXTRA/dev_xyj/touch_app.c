#include "touch_app.h"

// ÅÐ¶Ï´¥ÃþµãÊÇ·ñÂäÔÚ·¶Î§ÄÚ
//  1 yse 0 no
u8 touch_inRange(u16 rx1, u16 ry1, u16 rx2, u16 ry2, u16 x, u16 y) {
    if (x >= rx1 && x <= rx2) {
        if (y >= ry1 && y <= ry2) {
            return 1;
        }
    }
    return 0;
}
