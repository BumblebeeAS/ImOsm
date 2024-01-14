#include "ImOsmTileSourceUrlImpl.h"
#include <sstream>

namespace ImOsm
{
TileSourceUrlCustom::TileSourceUrlCustom(
    int request_limit, bool preload, const std::string& url_tpl,
    const std::string &cachePath) :
  TileSourceUrl{request_limit, preload,
                cachePath}, _url_tpl{url_tpl}
{}

std::string TileSourceUrlCustom::makeUrl(int z, int x, int y)
{
  std::string url{_url_tpl};
  const size_t pos_z =
    std::min({url.find("{z}"), url.find("{Z}"), url.find("$z$"), url.find("$Z$")});
  if (pos_z < url.size())
  {
    url = url.replace(pos_z, 3, std::to_string(z));
    const size_t pos_x =
        std::min({url.find("{x}"), url.find("{X}"), url.find("$x$"), url.find("$X$")});
    if (pos_x < url.size())
    {
      url = url.replace(pos_x, 3, std::to_string(x));
      const size_t pos_y =
        std::min({url.find("{y"), url.find("{Y}"), url.find("$y$"), url.find("$Y$")});
      if (pos_y < url.size())
      {
        url = url.replace(pos_y, 3, std::to_string(y));
      }
    }
  }
  return url;
}

// -----------------------------------------------------------------------------

TileSourceUrlOsm::TileSourceUrlOsm(int request_limit, bool preload,
    const std::string &cachePath) :
  TileSourceUrl{request_limit, preload, cachePath}
{}

std::string TileSourceUrlOsm::makeUrl(int z, int x, int y)
{
  std::ostringstream urlmaker;
  urlmaker << "https://a.tile.openstreetmap.org";
  urlmaker << '/' << z << '/' << x << '/' << y << ".png";
  return urlmaker.str();
}
// -----------------------------------------------------------------------------

TileSourceUrlArcImagery::TileSourceUrlArcImagery(int request_limit, bool preload,
    const std::string &cachePath) :
  TileSourceUrl{request_limit, preload, cachePath}
{}

std::string TileSourceUrlArcImagery::makeUrl(int z, int x, int y)
{
  std::ostringstream urlmaker;
  urlmaker << "https://server.arcgisonline.com/ArcGIS/rest/"
              "services/World_Imagery/MapServer/tile";
  urlmaker << '/' << z << '/' << y << '/' << x;
  return urlmaker.str();
}

}   // namespace ImOsm
