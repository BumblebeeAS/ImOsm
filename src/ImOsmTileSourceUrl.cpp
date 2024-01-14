#include "ImOsmTileSourceUrl.h"
#include <curl/curl.h>
#include <iostream>
#include <pwd.h>
#include <fstream>

namespace ImOsm {

size_t onWrite(void *data, size_t size, size_t nmemb, void *userp);
size_t onProgress(void *clientp, double dltotal, double dlnow, double ultotal,
                  double ulnow);
curl_socket_t onOpenSocket(void *clientp, curlsocktype purpose,
                           struct curl_sockaddr *address);

TileSourceUrl::TileSourceUrl(int requestLimit, bool preload,
                             const std::string &cachePath,
                             const std::string &userAgent)
    : TileSourceAsync{requestLimit, preload}, _userAgent{userAgent}
    {
      passwd *pw = getpwuid(getuid());
      std::string homedir = pw->pw_dir;
      _cachePath = std::filesystem::path{homedir};
      _cachePath.append(".cache");
      _cachePath.append("osm");
      _cachePath.append(cachePath);
    }

TileSourceUrl::~TileSourceUrl() {}

std::filesystem::path TileSourceUrl::CachePathDefault() {
  return std::filesystem::current_path().append("tiles");
}

std::filesystem::path TileSourceUrl::cachePath(std::string url) const {
  std::filesystem::path path{cachePath()};
  std::string cleaned_url = url;
  if (cleaned_url.find("://") != std::string::npos) {
    cleaned_url = cleaned_url.substr(cleaned_url.find("://") + 3);
  }
  path.append(cleaned_url);
  return path;
}

bool TileSourceUrl::receiveTile(int z, int x, int y, TileData &tileData) {
  auto url = makeUrl(z, x, y);
  // check fs cache
  std::filesystem::path path{cachePath(url)};
  std::ifstream reader(path,
                       std::fstream::in | std::fstream::binary | std::ios::ate);
  auto pos{reader.tellg()};
  if (reader && pos > 0) {
    // std::cout << "loading from cache" << std::endl;
    tileData.blob.resize(pos);
    reader.seekg(0, std::ios::beg);
    reader.read(reinterpret_cast<char *>(&tileData.blob[0]), pos);
    return true;
  }

  CURL *curl{curl_easy_init()};
  std::cout << "Fetching url: " << url << std::endl;
  curl_easy_setopt(curl, CURLOPT_URL, makeUrl(z, x, y).c_str());
  curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
  // curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
  curl_easy_setopt(curl, CURLOPT_USERAGENT, _userAgent.c_str());
  curl_easy_setopt(curl, CURLOPT_TIMEOUT, 1);
  curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 1);
  // Write
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, onWrite);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&tileData);
  // Progress
  curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, onProgress);
  curl_easy_setopt(curl, CURLOPT_PROGRESSDATA, &tileData);
  // Open Socket
  curl_easy_setopt(curl, CURLOPT_OPENSOCKETFUNCTION, onOpenSocket);
  curl_easy_setopt(curl, CURLOPT_OPENSOCKETDATA, &tileData);
  const bool ok{curl_easy_perform(curl) == CURLE_OK};
  curl_easy_cleanup(curl);


  // write image to cachePath
  std::cout << "saving tile to path: " << path << std::endl;
  std::filesystem::create_directories(path.parent_path());
  std::ofstream writer(path,
                       std::fstream::out | std::fstream::binary | std::ios::trunc);
  writer.write(reinterpret_cast<const char *>(&tileData.blob[0]),
                tileData.blob.size());
  writer.close();

  return ok;
}

size_t onWrite(void *data, size_t size, size_t nmemb, void *userp) {
  auto &tileData{*static_cast<TileSourceAsync::TileData *>(userp)};
  auto &blob{tileData.blob};
  size_t realsize{size * nmemb};
  auto const *const dataptr{static_cast<std::byte *>(data)};
  blob.insert(blob.cend(), dataptr, dataptr + realsize);
  return realsize;
}

size_t onProgress(void *clientp, double dltotal, double dlnow, double ultotal,
                  double ulnow) {
  auto &tileData{*static_cast<TileSourceAsync::TileData *>(clientp)};
  return 0;
}

curl_socket_t onOpenSocket(void *clientp, curlsocktype purpose,
                           struct curl_sockaddr *address) {
  auto &tileData{*static_cast<TileSourceAsync::TileData *>(clientp)};
  auto s = socket(address->family, address->socktype, address->protocol);
  return s;
}

} // namespace ImOsm
