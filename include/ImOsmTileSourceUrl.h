#pragma once
#include "ImOsmTileSourceAsync.h"
#include <filesystem>

namespace ImOsm {
class TileSourceUrl : public TileSourceAsync {
public:
  TileSourceUrl(int requestLimit, bool preload,
                const std::string &cachePath,
                const std::string &userAgent = "curl");
  virtual ~TileSourceUrl();
  static std::filesystem::path CachePathDefault();

protected:
  virtual bool receiveTile(int z, int x, int y, TileData &tileData) override;
  inline std::filesystem::path cachePath() const { return _cachePath; }
  std::filesystem::path cachePath(std::string url) const;


  virtual std::string makeUrl(int z, int x, int y) = 0;

private:
  std::string _userAgent{"curl"};
  std::filesystem::path _cachePath{CachePathDefault()};
};
} // namespace ImOsm
