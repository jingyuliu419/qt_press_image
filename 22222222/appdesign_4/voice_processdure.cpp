#include "voice_processdure.h"
#include <QTextCodec>
#include <QByteArray>
#include <QDebug>
#include <QThread>

//注意 hintsVec_第0个都空，便于计数
const QVector<QString> VoiceProcessdure::hintsVec_{"",
                                  "请判断患者呼吸脉搏情况",
                                  "呼叫救援，取得除颤仪，并立即开始胸外按压",
                                  "停止按压，开始示波",
                                  "立即除颤，单向波能量三百六十焦耳，开始充电",
                                  "大家离开患者，给予除颤",
                                  "立即恢复心外按压",
                                  "立即恢复心外按压，如有条件，行有创动脉压力监测和血氧饱和度监测",
                                  "立即恢复心外按压，立即给予肾上腺1毫克静脉注射",
                                  "继续按压，请判断通气效果是否满意，如难以有效通气，给予气管插管通气，监测呼气末二氧化碳浓度",
                                  "立即除颤，单向波能量三百六十焦耳，开始充电",
                                  "给予复苏后生命支持"};

//除颤仪就绪  //注意 status_ 比 asrMsgIndex 多个 INIT，因为假设处在状态1，才能通过识别语音判断是否进入状态2  也是这里6空着的原因
const QVector<QString> VoiceProcessdure::asrMsgsVec_{
    "患者没有反应了", "没有呼吸，没有脉搏","除颤仪到", "室颤",
    "充电完毕", "放电完毕","","通气完成","摸到脉搏", "仍为室颤"};

void VoiceProcessdure::hint() {
  int tmpIndex = static_cast<int>(status_);//获取状态

  if (tmpIndex >= hintsVec_.size()) return;

  if(isEnd == true){
      emit TTSMsg(hintsVec_[11]);
  }
  else if(isVen == true){
      emit forcePlay();//强制播放音频
      //通气完成
      if (venIndex == 1) {
          emit TTSMsg(hintsVec_[9]);
      }
      else emit TTSMsg(hintsVec_[6]);
      isVen = false;
  }
  else if (isDefib == true) {
      emit forcePlay();//强制播放音频
      // 放电完毕     defibIndex: 除颤次数
      if(defibIndex == 1) emit TTSMsg(hintsVec_[7]);
      else if(defibIndex >= 2) emit TTSMsg(hintsVec_[8]);
      else emit TTSMsg(hintsVec_[6]);
      isDefib = false;
  }
  //第二次识别到室颤语音提示内容改变
  else if(isVf == true  && defibIndex >= 1){
      if(can_secondDefi == true)
          can_secondDefi = false;
      else
          return;
      // isVf:室颤  defibIndex: 除颤次数
      emit forcePlay();//强制播放音频
      emit TTSMsg(hintsVec_[10]);
      qDebug() << "第二次识别到室颤";
  }
  else{
      if( 1 <= tmpIndex && tmpIndex <= 5){
          emit forcePlay();//强制播放音频
      }
      emit TTSMsg(hintsVec_[tmpIndex]);
  }
}

