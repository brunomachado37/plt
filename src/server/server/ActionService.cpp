#include "ActionService.h"

#include <engine.h>
#include "../shared/constants.h"

namespace server {

    ActionService::ActionService(engine::Engine& engine) : Service("action"), engine(engine) {}

    HttpStatus ActionService::get(int id, Json::Value& msg) {

        if(id >= (int)engine.getRecord()[JSON_RANDOM_ARRAY].size() || id < 0)
            return HttpStatus::NOT_FOUND;
        
        msg["action"] = engine.getRecord()[JSON_ARRAY][id];
        msg["random"] = engine.getRecord()[JSON_RANDOM_ARRAY][id]; 

        return HttpStatus::OK;

    }

    HttpStatus ActionService::put(const Json::Value& msgIn, Json::Value& msgOut) {

        msgOut["epoch"] = engine.getRecord()[JSON_RANDOM_ARRAY].size();

        int actionID = msgIn["actionID"].asInt();

        if(actionID == ACTION_ID_TILE) {

            std::shared_ptr<engine::PlayTile> action = std::make_shared<engine::PlayTile>();
            action->deserialize(msgIn);
            engine.play(action);

        }
        else if(actionID == ACTION_ID_LEADER) {

            std::shared_ptr<engine::PlayLeader> action = std::make_shared<engine::PlayLeader>();
            action->deserialize(msgIn);
            engine.play(action);

        }
        else if(actionID == ACTION_ID_CATASTRO) {

            std::shared_ptr<engine::PlayCatastrophe> action = std::make_shared<engine::PlayCatastrophe>();
            action->deserialize(msgIn);
            engine.play(action);

        }
        else if(actionID == ACTION_ID_DRAW) {

            std::shared_ptr<engine::PlayDrawTiles> action = std::make_shared<engine::PlayDrawTiles>();
            action->deserialize(msgIn);
            engine.play(action);

        }
        else if(actionID == ACTION_ID_BUILD_MONUM) {

            std::shared_ptr<engine::PlayBuildMonument> action = std::make_shared<engine::PlayBuildMonument>();
            action->deserialize(msgIn);
            engine.play(action);

        }
        else if(actionID == ACTION_ID_ATTACK) {

            std::shared_ptr<engine::PlayAttack> action = std::make_shared<engine::PlayAttack>();
            action->deserialize(msgIn);
            engine.play(action);

        }
        else if(actionID == ACTION_ID_DEFENSE) {

            std::shared_ptr<engine::PlayDefense> action = std::make_shared<engine::PlayDefense>();
            action->deserialize(msgIn);
            engine.play(action);

        }
        else if(actionID == ACTION_ID_MOVE_LEADER) {

            std::shared_ptr<engine::PlayMoveLeader> action = std::make_shared<engine::PlayMoveLeader>();
            action->deserialize(msgIn);
            engine.play(action);

        }

        return HttpStatus::CREATED;        

    }

}