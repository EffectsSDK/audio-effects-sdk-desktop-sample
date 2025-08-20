#include "audio_buffer_qiodevice.h"

namespace core {
AudioBufferQIODevice::AudioBufferQIODevice()
    : _bytesAvailable(0)
    , _currentBufferPos(0)
{ }

AudioBufferQIODevice::~AudioBufferQIODevice() = default;

void AudioBufferQIODevice::push(const QAudioBuffer& buffer)
{
    if (buffer.frameCount() < 1) {
        return;
    }
    {
        std::lock_guard lock(_mutex);
        _audioBufferQueue << buffer;
        _bytesAvailable.fetch_add(buffer.byteCount());
    }
    emit readyRead();
}

bool AudioBufferQIODevice::isSequential() const
{
    return true;
}

qint64 AudioBufferQIODevice::bytesAvailable() const
{
    return _bytesAvailable;
}

qint64 AudioBufferQIODevice::AudioBufferQIODevice::readData(char* data, qint64 maxLen)
{
    qint64 leftSpace = maxLen;
    while (leftSpace > 0) {
        if (isCurrentBufferEmpty()) {
            std::lock_guard lock(_mutex);
            if (_audioBufferQueue.isEmpty()) break;
           
            _currentBuffer = _audioBufferQueue.takeFirst();
            _currentBufferPos = 0;
        }

        qint64 read = readCurrentBuffer(data, leftSpace);
        if (0 == read) break;

        leftSpace -= read;
        data += read;
        _bytesAvailable.fetch_sub(read);
    }

    return (maxLen - leftSpace);
}

qint64 AudioBufferQIODevice::writeData(const char*, qint64)
{
    return 0;
}

bool AudioBufferQIODevice::isCurrentBufferEmpty() const
{
    return (_currentBuffer.frameCount() == _currentBufferPos);
}

qint64 AudioBufferQIODevice::readCurrentBuffer(char* data, qint64 maxLen)
{
    qint64 availableFrames = qint64(_currentBuffer.frameCount() - _currentBufferPos);
    if (0 == availableFrames) {
        return 0;
    }

    auto frameSize = _currentBuffer.format().bytesPerFrame();
    qint64 maxFramesToRead = maxLen / frameSize;
    qint64 toRead = std::min(availableFrames, maxFramesToRead);
    auto src = _currentBuffer.constData<uint8_t>() + (_currentBufferPos * frameSize);
    qint64 byteNum = toRead * frameSize;
    memcpy(data, src, byteNum);
    _currentBufferPos += toRead;
    return byteNum;
}

}