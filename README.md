# BattleNet_Chess
###This project was designed in c++, using OpenGl for visuals.

This application is a multiplayer game of chess. It is designed with a server and a client applications. To run this project, you must go into the Release folder inside of the ChessServer project and run that exe first. In order to run these exes you need a dll that comes from Microsoft. If you have visual studio installed it comes with it. Once the server is up and running, on your local area network, you can run any number of clients that you so desire. To run a client, run the exe in the Release folder of the ChessClient project When connection has been made with the server, the server will communicate to the client the instructions that it needs in order to run and play chess. 

This project is the first step in a larger overal project made to imitate the Battlenet launcher client produced by Blizzard, with the ultimate goal of being able to crate accounts, look at play statistics, view and chat with friends, and play a few games. I wanted to start out by taking a simple, 1v1 game with set rules. This would save me the trouble of coming up with mechanics and schemes, and instead allowed me to focus and learn the client server paridime. Once this project is complete, the next phase will be to create a "launcher" client and server that hosts player information and accessability to the coupled games, and finally add another game, this time creating a game from scratch when I have a solid grasp of how windows and c++ handle network communications. 

##Ownership:
I have written the majority of this project on my own with the help of the occasional stack overflow, msdn, and general google search. However, the server/client code is heavily modified from a tutorial I followed while trying to better learn the subject matter. The 3 methods for loading in textures for OpenGl are not mine but belong to a stack overflow answer. My goal is to focus on communications and not visuals. 

##Tasks left:
Allow player to select who they wish to talk to, (world or game)
Allow user to enter a Username and communicate with it.
Add more flexible way of limiting number of connections.

##Current Issues:
If players disconnect in rapid succession the server can throw an error. Hard to reproduce.


This project is approximatly 2,000 lines.