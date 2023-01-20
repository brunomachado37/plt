#include "NetworkClient.h"
#include "../server/server/GameStatus.h"
#include "../shared/messages.h"
#include "../shared/constants.h"
#include <engine.h>
#include <ai.h>

#include <SFML/Network.hpp>
#include <iostream>
#include <unistd.h>
#include <string>

namespace client {

    NetworkClient::NetworkClient(std::string url, int port, std::string playerName) {

        this->url = url;
        this->port = port;
        this->playerName = playerName;

        engine.init();

    }

    int NetworkClient::connect() {

        sf::Http http(url, port);

        sf::Http::Request req;
        req.setMethod(sf::Http::Request::Put);
        req.setUri("/player");

        Json::Value msg;
        msg["name"] = playerName;
        req.setBody(msg.toStyledString());

        sf::Http::Response resp = http.sendRequest(req);

        if(resp.getStatus() == sf::Http::Response::Status::Created) {

            Json::Reader jsonReader;
            Json::Value respJson;

            jsonReader.parse(resp.getBody(), respJson);
            playerID = respJson["id"].asInt();

            engine.setRandomConditions(respJson["InitialState"]);
            engine.enableRecord();

            std::shared_ptr<ai::HeuristicAI> ai = std::make_shared<ai::HeuristicAI>(playerID);
            this->ai = ai;

            std::cout << WELCOME_1_MSG << playerName << WELCOME_3_MSG << std::endl;

            return SUCCESS;

        }
        else if(resp.getStatus() == sf::Http::Response::Status::ServiceNotAvailable) {

            std::cout << FULL_LOBBY_MSG << std::endl;

        }

        return ERROR_CONNECT;

    }

    void NetworkClient::run(sf::RenderWindow& window) {

        // Create scene
        render::Scene scene(playerID);
        scene.drawState(engine.getState(), window);
        window.display();

        // Wait for the match to start
        while(serverGetGameStatus() == server::WAITING) {
            sleep(1);
        }

        while(window.isOpen()) {

            sf::Event event;

            while (window.pollEvent(event)) {

                if (event.type == sf::Event::Closed) {
                    window.close();
                }

            }

            window.display();

            // Check for end of game
            if(engine.gameNotOver()) {

                if(!engine.getDefensePendent()) {
                    if(engine.getState().getActivePlayerID() == playerID) {
                        ai->run(engine);
                        serverSendAction();
                    }
                }
                else {
                    if(engine.getActionsLog()[engine.getActionsLog().size() - 1]->getPlayerID() != playerID) {
                        ai->run(engine);
                        serverSendAction();
                    }
                }

                while(serverGetAction()) {
                    sleep(1);
                }

                scene.drawState(engine.getState(), window);

            }
            else
                scene.drawGameOver(engine.getFinalScore(), window);

        }

    }

    int NetworkClient::serverGetGameStatus() {

        sf::Http http(url, port);

        sf::Http::Request req;
        req.setMethod(sf::Http::Request::Get);
        req.setUri("/game");

        sf::Http::Response resp = http.sendRequest(req);

        if(resp.getStatus() == sf::Http::Response::Status::Ok) {

            Json::Reader jsonReader;
            Json::Value respJson;

            jsonReader.parse(resp.getBody(), respJson);
            return respJson["status"].asInt();

        }

        return server::WAITING;

    }

    void NetworkClient::serverSendAction() {

        sf::Http http(url, port);

        sf::Http::Request req;
        req.setMethod(sf::Http::Request::Put);
        req.setUri("/action");

        Json::Value action = engine.getLastPlayedAction();

        req.setBody(action.toStyledString());

        sf::Http::Response resp = http.sendRequest(req);

        if(resp.getStatus() == sf::Http::Response::Status::Created) {

            Json::Reader jsonReader;
            Json::Value respJson;

            jsonReader.parse(resp.getBody(), respJson);
            int epoch = respJson["epoch"].asInt();

            if(epoch != this->epoch)
                throw std::exception();

        }
        else
            std::cout << resp.getStatus() << std::endl;

        engine.clearAction();

    }

    int NetworkClient::serverGetAction() {

        sf::Http http(url, port);

        sf::Http::Request req;
        req.setMethod(sf::Http::Request::Get);

        std::string uri = "/action/" + std::to_string(epoch);
        req.setUri(uri);

        sf::Http::Response resp = http.sendRequest(req);

        if(resp.getStatus() == sf::Http::Response::Status::Ok) {

            Json::Reader jsonReader;
            Json::Value respJson;

            jsonReader.parse(resp.getBody(), respJson);

            deserializeAction(respJson);

            epoch++;

            return SUCCESS;

        }

        return FAILURE;

    }

    void NetworkClient::deserializeAction(Json::Value json_root) {

        Json::Value jsonAction = json_root["action"];

        int actionID = jsonAction["actionID"].asInt();

        if(actionID == ACTION_ID_TILE) {

            std::shared_ptr<engine::PlayTile> action = std::make_shared<engine::PlayTile>();
            action->deserialize(jsonAction);
            engine.play(action);

        }
        else if(actionID == ACTION_ID_LEADER) {

            std::shared_ptr<engine::PlayLeader> action = std::make_shared<engine::PlayLeader>();
            action->deserialize(jsonAction);
            engine.play(action);

        }
        else if(actionID == ACTION_ID_CATASTRO) {

            std::shared_ptr<engine::PlayCatastrophe> action = std::make_shared<engine::PlayCatastrophe>();
            action->deserialize(jsonAction);
            engine.play(action);

        }
        else if(actionID == ACTION_ID_DRAW) {

            std::shared_ptr<engine::PlayDrawTiles> action = std::make_shared<engine::PlayDrawTiles>();
            action->deserialize(jsonAction);
            engine.play(action);

        }
        else if(actionID == ACTION_ID_BUILD_MONUM) {

            std::shared_ptr<engine::PlayBuildMonument> action = std::make_shared<engine::PlayBuildMonument>();
            action->deserialize(jsonAction);
            engine.play(action);

        }
        else if(actionID == ACTION_ID_ATTACK) {

            std::shared_ptr<engine::PlayAttack> action = std::make_shared<engine::PlayAttack>();
            action->deserialize(jsonAction);
            engine.play(action);

        }
        else if(actionID == ACTION_ID_DEFENSE) {

            std::shared_ptr<engine::PlayDefense> action = std::make_shared<engine::PlayDefense>();
            action->deserialize(jsonAction);
            engine.play(action);

        }
        else if(actionID == ACTION_ID_MOVE_LEADER) {

            std::shared_ptr<engine::PlayMoveLeader> action = std::make_shared<engine::PlayMoveLeader>();
            action->deserialize(jsonAction);
            engine.play(action);

        }

        engine.update();

        engine.setRandomConditions(json_root["random"]);
        
    }

}