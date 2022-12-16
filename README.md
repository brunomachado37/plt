Tigris & Euphrates: An unofficial implementation
===============

This is an unofficial implementation of the board game Tigris & Euphrates, designed by Reiner Knizia and first published in 1997.

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
sudo apt-get install -y build-essential cmake dia lcov gcovr libxml2-dev libsfml-dev libboost-test-dev
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
                [help] 
```


* **Arguments:**
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
help                Display all available arguments
```