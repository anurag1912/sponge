#include "byte_stream.hh"

using namespace std;

ByteStream::ByteStream(const size_t capacity) : _capacity(static_cast<uint64_t>(capacity)),  _error(false), _eof(false), _endOfInput(false), _bytesRead(0), _bytesWritten(0), _stream({}){ 
}

size_t ByteStream::write(const string &data) {
    if(_error) {
        return 0;
    }

    if(_endOfInput) {
        return 0;
    }

    _eof = false;

    size_t i = 0;

    while(_capacity!=0 and i<data.length()) {
        _stream.push_back(data[i]);
        _capacity--;
        _bytesWritten++;
        i++;

    }

    return i;
}

//! \param[in] len bytes will be copied from the output side of the buffer
string ByteStream::peek_output(const size_t len) const {
    if(_error) {
        return {};
    }
    
    std::string result;
    size_t i =0;
    size_t maximumCharsRead = len<buffer_size() ? len : buffer_size();
    auto iterator = _stream.begin(); 
    while(i<maximumCharsRead) {
        result+=*iterator;
        i++;
        iterator++;

    }

    return result;
}

//! \param[in] len bytes will be removed from the output side of the buffer
void ByteStream::pop_output(const size_t len) { 
    if(_error) {
        return;
    }
    
    size_t i =0;
    size_t maximumCharsRead = len<buffer_size() ? len : buffer_size(); 
    while(i<maximumCharsRead) {
        _stream.pop_front();
        _capacity++;
        _bytesRead++;
        i++;
    }

    if(_stream.empty() and input_ended()) {
        _eof = true;
    }    
}

//! Read (i.e., copy and then pop) the next "len" bytes of the stream
//! \param[in] len bytes will be popped and returned
//! \returns a string
std::string ByteStream::read(const size_t len) {
    std::string result = peek_output(len);
    pop_output(len);
    return result;
}

void ByteStream::end_input() {
    _endOfInput = true;
    if(_stream.empty()) {
        _eof = true;
    }
}

bool ByteStream::input_ended() const { return _endOfInput; }

size_t ByteStream::buffer_size() const { return _stream.size(); }

bool ByteStream::buffer_empty() const { return _stream.empty(); }

bool ByteStream::eof() const { return _eof; }

size_t ByteStream::bytes_written() const { return _bytesWritten; }

size_t ByteStream::bytes_read() const { return _bytesRead; }

size_t ByteStream::remaining_capacity() const { return _capacity; }
