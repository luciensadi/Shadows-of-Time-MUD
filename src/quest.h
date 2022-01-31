#define MAX_RESCUE 15

typedef struct     rescue_type      RESCUE_DATA;
 
struct rescue_type
{
  sh_int vnumGood  [MAX_RESCUE];
  sh_int vnumEvil  [MAX_RESCUE];
};
