#pragma once
#include "ImOsmRichMapPlot.h"
#include "ImOsmRichMarkItemWidget.h"
#include "ImOsmRichMarkStorage.h"
#include <array>
#include <imgui.h>
#include <ini.h>

namespace ImOsm {
namespace Rich {
class MarkEditorWidget
{
public:
  MarkEditorWidget(std::shared_ptr<RichMapPlot> plot,
                   std::shared_ptr<MarkStorage> storage);
  virtual ~MarkEditorWidget() = default;

  void loadState(const mINI::INIStructure &ini);
  void saveState(mINI::INIStructure &ini) const;
  void paint();

private:
  void paint_latLonInput();
  void paint_mousePickBtn();
  void paint_markNameInput();
  void paint_addMarkBtn();
  void paint_markTable();
  void paint_markTableRow(const MarkStorage::ItemNode &item);

  std::array<float, 2> _latLonInput{0.f, 0.f};
  std::shared_ptr<RichMapPlot> _plot;
  std::shared_ptr<MarkStorage> _storage;
  std::unique_ptr<MarkItemWidget> _itemWidget;

  bool _isMousePick{false};
  std::string _markNameInputText{};
  bool _isMarkAdd{false};

  inline static const char _latlonFormat[]{"%.6f"};
};
} // namespace Rich
} // namespace ImOsm