#pragma once
#include "ImOsmTileSourceUrl.h"

// https://www.arcgis.com/apps/mapviewer/index.html
// See Basemap

namespace ImOsm {
class TileSourceUrlCustom : public TileSourceUrl {
public:
  TileSourceUrlCustom(int request_limit, bool preload,
                      const std::string &url_tpl,
                      const std::string &cachePath);

protected:
  virtual std::string makeUrl(int z, int x, int y) override;

private:
  const std::string _url_tpl;
};

// -----------------------------------------------------------------------------

class TileSourceUrlOsm : public TileSourceUrl {
public:
  TileSourceUrlOsm(int request_limit, bool preload,
                   const std::string &cachePath);

protected:
  virtual std::string makeUrl(int z, int x, int y) override;

public:
  inline static const char URL_TPL[]{
      "https://a.tile.openstreetmap.org/{z}/{x}/{y}.png"};
};

// -----------------------------------------------------------------------------

class TileSourceUrlArcImagery : public TileSourceUrl {
public:
  TileSourceUrlArcImagery(int request_limit, bool preload,
                          const std::string &cachePath);

protected:
  virtual std::string makeUrl(int z, int x, int y) override;

public:
  inline static const char URL_TPL[]{
      "https://server.arcgisonline.com/ArcGIS/rest/services/"
      "World_Imagery/MapServer/tile/{z}/{y}/{x}"};
};
using TileSourceUrlArc = TileSourceUrlArcImagery;

} // namespace ImOsm
