/*
  ==============================================================================

    CubeCalendarData.cpp
    Created: 21 Mar 2021 12:41:29pm
    Author:  corbin

  ==============================================================================
*/

#include "CubeCalendarData.h"

    // Wrappers around the juce streams...
    struct OutputArchiver final {
        OutputArchiver(juce::OutputStream& stream) : stream_{stream} {}

        //void operator()(bool x) { stream_.writeBool(x); }
        void operator()(int x) { stream_.writeInt(x); }
        //void operator()(float x) { stream_.writeFloat(x); }
        //void operator()(const juce::String& x) { stream_.writeString(x); }
        void operator()(CubeData::Type x) { stream_.writeInt(static_cast<int>(x)); }
        void operator()(const juce::Quaternion<float>& x) {
            stream_.writeFloat(x.vector.x);
            stream_.writeFloat(x.vector.y);
            stream_.writeFloat(x.vector.z);
            stream_.writeFloat(x.scalar);
        }

        juce::OutputStream& stream_;
    };

    struct InputArchiver final {
        InputArchiver(juce::InputStream& stream) : stream_{stream} {}

        //void operator()(bool& x) { x = stream_.readBool(); }
        void operator()(int& x) { x = stream_.readInt(); }
        //void operator()(float& x) { x = stream_.readFloat(); }
        //void operator()(juce::String& x) { x = stream_.readString(); }
        void operator()(CubeData::Type& x) { x = static_cast<CubeData::Type>(stream_.readInt()); }
        void operator()(juce::Quaternion<float>& x) {
            x.vector.x = stream_.readFloat();
            x.vector.y = stream_.readFloat();
            x.vector.z = stream_.readFloat();
            x.scalar = stream_.readFloat();
        }

        juce::InputStream& stream_;
    };



const juce::MemoryBlock CubeCalendarData::getData() {
    outStream.reset();
    OutputArchiver out{outStream};
    serialize(out);
    return outStream.getMemoryBlock();
}
void CubeCalendarData::setData(const juce::MemoryBlock& data) {
    juce::MemoryInputStream inStream(data, false);
    InputArchiver in{inStream};
    serialize(in);
}

CubeCalendarData::CubeCalendarData(const juce::MemoryBlock& data) : outStream() {
    setData(data);
}

CubeCalendarData::CubeCalendarData() :  outStream() {}
CubeCalendarData::~CubeCalendarData()
{
    onCommit = nullptr;
}

/*
const std::array<int, 3> CubeCalendarData::getDate() {
    std::array<int, 3> date {-1, -1, -1};
    for (const CubeData& cube : cubes) {
        if (cube.state >= 0 && cube.state <= 2)
            date[cube.state] = static_cast<int>(cube.type);
    }
    return date;
}
*/

const void CubeCalendarData::commit(bool hardCommit) {
    if (onCommit != nullptr) {
        onCommit(getData(), hardCommit);
    }
}

void CubeCalendarData::registerCommitCallback(std::function<void(const juce::MemoryBlock&, bool)> callback) {
    onCommit = callback;
}