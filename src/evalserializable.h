#ifndef _EVALSERIALIZABLE_H
#define _EVALSERIALIZABLE_H

#include <cstdio>
#include <cstring>
#include <exception>
#include <cstdint>
#include <string>
#include <vector>
#include <type_traits>

//#include <windows.h>

class EvalSerializable {
public:
  enum RetCode {
    SUCCESS,
    ERR_BADSIZE,
    ERR_BADTYPE,
    ERR_CANTDESERIALIZE,
    ERR_CANTOPEN,
    ERR_CANTREAD,
    ERR_CANTSERIALIZE,
    ERR_CANTWRITE,
    ERR_NOMEM,
    ERR_OUTOFRANGE,
    ERR_RUNTIME,
    ERR_UNKWFILE
  };

  typedef std::vector<char> ByteStream;

protected:
  template<bool B>
  struct bool_ {};

  template<typename I>
  struct Identity { typedef I type; };

  enum Direction {
    EVS_IN,  /* Deserialization */
    EVS_OUT  /* Serialization */
  };

  class DataManipulator {
  public:
    DataManipulator(const EvalSerializable& Subject, ByteStream *Stream) :
                    m_Subject(Subject), m_Stream(Stream), m_Ptr(nullptr), m_End(nullptr), m_Direction(EVS_OUT)
                    {}
    DataManipulator(const EvalSerializable& Subject, const char **Ptr, const char * const End) :
                    m_Subject(Subject), m_Stream(nullptr), m_Ptr(Ptr), m_End(End), m_Direction(EVS_IN)
                    {}

    RetCode Header(void)
    {
      switch (m_Direction) {
      case EVS_IN:
        return m_Subject.CheckHeader(*m_Ptr, m_End);
      case EVS_OUT:
        return m_Subject.WriteHeader(*m_Stream);
      default:
        return ERR_RUNTIME;
      }
    }

    template<typename T>
    RetCode Operate(T& Data)
    {
      return _Operate(bool_<std::is_base_of<EvalSerializable, T>::value>(), Data);
    }

    private:
    template<typename T>
    RetCode _Operate(bool_<false>, T &Data)
    {
      switch (m_Direction) {
        case EVS_IN:
          return ReadData(Data, *m_Ptr, m_End);
        case EVS_OUT:
          return AddData(Data, *m_Stream);
        default:
          return ERR_RUNTIME;
      }
    }

    template<typename T>
    RetCode _Operate(bool_<true>, T &Data)
    {
      switch (m_Direction) {
      case EVS_IN:
        return Data.Deserialize(m_Ptr, m_End);
      case EVS_OUT:
        return Data.Serialize(*m_Stream);
      default:
        return ERR_RUNTIME;
      }
    }

    const EvalSerializable &m_Subject;
    ByteStream *m_Stream;
    const char **m_Ptr;
    const char * const m_End;
    const Direction m_Direction;
  };

public:
  inline RetCode LastError() { return m_LastInternalError; }

  RetCode Deserialize(const ByteStream& Stream)
  {
    const char *Ptr = &Stream[0];

    return Deserialize(&Ptr, Ptr + Stream.size());
  }

  RetCode Deserialize(const char **Ptr, const char * const End)
  {
    DataManipulator DM(*this, Ptr, End);
    return Operate(DM);
  }

  RetCode Serialize(ByteStream& Stream)
  {
    DataManipulator DM(*this, &Stream);
    return Operate(DM);
  }

  virtual RetCode Operate(DataManipulator& Manipulator) = 0;

  static RetCode ReadFromFile(const char *FileName, EvalSerializable& Object)
  {
    FILE *Fh;
    size_t FSize;
    ByteStream Stream;
    RetCode Ret;

    Fh = fopen(FileName, "rb");
    if (Fh == nullptr)
      return ERR_CANTOPEN;

    fseek(Fh, 0, SEEK_END);
    FSize = ftell(Fh);
    fseek(Fh, 0, SEEK_SET);

    Stream.resize(FSize);
    if (fread(&Stream[0], FSize, 1, Fh) != 1)
      return ERR_CANTREAD;

    fclose(Fh);

    Ret = Object.Deserialize(Stream);
    if (Ret != SUCCESS) {
      Object.m_LastInternalError = Ret;
      return ERR_CANTDESERIALIZE;
    }

    return SUCCESS;
  }

  static std::string RetCodeToString(RetCode ret) {
  switch (ret) {
    case SUCCESS:
      return "Success";
    case ERR_BADSIZE:
      return "Size of the next data block differs from the size of the type";
    case ERR_BADTYPE:
      return "Object types do not match";
    case ERR_CANTDESERIALIZE:
      return "Object could not have been deserialized";
    case ERR_CANTOPEN:
      return "Cannot open file";
    case ERR_CANTREAD:
      return "Cannot read from file";
    case ERR_CANTSERIALIZE:
      return "Object could not have been serialized";
    case ERR_CANTWRITE:
      return "Cannt write to file";
    case ERR_NOMEM:
      return "Out of memory";
    case ERR_OUTOFRANGE:
      return "Attempted to read past the bytestream size";
    case ERR_RUNTIME:
      return "Runtime error";
    case ERR_UNKWFILE:
      return "Unknown type of file";
    default:
      return "Unknown error";
    }
  }

