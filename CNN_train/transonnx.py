import torch
import torch.onnx
from cnn_model.model import net  # 假设你的网络结构是 PressCNN

# 加载训练好的模型
net.load_state_dict(torch.load('./pressCNN_net.pth'))
net.eval()  # 进入推理模式

# 输入张量的大小（根据您的模型输入尺寸定义）
dummy_input = torch.randn(1, 3, 64, 64)  # (batch_size, channels, height, width)

# 导出到 ONNX 格式
torch.onnx.export(net, dummy_input, "pressCNN_net.onnx", export_params=True, opset_version=11)
