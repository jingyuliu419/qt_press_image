#ifndef WORKER_H
#define WORKER_H

#include <QObject>
#include <QString>
#include <opencv2/opencv.hpp>
#include <onnxruntime/core/providers/cpu/cpu_provider_factory.h>
#include <onnxruntime/core/session/onnxruntime_cxx_api.h>

class Worker : public QObject {
    Q_OBJECT
public:
    explicit Worker(QObject* parent = nullptr);

public slots:
    void runOnnxInfer(const cv::Mat& inputImage); // Perform ONNX inference
    void updateImage(const QString& base64Image); // Update the image

signals:
    void inferenceCompleted(const QString& result); // Signal when inference is done
    void imageUpdateCompleted(const QString& base64Image); // Signal when image is updated

private:
    QString onnxInfer1(const cv::Mat& img); // Helper function for ONNX inference
};

#endif // WORKER_H
