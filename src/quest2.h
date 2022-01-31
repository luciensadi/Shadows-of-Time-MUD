void add_quest args((CHAR_DATA *ch, int number));
bool has_quest args((CHAR_DATA *ch, int number));
bool on_quest args((CHAR_DATA *ch, int number));
bool has_completed_quest_step args ((CHAR_DATA *ch, int number, int step));
bool has_completed_quest args ((CHAR_DATA *ch, int number));
void quest_completed_step args ((CHAR_DATA *ch, int quest, int step));
void update_quest_status args ((CHAR_DATA *ch, int quest, char *status));
void load_quest_info args ((void));
void save_quest_info args ((void));
