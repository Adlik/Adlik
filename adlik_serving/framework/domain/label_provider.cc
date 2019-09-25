#include "adlik_serving/framework/domain/label_provider.h"

#include <iterator>

#include "tensorflow/core/lib/core/errors.h"
#include "tensorflow/core/lib/io/inputbuffer.h"
#include "tensorflow/core/platform/env.h"

namespace adlik {
namespace serving {
const std::string& LabelProvider::getLabel(const std::string& name, size_t index) const {
  static const std::string not_found;

  auto itr = label_map_.find(name);
  if (itr == label_map_.end()) {
    return not_found;
  }

  if (itr->second.size() <= index) {
    return not_found;
  }

  return itr->second[index];
}

tensorflow::Status LabelProvider::addLabels(const std::string& name, const std::string& filepath) {
  std::unique_ptr<tensorflow::RandomAccessFile> label_file;
  TF_RETURN_IF_ERROR(tensorflow::Env::Default()->NewRandomAccessFile(filepath, &label_file));

  auto p = label_map_.insert(std::make_pair(name, std::vector<std::string>()));
  if (!p.second) {
    return tensorflow::errors::Internal("multiple label files for '", name, "'");
  }

  auto itr = p.first;

  constexpr int kInputBufferSize = 1 * 1024 * 1024;
  tensorflow::io::InputBuffer input_buffer(label_file.get(), kInputBufferSize);
  tensorflow::string line;
  tensorflow::Status status = input_buffer.ReadLine(&line);
  while (status.ok()) {
    itr->second.push_back(line);
    status = input_buffer.ReadLine(&line);
  }

  if (!tensorflow::errors::IsOutOfRange(status)) {
    return status;
  }

  return tensorflow::Status::OK();
}

}  // namespace serving
}  // namespace adlik