  static RetCode WriteToFile(const char *FileName, EvalSerializable& Object)
  {
    FILE *Fh;
    RetCode Ret;
    ByteStream Stream;

    Ret = Object.Serialize(Stream);

    if (Ret != SUCCESS) {
      Object.m_LastInternalError = Ret;
      return ERR_CANTSERIALIZE;
    }

    Fh = fopen(FileName, "wb");
    if (Fh == nullptr)
      return ERR_CANTOPEN;

    if (fwrite(&Stream[0], Stream.size(), 1, Fh) != 1)
      return ERR_CANTWRITE;

    fflush(Fh);
    fclose(Fh);

    return SUCCESS;
  }

protected:
  explicit EvalSerializable() : m_LastInternalError(EvalSerializable::SUCCESS) {}
  ~EvalSerializable() {}

  RetCode CheckHeader(const char*& Ptr, const char * const End) const
  {
    std::string Header;
    std::string TN;

    ReadData(Header, Ptr, End);
    if (Header.compare(HEADER_TAG) != 0)
      return ERR_UNKWFILE;

    ReadData(TN, Ptr, End);
    if (TN.compare(TypeName()) != 0)
      return ERR_BADTYPE;

    return SUCCESS;
  }

  virtual std::string TypeName() const = 0;

  RetCode WriteHeader(ByteStream& Stream) const
  {
    RetCode Ret;

    Ret = AddData(HEADER_TAG, Stream);
    if (Ret != SUCCESS)
      return Ret;

    std::string TN = TypeName();
    return AddData(TN, Stream);
  }

  template<typename T>
  static RetCode AddData(const T& Data, ByteStream& Stream)
  {
    return _AddData(Data, Stream, Identity<T>());
  }

  template<typename T>
  static RetCode ReadData(T& Data, const char*& Ptr, const char * const End)
  {
    return _ReadData(Data, Ptr, End, Identity<T>());
  }

private:
  template<typename T>
  static RetCode _AddData_fundamental(const T& Data, ByteStream& Stream, bool_<true>)
  {
    const uint32_t S = sizeof(Data);
    const char *PSize = reinterpret_cast<const char*>(&S);
    const char *PData = reinterpret_cast<const char*>(&Data);

    try {
      for (size_t idx = 0; idx < sizeof(S); idx++)
        Stream.push_back(PSize[idx]);

      for (size_t idx = 0; idx < S; idx++)
        Stream.push_back(PData[idx]);

    } catch (std::bad_alloc& ) {
      return ERR_NOMEM;
    }

    return SUCCESS;
  }

  template<typename T>
  static RetCode _AddData(const T& Data, ByteStream& Stream, Identity<T>)
  {
    return _AddData_fundamental(Data, Stream, bool_<std::is_fundamental<T>::value>());
  }

  static RetCode _AddData(const std::string& Data, ByteStream& Stream, Identity<std::string>)
  {
    const uint32_t S = Data.length();
    const char *PLength = reinterpret_cast<const char*>(&S);
    const char *PData = Data.c_str();

    try {
      for (size_t idx = 0; idx < sizeof(S); idx++)
        Stream.push_back(PLength[idx]);

      for (size_t idx = 0; idx < S; idx++)
        Stream.push_back(PData[idx]);
    } catch (std::bad_alloc& ) {
      return ERR_NOMEM;
    }

    return SUCCESS;
  }

  static RetCode _AddData(const std::wstring& Data, ByteStream& Stream, Identity<std::wstring>)
  {
    const uint32_t S = Data.length() * sizeof(wchar_t);
    const char *PLength = reinterpret_cast<const char*>(&S);
    const char *PData = reinterpret_cast<const char*>(Data.c_str());

    try {
      for (size_t idx = 0; idx < sizeof(S); idx++)
        Stream.push_back(PLength[idx]);

      for (size_t idx = 0; idx < S; idx++)
        Stream.push_back(PData[idx]);
    } catch (std::bad_alloc& ) {
      return ERR_NOMEM;
    }

    return SUCCESS;
  }

  template<typename T>
  static RetCode _AddData(const std::vector<T>& Data, ByteStream& Stream, Identity<std::vector<T> >)
  {
    return _AddData_Vector(Data, Stream, bool_<std::is_base_of<EvalSerializable, T>::value>());
  }

  template<typename T>
  static RetCode _AddData_Vector(const std::vector<T>& Data, ByteStream& Stream, bool_<false>)
  {
    const uint32_t Size = Data.size();
    const char *PSize = reinterpret_cast<const char*>(&Size);

    try {
      for (size_t idx = 0; idx < sizeof(Size); idx++)
        Stream.push_back(PSize[idx]);

      for (size_t idx = 0; idx < Size; idx++) {
        RetCode Ret = AddData(Data.at(idx), Stream);
        if (Ret != SUCCESS)
          return Ret;
      }
    } catch (std::bad_alloc& ) {
      return ERR_NOMEM;
    }

    return SUCCESS;
  }

