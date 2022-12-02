// Client main      
#define INTRODUCTORY_MSG                    "Tigris & Euphrates: An unofficial implementation"
#define HELP_MSG                            "Available arguments:\nhello      Print an introduction message\nstate      Run unitary tests through script\nunit-test  Run unitary tests through make\ncode-cov   Check code coverage through make\nrender     Render a static game state, mouse left-click to change state\nengine     Display a succession of different states\nrandom_ai  A match is played between 2 simple AI's\nhelp       Display this message" 
#define UNKNOW_ARG_MSG                      "Unknown argument!\nUse the 'help' argument to check availble options."
#define MULTIPLE_ARGS_MSG                   "None or multiple arguments provided! The program takes exactly 1 argument.\nUse the 'help' argument to check availble options."

// Player           
#define TILE_NOT_IN_HAND_MSG                "Player doesn't have specified tile in hand to remove it"
#define MANY_TILE_IN_HAND_MSG               "Player already have too many tiles in hand"
#define LEADER_NOT_IN_HAND_MSG              "Player doesn't have specified leader in hand to remove it"
#define LEADER_WRONG_ID_MSG                 "This leader doesn't belong to this player"
#define LEADER_IN_HAND_MSG                  "Leader is already in player's hand"
#define NO_CATAST_LEFT_MSG                  "Player has no catastrophes left to use"

// Region           
#define TILE_NOT_IN_REGION_MSG              "There's no tile in this region at this position"
#define LEADER_NOT_IN_REGION_MSG            "This player has no leader of this type in this region"
#define TREASURE_NOT_IN_REGION_MSG          "There's no treasure in this region"

// Board            
#define OCCUPIED_POS_MSG                    "Position already occupied"
#define INVALID_POS_LAND_MSG                "A farm can only be placed on a river"
#define INVALID_POS_RIVER_MSG               "Only a farm can be placed on a river"
#define INVALID_REGION_MSG                  "A tile can't be placed to unite more than 2 kingdoms"
#define INVALID_LEAD_RIVER_MSG              "A leader can't be placed on a river"
#define INVALID_LEAD_TEMPLE_MSG             "A leader has to be placed adjacent to a temple"
#define INVALID_LEAD_REG_MSG                "A leader cannot be placed in a way that it would unite 2 or more regions"
#define TILE_NOT_IN_POS_MSG                 "The specified position doesn't contain a tile"
#define LEADER_NOT_IN_BOARD_MSG             "Specified leader is not on the board"

// Catastrophes
#define CATASTRO_IN_TREASURE_MSG            "A catastrophe can't be played in a tile holding a treasure"
#define CATASTRO_INVALID_MSG                "A catastrophe can't be played in a leader or a monument"

// Sanity checks        
#define REGION_ERROR_MSG                    "Region map isn't coherent with state map"
#define ADJ_REG_ERROR_1_MSG                 "Adjacent regions vector is inconsistent with number of adjacent regions int found"
#define ADJ_REG_ERROR_2_MSG                 "Adjacent regions counter is not between 0 and 4"
#define LEADER_ID_ERROR_MSG                 "2 leaders of the same type of the same player exists"
#define LEADER_MAP_ERROR_MSG                "Region or state map inconsistency in leader position"

#define MONUMENT_POSITION_ERROR_MSG         "Monument position is inconsistent with used draw methode"
#define TILE_POSITION_ERROR_MSG             "Tile position is inconsistent with used draw methode"
#define LEADER_POSITION_ERROR_MSG           "Leader position is inconsistent with used draw methode"

#define INVALID_MONUMENT_ADD_ERROR_MSG      "The board state map isn't coherent for a constructing this monument in the given position"
#define MONUMENT_BUILT_ERROR_MSG            "Monument position suggests that it's already built"
#define MONUMENT_BUILT_2_ERROR_MSG          "Monument not on list of not built monuments which suggests it's already built"
#define MONUMENT_REGION_ERROR_MSG           "Region map is inconsistent to build a monument in the given position"

#define REC_SEARCH_ERROR_MSG                "Temporary list is not empty"
#define REC_SEARCH_ERROR_2_MSG              "totalRegionList is different from regionPositionList"

#define TRIGGER_POSITION_REVOLT_ERROR_MSG   "Trigger position in revolt attack/defense action doesn't contain a leader"
#define REGION_REVOLT_ERROR_MSG             "Region of trigger poisition not in revolt!"
#define DEFENDER_LEADER_REVOLT_ERROR_MSG    "Defender leader was not found when solving revolt"

#define TRIGGER_POSITION_WAR_ERROR_MSG      "Trigger position in war attack/defense action doesn't contain a tile"
#define REGION_WAR_ERROR_MSG                "Region of trigger poisition not in revolt!"
#define REGION_WAR_POSITION_ERROR_MSG       "Trigger position in war attack/defense action is inconsistent with region unification position"
#define LEADER_WAR_ERROR_MSG                "Attacker or defender learders weren't found"
#define WAR_LOST_INVALID_MSG                "War lost receive a positions incoherent with stateMap"

#define NOT_ACTIVE_PLAYER_MSG               "Provided action doesn't belong to the active player"
#define ACTIVE_PLAYER_MSG                   "Provided action belong to the active player and it shouldn't"

#define AI_CONFLICT_RESOLUTION_ERROR_MSG    "Logic error in conflict resolution in the AI"
#define AI_DEFENSE_PENDENT_ERROR_MSG        "Defense is pendent, but attack is not the last action played"
#define AI_NOT_ACTIVE_ERROR_MSG             "Action required by not active player"
#define AI_ATTACK_ERROR_MSG                 "Attack pendent wasn't solved"
#define AI_MONUMENT_ERROR_MSG               "Monument pendent wasn't solved"

// Engine
#define INVALID_ACTION_ATTACK               "Invalid action! An attack is pendent and must be resolved first"
#define INVALID_ACTION_DEFENSE              "Invalid action! A defense is pendent and must be resolved first"
#define INVALID_ACTION_MONUMENT             "Invalid action! Active player must choose if it will build monument or not!"

#define END_GAME_TILE                       "Game over! Player tried to draw a tile and there wasn't any tile remaining"
#define GAME_END_ERROR_MSG                  "End game conditions were not reached!"

// Game over
#define GAME_OVER_MSG                       "Game Over!"
#define FINAL_SCORE_1_MSG                   "Player "
#define FINAL_SCORE_2_MSG                   " has scored "
#define FINAL_SCORE_3_MSG                   " points"
#define FINAL_SCORE_4_MSG                   " is the winner!"
#define FINAL_SCORE_5_MSG                   ": "
#define FINAL_SCORE_6_MSG                   " points\n                                   "
#define FINAL_SCORE_7_MSG                   "Final Score: "
#define TIE_MSG                             "All players have the same final score! The victory is shared!"
#define TIE_2_MSG                           "It's a tie! The victory is shared!"

#define EXIT_DEMO_MSG                       "Game random initial conditions are not favorable for the demo pre-defined actions script.\nPlease, re-run demo"