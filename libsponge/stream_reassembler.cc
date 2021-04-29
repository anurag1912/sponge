#include "stream_reassembler.hh"
#include <sstream>
#include <iostream>

// Dummy implementation of a stream reassembler.

// For Lab 1, please replace with a real implementation that passes the
// automated checks run by `make check_lab1`.

// You will need to add private members to the class declaration in `stream_reassembler.hh`

using namespace std;

namespace {
    uint64_t getGreatestIndex(const std::unordered_map<uint64_t, unsigned char> & mapOfIndexBytes, const uint64_t &indexOfLastByte) {
        if(indexOfLastByte==0) {
            return 0;
        }

        if(mapOfIndexBytes.find(indexOfLastByte)!=mapOfIndexBytes.end()) {
            return indexOfLastByte;
        }
        auto temp = indexOfLastByte;
        temp--;
        while(temp>0 and mapOfIndexBytes.find(temp)==mapOfIndexBytes.end()) {
            temp--;
        }

        return temp;
    }
}

StreamReassembler::StreamReassembler(const size_t capacity) : _output(capacity), _capacity(capacity), _mapOfUnassembledBytes({}), _nextIndex(0), _totalCapacity(capacity), _lastItemIndex(0), _indexOfLastByte(0) {}

//! \details This function accepts a substring (aka a segment) of bytes,
//! possibly out-of-order, from the logical stream, and assembles any newly
//! contiguous substrings and writes them into the output stream in order.
void StreamReassembler::push_substring(const string &data, const size_t index, const bool eof) {
    bool isLastByteWritten = false;

    if(_output.error()) {
        return;
    }

    if(_output.input_ended()) {
        return;
    }
    
    if(data=="" and eof) {
        _output.end_input();
        return;
    }

    if(eof) {
        _indexOfLastByte = index + data.size() - 1;
    }

    const auto& bytesInStream = _output.buffer_size();
    _capacity = _totalCapacity - (bytesInStream + _mapOfUnassembledBytes.size());

    uint64_t tempIndex = index;
    uint64_t i = 0; 
    while(i<data.size()) {    
        if(_nextIndex<=tempIndex) {
            if(_mapOfUnassembledBytes.find(tempIndex)==_mapOfUnassembledBytes.end()) {
                if(_capacity==0) {
                    auto greatestIndex = getGreatestIndex(_mapOfUnassembledBytes, _lastItemIndex);
                    if(tempIndex<greatestIndex) {
                        _mapOfUnassembledBytes.erase(greatestIndex);
                        _mapOfUnassembledBytes.insert(std::make_pair(tempIndex, data[i]));
                        _lastItemIndex = greatestIndex;
                    }
                } 
                else {
                    if(tempIndex>_lastItemIndex) {
                        _lastItemIndex = tempIndex;
                    }
                    _mapOfUnassembledBytes.insert(std::make_pair(tempIndex, data[i]));
                    _capacity--;
                
                }
            }
        }
        tempIndex++;
        i++;
    }

    while(_mapOfUnassembledBytes.find(_nextIndex)!=_mapOfUnassembledBytes.end()) {
        std::stringstream ss;
        ss << _mapOfUnassembledBytes[_nextIndex];
        _output.write(ss.str());
        if(_nextIndex==_indexOfLastByte and (_indexOfLastByte!=0 or  eof)) {
            isLastByteWritten = true;
        }
        _mapOfUnassembledBytes.erase(_nextIndex);
        _nextIndex++;
    }

    if(isLastByteWritten) {
        _output.end_input();
    }

}

size_t StreamReassembler::unassembled_bytes() const { return _mapOfUnassembledBytes.size(); }

bool StreamReassembler::empty() const { return _mapOfUnassembledBytes.empty(); }
