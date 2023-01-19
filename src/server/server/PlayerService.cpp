#include "PlayerService.h"

namespace server {

    PlayerService::PlayerService() : Service("player") {

        Game game;
        this->game = game;

    }

    HttpStatus PlayerService::get(int id, Json::Value& msg) {

        if(id < 0)
            msg = game.getAllPlayersInfo();
        
        else
            msg = game.getPlayerInfo(id);


        if(msg.empty())
            return HttpStatus::NOT_FOUND;

        return HttpStatus::OK;

    }

    HttpStatus PlayerService::post(int id, const Json::Value& msg) {

        if(!game.checkPlayerID(id))
            return HttpStatus::NOT_FOUND;   

        game.changePlayerName(id, msg["name"].asString());

        return HttpStatus::NO_CONTENT;     

    }

    HttpStatus PlayerService::put(const Json::Value& msgIn, Json::Value& msgOut) {

        int id = game.addPlayer(msgIn["name"].asString());

        if(id < 0)
            return HttpStatus::OUT_OF_RESOURCES;

        msgOut["id"] = id;

        return HttpStatus::CREATED;        

    }

    HttpStatus PlayerService::remove(int id) {

        if(!game.checkPlayerID(id))
            return HttpStatus::NOT_FOUND;

        game.removePlayer(id);

        return HttpStatus::NO_CONTENT;                 

    }

}