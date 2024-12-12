import os
import json

# 输入和输出目录
input_folder = "/home/liu/Doc/Doc1/mycode/original_data"
output_folder = "/home/liu/Doc/Doc1/mycode/original_data1"

# 确保输出文件夹存在
os.makedirs(output_folder, exist_ok=True)

# 遍历所有 .txt 文件
for file_name in os.listdir(input_folder):
    if file_name.endswith(".txt"):
        input_path = os.path.join(input_folder, file_name)
        output_path = os.path.join(output_folder, file_name.replace(".txt", ".json"))

        with open(input_path, "r") as txt_file:
            # 读取每行数据
            lines = txt_file.readlines()

            # 将每行数据存储为 JSON 数组中的对象
            data = [{"sensor": file_name.replace(".txt", ""), "data": line.strip()} for line in lines]

            # 写入 JSON 文件
            with open(output_path, "w") as json_file:
                json.dump(data, json_file, indent=4)

        print(f"Converted {file_name} to {output_path}")
