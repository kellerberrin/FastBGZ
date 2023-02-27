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
#include "kel_exec_env.h"
#include "kel_line_record.h"

namespace kel = kellerberrin;


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// After moving data out of the object, it is empty() but not EOF().
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::pair<size_t, std::string> kel::IOLineRecord::getLineData() {

  if (empty() or EOFRecord()) {

    // Complain and return the empty string.
    ExecEnv::log().warn("IOLineRecord::getLineData; attempt to move data from an empty()/EOFRecord() object");
    return {0, {}};

  }

  size_t out_line = line_count_;
  line_count_ = 0;
  empty_ = true;
  return {out_line, std::move(line_data_)};

}

