#include "evalserializable.h"

const std::string EvalSerializable::HEADER_TAG = "CUTEEVAL_1.0_DATAFILE";

EvalSerializable::DataManipulator::DataManipulator(const EvalSerializable& Subject, ByteStream *Stream) :
  m_Subject(Subject),
  m_Stream(Stream),
  m_Ptr(nullptr),
  m_End(nullptr),
  m_Direction(EVS_OUT)
{
}
EvalSerializable::DataManipulator::DataManipulator(const EvalSerializable& Subject, const char **Ptr, const char * const End) :
  m_Subject(Subject),
  m_Stream(nullptr),
  m_Ptr(Ptr),
  m_End(End),
  m_Direction(EVS_IN)
{
}

EvalSerializable::EvalSerializable() :
  m_LastInternalError(EvalSerializable::SUCCESS)
{
}

EvalSerializable::~EvalSerializable()
{
}

EvalSerializable::RetCode EvalSerializable::CheckHeader(const char*& Ptr, const char * const End) const
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

EvalSerializable::RetCode EvalSerializable::DataManipulator::Header()
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

EvalSerializable::RetCode EvalSerializable::Deserialize(const ByteStream& Stream)
{
  const char *Ptr = &Stream[0];

  return Deserialize(&Ptr, Ptr + Stream.size());
}

EvalSerializable::RetCode EvalSerializable::Deserialize(const char **Ptr, const char * const End)
{
  DataManipulator DM(*this, Ptr, End);
  return Operate(DM);
}

EvalSerializable::RetCode EvalSerializable::Serialize(ByteStream& Stream)
{
  DataManipulator DM(*this, &Stream);
  return Operate(DM);
}


EvalSerializable::RetCode EvalSerializable::ReadFromFile(const char *FileName, EvalSerializable& Object)
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

std::string EvalSerializable::RetCodeToString(RetCode ret) {
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

EvalSerializable::RetCode EvalSerializable::WriteToFile(const char *FileName, EvalSerializable& Object)
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

EvalSerializable::RetCode EvalSerializable::WriteHeader(ByteStream& Stream) const
{
  RetCode Ret;

  Ret = AddData(HEADER_TAG, Stream);
  if (Ret != SUCCESS)
    return Ret;

  std::string TN = TypeName();
  return AddData(TN, Stream);
}

EvalSerializable::RetCode EvalSerializable::_AddData(const std::string& Data, ByteStream& Stream, Identity<std::string>)
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

EvalSerializable::RetCode EvalSerializable::_AddData(const std::wstring& Data, ByteStream& Stream, Identity<std::wstring>)
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

EvalSerializable::RetCode EvalSerializable::_ReadData(std::string& Data, const char*& Ptr, const char * const End, Identity<std::string>)
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

EvalSerializable::RetCode EvalSerializable::_ReadData(std::wstring& Data, const char*& Ptr, const char * const End, Identity<std::wstring>)
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
