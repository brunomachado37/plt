#include "GameService.h"

namespace server {

    GameService::GameService(Game& game) : Service("game"), game(game) {}

    HttpStatus GameService::get(int id, Json::Value& msg) {

        msg["status"] = game.status;

        return HttpStatus::OK;

    }

}