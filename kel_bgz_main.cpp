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


#include "kel_exec_env.h"
#include "kel_exec_env_app.h"
#include "kel_bzip_workflow.h"

#include <iostream>

namespace kel = kellerberrin;

// Exec arguments.
struct ExecEnvVar {

  std::string workDirectory{"./"};
  std::string logFile{"bgz_example.log"};
  int max_error_count{100};
  int max_warn_count{100};
  std::string bgz_file_; // "path/file.bgz"
  bool file_specified{false};  // "-f"
  bool verify_file{false}; // "-v"
  bool stream_file{false}; // "-o"

};

// The runtime environment.
class ExecEnvBGZ {

public:

  ExecEnvBGZ() = delete;
  ~ExecEnvBGZ() = delete;

  [[nodiscard]] inline static const ExecEnvVar &getEnvVar() { return env_vars_; }

  // The following 5 static members are required for all applications.
  inline static constexpr const char *VERSION = "0.1";
  inline static constexpr const char *MODULE_NAME = "BGZ Example";

  static void executeApp(); // Application mainline.
  // Parse command line arguments.
  [[nodiscard]] static bool parseCommandLine();

  // Create application logger.
  [[nodiscard]] static std::unique_ptr<kel::Logger> createLogger() { return kel::ExecEnv::createLogger( MODULE_NAME
                                                                                                      , env_vars_.logFile
                                                                                                      , env_vars_.max_error_count
                                                                                                      , env_vars_.max_warn_count); }

private:

  inline static ExecEnvVar env_vars_;

};

// No valid logger exists before arguments are parsed.
bool ExecEnvBGZ::parseCommandLine() {

  std::vector<std::string> cmd_tokens = kel::ExecEnv::getCommandTokens();

  auto cmd_iterator = cmd_tokens.cbegin();
  while (cmd_iterator !=  cmd_tokens.cend()) {

    if (*cmd_iterator == "-f") {

      ++cmd_iterator;
      if (cmd_iterator ==  cmd_tokens.cend()) {

        std::cerr << "Expected .bgz file spec immediately after '-f' flag; '-f path/file.bgz'\n";
        return false;

      }
      if (not kel::Utility::fileExists(*cmd_iterator)) {

        std::cerr << "Specified .bgz file: " << *cmd_iterator << "not found\n";
        return false;

      }
      env_vars_.bgz_file_ = *cmd_iterator;
      env_vars_.file_specified = true;

    } else if (*cmd_iterator == "-v") {

      env_vars_.verify_file = true;

    } else if (*cmd_iterator == "-o") {

      env_vars_.stream_file = true;

    }

    ++cmd_iterator;

  }

  if (not env_vars_.file_specified) {

    std::cerr << "Usage:\n"
    << "[Required] '-f path_to/file.bgz'  Specify the .bgz file to decompress.\n"
    << "[Optional] '-v'   Verify the file has a valid .bgz format.\n"
    << "[Optional] '-o'   Output the decompressed records to stdout.\n";
    return false;
  }

  return true;

}

void decompressReader(size_t, bool);
void ExecEnvBGZ::executeApp() {

  if (getEnvVar().verify_file) {

    if (kel::BGZStreamIO::verify(getEnvVar().bgz_file_)) {

      kel::ExecEnv::log().info("File: {} verified as a compressed .bgz (RFC1952) file", getEnvVar().bgz_file_);

    } else {

      kel::ExecEnv::log().info("File: {}  is NOT a valid compressed .bgz (RFC1952) file", getEnvVar().bgz_file_);

    }

  } else {

    const size_t thread_count{15};
    decompressReader(thread_count, getEnvVar().stream_file);

  }

}

// The mainline.
int main(int argc, char const ** argv)
{

  return kel::ExecEnv::runApplication<ExecEnvBGZ>(argc, argv);

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void decompressReader(size_t thread_count, bool stream_file) {

  const std::string decompress_name{"BGZ Decompress"};
  std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
  kel::ExecEnv::log().info("{} begins Parsing BGZ file: {}", decompress_name, ExecEnvBGZ::getEnvVar().bgz_file_);

  std::chrono::time_point<std::chrono::system_clock> prior = std::chrono::system_clock::now();
  size_t count{0};
  size_t total_char_size{0};


  kel::BGZStreamIO bgz_stream(thread_count);
  if (not bgz_stream.open(ExecEnvBGZ::getEnvVar().bgz_file_)) {

    kel::ExecEnv::log().error("{} error opening file: {}", decompress_name, ExecEnvBGZ::getEnvVar().bgz_file_);
    return;

  }

  while (true) {

    // Get a decompressed line.
    auto line = bgz_stream.readLine();

    // Check if EOF
    if (line.EOFRecord()) break;

    // Retrieve the line record and line number.
    auto [line_num, line_str] = line.getLineData();
    total_char_size += line_str.size();

    ++count;
    // Progressive elapsed time.
    if (count % 1000000 == 0) {

      std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
      auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(now - prior);
      prior = now;
      kel::ExecEnv::log().info("{}, Lines Read: {}, Line queue size: {}, Elapsed Time (ms): {}"
          , decompress_name, count, bgz_stream.lineQueue().size(), milliseconds.count());

    }

    if (stream_file) std::cout << line_str << '\n';

  }

  // Final elapsed time.
  std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(end - start);
  kel::ExecEnv::log().info("{} ends, Elapsed time (sec): {}, Lines Read: {}, Total char size: {}, Thread count:{}"
      , decompress_name, elapsed.count(), count, total_char_size, thread_count);

}

