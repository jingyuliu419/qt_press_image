#include <numeric>
#include <algorithm>
#include "findpeaks.h"

bool comparePeaks(const peak& a, const peak& b)
{
    return a.value > b.value; // ��value��������
}

bool compareIndex(const peak& a, const peak& b)
{
    return a.index < b.index; // ��index��������
}

std::vector<peak>findPeaks(const std::vector<float>& src, int distance, float threshold)
{
    size_t length = src.size();
    if (length < 3)
    {
        //std::cout << "findPeaks:��ֵ������������٣�" << std::endl;
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
    //�ж����һ�����Ƿ�λ�ڷ�ֵλ��
    if (difference.back() > 0)
    {
        temp.index = difference.size();
        temp.value = src.back();
        result.emplace_back(temp);
    }

    //��ֵ����
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
    //�������
    //�㷨ʵ�֣���Ԫ�شӴ�С���У������Ŀ�ʼ���ĳһ�������Ƿ���������ֵ��
    //������������ֵ����ɾ����Ӧ��ֵ
    //��û�У�����ɾ��֮������ҵڶ�����ֵ������ظ���ֱ�����з�ֵ������
    if (distance != 0)
    {
        //����valueֵ��������
        std::sort(result.begin(), result.end(), comparePeaks);

        //������ֵ��ɾ��������ķ�ֵ
        for (int i = 0; i < result.size(); ++i)
        {
            for (int j = i + 1; j < result.size();)
            {
                if (std::abs(result[i].index - result[j].index) <= distance)
                {
                    // ɾ��result[j]��ע�������j��ɾ���󲻻��Զ�����
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
