#ifndef INDEBUG_H
#define INDEBUG_H

// NASTAVENI MOZNOSTI

#define INDEBUG_ON
#define INDEBUG_FULLPATH_OFF
#define INDEBUG_LINESNO_PREC 5

//pokud je INDEBUG OFF, je vypnuta i cela deklarace namespace. Tu lze zapnout : _CLASS_ON
//(externi globalni promenna InDebug::Stream vsak presto nebude deklarovana)
//(CLASS je vzdy ON pri INDEBUG ON)
#define INDEBUG_CLASS_OFF

// ------------------------------------------------------------------

#ifdef INDEBUG_ON
 #define INDEBUG_CLASS_ON
#endif

#ifdef INDEBUG_CLASS_ON

 #include <stdio.h>
 #include <stdarg.h>
 #include <string.h>
 #include <fstream.h>
 #include <time.h>

namespace InDebug {

 extern char Buff[255];

 char * Inspect(char *format,...){
  va_list arg;
  va_start(arg,format);
  vsprintf(Buff,format,arg);
  va_end(arg);
  return Buff;
 }

 char * CutPath(char *p){
  char *pp = strrchr(p,'\\');
  return  pp ? pp+1 : p;
 }

 class TStream {
 private:
  fstream f;
  int _Sct;
  string Name;

 public:

  TStream(void) : _Sct(0), Name("InDebug.log"){}
  TStream(const char *s) : _Sct(0), Name(s){}
  ~TStream(void){if (f.is_open()) f.close();}

  /* tyto fce nemusi byt normalne vubec volany, jen pro interni potreby nebo ladeni */

  void Open(void) {
   if (!f.is_open()) {
    f.open(Name.c_str(), ios::in | ios::out);
    if (!f){
     f.clear();
     f.open(Name.c_str(), ios::in | ios::out | ios::trunc);
    }
   } else return;
   f.seekp(0,ios::end);
   time_t t = time(NULL);
   strcpy(Buff,ctime(&t));
   Buff[strlen(Buff) - 1] = '\0';
   f << std::endl << "=========================   " << Buff << "   =========================" << std::endl << std::endl;
   f.flush();
  }

  void Reset(void){
   if (f.is_open()) f.close(); f.clear(); _Sct = 0;
  }

  void NewLine(void) {
   f << std::endl; for (int i = 0; i != _Sct; ++i) f << "  "; f.flush();
  }

  void SectionCount(int i){
   if (i > 0) _Sct = i;
   else _Sct = 0;
  }

  int SectionCount(void){
   return _Sct;
  }

  __fastcall operator bool(){
   return bool(f);
  }

  fstream & F(){
   return f;
  }

  /* Stream ma byt ovladan pomoci nasledujicich operatoru a fci */

  TStream & __fastcall Comment(const string &s){
   Open(); (f << s.c_str()).flush();  return *this;
  }

  TStream & __fastcall Section(bool _Open){
   Open();
   if (_Open) {
    NewLine();
    NewLine();
    (f << "InDebug::SECTION{#" << _Sct << "}").flush();
    ++_Sct;
    NewLine();
    NewLine();
   } else  {
    if (_Sct) --_Sct;
    NewLine();
    NewLine();
    (f << "InDebug::SECTION{#" << _Sct << "}").flush();
    NewLine();
    NewLine();
   }
   return *this;
  }

  TStream & __fastcall Time(void){
   Open();
   time_t t = time(NULL);
   strcpy(Buff,ctime(&t));
   Buff[strlen(Buff) - 1] = '\0';
   (f << "InDebug::TIME{" << Buff << "}").flush();
   NewLine();
   return *this;
  }

  template<class T> T * Str(T *a){
   Open(); (f << a).flush(); NewLine(); return a;
  }

  template<class T> T operator<<(T a){
   Open(); (f << a).flush(); NewLine(); return a;
  };

  template<class T> T * operator<< <T *> (T *a){
   sprintf(Buff,"%Fp",a);
   (f << "InDebug::POINTER{" << Buff << "}").flush();
   NewLine();
   return a;
  }

 };

 #ifdef INDEBUG_ON
  extern TStream Stream;
 #endif

}; //namespace

#endif // INDEBUG_CLASS_ON

#include "InDebugOn.h" //Musi byt takto az zde...

#endif //INDEBUH_H
