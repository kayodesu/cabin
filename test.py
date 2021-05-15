# -*- coding: utf-8 -*-
# @Author: Yo Ka

import os

VM = 'd:/code/cabin/build/cabin.exe -cp "d:/code/cabin/test-classes/out/production/test-classes" '

if __name__ == '__main__':
    for e in os.walk('./test-classes/src'):
        # e[0] # 目录名
        # e[1] # 此目录下的所有子目录
        # e[2] # 此目录下的所有文件
        
        for f in e[2]:  
            class_name = e[0].replace('/', '.')
            class_name = class_name.replace('\\', '.')
            i = class_name.find('src.')
            if i == -1: # 根目录
                class_name = f
            else:
                class_name = class_name[i+4:] # jump 'src.'
                class_name = class_name + "." + f
            i = class_name.rfind('.java')
            if i != -1:
                class_name = class_name[:i]
                print("---Test: " + class_name)
                os.system(VM + class_name)
