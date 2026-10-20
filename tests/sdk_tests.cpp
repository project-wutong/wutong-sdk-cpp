// SPDX-License-Identifier: AGPL-3.0-only
#include <wutong/wutong.hpp>

#include <cstdio>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>

namespace {
int failures = 0;
void check(bool ok, const char *what) {
    if (!ok) {
        std::fprintf(stderr, "FAIL: %s\n", what);
        ++failures;
    }
}
std::string slurp(const std::filesystem::path &p) {
    std::ifstream f(p, std::ios::binary);
    std::ostringstream s;
    s << f.rdbuf();
    return s.str();
}
} // namespace

int main() {
    check(wutong::canonicalize(R"({"b":1,"a":"x"})") == R"({"a":"x","b":1})", "jcs sorts keys");
    check(wutong::canonicalize(R"({"z":"中文","a":[" ","\n",1,true,null]})") ==
              R"({"a":[" ","\n",1,true,null],"z":"中文"})",
          "jcs_pairs case 0");
    check(wutong::digest("") == "sha256:e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855",
          "sha256 empty");
    check(wutong::digest("Hello, Wutong") ==
              "sha256:ed8a6d893471ec2a4895c1acc601904f65d5416d16f15f1c1fd6e221d0bf875e",
          "sha256 vector case 1");
    for (const char *bad :
         {R"({"a":1,"a":2})", R"({"x":1.2})", R"({"x":NaN})", R"({"x":9007199254740992})"}) {
        try {
            (void)wutong::canonicalize(bad);
            check(false, bad);
        } catch (const wutong::Error &e) {
            check(e.code() == "ERR_SCHEMA", "strict JSON rejection is ERR_SCHEMA");
        }
    }
    int examples = 0;
    for (const auto &entry : std::filesystem::directory_iterator(WUTONG_VECTORS_DIR)) {
        if (entry.path().extension() != ".json" || entry.path().stem() == "sync_frame")
            continue;
        try {
            wutong::validate(slurp(entry.path()));
            ++examples;
        } catch (const wutong::Error &e) {
            std::fprintf(stderr, "%s: %s\n", entry.path().filename().string().c_str(), e.what());
            ++failures;
        }
    }
    check(examples >= 23, "all normative examples validate");
    try {
        wutong::validate(R"({"protocol":"wutong/0.1"})");
        check(false, "incomplete envelope rejected");
    } catch (const wutong::Error &e) {
        check(e.code().rfind("ERR_", 0) == 0, "error carries an ERR_* code");
    }
    std::printf("%s (abi %u): %d examples validated, %d failures\n",
                std::string(wutong::implementation()).c_str(), wutong::abi_version(), examples, failures);
    return failures == 0 ? 0 : 1;
}
