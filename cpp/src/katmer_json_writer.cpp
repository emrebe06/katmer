#include "katmer_internal.h"
#include "katmer/katmer_json_writer.h"

struct katmer_json_writer {
    katmer_buffer* buffer = nullptr;
    std::vector<int> counts;
    bool after_key = false;
};

namespace {
int write_raw(katmer_json_writer* writer, const char* text) {
    return writer && writer->buffer ? katmer_buffer_write_cstr(writer->buffer, text) : 0;
}

int write_separator(katmer_json_writer* writer) {
    if (!writer || writer->counts.empty() || writer->after_key) return 1;
    int& count = writer->counts.back();
    if (count++ > 0 && !write_raw(writer, ",")) return 0;
    return 1;
}

int write_quoted(katmer_json_writer* writer, const char* value) {
    std::string escaped = katmer_json_escape(value ? value : "");
    return write_raw(writer, "\"") && katmer_buffer_write(writer->buffer, escaped.data(), escaped.size()) && write_raw(writer, "\"");
}
}

katmer_json_writer* katmer_json_writer_create(void) {
    katmer_json_writer* writer = new katmer_json_writer();
    writer->buffer = katmer_buffer_create(256);
    if (!writer->buffer) {
        delete writer;
        return nullptr;
    }
    return writer;
}

void katmer_json_writer_destroy(katmer_json_writer* writer) {
    if (!writer) return;
    katmer_buffer_destroy(writer->buffer);
    delete writer;
}

void katmer_json_writer_reset(katmer_json_writer* writer) {
    if (!writer) return;
    katmer_buffer_clear(writer->buffer);
    writer->counts.clear();
    writer->after_key = false;
}

int katmer_json_begin_object(katmer_json_writer* writer) {
    if (!write_separator(writer)) return 0;
    writer->after_key = false;
    writer->counts.push_back(0);
    return write_raw(writer, "{");
}

int katmer_json_end_object(katmer_json_writer* writer) {
    if (!writer || writer->counts.empty()) return 0;
    writer->counts.pop_back();
    writer->after_key = false;
    return write_raw(writer, "}");
}

int katmer_json_begin_array(katmer_json_writer* writer) {
    if (!write_separator(writer)) return 0;
    writer->after_key = false;
    writer->counts.push_back(0);
    return write_raw(writer, "[");
}

int katmer_json_end_array(katmer_json_writer* writer) {
    if (!writer || writer->counts.empty()) return 0;
    writer->counts.pop_back();
    writer->after_key = false;
    return write_raw(writer, "]");
}

int katmer_json_key(katmer_json_writer* writer, const char* key) {
    if (!writer || writer->counts.empty()) return 0;
    int& count = writer->counts.back();
    if (count++ > 0 && !write_raw(writer, ",")) return 0;
    if (!write_quoted(writer, key)) return 0;
    writer->after_key = true;
    return write_raw(writer, ":");
}

int katmer_json_string(katmer_json_writer* writer, const char* value) {
    if (!write_separator(writer)) return 0;
    writer->after_key = false;
    return write_quoted(writer, value);
}

int katmer_json_number(katmer_json_writer* writer, double value) {
    if (!write_separator(writer)) return 0;
    writer->after_key = false;
    std::ostringstream out;
    out << value;
    return katmer_buffer_write_cstr(writer->buffer, out.str().c_str());
}

int katmer_json_bool(katmer_json_writer* writer, int value) {
    if (!write_separator(writer)) return 0;
    writer->after_key = false;
    return write_raw(writer, value ? "true" : "false");
}

int katmer_json_null(katmer_json_writer* writer) {
    if (!write_separator(writer)) return 0;
    writer->after_key = false;
    return write_raw(writer, "null");
}

const char* katmer_json_writer_cstr(katmer_json_writer* writer) {
    return katmer_copy_cstr(writer && writer->buffer ? katmer_buffer_data(writer->buffer) : "");
}
