#ifndef MINOTAUR_CPP_CONVERTER_H
#define MINOTAUR_CPP_CONVERTER_H

#include <QObject>
#include <QBasicTimer>

#include <opencv2/opencv.hpp>

#include "../video/modify.h"
#include "../video/recorder.h"

class CameraDisplay;

class Converter : public QObject {
    Q_OBJECT

public:
    explicit Converter(QObject *parent = nullptr, CameraDisplay *display = nullptr);

    void setProcessAll(bool process_all);

    bool is_recording() const;

    int get_and_reset_frames();

    Q_SIGNAL void imageReady(const QImage &);

    /**
     * Signal emitted after a cv::UMat frame has been read
     * from the VideoCapture and modified.
     *
     * @param img reference to processed image;
     */
    Q_SIGNAL void frameProcessed(cv::UMat &img);

    Q_SLOT void processFrame(const cv::UMat &frame);

    Q_SLOT void modifierChanged(int modifier_index, ActionBox *box);

    Q_SLOT void imageKeyEvent(int key);

    Q_SLOT void startRecording(QString file, int width, int height);

    Q_SLOT void stopRecording();

private:
    static void matDelete(void *mat);

    void queue(const cv::UMat &frame);

    void process(cv::UMat frame);

    void timerEvent(QTimerEvent *ev) override;

    CameraDisplay *m_display;
    QBasicTimer m_timer;

    cv::UMat m_frame;

    std::unique_ptr<VideoModifier> m_modifier;
    std::unique_ptr<Recorder> m_recorder;

    bool m_process_all = true;
    int m_frame_count = 0;
};


#endif //MINOTAUR_CPP_CONVERTER_H
