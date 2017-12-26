#ifndef ASCSUPPORT_HANDLERS_H
#define ASCSUPPORT_HANDLERS_H

#include <functional>
#include <map>
#include <string>
#include <utility>
#include <vector>

namespace backend {

class ASCContext
{
public:
  ASCContext(const std::string &name, const std::string path,
             const size_t nChans, const std::string &kvDelim, const char valueDelim,
             const char dataDecimalPoint) :
    name{name},
    path{path},
    nChans{nChans},
    kvDelim(kvDelim),
    valueDelim{valueDelim},
    dataDecimalPoint{dataDecimalPoint}
  {
  }

  const std::string name;
  const std::string path;
  const size_t nChans;
  const std::string kvDelim;
  const char valueDelim;
  const char dataDecimalPoint;

  std::vector<int32_t> nDatapoints;
  std::vector<double> samplingRates;
  std::vector<double> xAxisMultipliers;
  std::vector<double> yAxisMultipliers;
  std::vector<std::string> xAxisTitles;
  std::vector<std::string> yAxisTitles;

  bool validate() {
    if (!((nDatapoints.size() == nChans) &&
          (samplingRates.size() == nChans) &&
          (xAxisMultipliers.size() == nChans) &&
          (yAxisMultipliers.size() == nChans)))
      return false;

    if (xAxisTitles.size() != nChans)
      xAxisTitles.resize(nChans, "");
    if (yAxisTitles.size() != nChans)
      yAxisTitles.resize(nChans, "");

    return true;
  }
};

class ASCTraitException : public std::exception
{
public:
  explicit ASCTraitException(std::string &&what) noexcept :
    m_what(what)
  {}

  const char * what() const noexcept override
  {
    return m_what.c_str();
  }

private:
  const std::string m_what;
};

class ASCFormatException : public std::exception
{
public:
  explicit ASCFormatException(const std::string &what) :
    std::exception{},
    m_what{what}
  {
  }

  const char * what() const noexcept{
    return m_what.c_str();
  }
private:
  const std::string m_what;
};

class EntryHandler
{
public:
  virtual ~EntryHandler() {}
  virtual void process(ASCContext &ctx, const std::string &entry) const = 0;
  virtual bool essential() const = 0;
};

template <bool ESSENTIAL>
class EntryHandlerEssentalityTrait : public EntryHandler {
public:
  virtual ~EntryHandlerEssentalityTrait() override {}
  virtual bool essential() const override { return ESSENTIAL; }
};

template <int EXPECTED, typename VT>
class EntryHandlerSizeTrait {
protected:
  void validateSize(const std::vector<VT> &v) const
  {
    if (v.size() != EXPECTED)
      throw ASCTraitException{"Unexpected array size"};
  }
};

template <typename VT>
class EntryHandlerSizeTrait<-1, VT>
{
protected:
  void checkIsSet(const std::vector<VT> &v, const ASCContext &ctx) const
  {
    if (v.size() == ctx.nChans)
      throw ASCTraitException{"Duplicit definition"};
  }

  void validateSize(const std::vector<VT> &v, const ASCContext &ctx) const
  {
    if (v.size() != ctx.nChans)
      throw ASCTraitException{"Unexpected array size"};
  }
};

template<typename VT>
class EntryHandlerMustBePositiveTrait
{
protected:
  void validatePositive(const std::vector<VT> &v) const
  {
    for (const auto &item : v)
      validatePositive(item);
  }

  void validatePositive(const VT &v) const
  {
    if (v <= VT(0)) throw ASCTraitException{"Value in array must be positive"};
  }
};

class EntryHandlerSamplingRate : public EntryHandlerEssentalityTrait<true>,
                                        EntryHandlerSizeTrait<-1, double>,
                                        EntryHandlerMustBePositiveTrait<double>
{
public:
  typedef std::function<void (std::vector<double>&, const char, const char, const std::string&)> Executor;

  EntryHandlerSamplingRate(const Executor &executor) :
    EntryHandlerSizeTrait<-1, double>{},
    EntryHandlerMustBePositiveTrait<double>{},
    m_executor{executor}
  {
  }

  virtual void process(ASCContext &ctx, const std::string &entry) const override
  {
    std::vector<double> rates{};

    checkIsSet(ctx.samplingRates, ctx);

    rates.reserve(ctx.nChans);
    m_executor(rates, ctx.valueDelim, ctx.dataDecimalPoint, entry);
    try {
      validateSize(rates, ctx);
      validatePositive(rates);
    } catch (const ASCTraitException &ex) {
      throw ASCFormatException(ID + std::string{": "} + ex.what());
    }

    ctx.samplingRates = std::move(rates);
  }

  static const std::string ID;

private:
  const Executor m_executor;
};
const std::string EntryHandlerSamplingRate::ID{"sampling rate"};

class EntryHandlerTotalDataPoints : public EntryHandlerEssentalityTrait<true>, EntryHandlerSizeTrait<-1, int32_t>
{
public:
  typedef std::function<void (std::vector<int32_t>&, const char, const std::string&)> Executor;

  EntryHandlerTotalDataPoints(const Executor &executor) :
    EntryHandlerSizeTrait<-1, int32_t>{},
    m_executor{executor}
  {
  }

