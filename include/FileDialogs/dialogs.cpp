#include <cstdlib>

#include "Process/process.hpp"
#include "dialogs.hpp"

using std::string;

string res_str;
double res_num = 0;

static string string_replace_all(string str, string substr, string nstr) {
  size_t pos = 0;
  while ((pos = str.find(substr, pos)) != string::npos) {
    str.replace(pos, substr.length(), nstr);
    pos += nstr.length();
  }
  return str;
}

namespace ngs::imgui {

  std::string result_string() {
    return res_str;
  }
  
  double result_number() {
    return res_num;
  }

  void get_open_filename(string key, string title, string filter, string fname, string dir, bool multi) {
    string cmd;
    if (multi) {
      cmd = "Data/filedialogs --get-open-filenames-ext \"" + string_replace_all(filter, "\"", "\\\"") + "\"";
    } else {
      cmd = "Data/filedialogs --get-open-filename-ext \"" + string_replace_all(filter, "\"", "\\\"") + "\"";
    }
    cmd += " \"" + string_replace_all(fname, "\"", "\\\"") + "\"";
    cmd += " \"" + string_replace_all(dir, "\"", "\\\"") + "\"";
    cmd += " \"" + string_replace_all(title, "\"", "\\\"") + "\"";
    ngs::ps::NGS_PROCID proc_id = ngs::ps::spawn_child_proc_id(cmd, true);
    res_str = ngs::ps::read_from_stdout_for_child_proc_id(proc_id);
    res_str = string_replace_all(res_str, "\b", "");
    while (!res_str.empty() && (res_str.back() == ' ' || res_str.back() == '\r' || res_str.back() == '\n' ||
    res_str.back() == '\t' || res_str.back() == '\v' || res_str.back() == '\f')) res_str.pop_back();
    ngs::ps::free_stdout_for_child_proc_id(proc_id);
    ngs::ps::free_stdin_for_child_proc_id(proc_id);
  }

  void get_save_filename(string key, string title, string filter, string fname, string dir) {
    string cmd = "Data/filedialogs --get-save-filename-ext \"" + string_replace_all(filter, "\"", "\\\"") + "\"";
    cmd += " \"" + string_replace_all(fname, "\"", "\\\"") + "\"";
    cmd += " \"" + string_replace_all(dir, "\"", "\\\"") + "\"";
    cmd += " \"" + string_replace_all(title, "\"", "\\\"") + "\"";
    ngs::ps::NGS_PROCID proc_id = ngs::ps::spawn_child_proc_id(cmd, true);
    res_str = ngs::ps::read_from_stdout_for_child_proc_id(proc_id);
    res_str = string_replace_all(res_str, "\b", "");
    while (!res_str.empty() && (res_str.back() == ' ' || res_str.back() == '\r' || res_str.back() == '\n' ||
    res_str.back() == '\t' || res_str.back() == '\v' || res_str.back() == '\f')) res_str.pop_back();
    ngs::ps::free_stdout_for_child_proc_id(proc_id);
    ngs::ps::free_stdin_for_child_proc_id(proc_id);
  }

  void get_directory(string key, string title, string dname) {
    string cmd = "Data/filedialogs --get-directory-alt \"" + string_replace_all(title, "\"", "\\\"") + "\"";
    cmd += " \"" + string_replace_all(dname, "\"", "\\\"") + "\"";
    ngs::ps::NGS_PROCID proc_id = ngs::ps::spawn_child_proc_id(cmd, true);
    res_str = ngs::ps::read_from_stdout_for_child_proc_id(proc_id);
    res_str = string_replace_all(res_str, "\b", "");
    while (!res_str.empty() && (res_str.back() == ' ' || res_str.back() == '\r' || res_str.back() == '\n' ||
    res_str.back() == '\t' || res_str.back() == '\v' || res_str.back() == '\f')) res_str.pop_back();
    ngs::ps::free_stdout_for_child_proc_id(proc_id);
    ngs::ps::free_stdin_for_child_proc_id(proc_id);
  }
  
