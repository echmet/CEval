#include "ascsupport_handlers.h"

namespace backend {

const std::string & EntryHandlerSamplingRate::ID() noexcept
{
  static const std::string s{"sampling rate"};

  return s;
}

const std::string & EntryHandlerTotalDataPoints::ID() noexcept
{
  static const std::string s{"total data points"};

  return s;
}

const std::string & EntryHandlerXAxisMultiplier::ID() noexcept
{
  static const std::string s{"x axis multiplier"};

  return s;
}
const std::string & EntryHandlerXAxisMultiplier::id() const noexcept
{
  return this->ID();
}

const std::string & EntryHandlerYAxisMultiplier::ID() noexcept
{
  static const std::string s{"y axis multiplier"};

  return s;
}
const std::string & EntryHandlerYAxisMultiplier::id() const noexcept
{
  return this->ID();
}

const std::string & EntryHandlerXAxisTitle::ID() noexcept
{
  static const std::string s{"x axis title"};

  return s;
}
const std::string & EntryHandlerXAxisTitle::id() const noexcept
{
  return this->ID();
}

const std::string & EntryHandlerYAxisTitle::ID() noexcept
{
  static const std::string s{"y axis title"};

  return s;
}
const std::string & EntryHandlerYAxisTitle::id() const noexcept
{
  return this->ID();
}

} // namespace backend