  virtual void process(ASCContext &ctx, const std::string &entry) const override
  {
    std::vector<int32_t> numPoints{};

    checkIsSet(ctx.nDatapoints, ctx);

    numPoints.reserve(ctx.nChans);
    try {
      m_executor(numPoints, ctx.valueDelim, entry);
      validateSize(numPoints, ctx);
    } catch (const ASCTraitException &ex) {
      throw ASCFormatException(ID + std::string{": "} + ex.what());
    }

    ctx.nDatapoints = std::move(numPoints);
  }

  static const std::string ID;

private:
  const Executor m_executor;
};
const std::string EntryHandlerTotalDataPoints::ID{"total data points"};

class EntryHandlerXAxisMultiplier : public EntryHandlerEssentalityTrait<true>,
                                           EntryHandlerSizeTrait<-1, double>,
                                           EntryHandlerMustBePositiveTrait<double>
{
public:
  typedef std::function<void (std::vector<double>&, const char, const char, const std::string&)> Executor;

  EntryHandlerXAxisMultiplier(const Executor &executor) :
    EntryHandlerSizeTrait<-1, double>{},
    EntryHandlerMustBePositiveTrait<double>{},
    m_executor{executor}
  {
  }

  virtual void process(ASCContext &ctx, const std::string &entry) const override
  {
    std::vector<double> multipliers;

    checkIsSet(ctx.xAxisMultipliers, ctx);

    multipliers.reserve(ctx.nChans);
    m_executor(multipliers, ctx.valueDelim, ctx.dataDecimalPoint, entry);
    try {
      validateSize(multipliers, ctx);
      validatePositive(multipliers);
    } catch (const ASCTraitException &ex) {
      throw ASCFormatException(ID + std::string{": "} + ex.what());
    }

    ctx.xAxisMultipliers = std::move(multipliers);
  }

  static const std::string ID;

private:
  const Executor m_executor;
};
const std::string EntryHandlerXAxisMultiplier::ID{"x axis multiplier"};

class EntryHandlerYAxisMultiplier : public EntryHandlerEssentalityTrait<true>,
                                           EntryHandlerSizeTrait<-1, double>,
                                           EntryHandlerMustBePositiveTrait<double>
{
public:
  typedef std::function<void (std::vector<double>&, const char, const char, const std::string&)> Executor;

  EntryHandlerYAxisMultiplier(const Executor &executor) :
    EntryHandlerSizeTrait<-1, double>{},
    EntryHandlerMustBePositiveTrait<double>{},
    m_executor{executor}
  {
  }

  virtual void process(ASCContext &ctx, const std::string &entry) const override
  {
    std::vector<double> multipliers;

    checkIsSet(ctx.yAxisMultipliers, ctx);

    multipliers.reserve(ctx.nChans);
    m_executor(multipliers, ctx.valueDelim, ctx.dataDecimalPoint, entry);
    try {
      validateSize(multipliers, ctx);
      validatePositive(multipliers);
    } catch (const ASCTraitException &ex) {
      throw ASCFormatException(ID + std::string{": "} + ex.what());
    }

    ctx.yAxisMultipliers = std::move(multipliers);
  }

  static const std::string ID;

private:
  const Executor m_executor;
};
const std::string EntryHandlerYAxisMultiplier::ID{"y axis multiplier"};

class EntryHandlerXAxisTitle : public EntryHandlerEssentalityTrait<false>, EntryHandlerSizeTrait<-1, std::string>
{
public:
  typedef std::function<void (std::vector<std::string>&, const char, const std::string&)> Executor;

  EntryHandlerXAxisTitle(const Executor &executor) :
    EntryHandlerSizeTrait<-1, std::string>{},
    m_executor{executor}
  {
  }

  virtual void process(ASCContext &ctx, const std::string &entry) const override
  {
    std::vector<std::string> titles;

    checkIsSet(ctx.xAxisTitles, ctx);

    titles.reserve(ctx.nChans);
    m_executor(titles, ctx.valueDelim, entry);
    try {
      validateSize(titles, ctx);
    } catch (const ASCTraitException &ex) {
      throw ASCFormatException(ID + std::string{": "} + ex.what());
    }

    ctx.xAxisTitles = std::move(titles);
  }

  static const std::string ID;

private:
  const Executor m_executor;
};
const std::string EntryHandlerXAxisTitle::ID{"x axis title"};

class EntryHandlerYAxisTitle : public EntryHandlerEssentalityTrait<false>, EntryHandlerSizeTrait<-1, std::string>
{
public:
  typedef std::function<void (std::vector<std::string>&, const char, const std::string&)> Executor;

  EntryHandlerYAxisTitle(const Executor &executor) :
    EntryHandlerSizeTrait<-1, std::string>{},
    m_executor{executor}
  {
  }

  virtual void process(ASCContext &ctx, const std::string &entry) const override
  {
    std::vector<std::string> titles;

    checkIsSet(ctx.yAxisTitles, ctx);

    titles.reserve(ctx.nChans);
    m_executor(titles, ctx.valueDelim, entry);
    try {
      validateSize(titles, ctx);
    } catch (const ASCTraitException &ex) {
      throw ASCFormatException(ID + std::string{": "} + ex.what());
    }

    ctx.yAxisTitles = std::move(titles);
  }

  static const std::string ID;

private:
  const Executor m_executor;
};
const std::string EntryHandlerYAxisTitle::ID{"y axis title"};

typedef std::map<std::string, EntryHandler *> EntryHandlersMap;

} // namespace backend

#endif // ASCSUPPORT_HANDLERS_H
