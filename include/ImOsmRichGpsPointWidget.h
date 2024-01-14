#pragma once
#include "ImOsmCoords.h"
#include "ImOsmRichMarkItem.h"
#include <memory>

namespace ImOsm {
namespace Rich {
class MarkItem;

class GpsPointWidget {
public:
  GpsPointWidget(std::shared_ptr<MarkItem> item);
  ~GpsPointWidget();

  void paint();
  MarkItem::Style apply();

private:
  void paint_pickedBtn();
  void paint_markerCombo();

private:
  std::shared_ptr<MarkItem> _item;

  struct Ui;
  std::unique_ptr<Ui> _ui;
};
} // namespace Rich
} // namespace ImOsm
