# 🎲 Monopoly Game Simulation 
A custom version of the Monopoly game, themed around my faculty at **AGH University of Science and Technology**. 

## 🎯 Project Goal
The main goal was to turn the complex rules of Monopoly into working code. Instead of focusing only on graphics, I focused on **logic and data accuracy** (tracking money, transactions, and game states).

## ⚙️ Key Features
* **Game Rules:** The code handles all core mechanics: dice rolling, moving players, going to jail, and switching turns.
* **Money & Data:** I created a system that correctly calculates rent, buys properties, and updates player bank balances.
* **Clean Code (OOP):** I used C++ classes to organize Players, Board Tiles, and Cards. This makes the code easy to read and manage.
* **Accuracy:** I ensured that all data (money, ownership, player position) updates correctly after every move.

## 🏗️ Class Structure
* **MyGameInstance:** The "brain" of the project. It stores the global state of the game, manages player data, and tracks whose turn it is.
* **ABoardTile:** A base class for all spaces on the board. It defines shared data like `TileName`, `ID`, and `Location`.
    * *Inheritance:* Specialized classes like `PropertyTile` or `TaxTile` inherit from this to add specific logic (e.g., calculating rent).
* **DiceActor:** Handles the random number generation logic (1-6) to determine player movement.
* **Player Class:** Stores individual player data: current balance (money), list of owned properties, and current position on the board.
* **Tests (e.g., Test_TaxTile):** Scripts created to verify if the math (taxes, money exchange) is calculated correctly without bugs.

## 🛠 Technology Stack
* **Language:** C++
* **Engine:** Unreal Engine 5.4
* **Concepts:** Object-Oriented Programming (OOP), Algorithms, Data Structures
* **Tools:** Visual Studio, Git

## 📂 Source Code
You can find the main C++ logic in the `/Source/` directory.