  void show_message(string message) {
    string cmd = "Data/filedialogs --show-message \"" + string_replace_all(message, "\"", "\\\"") + "\"";
    ngs::ps::NGS_PROCID proc_id = ngs::ps::spawn_child_proc_id(cmd, true);
    res_str = ngs::ps::read_from_stdout_for_child_proc_id(proc_id);
    res_str = string_replace_all(res_str, "\b", "");
    while (!res_str.empty() && (res_str.back() == ' ' || res_str.back() == '\r' || res_str.back() == '\n' ||
    res_str.back() == '\t' || res_str.back() == '\v' || res_str.back() == '\f')) res_str.pop_back();
    ngs::ps::free_stdout_for_child_proc_id(proc_id);
    ngs::ps::free_stdin_for_child_proc_id(proc_id);
  }

  void show_question(string message) {
    string cmd = "Data/filedialogs --show-question \"" + string_replace_all(message, "\"", "\\\"") + "\"";
    ngs::ps::NGS_PROCID proc_id = ngs::ps::spawn_child_proc_id(cmd, true);
    res_str = ngs::ps::read_from_stdout_for_child_proc_id(proc_id);
    res_str = string_replace_all(res_str, "\b", "");
    while (!res_str.empty() && (res_str.back() == ' ' || res_str.back() == '\r' || res_str.back() == '\n' ||
    res_str.back() == '\t' || res_str.back() == '\v' || res_str.back() == '\f')) res_str.pop_back();
    ngs::ps::free_stdout_for_child_proc_id(proc_id);
    ngs::ps::free_stdin_for_child_proc_id(proc_id);
  }

  void show_question_ext(string message) {
    string cmd = "Data/filedialogs --show-question-ext \"" + string_replace_all(message, "\"", "\\\"") + "\"";
    ngs::ps::NGS_PROCID proc_id = ngs::ps::spawn_child_proc_id(cmd, true);
    res_str = ngs::ps::read_from_stdout_for_child_proc_id(proc_id);
    res_str = string_replace_all(res_str, "\b", "");
    while (!res_str.empty() && (res_str.back() == ' ' || res_str.back() == '\r' || res_str.back() == '\n' ||
    res_str.back() == '\t' || res_str.back() == '\v' || res_str.back() == '\f')) res_str.pop_back();
    ngs::ps::free_stdout_for_child_proc_id(proc_id);
    ngs::ps::free_stdin_for_child_proc_id(proc_id);
  }

  void get_string(string message, string defstr) {
    string cmd = "Data/filedialogs --get-string \"" + string_replace_all(message, "\"", "\\\"") + "\"";
    cmd += " \"" + string_replace_all(defstr, "\"", "\\\"") + "\"";
    ngs::ps::NGS_PROCID proc_id = ngs::ps::spawn_child_proc_id(cmd, true);
    res_str = ngs::ps::read_from_stdout_for_child_proc_id(proc_id);
    res_str = string_replace_all(res_str, "\b", "");
    while (!res_str.empty() && (res_str.back() == ' ' || res_str.back() == '\r' || res_str.back() == '\n' ||
    res_str.back() == '\t' || res_str.back() == '\v' || res_str.back() == '\f')) res_str.pop_back();
    ngs::ps::free_stdout_for_child_proc_id(proc_id);
    ngs::ps::free_stdin_for_child_proc_id(proc_id);
  }

  void get_number(string message, double defnum) {
    string cmd = "Data/filedialogs --get-number \"" + string_replace_all(message, "\"", "\\\"") + "\" ";
    cmd += std::to_string(defnum);
    ngs::ps::NGS_PROCID proc_id = ngs::ps::spawn_child_proc_id(cmd, true);
    res_num = strtod(ngs::ps::read_from_stdout_for_child_proc_id(proc_id).c_str(), nullptr);
    ngs::ps::free_stdout_for_child_proc_id(proc_id);
    ngs::ps::free_stdin_for_child_proc_id(proc_id);
  }

} // namespace ngs::imgui

