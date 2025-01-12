#include "worker.h"
#include <QDebug>
#include <QMutex>
#include <QMutexLocker>
#include <onnxruntime/core/providers/cpu/cpu_provider_factory.h>
#include <onnxruntime/core/session/onnxruntime_cxx_api.h>

Worker::Worker(QObject* parent) : QObject(parent) {}

// Run ONNX inference (called in a separate thread)
void Worker::runOnnxInfer(const cv::Mat& inputImage) {
    qDebug() << "runOnnxInfer 被调用";
    try {
        // Perform the inference
        QString result = onnxInfer1(inputImage);

        // Emit signal with result
        emit inferenceCompleted(result);

    } catch (const std::exception& e) {
        qWarning() << "Error during ONNX inference:" << e.what();
        emit inferenceCompleted("Error");
    }
}

// Update image (called in a separate thread)
void Worker::updateImage(const QString& base64Image) {
    emit imageUpdateCompleted(base64Image);
}

// Perform ONNX inference
QString Worker::onnxInfer1(const cv::Mat& img) {
    static Ort::Env env(ORT_LOGGING_LEVEL_WARNING, "ONNXRuntimeModel");
    static Ort::SessionOptions sessionOptions;

    // Configure session options
    sessionOptions.SetIntraOpNumThreads(1);
    sessionOptions.SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_ENABLE_EXTENDED);

    // Mutex for thread-safe access to static variables
    static QMutex mutex;

    // Load the ONNX model (ensure thread safety)
    static std::unique_ptr<Ort::Session> session;
    {
        QMutexLocker locker(&mutex);
        if (!session) {
            const std::string modelPath = "/home/ljy/Doc1/mycode/CNN_train/pressCNN_net.onnx";
            session = std::make_unique<Ort::Session>(env, modelPath.c_str(), sessionOptions);
        }
    }

    // Check if the input image is valid
    if (img.empty()) {
        qWarning() << "Input image is empty!";
        return "Invalid Image";
    }

    // Preprocess the image
    cv::Mat resizedImg, floatImg;
    cv::resize(img, resizedImg, cv::Size(64, 64)); // Resize to 64x64
    cv::cvtColor(resizedImg, resizedImg, cv::COLOR_BGR2RGB); // Convert BGR to RGB
    resizedImg.convertTo(floatImg, CV_32F, 1.0 / 255.0); // Normalize to [0, 1]
    floatImg = (floatImg - 0.5) / 0.5; // Normalize to [-1, 1]

    // Prepare input tensor data (channels-first)
    std::vector<float> inputTensorValues;
    inputTensorValues.reserve(1 * 3 * 64 * 64);
    for (int c = 0; c < 3; ++c) {
        for (int i = 0; i < 64; ++i) {
            for (int j = 0; j < 64; ++j) {
                inputTensorValues.push_back(floatImg.at<cv::Vec3f>(i, j)[c]);
            }
        }
    }

    // Prepare ONNX Runtime tensor
    std::vector<int64_t> inputDims = {1, 3, 64, 64};
    Ort::MemoryInfo memoryInfo = Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);
    Ort::Value inputTensor = Ort::Value::CreateTensor<float>(
        memoryInfo, inputTensorValues.data(), inputTensorValues.size(), inputDims.data(), inputDims.size());

    // Perform inference
    try {
        Ort::AllocatorWithDefaultOptions allocator;
        const char* inputName = session->GetInputName(0, allocator);
        const char* outputName = session->GetOutputName(0, allocator);
        auto outputTensors = session->Run(Ort::RunOptions{nullptr}, &inputName, &inputTensor, 1, &outputName, 1);

        // Get output data
        float* outputData = outputTensors.front().GetTensorMutableData<float>();

        // Apply softmax to output
        std::vector<float> softmaxOutput(5);
        float sum = 0.0f;
        for (int i = 0; i < 5; ++i) {
            softmaxOutput[i] = std::exp(outputData[i]);
            sum += softmaxOutput[i];
        }
        for (int i = 0; i < 5; ++i) {
            softmaxOutput[i] /= sum;
        }

        // Determine the predicted class
        int predictedClass = std::distance(
            softmaxOutput.begin(),
            std::max_element(softmaxOutput.begin(), softmaxOutput.end())
        );

        // Return predicted class
        const std::vector<std::string> classes = {"class1", "class2", "class3", "class4", "class5"};
        return QString::fromStdString(classes[predictedClass]);

    } catch (const Ort::Exception& e) {
        qWarning() << "ONNX inference failed:" << e.what();
        return "Error";
    }
}
