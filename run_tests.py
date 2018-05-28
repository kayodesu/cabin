# -*- coding: utf-8 -*-
import os


def get_all_classes(root_dir):
    """
    返回给定目录下的所有 class 文件
    """
    classes = []

    for lists in os.listdir(root_dir):
        path = os.path.join(root_dir, lists)
        if os.path.isdir(path):
            get_all_classes(path)
        elif path.endswith(".class") and path.find('$') == -1:  # 是 class 文件，且不是内部类
            classes.append(path)

    return classes


if __name__ == '__main__':
    # 生成的 classes 文件的存放目录
    jvm_path = "D:/code/jvm/cmake-build-release/jvm.exe -printlevel 3 -bootstrapclasspath 'C:\Program Files\Java\jre1.8.0_162\lib' "
    all_classes = get_all_classes("testclasses/out/production/testclasses/io/github/jiayanggo")
    for clazz in all_classes:
        cmd = jvm_path + clazz
        print(cmd)
        os.system(cmd)
        # while True:
        #     pass

