#ifndef MINOTAUR_CPP_RECORDER_H
#define MINOTAUR_CPP_RECORDER_H

#include <QObject>
#include <memory>

// OpenCV forward declarations
namespace cv {
    class UMat;
    class VideoWriter;
}

/**
 * This class handles a cv::VideoWriter instance that is used to
 * write preprocessed cv::Mat objects to a video file.
 */
class Recorder : public QObject {
Q_OBJECT

public:
    enum {
        // Hard value for frame rate writing
        DEFAULT_FRAME_RATE = 30
    };

    explicit Recorder(
        int frame_rate = DEFAULT_FRAME_RATE,
        bool color = true
    );

    /**
     * Tell the recorder to start capturing video from
     * its stream, given by Qt signals.
     *
     * @param file   the file name to save to
     * @param width  the width of the video
     * @param height the height of the video
     */
    Q_SLOT void start_recording(const QString &file, int width, int height);

    /**
     * Tell the recorder to stop recording video from the
     * signal stream. The file will be closed.
     */
    Q_SLOT void stop_recording();

    /**
     * Slot called with a processed frame that is written to the video
     * file, if the recorder is active.
     *
     * @param img frame image
     */
    Q_SLOT void frame_received(const cv::UMat &img);

    bool is_recording() const;

private:
    /**
     * Handle video writer.
     */
    std::unique_ptr<cv::VideoWriter> m_video_writer;

    int m_frame_rate;
    bool m_color;
    bool m_recording;
};

#endif //MINOTAUR_CPP_RECORDER_H
