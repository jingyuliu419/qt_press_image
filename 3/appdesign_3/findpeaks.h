#ifndef __FINDPEAKS__
#define __FINDPEAKS__
#include <vector>
struct peak
{
	int index;
	float value;
};
bool comparePeaks(const peak& a, const peak& b);
bool compareIndex(const peak& a, const peak& b);
std::vector<peak> detectInitialPeaks(const std::vector<float>& src, const std::vector<int>& sign);
std::vector<peak> filterPeaksByThreshold(std::vector<peak>& peaks, float threshold);
std::vector<peak> filterPeaksByDistance(std::vector<peak>& peaks, int distance);
std::vector<peak>findPeaks(const std::vector<float>& src, int distance = 0, float threshold = 0);
#endif



