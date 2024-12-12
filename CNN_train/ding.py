from pydub import AudioSegment
from pydub.generators import Sine

# 生成一个 1000 Hz，持续时间 100 毫秒的纯音
tone = Sine(1000).to_audio_segment(duration=100)

# 保存为 .wav 文件
tone.export("ding_sound.wav", format="wav")
