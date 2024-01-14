#include "ImOsmRichGpsPointWidget.h"
#include "ImOsmRichMarkItem.h"
#include <misc/cpp/imgui_stdlib.h>

namespace ImOsm {
namespace Rich {

struct GpsPointWidget::Ui {
  inline static constexpr char latLonFormat[]{"%.6f"};
  std::array<float, 2> latLon{};
  std::string text;
  std::string markerTypeName;
  ImPlotMarker markerType;
  bool textEnabled{}, markerEnabled{};
  float markerSize{}, markerWeight{};
  std::array<float, 3> markerFill{};
};

inline static constexpr const char *MarkerTypeName(ImPlotMarker marker);

GpsPointWidget::GpsPointWidget(std::shared_ptr<MarkItem> item)
    : _item{item}, _ui{std::make_unique<Ui>()} {
  _ui->text = _item->text();
  _ui->latLon[0] = _item->geoCoords().lat;
  _ui->latLon[1] = _item->geoCoords().lon;
  _ui->textEnabled = _item->style().textEnabled;
  _ui->markerEnabled = _item->style().markerEnabled;
  _ui->markerType = _item->style().markerType;
  _ui->markerTypeName = MarkerTypeName(_ui->markerType);
  _ui->markerSize = _item->style().markerSize;
  _ui->markerWeight = _item->style().markerWeight;
  _ui->markerFill = {_item->style().markerFill.x, _item->style().markerFill.y,
                     _item->style().markerFill.z};
}

GpsPointWidget::~GpsPointWidget() = default;

void GpsPointWidget::paint() {
  ImGui::BeginDisabled();
  ImGui::InputText("Name", &_ui->text);
  ImGui::InputFloat2("Lat/Lon [deg]", _ui->latLon.data(), _ui->latLonFormat);
  ImGui::EndDisabled();
  ImGui::Separator();
  ImGui::TextUnformatted("Style");
  ImGui::Checkbox("Text Enabled", &_ui->textEnabled);
  ImGui::SameLine();
  ImGui::Checkbox("Marker Enabled", &_ui->markerEnabled);
  paint_markerCombo();
  ImGui::InputFloat("Marker Size", &_ui->markerSize, 1.0f);
  // ImGui::InputFloat("Marker Weight", &_markerWeight, 1.0f);
  ImGui::ColorEdit3("Marker Fill", _ui->markerFill.data());
}

MarkItem::Style GpsPointWidget::apply() {
  _item->style().textEnabled = _ui->textEnabled;
  _item->style().markerEnabled = _ui->markerEnabled;

  _item->style().markerType = _ui->markerType;
  _item->style().markerSize = _ui->markerSize;
  _item->style().markerWeight = _ui->markerWeight;
  _item->style().markerFill = {_ui->markerFill[0], _ui->markerFill[1],
                               _ui->markerFill[2], 1.0};
  _item->setText(_ui->text);
  _item->setCoords({_ui->latLon[0], _ui->latLon[1]});

  return _item->style();
}

void GpsPointWidget::paint_markerCombo() {
  if (ImGui::BeginCombo("Marker", _ui->markerTypeName.c_str())) {
    if (ImGui::Selectable(MarkerTypeName(ImPlotMarker_Circle),
                          _ui->markerType == ImPlotMarker_Circle)) {
      _ui->markerType = ImPlotMarker_Circle;
      _ui->markerTypeName = MarkerTypeName(ImPlotMarker_Circle);
    } else if (ImGui::Selectable(MarkerTypeName(ImPlotMarker_Square),
                                 _ui->markerType == ImPlotMarker_Square)) {
      _ui->markerType = ImPlotMarker_Square;
      _ui->markerTypeName = MarkerTypeName(ImPlotMarker_Square);
    } else if (ImGui::Selectable(MarkerTypeName(ImPlotMarker_Diamond),
                                 _ui->markerType == ImPlotMarker_Diamond)) {
      _ui->markerType = ImPlotMarker_Diamond;
      _ui->markerTypeName = MarkerTypeName(ImPlotMarker_Diamond);
    } else if (ImGui::Selectable(MarkerTypeName(ImPlotMarker_Up),
                                 _ui->markerType == ImPlotMarker_Up)) {
      _ui->markerType = ImPlotMarker_Up;
      _ui->markerTypeName = MarkerTypeName(ImPlotMarker_Up);
    }
    ImGui::EndCombo();
  }
}

inline static constexpr const char *MarkerTypeName(ImPlotMarker marker) {
  if (marker == ImPlotMarker_Circle) {
    return "Circle";
  } else if (marker == ImPlotMarker_Square) {
    return "Square";
  } else if (marker == ImPlotMarker_Diamond) {
    return "Diamond";
  } else if (marker == ImPlotMarker_Up) {
    return "Triangle UP";
  }
  return "Unknow";
}
} // namespace Rich
} // namespace ImOsm
