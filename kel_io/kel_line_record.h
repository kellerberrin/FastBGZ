// Copyright 2023 Kellerberrin
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
// documentation files (the "Software"), to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
// and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
// WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
// WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
// OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//
//

#ifndef KEL_LINE_RECORD_H
#define KEL_LINE_RECORD_H

#include <string>
#include <memory>
#include <string_view>
#include <vector>
#include <optional>
#include <fstream>


namespace kellerberrin {   //  organization::project level namespace

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// A lightweight object that contains the readLine() output of StreamIO with file line number and associated line text.
// Note that the line text has been stripped of the terminating '\n'.
// Once the line data has been moved from the object using getLineData() it is tagged as empty().
// A subsequent attempt to access the moved data using getLineData() will generate a warning.
// A static function createEOFMarker() creates an object to serve as EOF, this can also be pushed onto a queue.
// Since the stored std::string data can be large, for optimal performance the object cannot be copied.
// The object is designed to be enqueued and dequeued from thread safe queues without incurring significant processing overhead.
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////

class IOLineRecord {

public:

  IOLineRecord(size_t line_count, std::string&& line_data) noexcept : line_count_(line_count), line_data_(std::move(line_data)) {}
  IOLineRecord(IOLineRecord&& line_record) noexcept : line_count_(line_record.line_count_)
                                                    , line_data_(std::move(line_record.line_data_))
                                                    , EOF_(line_record.EOF_)
                                                    , empty_(line_record.empty_) {}
  IOLineRecord(const IOLineRecord& line_record) = delete;
  ~IOLineRecord() = default;

  IOLineRecord& operator=(const IOLineRecord&) = delete;

  // After moving data out of the object, it is empty() but not EOF().
  [[nodiscard]] std::pair<size_t, std::string> getLineData();
  [[nodiscard]] size_t lineCount() const { return line_count_; }
  [[nodiscard]] const std::string_view getView() const { return {line_data_}; }
   [[nodiscard]] bool EOFRecord() const { return EOF_; }
  [[nodiscard]] bool empty() const { return empty_; }


  // ReturnType the EOF marker.
  [[nodiscard]] static IOLineRecord createEOFMarker() { return {}; }

private:

  IOLineRecord() : EOF_{true} {} // Only create as an EOF marker.

  size_t line_count_{0}; // Actual line counts begin at 1.
  std::string line_data_; // This string is NOT '\n' terminated.
  bool EOF_{false};
  bool empty_{false};

};



} // namespace


#endif //KEL_BASIC_IO_H
