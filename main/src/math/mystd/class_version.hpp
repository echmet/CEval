template<typename DERIVED, unsigned VER> class class_version;

template<typename DERIVED> class class_version<DERIVED, 0> {
protected:

  template<unsigned AVER> ALoad();

  template<> ALoad<0>() {}

public:

  void Load() {}

};

template<typename DERIVED, unsigned VER>
class class_version : public class_version<DERIVED, VER - 1>
{
private:
public:

  void Load()
  {

    if (LoadVersion() != VER) {
      if ( static_cast<DERIVED *>(this)->IsBackCompatible<VER>() ) {

        static_cast< class_version<DERIVED, VER-1> *>(this)->Load();
        static_cast< DERIVED *>                      (this)->RemainderDefault<VER>();

      }
    } else {

        static_cast< DERIVED *>(this)->ALoad<VER>();

    }

  }

  $$$ nejak v tomhle smyslu...

};

