/* date = August 12th 2022 4:11 pm */

#ifndef CAMELLIA_H
#define CAMELLIA_H

#include "camellia_platform.h"

#define GAME_UPDATE(name) void name()
typedef GAME_UPDATE(game_update);
GAME_UPDATE(GameUpdateStub)
{
}

#endif //CAMELLIA_H
