CHAR_DATA *get_player_world_special (char *argument);


// Player class keep track of players
class Player
{
  public:
    Player() { name = str_dup(""); };
    Player(char *iName) { name = str_dup(iName); };  // Constructor
    Player(const Player *obj); // Copy Constructor
    void free_player() { free_string(name); };
    bool operator== (const Player &player2); // Overload == 
    void set_name(char *iName) { name = str_dup(iName); };  
    char *get_name() { return name; };
    void set_score(int iScore) { score = iScore; };
    int get_score() { return score; }; 
  private:
    char *name;
    int score;
};

// Class for one die
class Die
{
  public: 
    void roll();
    int get_value() { return value; };
  private:
    int value;
};

// Class for all crowns games
class Crowns
{
  public:
    Crowns(int dice, char *name);
    void roll();
    void play(CHAR_DATA *ch);
    void check_win(void);
    Crowns *find_game(char *name);
    Crowns *find_new_game(char *name);
    void show_value(CHAR_DATA *ch);
    void status(CHAR_DATA *ch);
    bool check_game(void);
    bool operator== (const Crowns &game2); // Overload == 


    // Manipulate data
    void set_type(int iType) { type = iType; };
    void set_bet(int iBet) { bet = iBet; };
    void set_pot(int iPot) { pot = iPot; };
    void set_potType(int iPot) { potType = iPot; };
    int get_pot() { return pot; };
    int get_potType() { return potType; };
    int get_bet() { return bet; };
    int get_value();
    int get_timer() { return timer; };
    void set_timer(int iTimer) { timer = iTimer; }; 
    void tick_timer() { timer--; };  
  
    // Echo to players
    void cecho(char *text);
    void cecho(char *text, char *exclude);

    // Player routines
    void clear_players();
    Player *get_player(char *name);
    Player *get_starter() { return startedBy; };
    Player *get_turn() { return turn; };
    Player *get_first_roll() { return firstRoll; };
    void remove_player(Player *player);
    void new_player(char *iName);

  private:
    Die dice[6]; // Array of die
    int numDice; // Number of dice
    int type; // Type of game
    int timer; // Timer for roll
    int bet; // Ante
    int potType; // gold or qp
    int pot; // Pot of bets
    list<Player> players; // List of players
    list<Player> rollList; // List of rollers
    Player *turn; // Whose turn is it?
    Player *startedBy; // Started by
    Player *firstRoll; // Who gets to roll first
};

// List of games playing
extern list<Crowns> CrownsList;
