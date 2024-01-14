#include "ImOsmRichMarkViewerWidget.h"
#include "ImOsmRichMapPlot.h"
#include "ImOsmRichMarkItem.h"
#include "ImOsmRichGpsPointWidget.h"
#include <misc/cpp/imgui_stdlib.h>
#include <random>

namespace ImOsm {
namespace Rich {

struct MarkViewerWidget::Ui {
  inline static const char latlonFormat[]{"%.6f"};
  inline static const char radiusFormat_m[]{"%.1f [m]"};
  inline static const char radiusFormat_km[]{"%.1f [km]"};
  std::array<float, 2> latLonInput{0.f, 0.f};
  std::string markNameInputText{};
  bool isMousePick{false};
  bool isMarkAdd{false};
};

MarkViewerWidget::MarkViewerWidget(std::shared_ptr<RichMapPlot> plot,
                                   std::shared_ptr<MarkStorage> storage)
    : _plot{plot}, _storage{storage}, _ui{std::make_unique<Ui>()},
      historySize{10}, coords {} {
  _ui->markNameInputText.reserve(32);
}

MarkViewerWidget::~MarkViewerWidget() = default;

void MarkViewerWidget::paint() {
  ImGui::TextUnformatted("Gps Coords Viewer");
  // paint_latLonInput();
  // ImGui::SameLine();
  // paint_mousePickBtn();
  // paint_markNameInput();
  // ImGui::SameLine();
  // paint_addMarkBtn();
  paint_markTable();
}

// void MarkViewerWidget::paint_latLonInput() {
//   ImGui::PushItemWidth(200 + ImGui::GetStyle().ItemSpacing.x / 2);
//   ImGui::InputFloat2("Lat/Lon", _ui->latLonInput.data(), _ui->latlonFormat);
//   ImGui::PopItemWidth();
// }

void MarkViewerWidget::paint_mousePickBtn() {
  if (!_ui->isMousePick) {
    if (ImGui::Button("Mouse Pick")) {
      _ui->isMousePick = !_ui->isMousePick;
    }
  } else {
    const auto color{ImGui::GetStyleColorVec4(ImGuiCol_ButtonHovered)};
    ImGui::PushStyleColor(ImGuiCol_Button, color);
    if (ImGui::Button("Mouse Pick")) {
      _ui->isMousePick = !_ui->isMousePick;
    }
    ImGui::PopStyleColor();
  }
}

void MarkViewerWidget::paint_markNameInput() {
  ImGui::PushItemWidth(100);
  ImGui::InputText("Mark Name", &_ui->markNameInputText);
  ImGui::PopItemWidth();
}

void MarkViewerWidget::paint_addMarkBtn() {
  if (ImGui::Button("Add Mark")) {
    _ui->isMarkAdd = true;
  }
}

void MarkViewerWidget::insert_latLon(float lat, float lon, uint64_t stamp, const std::string name) {
  if (coords.find(name) == coords.end()) {
    std::cout << "creating new coords class" << std::endl;
    auto new_coords = std::make_shared<Coords>();
    new_coords->visible = true;
    new_coords->name = name;
    std::random_device rd;  // Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()
    std::uniform_real_distribution<> dis(0.5, 0.9);
    new_coords->style.markerFill.x = dis(gen);
    new_coords->style.markerFill.y = dis(gen);
    new_coords->style.markerFill.z = dis(gen);
    coords[name] = new_coords;
  }

  historySize = std::max(1, historySize);
  while (coords[name]->coords.size() >= historySize) {
    _storage->rmMark(coords[name]->coords.front().name);
    coords[name]->coords.erase(coords[name]->coords.begin());
  }
  if (!coords[name]->visible) {
    std::cout << "coords not updating" << std::endl;
    return;
  }

  std::string coord_name = name + "_" + std::to_string(stamp);
  Coord new_coord {lat, lon, stamp, coord_name};
  coords[name]->coords.push_back(new_coord);

  GeoCoords geoCoords{lat, lon};
  _storage->addMark(geoCoords, coord_name, name);
  auto node{_storage->findMark(coord_name)};
  node->setStyle(coords[name]->style);
  _plot->addItem(node);
}

void MarkViewerWidget::paint_markTable() {
  static const int tableCols{6};
  static const ImGuiTableColumnFlags colFlags{ImGuiTableColumnFlags_WidthFixed};

  if (ImGui::BeginTable("GpsTable", tableCols)) {
    ImGui::TableSetupColumn("Name", colFlags, 150);
    ImGui::TableSetupColumn("Lat", colFlags, 60);
    ImGui::TableSetupColumn("Lon", colFlags, 60);
    ImGui::TableSetupColumn("Setup", colFlags, 50);
    ImGui::TableSetupColumn("Update", colFlags, 50);
    ImGui::TableHeadersRow();

    _storage->rmMarks();

    
    // const auto &markItems{_storage->markItems()};
    
    for (auto [key, value] : coords) {
      ImGui::TableNextRow();
      ImGui::PushID(key.c_str());
      paint_markTableRow(value);
      ImGui::PopID();
    }

    // std::for_each(markItems.begin(), markItems.end(), [this](auto &item) {
    //   ImGui::TableNextRow();
    //   ImGui::PushID(item.ptr.get());
    //   paint_markTableRow(item);
    //   ImGui::PopID();
    // });

    ImGui::EndTable();
  }
}

void MarkViewerWidget::paint_markTableRow(std::shared_ptr<Coords> item) {
  // Name
  ImGui::TableNextColumn();
  ImGui::TextUnformatted(item->name.c_str());

  if (item->coords.size() == 0) {
    ImGui::TableNextColumn();
    ImGui::TextUnformatted("-");
    ImGui::TableNextColumn();
    ImGui::TextUnformatted("-");
    ImGui::TableNextColumn();
    ImGui::TextUnformatted("-");
  } else {
    Coord last_coord = item->coords.back();
    // Lat
    ImGui::TableNextColumn();
    ImGui::Text(_ui->latlonFormat, last_coord.lat);

    // Lon
    ImGui::TableNextColumn();
    ImGui::Text(_ui->latlonFormat, last_coord.lon);

    // Setup
    ImGui::TableNextColumn();

    auto markItem{_storage->findMark(last_coord.name)};
    if (ImGui::Button("Setup")) {
      ImGui::OpenPopup("Setup Item");
      _itemWidget =
          std::make_unique<GpsPointWidget>(markItem);
    }

    if (ImGui::BeginPopupModal("Setup Item")) {
      // Draw popup contents
      _itemWidget->paint();
      ImGui::Separator();
      if (ImGui::Button("Cancel")) {
        ImGui::CloseCurrentPopup();
        _itemWidget.reset();
      }
      ImGui::SameLine();
      if (ImGui::Button("Apply")) {
        auto new_style = _itemWidget->apply();
        item->style = new_style;
        for (auto &coord : item->coords) {
          auto markItem{_storage->findMark(coord.name)};
          markItem->setStyle(new_style);
        }
        ImGui::CloseCurrentPopup();
        _itemWidget.reset();
      }
      ImGui::EndPopup();
    }
  }

  // Delete
  ImGui::TableNextColumn();
  static bool visible(item->visible);
  ImGui::Checkbox("##Show", &visible);
  if (visible != item->visible) {
    item->visible = visible;
  }
}
} // namespace Rich
} // namespace ImOsm
