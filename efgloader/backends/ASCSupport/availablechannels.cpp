#include "availablechannels.h"

namespace backend {

AvailableChannels AvailableChannels::headerless()
{
  AvailableChannels ac{};
  const_cast<State&>(ac.state) = State::HEADERLESS;

  return ac;
}

AvailableChannels::AvailableChannels() noexcept :
  state{State::NOT_SET}
{}

AvailableChannels::AvailableChannels(const AvailableChannels &other) :
  state{other.state},
  m_channels{other.m_channels}
{}

AvailableChannels::AvailableChannels(AvailableChannels &&other) noexcept :
  state{other.state},
  m_channels(std::move(other.m_channels))
{}

AvailableChannels::AvailableChannels(const std::vector<std::string> &channels) :
  state{State::SET},
  m_channels{channels}
{}

AvailableChannels::AvailableChannels(std::vector<std::string> &&channels) noexcept :
  state{State::SET},
  m_channels(channels)
{}

const std::vector<std::string> & AvailableChannels::channels() const noexcept
{
  return m_channels;
}

bool AvailableChannels::matches(const ASCContext &ctx) const
{
  if (state == State::NOT_SET)
    return false;

  if (ctx.yAxisTitles.size() != m_channels.size())
    return false;

  for (size_t idx = 0; idx < m_channels.size(); idx++) {
    if (ctx.yAxisTitles.at(idx) != m_channels.at(idx))
      return false;
  }

  return true;
}

AvailableChannels & AvailableChannels::operator=(const AvailableChannels &other)
{
  const_cast<State&>(state) = other.state;
  m_channels = other.m_channels;

  return *this;
}

AvailableChannels & AvailableChannels::operator=(AvailableChannels &&other) noexcept
{
  const_cast<State&>(state) = other.state;
  m_channels = std::move(other.m_channels);

  return *this;
}

} // namespace backend
