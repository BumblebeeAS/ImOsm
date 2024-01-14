#pragma once
#include "ImOsmRichMarkStorage.h"
#include "ImOsmRichGpsPointWidget.h"
#include <map>
#include <ini.h>

namespace ImOsm {
namespace Rich {
class RichMapPlot;
class MarkStorage;
class MarkItemWidget;

struct Coord {
  float lat{0.0f};
  float lon{0.0f};
  uint64_t time{0};
  std::string name;
};

struct Coords {
  std::vector<Coord> coords;
  std::string name;
  bool visible{true};
  MarkItem::Style style;
};

class MarkViewerWidget {
public:
  MarkViewerWidget(std::shared_ptr<RichMapPlot> plot,
                   std::shared_ptr<MarkStorage> storage);
  ~MarkViewerWidget();

  int historySize;
  void paint();
  void insert_latLon(float lat, float lon, uint64_t stamp, const std::string name);

private:
  void paint_mousePickBtn();
  void paint_markNameInput();
  void paint_addMarkBtn();
  void paint_markTable();
  void paint_markTableRow(std::shared_ptr<Coords> item);

  std::shared_ptr<RichMapPlot> _plot;
  std::shared_ptr<MarkStorage> _storage;
  std::unique_ptr<GpsPointWidget> _itemWidget;
  std::map<std::string, std::shared_ptr<Coords>> coords;


  struct Ui;
  std::unique_ptr<Ui> _ui;
};
} // namespace Rich
} // namespace ImOsm
