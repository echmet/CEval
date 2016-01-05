#ifndef MODECONTEXTTYPES
#define MODECONTEXTTYPES

class ModeContextTypes {
public:
  enum class Types {
    EVALUATION = 0,
    HYPERBOLE_FIT = 1,
    LAST_INDEX = 2
  };

  static constexpr int ID(const Types item) {
    return static_cast<std::underlying_type<Types>::type>(item);
  }

  template <typename T>
  static T fromID(const int idx)
  {
    Q_ASSERT(idx >= 0 && idx < static_cast<int>(T::LAST_INDEX));

    return static_cast<T>(idx);
  }
};

#endif // MODECONTEXTTYPES

