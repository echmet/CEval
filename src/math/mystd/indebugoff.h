#pragma warn -8017

#define INDEBUG_HDR
#define INDEBUG_FHDR(F)

#define INDEBUG_CALL(F) (F)
#define INDEBUG_STR(F) (F)
#define INDEBUG_CALL2(T,F) (F)
#define INDEBUG_STR2(T,F) (F)
#define INDEBUG_VOID(F) F;
#define INDEBUG_COMMENT(S)
#define INDEBUG_INSPECT(F)
#define INDEBUG_SECTION(C)
#define INDEBUG_TIME
#define INDEBUG_DIRECT(C)

#define INDEBUG_NEWSTREAM(S)
#define INDEBUG_SCALL(S,F) (F)
#define INDEBUG_SSTR(S,F) (F)
#define INDEBUG_SCALL2(S,T,F) (F)
#define INDEBUG_SSTR2(S,T,F) (F)
#define INDEBUG_SVOID(S,F) F;
#define INDEBUG_SCOMMENT(S,C)
#define INDEBUG_SINSPECT(S,F)
#define INDEBUG_SSECTION(S,C)
#define INDEBUG_STIME(S)
#define INDEBUG_SDIRECT(S,C)

#pragma warn .8017


