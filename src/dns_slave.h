enum
{
  SLAVE_IPTONAME = 'h',
  SLAVE_IDENTQ = 'i',
  SLAVE_IDENTQ_FAIL = 'j',
  SLAVE_IPTONAME_FAIL = 'k'
};

#define HostAddr      (1<<0)
#define HostName      (1<<1)
#define HostNameFail  (1<<2)
#define UserID        (1<<3)
#define UserIDFail    (1<<4)
