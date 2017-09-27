#ifndef BACKENDINTERFACE_H
#define BACKENDINTERFACE_H

#include <string>
#include <tuple>
#include <vector>

namespace backend {

/*!
 * \brief Class representing the loaded data.
 */
class Data {
public:
  const std::string name;                                     /*!< Name of the source file */
  const std::string path;                                     /*!< Absolute path to the source file */
  const std::string xDescription;                             /*!< Description (label) of X axis */
  const std::string yDescription;                             /*!< Description (label) of Y axis */
  const std::string xUnit;                                    /*!< Units of data on X axis */
  const std::string yUnit;                                    /*!< Units of data on Y axis */
  const std::vector<std::tuple<double, double>> datapoints;   /*!< [X, Y] tuples of datapoints */
};

/*!
 * Identifier of a specific backed
 */
class Identifier {
public:
  const std::string longDescription;            /*!< Human-readable description of the loader backend. This should be as descriptive as possible. */
  const std::string shortDescription;           /*!< Breif description of the loader. The string should be suitable for display in menus and other UI elements. */
  const std::string tag;                        /*!< Unique ID tag */
  const std::vector<std::string> loadOptions;   /*!< Description of each modifier of loading behavior */
};

class LoaderBackend {
public:
  /*!
   * \brief Destroys the loader object.
   */
  virtual void destroy() = 0;

  /*!
   * \brief Returns the identifier for this loader backend .
   * \return Identifier object.
   */
  virtual Identifier identifier() const = 0;

  /*!
   * \brief Loads data interactively.
   * \param option Loading behavior modifier.
   * \return Vector of <tt>Data</tt> objects, each corresponding to one loaded data file.
   */
  virtual std::vector<Data> load(const int option) = 0;

  /*!
   * \brief Loads data file interactively with a hint where to load data from.
   * \param hintPath Path where to look for the data. This may be adjusted form interactive prompt by the user.
   * \param option Loading behavior modifier.
   * \return Vector of <tt>Data</tt> objects, each corresponding to one loaded data file.
   */
  virtual std::vector<Data> loadHint(const std::string &hintPath, const int option) = 0;

  /*!
   * \brief Loads data from a given path.
   * \param path Path to a file or directory where to load data from.
   * \param option Loading behavior modifier.
   * \return Vector of <tt>Data</tt> objects, each corresponding to one loaded data file.
   */
  virtual std::vector<Data> loadPath(const std::string &path, const int option) = 0;
protected:
  virtual ~LoaderBackend() = 0;
};

typedef LoaderBackend *(*BackendInitializer)();

} // namespace backend

#endif // BACKENDINTERFACE_H
