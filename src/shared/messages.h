// Client main
#define INTRODUCTORY_MSG            "Tigris & Euphrates: An unofficial implementation"
#define HELP_MSG                    "Available arguments:\nhello      Print an introduction message\nstate      Run unitary tests through script\nunit-test  Run unitary tests through make\ncode-cov   Check code coverage through make\nrender     Render a static game state\nhelp       Display this message" 
#define UNKNOW_ARG_MSG              "Unknown argument!\nUse the 'help' argument to check availble options."
#define MULTIPLE_ARGS_MSG           "None or multiple arguments provided! The program takes exactly 1 argument.\nUse the 'help' argument to check availble options."

// Player   
#define TILE_NOT_IN_HAND_MSG        "Player doesn't have specified tile in hand to remove it"
#define MANY_TILE_IN_HAND_MSG       "Player already have too many tiles in hand"
#define LEADER_NOT_IN_HAND_MSG      "Player doesn't have specified leader in hand to remove it"
#define LEADER_WRONG_ID_MSG         "This leader doesn't belong to this player"
#define LEADER_IN_HAND_MSG          "Leader is already in player's hand"
#define NO_CATAST_LEFT_MSG          "Player has no catastrophes left to use"

// Region   
#define TILE_NOT_IN_REGION_MSG      "There's no tile in this region at this position"
#define LEADER_NOT_IN_REGION_MSG    "This player has no leader of this type in this region"
#define TREASURE_NOT_IN_REGION_MSG  "There's no treasure in this region at this position"

// Board    
#define OCCUPIED_POS_MSG            "Position already occupied"
#define INVALID_POS_LAND_MSG        "A farm can only be placed on a river"
#define INVALID_POS_RIVER_MSG       "Only a farm can be placed on a river"
#define INVALID_REGION_MSG          "A tile can't be placed to unite more than 2 kingdoms"
#define INVALID_LEAD_RIVER_MSG      "A leader can't be placed on a river"
#define INVALID_LEAD_TEMPLE_MSG     "A leader has to be placed adjacent to a temple"
#define INVALID_LEAD_REG_MSG        "A leader cannot be placed in a way that it would unite 2 or more regions"
#define TILE_NOT_IN_POS_MSG         "The specified position doesn't contain a tile"
#define LEADER_NOT_IN_BOARD_MSG     "Specified leader is not on the board"


// Sanity checks
#define REGION_ERROR_MSG            "Region map isn't coherent with state map"
#define ADJ_REG_ERROR_1_MSG         "Adjacent regions vector is inconsistent with number of adjacent regions int found"
#define ADJ_REG_ERROR_2_MSG         "Adjacent regions counter is not between 0 and 4"
#define LEADER_ID_ERROR_MSG         "2 leaders of the same type of the same player exists"
#define LEADER_STRENGTH_ERROR_MSG   "Leader added to the board with strength different from 0"
#define LEADER_MAP_ERROR_MSG        "Region or state map inconsistency in leader position"