#include <iostream>
#include <fstream>
#include <cmath>
#include <cstdint>

using namespace std;

const int SAMPLE_RATE = 44100; // 采样率，标准音频采样率为 44.1 kHz

// 写入 WAV 文件的头部信息
void writeWavHeader(ofstream &file, int dataSize)
{
    int fileSize = 44 + dataSize; // 44 字节的头部 + 数据
    file.write("RIFF", 4);        // Chunk ID
    file.write(reinterpret_cast<const char *>(&fileSize), 4); // Chunk Size
    file.write("WAVE", 4);        // Format
    file.write("fmt ", 4);        // Subchunk1 ID
    int subchunk1Size = 16;       // Subchunk1 Size (PCM 固定为 16)
    file.write(reinterpret_cast<const char *>(&subchunk1Size), 4);
    int audioFormat = 1;          // PCM 音频格式
    file.write(reinterpret_cast<const char *>(&audioFormat), 2);
    int numChannels = 1;          // 单声道
    file.write(reinterpret_cast<const char *>(&numChannels), 2);
    file.write(reinterpret_cast<const char *>(&SAMPLE_RATE), 4); // 采样率
    int byteRate = SAMPLE_RATE * numChannels * 2; // ByteRate = 采样率 × 通道数 × 每个样本字节数
    file.write(reinterpret_cast<const char *>(&byteRate), 4);
    int blockAlign = numChannels * 2; // 每帧字节数 = 通道数 × 每个样本字节数
    file.write(reinterpret_cast<const char *>(&blockAlign), 2);
    int bitsPerSample = 16;      // 每个样本 16 位
    file.write(reinterpret_cast<const char *>(&bitsPerSample), 2);
    file.write("data", 4);       // Subchunk2 ID
    file.write(reinterpret_cast<const char *>(&dataSize), 4); // Subchunk2 Size
}

// 主函数
int main()
{
    double f; // 频率（Hz）
    double A; // 振幅（0 到 1 的值）
    double t; // 时长（秒）

    cout << "请输入频率 (Hz): ";
    cin >> f;
    cout << "请输入振幅 (0 到 1): ";
    cin >> A;
    cout << "请输入时长 (秒): ";
    cin >> t;

    if (A < 0 || A > 1 || f <= 0 || t <= 0)
    {
        cerr << "输入无效，请确保频率 > 0，振幅在 0 到 1 之间，时长 > 0！" << endl;
        return 1;
    }

    int totalSamples = static_cast<int>(SAMPLE_RATE * t); // 总样本数
    int16_t *buffer = new int16_t[totalSamples]; // 16 位整数表示的样本数据
    double twoPiF = 2 * M_PI * f;

    // 生成正弦波样本
    for (int i = 0; i < totalSamples; ++i)
    {
        double t = static_cast<double>(i) / SAMPLE_RATE;
        buffer[i] = static_cast<int16_t>(A * 32767 * sin(twoPiF * t));
    }

    // 写入 WAV 文件
    ofstream wavFile("output.wav", ios::binary);
    if (!wavFile)
    {
        cerr << "无法创建文件！" << endl;
        delete[] buffer;
        return 1;
    }

    int dataSize = totalSamples * 2; // 每个样本 2 字节
    writeWavHeader(wavFile, dataSize);
    wavFile.write(reinterpret_cast<const char *>(buffer), dataSize);

    wavFile.close();
    delete[] buffer;

    cout << "生成完成！文件已保存为 output.wav" << endl;
    return 0;
}
