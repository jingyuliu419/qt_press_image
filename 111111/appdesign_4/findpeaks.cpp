#include <numeric>
#include <algorithm>
#include "findpeaks.h"

bool comparePeaks(const peak& a, const peak& b)
{
    return a.value > b.value; // 按value降序排序
}

bool compareIndex(const peak& a, const peak& b)
{
    return a.index < b.index; // 按index升序排序
}

std::vector<peak>findPeaks(const std::vector<float>& src, int distance, float threshold)
{
    size_t length = src.size();
    if (length < 3)
    {
        //std::cout << "findPeaks:峰值检测数据量过少！" << std::endl;
        return std::vector<peak>();
    }
    std::vector<int> sign(length, -1);
    std::vector<float> difference(length, 0);
    std::vector<peak>result;
    adjacent_difference(src.begin(), src.end(), difference.begin());
    for (int i = 0; i < difference.size(); ++i)
    {
        if (difference[i] >= 0) sign[i] = 1;
    }

    //second-order difference
    peak temp;
    for (int j = 1; j < length - 1; ++j)
    {
        int  diff = sign[j] - sign[j - 1];
        if (diff < 0) {
            temp.index = j;
            temp.value = src[j - 1];
            result.emplace_back(temp);
        }
    }
    //判断最后一个数是否位于峰值位置
    if (difference.back() > 0)
    {
        temp.index = difference.size();
        temp.value = src.back();
        result.emplace_back(temp);
    }

    //阈值过滤
    if (threshold != 0)
    {
        auto it = result.begin();
        while (it != result.end())
        {
            if (it->value < threshold)
            {
                it = result.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }
    //距离过滤
    //算法实现：将元素从大到小排列，从最大的开始检测某一距离内是否有其他峰值，
    //若存在其他峰值，则删除对应峰值
    //若没有，或者删除之后，则查找第二个峰值，如此重复，直到所有峰值检测完成
    if (distance != 0)
    {
        //按照value值降序排列
        std::sort(result.begin(), result.end(), comparePeaks);

        //遍历峰值，删除近距离的峰值
        for (int i = 0; i < result.size(); ++i)
        {
            for (int j = i + 1; j < result.size();)
            {
                if (std::abs(result[i].index - result[j].index) <= distance)
                {
                    // 删除result[j]，注意迭代器j在删除后不会自动增加
                    result.erase(result.begin() + j);
                }
                else
                {
                    ++j;
                }
            }
        }

        std::sort(result.begin(), result.end(), compareIndex);
    }

    return result;
}