bool VoiceProcessdure::checkStatus(const QString &msg, int confidence) {
    if(confidence == 100)
        qDebug()<< "yolo msg: " << msg;
    else if(confidence == 99)
        qDebug()<< "KWS msg: " << msg;
    else
        qDebug()<< "checkStatus msg: " << msg;

    // 除颤仪相关语音识别结果处理
    if (confidence < 10) return false;
    bool hintFlag = false;

    //asrMsgsVec_[asrMsgIndex].contains(msg)
    int asrMsgIndex = static_cast<int>(status_);//可用于后续判断 语音 和 status_ 是否匹配, 可以避免错误识别

    if(msg.contains("仍为室颤")){
        asrMsgIndex = 9;
    }

    if(msg.contains("继续按压")){
        if(defibIndex >= 1)
            status_ = Status::AED_ARRIVED; //除颤仪到
        else
            status_ = Status::NO_BREATH_PULSE; //没有呼吸，没有脉搏
        //错误识别:除颤仪到  恢复按压 总轮数不清零  按压次数不清零
        emit setZero(-2);
        emit forcePlay();//强制播放音频
        emit TTSMsg("恢复心外按压");
    }

    if(msg.contains("cover") && coverTimes<5){
        if(respiration == true){
            //面罩覆盖(开始通气)
            ++coverTimes;
            qDebug() << "面罩覆盖coverTimes" << coverTimes << " frames  ";
        }else{
            coverTimes = 0;
        }
    }

    qDebug() << "asrMsgIndex is" << asrMsgIndex;
    if(!msg.contains(asrMsgsVec_[2]))
        electrodeTimes = 0;//一直清零
    if(respiration == false){
        if(!msg.contains(asrMsgsVec_[7])){
            ventilateTimes = 0;
            intervalTimes = 0;//一直清零
        }
        if(!msg.contains("cover")){
            coverTimes = 0;
        }
    }

    //识别到通气完成
    if(asrMsgsVec_[7].contains(msg) || msg.contains(asrMsgsVec_[7])){
        if(respiration == false)
            return false;//按压30次,respiration会true 才能通气  还可以避免重复通气完成
        if(confidence == 100){
            //from yoloMsg
            if(coverTimes <5)
                return false; //面罩覆盖(开始通气)

            ++intervalTimes;
            if(intervalTimes<10){
                if(intervalTimes % 3 == 0 && ventilateTimes == 0)
                    qDebug() << "第一次挤压通气瓶" << intervalTimes << " frames  ";
                if(intervalTimes % 3 == 0 && ventilateTimes == 2)
                    qDebug() << "第二次挤压通气瓶" << intervalTimes << " frames  ";
                return false;
            }else{
                //intervalTimes >= 10
                if(ventilateTimes == 0){
                    ++ventilateTimes;
                    qDebug() << "第一次挤压通气瓶" << intervalTimes << "times   ";
                    firstVentilateTime = QDateTime::currentDateTime();
                    return false;
                }
                if(ventilateTimes == 1){
                    QDateTime secondVentilateTime=  QDateTime::currentDateTime();
                    int intervalTime = firstVentilateTime.msecsTo(secondVentilateTime);
                    if(intervalTime>1000){
                        intervalTimes = 0;
                        ++ventilateTimes;
                    }
                    return false;
                }
                if(ventilateTimes == 2)
                    qDebug() << "第二次挤压通气瓶" << intervalTimes << "times   ";
            }
        }
        intervalTimes = 0;
        ventilateTimes = 0;
        coverTimes = 0;

        respiration = false;
        isVen = true; //通气完成
        //除颤两次后，开始通气计数    defibIndex: 除颤次数
        if(defibIndex == 2) venIndex++;
        //清零按压次数
        emit setZero(0);
        hintFlag = true;
    }
    //识别到摸到脉搏
    else if(asrMsgsVec_[8].contains(msg) || msg.contains(asrMsgsVec_[8])){
        if(can_secondDefi == false)
            return false;
        qDebug()<< "-----------" << "识别到摸到脉搏" << "-------------";
        isEnd = true;
        hintFlag = true;
    }
    //识别到其他语音  setzero: 设置 pcpr_thread 的状态
    else if (asrMsgsVec_[asrMsgIndex].contains(msg) || msg.contains(asrMsgsVec_[asrMsgIndex])) {
                if(asrMsgIndex == 0 || asrMsgIndex ==1 || asrMsgIndex == 2){
                    if(asrMsgIndex == 2){
                        if(status_ != Status::NO_BREATH_PULSE || respiration == true)
                            return false;
//                        if(confidence == 99) {
//                             //除颤仪到from KWS
//                            return false;
//                        }
                        if(confidence == 100){
                            //from yoloMsg
                            ++electrodeTimes;
                            if(electrodeTimes<10){
                                if(electrodeTimes % 3 == 0)
                                    qDebug() << "除颤仪electrodeTimes" << electrodeTimes << " frames   ";
                                return false;
                            }
                        }
                        electrodeTimes = 0;
                    }

                    // 没有反应 / 无呼吸,无脉搏 / 除颤仪到
                    status_ = static_cast<Status>(status_+1);
                    //将count清0，避免按压时出现语音提示
                    if(asrMsgIndex == 1) emit setZero(-1);//进行心扉按压流程
                    else if(asrMsgIndex == 2) emit setZero(3);
                }
                //室颤 or 仍为室颤
                if( asrMsgIndex == 3 || asrMsgIndex == 9){
                    if(respiration == true)
                        return false;
                    if(defibIndex >= 1 && can_secondDefi == false){
                        //第二次识别到室颤
                        return false;
                    }
                    setZero(3);
                    status_ = static_cast<Status>(status_+1);
                    isVf = true;
                }
                //充电完毕
                if(asrMsgIndex == 4){
                    emit setZero(1);
                    status_ = static_cast<Status>(status_+1);
                    isVf = false;
                }
                //放电完毕
                if(asrMsgIndex == 5){
                    emit setZero(2);
                    //当第一轮按压结束后，识别“仍为室颤”
                    status_ = Status::AED_ARRIVED; //除颤仪到
                    isDefib = true;
                    //除颤计数+1
                    defibIndex++;
                }
                hintFlag = true;
    }

  if (hintFlag) hint(); // 处理完毕了，调用播放语音函数
  return true;
}
