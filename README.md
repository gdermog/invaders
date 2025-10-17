# SimFire

Exercise (c++ demo project): space invaders

![Space invaders title screen](screenshot1.png?raw=true "Space invaders title screen")

# Basic problem description

Create a basic Space Invaders game. It must include the following basic functionalities:

- Monsters and the player's ship must be destructible, there must be a collision detector
- The player must be able to destroy enemies by shooting
- The game must keep score and keep a high score leaderboard
- The game code must be meaningfully documented
- The game must be fun

# Solution description

The game is implemented in C++ using the windows API and DirectX library for graphics and input
handling. The code is organized into several classes responsible for specific functionalities,
such as rendering, updating game state, and handling collisions. All game actors are constructed
according to [Entity Component System](https://en.wikipedia.org/wiki/Entity_component_system)
architecture, to be more specific, using [EnTT](https://github.com/skypjack/entt) library. See
code comments for detailed explanations.

# In-game features

![Space invaders title screen](screenshot2.png?raw=true "Space invaders title screen")

- Player can move left, right, up and down using arrow keys and shoot upwards using space key
- Monsters move left and right, and down when they hit the screen edge, leaving the player less space
  to maneuver (but there is always some space left, so it is possible to destroy them)
- Monsters shoot downwards randomly
- Monsters speed up as their numbers decrease
- Monsters gone to raids (leaving formation and chasing player ship) randomly
- Alien bonus vessel appears randomly, giving extra points when destroyed
- There is limited time to destroy all monsters in each level. When time runs out, sudden death mode starts:
  monsters speed up and relentlessly chase the player ship
- With each next level, monsters become faster and shoot more often
- Time to sudden death, score, ammo and player lives are displayed at the bottom of the screen
- When player ship is destroyed, there is a brief invulnerability period indicated by blinking ship
- When player is above all monsters, aliens gone to raids more often
- Special crossower guest appears time to time, chasing player ship from sides of the screen

# Compiling and running Invaders

SimFire ​​is a standard project for MS Visual Studio and MSVC. It contains both a solution file and a project.
It  assumes the C++20 language standard. Library [EnTT](https://github.com/skypjack/entt) is included in
the project.

To compile and run the project, open the solution file `invaders.sln` in MS Visual Studio, build the project,
and then run the executable. The game should open in a new window.

**Remark:** to compile the game, you need to have the
[DirectX 9 SDK](https://www.microsoft.com/en-us/download/details.aspx?id=8109) installed on your system.

# Work to be done

There are still some tasks that need to be completed on the project, in particular:

   - To suggest and implement some form of testing (e.g. via some unit-test framework, possible
     MS Visual Studio / MSVC)
   - To implement CI/CD pipelne for GitHub to automatize compilation of the project and unit tests and check for
     possible errors
   - To fine-tune game parameters (e.g. monster speed, shooting frequency, etc.) to make the game more playable

And maybe sometime in the future:

   - Add more varieties of monsters with different behaviors and appearances
   - Add power-ups that the player can collect to gain temporary advantages
   - Add more variable levels with different layouts and challenges
   - Add more different backgrounds, ambient music and sound effects to enhance the game atmosphere
   - Possibly, 2-player mode
   - Porting the game to other platforms (e.g. Linux, MacOS)

and, of course

   - Achieve status of a most played game on Steam :-) !
