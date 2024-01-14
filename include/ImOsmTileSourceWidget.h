#pragma once
#include <ini.h>
#include <memory>
#include <string>

namespace ImOsm {
class MapPlot;
class ITileLoader;

class TileSourceWidget {
public:
  TileSourceWidget(std::shared_ptr<MapPlot> mapPlot, std::string cache_dir);
  ~TileSourceWidget();

  void loadState(const mINI::INIStructure &ini);
  void saveState(mINI::INIStructure &ini) const;
  void paint();

private:
  void updateTileLoader();

private:
  std::string _cache_dir;
  std::shared_ptr<MapPlot> _mapPlot;
  std::shared_ptr<ITileLoader> _tileLoader;

  struct Ui;
  std::unique_ptr<Ui> _ui;
};
} // namespace ImOsm
