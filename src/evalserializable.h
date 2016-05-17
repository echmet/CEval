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
    DataManipulator(const EvalSerializable& Subject, ByteStream *Stream);
    DataManipulator(const EvalSerializable& Subject, const char **Ptr, const char * const End);

    RetCode Header();

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

  RetCode Deserialize(const ByteStream& Stream);
  RetCode Deserialize(const char **Ptr, const char * const End);
  RetCode Serialize(ByteStream& Stream);

  virtual RetCode Operate(DataManipulator& Manipulator) = 0;

  static RetCode ReadFromFile(const char *FileName, EvalSerializable& Object);
  static std::string RetCodeToString(RetCode ret);
  static RetCode WriteToFile(const char *FileName, EvalSerializable& Object);

protected:
  explicit EvalSerializable();
  ~EvalSerializable();

  RetCode CheckHeader(const char*& Ptr, const char * const End) const;

  virtual std::string TypeName() const = 0;

  RetCode WriteHeader(ByteStream& Stream) const;

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

  static RetCode _ReadData(std::string& Data, const char*& Ptr, const char * const End, Identity<std::string>);
  static RetCode _ReadData(std::wstring& Data, const char*& Ptr, const char * const End, Identity<std::wstring>);

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
