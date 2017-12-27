#ifndef AVAILABLECHANNELS_H
#define AVAILABLECHANNELS_H

#include "ascsupport_handlers.h"

namespace backend {

class AvailableChannels
{
public:
  enum class State {
    NOT_SET,
    HEADERLESS,
    SET
  };

  explicit AvailableChannels() noexcept;
  AvailableChannels(const AvailableChannels &other);
  AvailableChannels(AvailableChannels &&other) noexcept;
  explicit AvailableChannels(const std::vector<std::string> &channels);
  explicit AvailableChannels(std::vector<std::string> &&channels) noexcept;
  const std::vector<std::string> & channels() const noexcept;
  bool matches(const ASCContext &ctx) const;
  AvailableChannels & operator=(const AvailableChannels &other);
  AvailableChannels & operator=(AvailableChannels &&other) noexcept;

  const State state;

  static AvailableChannels headerless();

private:
  std::vector<std::string> m_channels;
};

} // namespace backend

#endif // AVAILABLECHANNELS_H
