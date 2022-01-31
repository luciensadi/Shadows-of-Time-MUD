#include <iostream.h>
#include <pspell/pspell.h>

void main ()
{
  char *word;
  char *wrong = "tst";

  PspellConfig * spell_config = new_pspell_config();
  pspell_config_replace(spell_config, "lang", "en_US");
  PspellCanHaveError * possible_err = new_pspell_manager(spell_config);
  PspellManager * spell_checker = 0;
  if (pspell_error_number(possible_err) != 0) 
  {
    cout << (pspell_error_message(possible_err)) << endl;
    return 0;
  }
  else
    spell_checker = to_pspell_manager(possible_err);

  int correct = pspell_manager_check(spell_checker, wrong);  

  const PspellWordList * suggestions = pspell_manager_suggest(spell_checker,
                                     wrong);
  PspellStringEmulation * elements = pspell_word_list_elements(suggestions);
  while (( word = (char *) pspell_string_emulation_next(elements)) != NULL)
  {
     cout << word;
  }
  delete_pspell_string_emulation(elements); 

}


