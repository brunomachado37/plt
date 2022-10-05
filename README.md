Tigris & Euphrates: An unofficial implementation
===============

> This is an unofficial implementation of the board game Tigris & Euphrates, designed by Reiner Knizia and first published in 1997.

> This project is part of the "Informatique et systèmes" specialization for ENSEA's last year of engineering serving an academic purpose.


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
mkdir plt/build && cd _$
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
                [render]
                [help] 
```


* **Arguments:**
```
hello               Print message
state               Generate a static game state
render              Render a static game state    
help                Display all available arguments
```