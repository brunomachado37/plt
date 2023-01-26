Tigris & Euphrates: An unofficial implementation
===============

This is an unofficial implementation of the board game Tigris & Euphrates, designed by Reiner Knizia and first published in 1997.  
  
![alt text](rapport/images/TeE_box.png "Original game box")  
  
> This project is part of the "Informatique et systèmes" specialization of ENSEA's final engineering year. Therefore, it has an academic purpose.  

Report
------------
The report of the project is available in a [markdown version](rapport/Rapport.md) and in a [pdf version](Rapport.pdf).

Installation
------------

* **Install all the dependencies:**
```
sudo apt-get update
```
```
sudo apt-get install -y build-essential cmake dia lcov gcovr libxml2-dev libsfml-dev libboost-test-dev libmicrohttpd-dev
```

* **Clone repository:**
```
git clone https://github.com/brunomachado37/plt.git
```

* **Cmake configure:**
```
mkdir plt/build && cd plt/build
```
```
cmake ..
```

* **Build:**
```
make
```

Usage
------------

* Client:

```
./bin/client    [hello]
                [state]
                [unit_test]
                [code_cov]
                [render]
                [engine]
                [random_ai]
                [heuristic_ai]
                [2_heuristic_ai]
                [rollback]
                [deep_ai]
                [contest_heuristic]
                [contest_deep]
                [thread]
                [play]
                [lobby]
                [network]
                [help] 
```

* Server:

```
./bin/server    [hello]
                [record]
                [lobby]
                [listen]
                [help] 
```


**Arguments:**
    
* Client:

```
hello               Print message
state               Run unitary tests through script
unit_test           Run unitary tests through make
code_cov            Check code coverage through make
render              Render a static game state, mouse left-click to change displayed state    
engine              Display a succession of different game states
random_ai           A match is played between 2 simple AI's
heuristic_ai        A match is played between an intermediary AI and a simple AI
2_heuristic_ai      A match is played between 2 intermediary AI's
rollback            A match is played between 2 intermediary AI's for 10 turns. Then the match is displayed backwards
deep_ai             A match is played between an advanced AI and an intermediary AI
contest_heuristic   100 matches are played between an intermediary AI and a simple AI, without rendering
contest_deep        100 matches are played between an advanced AI and an intermediary AI, without rendering
thread              Run match with engine running on a different thread
play                Run the match recorded on the file /res/replay.json
lobby               Allow the client to join a lobby launched with ./server/listen
network             Runs the game in a server launched by ./server/listen. The match starts after 2 clients join
help                Display all available arguments
```
  
* Server:
  
```
hello               Print message
record              Run a match for 10 turns and record the commands on /res/replay.json
lobby               Creates a lobby, allowing different clients to join using ./client/network
listen              Creates a server to run the game in network. Clients can join with ./client/network
help                Display all available arguments
```