  template<typename T>
  static RetCode _AddData_Vector(const std::vector<T>& Data, ByteStream& Stream, bool_<true>)
  {
    const uint32_t Size = Data.size();
    const char *PSize = reinterpret_cast<const char*>(&Size);

    try {
      for (size_t idx = 0; idx < sizeof(Size); idx++)
        Stream.push_back(PSize[idx]);

      for (size_t idx = 0; idx < Size; idx++) {
        T& Item = const_cast<T&>(Data.at(idx));
        RetCode Ret = Item.Serialize(Stream);
        if (Ret != SUCCESS)
          return Ret;
      }
    } catch (std::bad_alloc& ) {
      return ERR_NOMEM;
    }

    return SUCCESS;
  }

  template<typename T>
  static RetCode _ReadData_fundamental(T& Data, const char*& Ptr, const char * const End, bool_<true>)
  {
    uint32_t Size;

    Size = *reinterpret_cast<const uint32_t*>(Ptr);
    Ptr += sizeof(Size);

    if (Size != sizeof(T))
      return ERR_BADSIZE;
    if (Ptr + Size > End)
      return ERR_OUTOFRANGE;

    Data = *reinterpret_cast<const T*>(Ptr);
    Ptr += sizeof(T);

    return SUCCESS;
  }

  template<typename T>
  static RetCode _ReadData(T& Data, const char*& Ptr, const char * const End, Identity<T>)
  {
    return _ReadData_fundamental(Data, Ptr, End, bool_<std::is_fundamental<T>::value>());
  }

  static RetCode _ReadData(std::string& Data, const char*& Ptr, const char * const End, Identity<std::string>)
  {
    uint32_t Length;
    char *Ch;

    Length = *reinterpret_cast<const uint32_t*>(Ptr);
    Ptr += sizeof(Length);

    if (Ptr + Length > End)
      return ERR_OUTOFRANGE;

    if (Length == 0) {
      Data = std::string();
      return SUCCESS;
    }

    Ch = new char[Length+1];
    if (Ch == nullptr)
      return ERR_NOMEM;

    memcpy(Ch, Ptr, Length);
    Ch[Length] = '\0';
    Data = std::string(Ch);
    Ptr += Length;
    delete[] Ch;

    return SUCCESS;
  }

  static RetCode _ReadData(std::wstring& Data, const char*& Ptr, const char * const End, Identity<std::wstring>)
  {
    uint32_t Length;
    uint32_t WLength;
    wchar_t *Ch;

    Length = *reinterpret_cast<const uint32_t*>(Ptr);
    Ptr += sizeof(Length);

    if (Ptr + Length > End)
      return ERR_OUTOFRANGE;

    if (Length == 0) {
      Data = std::wstring();
      return SUCCESS;
    }
    WLength = Length / sizeof(wchar_t);


    Ch = new wchar_t[WLength + 1];
    if (Ch == nullptr)
      return ERR_NOMEM;

    memcpy(Ch, Ptr, Length);
    Ch[WLength] = '\0';
    Data = std::wstring(Ch);
    Ptr += Length;
    delete[] Ch;

    return SUCCESS;
  }

  template<typename T>
  static RetCode _ReadData(std::vector<T>& Data, const char*& Ptr, const char * const End, Identity<std::vector<T>>)
  {
    return _ReadData_Vector(Data, Ptr, End, bool_<std::is_base_of<EvalSerializable, T>::value>());
  }

  template<typename T>
  static RetCode _ReadData_Vector(std::vector<T>& Data, const char*& Ptr, const char * const End, bool_<false>)
  {
    uint32_t Length;

    Length = *reinterpret_cast<const uint32_t*>(Ptr);
    Ptr += sizeof(Length);

    if (Ptr + Length > End)
      return ERR_OUTOFRANGE;

    for (size_t idx = 0; idx < Length; idx++) {
      T Item;
      RetCode Ret = ReadData(Item, Ptr, End);

      if (Ret != SUCCESS)
        return Ret;

      Data.push_back(Item);
    }

    return SUCCESS;
  }

  template<typename T>
  static RetCode _ReadData_Vector(std::vector<T>& Data, const char*& Ptr, const char * const End, bool_<true>)
  {
    uint32_t Length;

    Length = *reinterpret_cast<const uint32_t*>(Ptr);
    Ptr += sizeof(Length);

    if (Ptr + Length > End)
      return ERR_OUTOFRANGE;

    for (size_t idx = 0; idx < Length; idx++) {
      T Item;
      RetCode Ret = Item.Deserialize(&Ptr, End);

      if (Ret != SUCCESS)
        return Ret;

      Data.push_back(Item);
    }

    return SUCCESS;
  }

  RetCode m_LastInternalError;

  static const std::string HEADER_TAG;
};

#endif //_IEVALSERIALIZABLE_H
