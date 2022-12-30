#define NUM_LINES               11
#define NUM_ROWS                16
#define HAND_LIMIT              6

#define TOTAL_NUMBER_OF_TILES   153

#define NO_REGION_ID            -1
#define NOT_IN_MAP_ID           -1
#define NO_UNIFICATION_POS      -2
#define NOT_FOUND_POS           -3
#define STD_FINAL_SCORE         -4

#define LAND                    "land"
#define RIVER                   "river"
#define OUTSIDE_OF_BOARD        "out"
            
#define TEMPLE                  "temple"
#define MARKET                  "market"
#define FARM                    "farm"
#define SETTLEMENT              "settlement"
#define MONUMENT                "monument"
#define CATASTRO                "catastrophe"
#define UNION                   "union"
        
#define LEADER                  "leader"
#define KING                    "king"
#define TRADER                  "trader"
#define PRIEST                  "priest"
#define FARMER                  "farmer"

#define RED                     "red"
#define BLUE                    "blue"
#define BLACK                   "black"
#define GREEN                   "green"
#define TREASURE                "treasure"

#define ACTION_ID_TILE          0
#define ACTION_ID_LEADER        1
#define ACTION_ID_CATASTRO      2
#define ACTION_ID_DRAW          3
#define ACTION_ID_BUILD_MONUM   4
#define ACTION_ID_ATTACK        5
#define ACTION_ID_DEFENSE       6
#define ACTION_ID_MOVE_LEADER   7

#define WAR                     0
#define REVOLT                  1

#define MAX_TURN_EVAL_SCORE     10
#define EXPETION_FLAG           -58

#define PROB_TILE               0.5
#define PROB_LEADER             0.2
#define PROB_CATASTRO           0.05
#define PROB_DRAW               0.25
#define PROB_MOVE               0.05

#define LIMIT_EXPLORE           5
#define DEPTH_EXPLORE           5

#define ROUNDS_CONTEST          100

#define RANDOM_AI_TYPE          "Random"
#define HEURISTIC_AI_TYPE       "Heuristic"
#define DEEP_AI_TYPE            "Deep"