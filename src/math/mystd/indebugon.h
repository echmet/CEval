#ifdef INDEBUG_ON

 #pragma warn -8017

 #ifdef INDEBUG_FULLPATH_ON
  #define INDEBUG_HDR (string(__FILE__) + " " + InDebug::Inspect("%0*d",INDEBUG_LINESNO_PREC,__LINE__) + " :: ")
 #else
  #define INDEBUG_HDR (string(InDebug::CutPath(__FILE__)) + " " + InDebug::Inspect("%0*d",INDEBUG_LINESNO_PREC,__LINE__) + " :: ")
 #endif
 #define INDEBUG_FHDR(F) (INDEBUG_HDR + (#F " -> "))

 #define INDEBUG_CALL(F) (InDebug::Stream.Comment(INDEBUG_FHDR(F)) << (F))
 #define INDEBUG_STR(F) (InDebug::Stream.Comment(INDEBUG_FHDR(F)).Str(F))
 #define INDEBUG_CALL2(T,F) (InDebug::Stream.Comment(INDEBUG_FHDR(F)).operator<< <T> (F))
 #define INDEBUG_STR2(T,F) (InDebug::Stream.Comment(INDEBUG_FHDR(F)).Str<T>(F))
 #define INDEBUG_VOID(F) {(InDebug::Stream.Comment(INDEBUG_FHDR(F) + "void").NewLine(); F;}
 #define INDEBUG_COMMENT(S) (InDebug::Stream.Comment(INDEBUG_HDR + ("InDebug::COMMENT{" S "}"))).NewLine();
 #define INDEBUG_INSPECT(F) {(InDebug::Stream.Comment(INDEBUG_HDR + ("InDebug::INSPECT{" #F " -> "))) << F;}
 #define INDEBUG_SECTION(C) InDebug::Stream.Comment(INDEBUG_HDR).Section(C);
 #define INDEBUG_TIME InDebug::Stream.Comment(INDEBUG_HDR).Time();
 #define INDEBUG_DIRECT(C) InDebug::Stream.F << C;

 #define INDEBUG_NEWSTREAM(S) InDebug::TStream S(#S);
 #define INDEBUG_SCALL(S,F) (InDebug::##S##.Comment(INDEBUG_FHDR(F)) << (F))
 #define INDEBUG_SSTR(S,F) (InDebug::##S##.Comment(INDEBUG_FHDR(F)).Str(F))
 #define INDEBUG_SCALL2(S,T,F) (InDebug::##S##.Comment(INDEBUG_FHDR(F)).operator<< <T> (F))
 #define INDEBUG_SSTR2(S,T,F) (InDebug::##S##.Comment(INDEBUG_FHDR(F)).Str<T>(F))
 #define INDEBUG_SVOID(S,F) {(InDebug::##S##.Comment(INDEBUG_FHDR(F) + "void").NewLine(); F;}
 #define INDEBUG_SCOMMENT(S,C) (InDebug::##S##.Comment(INDEBUG_HDR + ("InDebug::COMMENT{" C "}"))).NewLine();
 #define INDEBUG_SINSPECT(S,F) {(InDebug::##S##.Comment(INDEBUG_HDR + ("InDebug::INSPECT{" #F " -> "))) << F;}
 #define INDEBUG_SSECTION(S,C) InDebug::##S##.Comment(INDEBUG_HDR).Section(C);
 #define INDEBUG_STIME(S) InDebug::##S##.Comment(INDEBUG_HDR).Time();
 #define INDEBUG_SDIRECT(S,C) InDebug::##S##.F << C;

 #pragma warn .8017

#else

 #include "InDebugOff.h"

#endif //INDEBUG_ON

