#include "movie.h"
#include <Helper/str.h>
#include <chrono>
#include <cstdio>
#include <cstdarg>
#include <type_traits>
#include <cassert>
#include "videowriter.h"
#include "stream.h"

Movie::Movie(
    uint32_t width, uint32_t height,
    std::filesystem::path&& root_path, const std::vector<std::shared_ptr<Stream>>& streams,
    size_t framepool_size, ErrorCallback error_callback, const EncoderConfig& default_videoconfig
)   : m_root_path(std::move(root_path)),
    m_temp_audio_name(CreateTempAudioName(".wav")),
    m_error_callback(error_callback)
{
    // Create the movie directory structure
    std::error_code err;
    std::filesystem::create_directory(m_root_path, err);
    if (err)
    {
        LogError(Helper::sprintf(
            "Failed to create folder (%s): '%s'\n", err.message().c_str(), m_root_path.string().c_str()
        ));
        return;
    }

    for (auto stream : streams)
    {
        const EncoderConfig& config = default_videoconfig;
        std::filesystem::path stream_path = m_root_path / stream->GetName();

        // For image sequences, create an additional folder to contain it
        if (config.type != EncoderConfig::TYPE_FFMPEG)
        {
            std::error_code err;
            std::filesystem::create_directory(stream_path, err);
            if (err)
            {
                LogError(Helper::sprintf(
                    "Failed to create folder (%s): '%s'\n", err.message().c_str(), stream_path.string().c_str()
                ));
                return;
            }
            stream_path /= "frame_";
        }

        std::shared_ptr<VideoWriter> writer;
        if (config.type == EncoderConfig::TYPE_QOI)
            writer = std::make_shared<ImageWriter>(width, height, ImageWriter::Format::QOI, std::move(stream_path));
        else if (config.type == EncoderConfig::TYPE_PNG)
        {
            auto png_writer = std::make_shared<ImageWriter>(width, height, ImageWriter::Format::PNG, std::move(stream_path));
            png_writer->SetPngCompression(config.png_compression);
            writer = std::move(png_writer);
        }
        else
        {
            LogError(Helper::sprintf("Invalid or unsupported EncoderConfig type: %s\n", config.type ? config.type->name : "(null)"));
            return;
        }

        m_streams.emplace_back(StreamPair{stream, writer});
    }

    auto num_threads = std::thread::hardware_concurrency();
    if (num_threads <= 0)
        num_threads = 1;
    m_framepool.emplace(num_threads, framepool_size, width, height);
}

FramePool& Movie::GetFramePool()
{
    if (!m_framepool)
        assert(0 && "Do not call GetFramePool when `Failed() == true` immediately after construction");
    return *m_framepool;
}

void Movie::LogError(const std::string& error)
{
    if (m_error_callback)
        m_error_callback(error);
    m_has_errors = true;
}

std::string Movie::CreateTempAudioName(const char* suffix)
{
    auto time = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now().time_since_epoch()
    );
    return std::string("audio-") + std::to_string(time.count()) + suffix;
}